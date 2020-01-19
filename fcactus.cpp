#include "fcactus.hpp"
#include "cfg.hpp"
#include <cstring>
#include <climits>
#include <errno.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/timerfd.h>
#include <nk/string_replace_all.hpp>
#include <nk/from_string.hpp>
#include <nk/optionarg.hpp>
extern "C" {
#include "nk/io.h"
#include "nk/exec.h"
#include "nk/privilege.h"
}

#define FCACTUS_VERSION "0.2"
#define MAX_CENV 50
#define MAX_ENVBUF 2048

// Maps timerfds to inotify watch fds.
static std::vector<std::pair<int, int>> debounce_rise_timers;

static epollfd epfd;

static std::string g_fcactus_conf("/etc/fcactus.actions");

static void fail_on_fdne(const std::string &file, int mode)
{
    if (access(file.c_str(), mode))
        exit(EXIT_FAILURE);
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#endif
void watch_meta::exec(const std::string &args)
{
    char *env[MAX_CENV];
    char envbuf[MAX_ENVBUF];
    switch ((int)fork()) {
    case 0:
        if (nk_generate_env(user_, chroot_.empty() ? nullptr : chroot_.c_str(),
                            path_.empty() ? nullptr : path_.c_str(),
                            env, MAX_CENV, envbuf, sizeof envbuf) < 0) {
            const char errstr[] = "watch_meta::exec: failed to generate environment\n";
            write(STDERR_FILENO, errstr, sizeof errstr);
            std::exit(EXIT_FAILURE);
        }
        if (limits_.exist() && limits_.enforce(user_, group_, cmd_)) {
            const char errstr[] = "watch_meta::exec: rlimits::enforce failed\n";
            write(STDERR_FILENO, errstr, sizeof errstr);
            std::exit(EXIT_FAILURE);
        }
        if (group_) {
            if (setresgid(group_, group_, group_)) {
                const char errstr[] = "watch_meta::exec: setresgid failed\n";
                write(STDERR_FILENO, errstr, sizeof errstr);
                std::exit(EXIT_FAILURE);
            }
            if (getgid() == 0) {
                const char errstr[] = "watch_meta::exec: child is still gid=root after setgid()\n";
                write(STDERR_FILENO, errstr, sizeof errstr);
                std::exit(EXIT_FAILURE);
            }
        }
        if (user_) {
            if (setresuid(user_, user_, user_)) {
                const char errstr[] = "watch_meta::exec: setresuid failed\n";
                write(STDERR_FILENO, errstr, sizeof errstr);
                std::exit(EXIT_FAILURE);
            }
            if (getuid() == 0) {
                const char errstr[] = "watch_meta::exec: child is still uid=root after setuid()\n";
                write(STDERR_FILENO, errstr, sizeof errstr);
                std::exit(EXIT_FAILURE);
            }
        }
        nk_execute(cmd_.c_str(), args.c_str(), env);
    case -1: {
        const char errstr[] = "watch_meta::exec: fork failed\n";
        write(STDERR_FILENO, errstr, sizeof errstr);
        std::exit(EXIT_FAILURE);
    }
    default: break;
    }
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

void epollfd::manip(int fd, int mtype) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
    ev.data.fd = fd;
    int r = epoll_ctl(fd_, mtype, fd, &ev);
    if (r < 0)
        throw std::runtime_error(fmt::format("epoll_ctl failed {}", strerror(errno)));
}

std::pair<bool, int> inotify::add(std::unique_ptr<watch_meta> &&meta) {
    int fdt = inotify_add_watch(inotify_fd_.fd_, meta->filepath_.c_str(), meta->eventflags_);
    if (fdt < 0) {
        fmt::print(stderr, "Failed to set watch [{}][{}]: {}\n", meta->filepath_,
                   meta->eventflags_, strerror(errno));
        return std::make_pair(false, fdt);
    }
    watches_.emplace(std::make_pair(fdt, std::move(meta)));
    return std::make_pair(true, fdt);
}

static void mask_string_addm(std::string &acc, uint32_t mask, int bit, const char * const txt) {
    if (mask & bit) {
        if (acc.size()) acc.append(1, ',');
        acc.append(txt);
    }
}
static std::string mask_string(uint32_t mask)
{
    std::string acc;
    if ((mask & IN_MOVED_FROM) && (mask & IN_MOVED_TO)) {
        if (acc.size()) acc.append(1, ',');
        acc.append("IN_MOVE");
        mask &= ~(IN_MOVED_FROM|IN_MOVED_TO);
    }
    if ((mask & IN_CLOSE_WRITE) && (mask & IN_CLOSE_NOWRITE)) {
        if (acc.size()) acc.append(1, ',');
        acc.append("IN_CLOSE");
        mask &= ~(IN_CLOSE_WRITE|IN_CLOSE_NOWRITE);
    }
    mask_string_addm(acc, mask, IN_ACCESS, "IN_ACCESS");
    mask_string_addm(acc, mask, IN_ATTRIB, "IN_ATTRIB");
    mask_string_addm(acc, mask, IN_CLOSE_WRITE, "IN_CLOSE_WRITE");
    mask_string_addm(acc, mask, IN_CLOSE_NOWRITE, "IN_CLOSE_NOWRITE");
    mask_string_addm(acc, mask, IN_CREATE, "IN_CREATE");
    mask_string_addm(acc, mask, IN_DELETE, "IN_DELETE");
    mask_string_addm(acc, mask, IN_DELETE_SELF, "IN_DELETE_SELF");
    mask_string_addm(acc, mask, IN_MODIFY, "IN_MODIFY");
    mask_string_addm(acc, mask, IN_MOVE_SELF, "IN_MOVE_SELF");
    mask_string_addm(acc, mask, IN_MOVED_FROM, "IN_MOVED_FROM");
    mask_string_addm(acc, mask, IN_MOVED_TO, "IN_MOVED_TO");
    mask_string_addm(acc, mask, IN_OPEN, "IN_OPEN");
    return acc;
}

void inotify::dispatch_do(std::map<int, std::unique_ptr<watch_meta>>::iterator wmi,
                 const struct inotify_event *event)
{
    // XXX: Not used for now.
#if 0
    const bool is_dir(!!(event->mask & IN_ISDIR));
    const bool watch_del(!!(event->mask & IN_IGNORED));
    const bool fs_unmounted(!!(event->mask & IN_UNMOUNT));
    const bool buf_overflow(!!(event->mask & IN_Q_OVERFLOW));
#endif

    std::string args(wmi->second->args_);
    string_replace_all(args, "$%", 2, mask_string(event->mask).c_str());
    string_replace_all(args, "$&", 2, std::to_string(event->mask).c_str());
    string_replace_all(args, "$#", 2, std::string(event->name).c_str());
    string_replace_all(args, "$@", 2, wmi->second->filepath_.c_str());
    string_replace_all(args, "$$", 2, "$"); // must be last

    fmt::print("Event[{}]: exec '{} {}'\n", wmi->second->filepath_,
               wmi->second->cmd_, args);
    std::fflush(stdout);

    wmi->second->exec(args);
}

void inotify::dispatch(const struct inotify_event *event)
{
    auto wmi = watches_.find(event->wd);
    if (wmi == watches_.end()) {
        fmt::print(stderr, "no job metadata for wd [{}]\n", event->wd);
        return;
    }

    if (wmi->second->debounce_rise_ms_ > 0u) {
        // We do not immediately execute the event.  Instead, we queue
        // the event to be run only after debounce_rise_ms_ has passed with
        // no events.  If an event does arrive before that time, then we
        // reset the timer to wait again for debounce_rise_ms_.  This means
        // that the event could possibly be deferred forever.
        struct itimerspec its;
        {
            its.it_interval.tv_sec = 0;
            its.it_interval.tv_nsec = 0;
            const auto dbs = wmi->second->debounce_rise_ms_ / 1000u;
            const auto dbr = wmi->second->debounce_rise_ms_ - (1000u * dbs);
            its.it_value.tv_sec = dbs;
            its.it_value.tv_nsec = dbr * 1000000u;
        }
        int timerfd = -1;
        const auto end = debounce_rise_timers.end();
        auto dbti = debounce_rise_timers.begin();
        for (; dbti != end; ++dbti) {
            if (dbti->second == event->wd) { timerfd = dbti->first; break; }
        }
        if (timerfd < 0) {
            timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
            if (timerfd < 0) {
                fmt::print(stderr, "timerfd_create failed ({}); debouncing ignored\n",
                           strerror(errno));
                goto skip_debounce_rise;
            }
        }
        if (timerfd_settime(timerfd, 0, &its, nullptr) < 0) {
            if (dbti == end) {
                fmt::print(stderr, "timerfd_settime failed ({}); debouncing ignored\n",
                           strerror(errno));
                close(timerfd);
                goto skip_debounce_rise;
            }
            fmt::print(stderr, "timerfd_settime failed on existing timer ({})\n",
                       strerror(errno));
            return;
        }
        if (dbti == end) {
            epfd.add(timerfd);
            debounce_rise_timers.emplace_back(std::make_pair(timerfd, event->wd));
        }
        memcpy(&wmi->second->last_event_, event, sizeof wmi->second->last_event_);
        return;
    }
skip_debounce_rise:

    if (wmi->second->debounce_fall_ms_ > 0u) {
        // We execute the event immediately (on the 'rising' edge).  Then we
        // debounce the falling edge, ignoring events of the same kind until at least
        // debounce_fall_ms_ has passed after the rising edge.  The event will
        // never be deferred forever.
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0) {
            fmt::print(stderr, "clock_gettime failed ({}); debouncing ignored\n",
                       strerror(errno));
        } else {
            size_t diff_s = ts.tv_sec - wmi->second->last_ran_ts_.tv_sec;
            size_t diff_ms = (ts.tv_nsec - wmi->second->last_ran_ts_.tv_nsec) / 1000000u;
            size_t tt = diff_s * 1000u;
            if (tt >= diff_s) {
                size_t diff_ms_sum = diff_ms + tt;
                if (diff_ms_sum >= diff_ms && diff_ms < wmi->second->debounce_fall_ms_)
                    return;
            }
            memcpy(&wmi->second->last_ran_ts_, &ts, sizeof wmi->second->last_ran_ts_);
        }
    }
    dispatch_do(wmi, event);
}

void inotify::dispatch_debounce_rise_event(int fd)
{
    auto wmi = watches_.find(fd);
    if (wmi == watches_.end()) {
        fmt::print(stderr, "no job metadata for wd [{}]\n", fd);
        return;
    }
    dispatch_do(wmi, &wmi->second->last_event_);
}

void inotify::dispatch()
{
    char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;

    auto r = safe_read(inotify_fd_.fd_, buf, sizeof buf);
    if (r == -1)
        throw std::runtime_error("inotifyfd::dispatch: read failed");
    if (r == 0)
        return;
    const size_t len(std::min(r, SSIZE_MAX));
    for (auto p = buf; p < buf + len; p += sizeof(struct inotify_event) + event->len) {
        event = reinterpret_cast<const struct inotify_event *>(p);
        dispatch(event);
    }
}

signal_fd::signal_fd() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    if (sigprocmask(SIG_BLOCK, &mask, nullptr) < 0)
        throw std::runtime_error("sigprocmask failed\n");
    fd_ = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
    if (fd_ < 0)
        throw std::runtime_error(fmt::format("Failed to create signal_fd fd: {}", fd_));
}

void signal_fd::dispatch(void)
{
    struct signalfd_siginfo si;
    memset(&si, 0, sizeof si);
    ssize_t r = safe_read(fd_, (char *)&si, sizeof si);
    if (r < 0) {
        fmt::print(stderr, "error reading from signalfd: {}", strerror(errno));
        return;
    }
    if ((size_t)r < sizeof si) {
        fmt::print(stderr, "short read from signalfd: {} < {}\n", r, sizeof si);
        return;
    }
    static const char *signames[] = { "HUP", "TERM", "INT" };
    unsigned signame = 0;
    switch (si.ssi_signo) {
        case SIGHUP: signame = 0; break;
        case SIGCHLD:
            while (waitpid(-1, nullptr, WNOHANG) > 0);
            return;
        case SIGTERM: signame = 1; break;
        case SIGINT: signame = 2; break;
        default: return;
    }
    fmt::print("Received SIG{}.  Exiting gracefully.\n", signames[signame]);
    std::fflush(stdout);
    exit(EXIT_SUCCESS);
}

static void print_version(void)
{
    fmt::print("fcactus " FCACTUS_VERSION ", inotify action daemon.\n"
               "Copyright 2015-2016 Nicholas J. Kain\n"
               "All rights reserved.\n\n"
               "Redistribution and use in source and binary forms, with or without\n"
               "modification, are permitted provided that the following conditions are met:\n\n"
               "- Redistributions of source code must retain the above copyright notice,\n"
               "  this list of conditions and the following disclaimer.\n"
               "- Redistributions in binary form must reproduce the above copyright notice,\n"
               "  this list of conditions and the following disclaimer in the documentation\n"
               "  and/or other materials provided with the distribution.\n\n"
               "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n"
               "AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n"
               "IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n"
               "ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE\n"
               "LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR\n"
               "CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF\n"
               "SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS\n"
               "INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n"
               "CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)\n"
               "ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n"
               "POSSIBILITY OF SUCH DAMAGE.\n");
}

enum OpIdx {
    OPT_UNKNOWN, OPT_HELP, OPT_VERSION, OPT_CONFIG
};
static const option::Descriptor usage[] = {
    { OPT_UNKNOWN,    0,  "",           "", Arg::Unknown,
        "fcactus " FCACTUS_VERSION ", inotify action daemon.\n"
        "Copyright 2015-2016 Nicholas J. Kain\n"
        "fcactus [options]...\n\nOptions:" },
    { OPT_HELP,       0, "h",            "help",    Arg::None, "\t-h, \t--help  \tPrint usage and exit." },
    { OPT_VERSION,    0, "v",         "version",    Arg::None, "\t-v, \t--version  \tPrint version and exit." },
    { OPT_CONFIG,     0, "c",          "config",  Arg::String, "\t-c, \t--config  \tPath to configuration file." },
    {0,0,0,0,0,0}
};
static void process_options(int ac, char *av[])
{
    ac-=ac>0; av+=ac>0;
    option::Stats stats(usage, ac, av);
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvla"
    option::Option options[stats.options_max], buffer[stats.buffer_max];
#pragma GCC diagnostic pop
    option::Parser parse(usage, ac, av, options, buffer);
#else
    auto options = std::make_unique<option::Option[]>(stats.options_max);
    auto buffer = std::make_unique<option::Option[]>(stats.buffer_max);
    option::Parser parse(usage, ac, av, options.get(), buffer.get());
#endif
    if (parse.error())
        std::exit(EXIT_FAILURE);
    if (options[OPT_HELP]) {
        uint16_t col{80};
        if (const auto cols = getenv("COLUMNS")) {
            if (auto t = nk::from_string<uint16_t>(cols)) col = *t;
        }
        option::printUsage(fwrite, stdout, usage, col);
        std::exit(EXIT_FAILURE);
    }
    if (options[OPT_VERSION]) {
        print_version();
        std::exit(EXIT_FAILURE);
    }

    for (int i = 0; i < parse.optionsCount(); ++i) {
        option::Option &opt = buffer[i];
        if (opt.index() == OPT_CONFIG)
            g_fcactus_conf = std::string(opt.arg);
    }
}

int main(int argc, char* argv[])
{
    process_options(argc, argv);
    fail_on_fdne(g_fcactus_conf, R_OK);

    inotify inyfd;

    parse_config(inyfd, g_fcactus_conf);

    if (!inyfd.has_jobs()) {
        fmt::print(stderr, "{}: no jobs, exiting\n", __func__);
        std::exit(EXIT_FAILURE);
    }

    umask(077);

#ifdef __linux__
    prctl(PR_SET_DUMPABLE, 0, 0, 0, 0);
    prctl(PR_SET_KEEPCAPS, 0, 0, 0, 0);
    prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0);
#endif

    signal_fd sigfd;
    struct epoll_event events[2];

    epfd.add(sigfd.fd());
    epfd.add(inyfd.fd());

    while (1) {
        int nevents = epoll_wait(epfd.fd_, events, 1, -1);
        if (nevents < 0) {
            if (errno == EINTR)
                continue;
            throw std::runtime_error(fmt::format("epoll_wait failed: {}", nevents));
        }
        for (int i = 0; i < nevents; ++i) {
            int fd = events[i].data.fd;
            if (fd == sigfd.fd()) {
                if (!(events[i].events & EPOLLIN)) {
                    fmt::print(stderr, "sigfd event that isn't IN\n");
                    continue;
                }
                sigfd.dispatch();
            } else if (fd == inyfd.fd()) {
                if (!(events[i].events & EPOLLIN)) {
                    fmt::print(stderr, "inyfd event that isn't IN\n");
                    continue;
                }
                inyfd.dispatch();
            } else {
                bool found_timer{false};
                const auto end = debounce_rise_timers.end();
                for (auto j = debounce_rise_timers.begin(); j != end; ++j) {
                    if (fd == j->first) {
                        found_timer = true;
                        if (!(events[i].events & EPOLLIN)) {
                            fmt::print(stderr, "timerfd event that isn't IN\n");
                            break;
                        }
                        epfd.del(j->first);
                        close(j->first);
                        inyfd.dispatch_debounce_rise_event(j->second);
                        debounce_rise_timers.erase(j);
                        break;
                    }
                }
                if (!found_timer)
                    throw std::runtime_error("event on unknown fd");
            }
        }
    }
    exit(EXIT_SUCCESS);
}

