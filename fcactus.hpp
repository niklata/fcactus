#ifndef NJK_INOTIFY_CRON_
#define NJK_INOTIFY_CRON_

#include <memory>
#include <string>
#include <map>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/prctl.h>
#include <sys/inotify.h>
#include <format.hpp>
#include "rlimit.hpp"

struct watch_meta {
    watch_meta() : eventflags_(0), user_(0), group_(0) {}
    std::string cmd_;
    std::string args_;
    std::string filepath_;
    std::string chroot_;
    std::string path_;
    rlimits limits_;
    int eventflags_;
    uid_t user_;
    gid_t group_;

    void exec(const std::string &args);
    inline bool valid() const { return !cmd_.empty() && !filepath_.empty() && eventflags_; }
};

struct inotifyfd
{
    inotifyfd() {
        fd_ = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
        if (fd_ < 0)
            throw std::runtime_error(fmt::format("Failed to create inotify fd: {}", fd_));
    }
    ~inotifyfd() { close(fd_); }
    inotifyfd(const inotifyfd &) = delete;
    inotifyfd(inotifyfd &&) = delete;
    inotifyfd& operator=(const inotifyfd &) = delete;
    inotifyfd& operator=(inotifyfd &&) = delete;
    int fd_;
};

struct epollfd
{
    epollfd() {
        fd_ = epoll_create1(EPOLL_CLOEXEC);
        if (fd_ < 0)
            throw std::runtime_error(fmt::format("Failed to create epoll fd: {}", fd_));
    }
    ~epollfd() { close(fd_); }
    epollfd(const epollfd &) = delete;
    epollfd(epollfd &&) = delete;
    epollfd& operator=(const epollfd &) = delete;
    epollfd& operator=(epollfd &&) = delete;
    inline void add(int fd) { manip(fd, EPOLL_CTL_ADD); }
    inline void del(int fd) { manip(fd, EPOLL_CTL_DEL); }
    int fd_;
private:
    void manip(int fd, int mtype);
};

struct inotify
{
    std::pair<bool, int> add(std::unique_ptr<watch_meta> &&meta);
    void dispatch();
    bool has_jobs() { return !!watches_.size(); }
    inotifyfd inotify_fd_;
    std::map<int, std::unique_ptr<watch_meta>> watches_;
    int fd() const { return inotify_fd_.fd_; }
};

struct signal_fd
{
    signal_fd();
    ~signal_fd() { close(fd_); }
    signal_fd(const signal_fd &) = delete;
    signal_fd(signal_fd &&) = delete;
    signal_fd& operator=(const signal_fd &) = delete;
    signal_fd& operator=(signal_fd &&) = delete;
    void dispatch(void);
    int fd() const { return fd_; }
    int fd_;
};

#endif

