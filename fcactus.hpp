#ifndef NJK_INOTIFY_CRON_
#define NJK_INOTIFY_CRON_

#include <memory>
#include <string>
#include <map>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/prctl.h>
#include <sys/inotify.h>
#include <time.h>
#include "rlimit.hpp"
#include <cstring>
extern "C" {
#include "nk/log.h"
}

struct watch_meta {
    watch_meta() : eventflags_(0), debounce_rise_ms_(0u), debounce_fall_ms_(0u),
                   user_(0), group_(0) {
        last_ran_ts_.tv_sec = 0;
        last_ran_ts_.tv_nsec = 0;
    }
    std::string cmd_;
    std::string args_;
    std::string filepath_;
    std::string chroot_;
    std::string path_;
    rlimits limits_;
    int eventflags_;
    unsigned debounce_rise_ms_;
    unsigned debounce_fall_ms_;
    uid_t user_;
    gid_t group_;
    struct timespec last_ran_ts_;
    struct inotify_event last_event_;

    void exec(const std::string &args);
    inline bool valid() const { return !cmd_.empty() && !filepath_.empty() && eventflags_; }
};

struct inotifyfd
{
    inotifyfd() {
        fd_ = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
        if (fd_ < 0)
            suicide("Failed to create inotify fd: %s", strerror(errno));
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
            suicide("Failed to create epoll fd: %s", strerror(errno));
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
    void dispatch_debounce_rise_event(int fd);
    void dispatch(const struct inotify_event *event);
    bool has_jobs() { return !!watches_.size(); }
    inotifyfd inotify_fd_;
    std::map<int, std::unique_ptr<watch_meta>> watches_;
    int fd() const { return inotify_fd_.fd_; }
private:
    void dispatch_do(std::map<int, std::unique_ptr<watch_meta>>::iterator wmi,
                     const struct inotify_event *event);
};

#endif

