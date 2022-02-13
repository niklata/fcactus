// Copyright 2015-2022 Nicholas J. Kain <njkain at gmail dot com>
// SPDX-License-Identifier: MIT
#include "fcactus.hpp"
#include "cfg.hpp"
#include <cstring>
#include <climits>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/timerfd.h>
#include <getopt.h>
#include <nk/string_replace_all.hpp>
#include <nk/from_string.hpp>
extern "C" {
#include "nk/io.h"
#include "nk/exec.h"
#include "nk/privs.h"
}

#define FCACTUS_VERSION "0.2"
#define MAX_CENV 50
#define MAX_ENVBUF 16384

// Maps timerfds to inotify watch fds.
static std::vector<std::pair<int, int>> debounce_rise_timers;

static epollfd epfd;

static std::string g_fcactus_conf("/etc/fcactus.actions");
static std::optional<int> s6_notify_fd;

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
        {
            int r = nk_generate_env(user_, chroot_.empty() ? nullptr : chroot_.c_str(),
                                    path_.empty() ? nullptr : path_.c_str(),
                                    env, MAX_CENV, envbuf, sizeof envbuf);
            if (r < 0) {
                static const char errstr[] = "exec: failed to generate environment - ";
                write(STDERR_FILENO, errstr, sizeof errstr);
                static const char errstr0[] = "(?) unknown error";
                static const char errstr1[] = "(-1) account for uid does not exist";
                static const char errstr2[] = "(-2) not enough space in envbuf";
                static const char errstr3[] = "(-3) not enough space in env";
                static const char errstr4[] = "(-4) chdir to homedir or rootdir failed";
                switch (r) {
                default: write(STDERR_FILENO, errstr0, sizeof errstr0); break;
                case -1: write(STDERR_FILENO, errstr1, sizeof errstr1); break;
                case -2: write(STDERR_FILENO, errstr2, sizeof errstr2); break;
                case -3: write(STDERR_FILENO, errstr3, sizeof errstr3); break;
                case -4: write(STDERR_FILENO, errstr4, sizeof errstr4); break;
                }
                write(STDERR_FILENO, "\n", 1);
                std::exit(EXIT_FAILURE);
            }
        }
        if (limits_.exist() && limits_.enforce(user_, group_, cmd_)) {
            static const char errstr[] = "watch_meta::exec: rlimits::enforce failed\n";
            write(STDERR_FILENO, errstr, sizeof errstr);
            std::exit(EXIT_FAILURE);
        }
        if (group_) {
            if (setresgid(group_, group_, group_)) {
                static const char errstr[] = "watch_meta::exec: setresgid failed\n";
                write(STDERR_FILENO, errstr, sizeof errstr);
                std::exit(EXIT_FAILURE);
            }
            if (getgid() == 0) {
                static const char errstr[] = "watch_meta::exec: child is still gid=root after setgid()\n";
                write(STDERR_FILENO, errstr, sizeof errstr);
                std::exit(EXIT_FAILURE);
            }
        }
        if (user_) {
            if (setresuid(user_, user_, user_)) {
                static const char errstr[] = "watch_meta::exec: setresuid failed\n";
                write(STDERR_FILENO, errstr, sizeof errstr);
                std::exit(EXIT_FAILURE);
            }
            if (getuid() == 0) {
                static const char errstr[] = "watch_meta::exec: child is still uid=root after setuid()\n";
                write(STDERR_FILENO, errstr, sizeof errstr);
                std::exit(EXIT_FAILURE);
            }
        }
        nk_execute(cmd_.c_str(), args.c_str(), env);
    case -1: {
        static const char errstr[] = "watch_meta::exec: fork failed\n";
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
        suicide("epoll_ctl failed: %s", strerror(errno));
}

std::pair<bool, int> inotify::add(std::unique_ptr<watch_meta> &&meta) {
    int fdt = inotify_add_watch(inotify_fd_.fd_, meta->filepath_.c_str(), meta->eventflags_);
    if (fdt < 0) {
        log_line("Failed to set watch [%s][%d]: %s", meta->filepath_.c_str(),
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

    log_line("Event[%s]: exec '%s %s'", wmi->second->filepath_.c_str(),
             wmi->second->cmd_.c_str(), args.c_str());
    std::fflush(stdout);

    wmi->second->exec(args);
}

void inotify::dispatch(const struct inotify_event *event)
{
    auto wmi = watches_.find(event->wd);
    if (wmi == watches_.end()) {
        log_line("no job metadata for wd [%d]", event->wd);
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
                log_line("timerfd_create failed (%s); debouncing ignored",
                         strerror(errno));
                goto skip_debounce_rise;
            }
        }
        if (timerfd_settime(timerfd, 0, &its, nullptr) < 0) {
            if (dbti == end) {
                log_line("timerfd_settime failed (%s); debouncing ignored",
                         strerror(errno));
                close(timerfd);
                goto skip_debounce_rise;
            }
            log_line("timerfd_settime failed on existing timer (%s)",
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
            log_line("clock_gettime failed (%s); debouncing ignored", strerror(errno));
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
        log_line("no job metadata for wd [%d]", fd);
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
        suicide("inotifyfd::dispatch: read failed: %s", strerror(errno));
    if (r == 0)
        return;
    const size_t len(std::min(r, SSIZE_MAX));
    for (auto p = buf; p < buf + len; p += sizeof(struct inotify_event) + event->len) {
        event = reinterpret_cast<const struct inotify_event *>(p);
        dispatch(event);
    }
}

static void signal_handler(int signo)
{
    switch (signo) {
    case SIGCHLD: while (waitpid(-1, nullptr, WNOHANG) > 0); break;
    case SIGINT:
    case SIGTERM: {
        static const char errstr[] = "Got terminating signal: exiting.\n";
        safe_write(STDOUT_FILENO, errstr, sizeof errstr - 1);
        _exit(EXIT_FAILURE);
        break;
    }
    default: break;
    }
}

static void setup_signals_fcactus()
{
    static const int ss[] = {
        SIGCHLD, SIGINT, SIGTERM, SIGHUP, SIGKILL
    };
    sigset_t mask;

    if (sigprocmask(0, 0, &mask) < 0)
        suicide("sigprocmask failed");
    for (int i = 0; ss[i] != SIGKILL; ++i)
        if (sigdelset(&mask, ss[i]))
            suicide("sigdelset failed");
    if (sigaddset(&mask, SIGPIPE))
        suicide("sigaddset failed");
    if (sigprocmask(SIG_SETMASK, &mask, (sigset_t *)0) < 0)
        suicide("sigprocmask failed");

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART;
    if (sigemptyset(&sa.sa_mask))
        suicide("sigemptyset failed");
    for (int i = 0; ss[i] != SIGKILL; ++i)
        if (sigaction(ss[i], &sa, NULL))
            suicide("sigaction failed");
}

static void usage()
{
    printf("fcactus " FCACTUS_VERSION ", inotify action daemon.\n"
           "Copyright 2003-2022 Nicholas J. Kain\n"
           "Usage: fcactus [options]...\n\nOptions:\n"
           "--help         -h    Print usage and exit.\n"
           "--version      -v    Print version and exit.\n"
           "--crontab      -c [] Path to configuration file.\n"
    );
}

static void print_version(void)
{
    log_line("fcactus " FCACTUS_VERSION ", inotify action daemon.\n"
             "Copyright 2015-2022 Nicholas J. Kain\n\n"
"Permission is hereby granted, free of charge, to any person obtaining\n"
"a copy of this software and associated documentation files (the\n"
"\"Software\"), to deal in the Software without restriction, including\n"
"without limitation the rights to use, copy, modify, merge, publish,\n"
"distribute, sublicense, and/or sell copies of the Software, and to\n"
"permit persons to whom the Software is furnished to do so, subject to\n"
"the following conditions:\n\n"
"The above copyright notice and this permission notice shall be\n"
"included in all copies or substantial portions of the Software.\n\n"
"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,\n"
"EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF\n"
"MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND\n"
"NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE\n"
"LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION\n"
"OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION\n"
"WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n"
             );
}

static void process_options(int ac, char *av[])
{
    static struct option long_options[] = {
        {"help", 0, (int *)0, 'h'},
        {"version", 0, (int *)0, 'v'},
        {"config", 1, (int *)0, 'c'},
        {"s6-notify", 1, (int *)0, 'd'},
        {(const char *)0, 0, (int *)0, 0 }
    };
    for (;;) {
        auto c = getopt_long(ac, av, "hvc:d:", long_options, (int *)0);
        if (c == -1) break;
        switch (c) {
            case 'h': usage(); std::exit(EXIT_SUCCESS); break;
            case 'v': print_version(); std::exit(EXIT_SUCCESS); break;
            case 'c': g_fcactus_conf = optarg; break;
            case 'd': s6_notify_fd = atoi(optarg); break;
            default: break;
        }
    }
}

int main(int argc, char* argv[])
{
    process_options(argc, argv);
    fail_on_fdne(g_fcactus_conf, R_OK);

    inotify inyfd;

    parse_config(inyfd, g_fcactus_conf);

    if (!inyfd.has_jobs()) {
        log_line("%s: no jobs, exiting", __func__);
        std::exit(EXIT_FAILURE);
    }

    umask(077);
    setup_signals_fcactus();

#ifdef __linux__
    prctl(PR_SET_DUMPABLE, 0, 0, 0, 0);
    prctl(PR_SET_KEEPCAPS, 0, 0, 0, 0);
    prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0);
#endif

    struct epoll_event events[2];

    epfd.add(inyfd.fd());

    if (s6_notify_fd) {
        char buf[] = "\n";
        safe_write(*s6_notify_fd, buf, 1);
        close(*s6_notify_fd);
    }

    while (1) {
        int nevents = epoll_wait(epfd.fd_, events, 1, -1);
        if (nevents < 0) {
            if (errno == EINTR)
                continue;
            suicide("epoll_wait failed: %s", strerror(errno));
        }
        for (int i = 0; i < nevents; ++i) {
            int fd = events[i].data.fd;
            if (fd == inyfd.fd()) {
                if (!(events[i].events & EPOLLIN)) {
                    log_line("inyfd event that isn't IN");
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
                            log_line("timerfd event that isn't IN");
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
                    suicide("event on unknown fd");
            }
        }
    }
    exit(EXIT_SUCCESS);
}

