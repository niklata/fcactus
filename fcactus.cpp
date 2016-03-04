#include "fcactus.hpp"
#include "cfg.hpp"
#include <cstring>
#include <climits>
#include <fstream>
#include <errno.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/replace.hpp>
extern "C" {
#include "nk/io.h"
#include "nk/exec.h"
#include "nk/privilege.h"
#include "nk/pidfile.h"
}

#define FCACTUS_VERSION "0.1"

static std::string g_fcactus_conf("/etc/fcactus.actions");

static void fail_on_fdne(const std::string &file, const char *mode)
{
    if (file_exists(file.c_str(), mode))
        exit(EXIT_FAILURE);
}

void watch_meta::exec(const std::string &args)
{
    switch ((int)fork()) {
    case 0:
        if (!chroot_.empty())
            nk_set_chroot(chroot_.c_str());
        if (limits_ && limits_->enforce(user_, group_, cmd_)) {
            fmt::print(stderr, "{}: rlimits::enforce failed\n", __func__);
            std::exit(EXIT_FAILURE);
        }
        if (group_) {
            if (setresgid(group_, group_, group_)) {
                fmt::print(stderr, "{}: setgid({}) failed for \"{}\": {}\n",
                           __func__, group_, cmd_, strerror(errno));
                std::exit(EXIT_FAILURE);
            }
            if (getgid() == 0) {
                fmt::print(stderr, "{}: child is still gid=root after setgid()\n",
                           __func__);
                std::exit(EXIT_FAILURE);
            }
        }
        if (user_) {
            if (setresuid(user_, user_, user_)) {
                fmt::print(stderr, "{}: setuid({}) failed for \"{}\": {}\n",
                           __func__, user_, cmd_, strerror(errno));
                std::exit(EXIT_FAILURE);
            }
            if (getuid() == 0) {
                fmt::print(stderr, "{}: child is still uid=root after setuid()\n", __func__);
                std::exit(EXIT_FAILURE);
            }
            nk_fix_env(user_, true);
        }
        nk_execute(cmd_.c_str(), args.c_str());
    case -1:
        fmt::print(stderr, "{}: fork failed: {}\n", __func__, strerror(errno));
        std::exit(EXIT_FAILURE);
    default: break;
    }
}

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

void inotify::dispatch() {
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

        auto wmi = watches_.find(event->wd);
        if (wmi == watches_.end()) {
            fmt::print(stderr, "no job metadata for wd [{}]\n", event->wd);
            continue;
        }

        // XXX: Not used for now.
#if 0
        const bool is_dir(!!(event->mask & IN_ISDIR));
        const bool watch_del(!!(event->mask & IN_IGNORED));
        const bool fs_unmounted(!!(event->mask & IN_UNMOUNT));
        const bool buf_overflow(!!(event->mask & IN_Q_OVERFLOW));
#endif

        std::string args(wmi->second->args_);
        boost::algorithm::replace_all(args, "$%", mask_string(event->mask));
        boost::algorithm::replace_all(args, "$&", std::to_string(event->mask));
        boost::algorithm::replace_all(args, "$#", std::string(event->name));
        boost::algorithm::replace_all(args, "$@", wmi->second->filepath_);
        boost::algorithm::replace_all(args, "$$", "$"); // must be last

        fmt::print("Event[{}]: exec '{} {}'\n", wmi->second->filepath_,
                   wmi->second->cmd_, args);
        std::fflush(stdout);

        wmi->second->exec(args);
    }
}

signal_fd::signal_fd() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
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
            while (waitpid(-1, NULL, WNOHANG) > 0);
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
    printf("fcactus %s, inotify action daemon.\n", FCACTUS_VERSION);
    printf(
"Copyright (c) 2015-2016 Nicholas J. Kain\n"
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
"POSSIBILITY OF SUCH DAMAGE.\n"
           );
}

static void process_options(int ac, char *av[]) {
    namespace po = boost::program_options;
    po::options_description cli_opts("Command-line options");
    cli_opts.add_options()
        ("config,c", po::value<std::string>(),
         "path to configuration file")
        ("help,h", "print help message")
        ("version,v", "print version information")
        ;

    po::options_description cmdline_options;
    cmdline_options.add(cli_opts);
    po::options_description cfgfile_options;

    po::positional_options_description p;
    po::variables_map vm;
    try {
        po::store(po::command_line_parser(ac, av).
                  options(cmdline_options).positional(p).run(), vm);
    } catch (const std::exception& e) {
        fmt::print(stderr, "{}\n", e.what());
    }
    po::notify(vm);

    if (vm.count("config"))
        g_fcactus_conf = vm["config"].as<std::string>();
    if (vm.count("help")) {
        fmt::print("fcactus " FCACTUS_VERSION ", inotify action daemon.\n"
                   "Copyright (c) 2015-2016 Nicholas J. Kain\n"
                   "{} [options]...\n{}\n", av[0], cmdline_options);
        std::exit(EXIT_FAILURE);
    }
    if (vm.count("version")) {
        print_version();
        std::exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
    process_options(argc, argv);
    fail_on_fdne(g_fcactus_conf, "r");

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
    epollfd epfd;
    struct epoll_event events[2];

    epfd.add(sigfd.fd());
    epfd.add(inyfd.fd());

    while (1) {
        int nevents = epoll_wait(epfd.fd_, events, 2, -1);
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
            }
        }
    }
    exit(EXIT_SUCCESS);
}

