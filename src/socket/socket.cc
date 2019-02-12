#include "socket/socket.hh"
#include <iostream>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <cerrno>
#include <cstring>

namespace http
{
    Socket::Socket(const misc::shared_fd& fd)
        : fd_{fd}
    {}

    ssize_t Socket::recv(void* dst, size_t len) override
    {
        char *buf = new char[len];
        int fd = *fd_;
        return recv(fd, buf, len, 0);
    }

    ssize_t Socket::send(const void* src, size_t len)
    {
        int fd = *fd_;
        return send(fd, src, len, 0);
    }

    ssize_t Socket::sendfile(misc::shared_fd& in_fd, off_t& offset, size_t c)
    {
        int fd = *fd_;
        return sendfile(fd, in_fd, offset, c);
    }

    void Socket::bind(const sockaddr* addr, socklen_t addrlen)
    {
        if (!addr)
            return;

        int fd = *fd_;
        if (bind(fd, addr, addrlen) < 0)
            std::cerr << std::strerror(errno) << '\n';
    }

    void Socket::listen(int backlog)
    {
        int fd = *fd_;
        if (listen(fd, backlog) < 0)
            std::cerr << std::strerror(errno) << '\n';
    }

    void Socket::setsockopt(int level, int optname, int optval)
    {
        int fd = *fd_;
        char *opt_buf = new char[optval];
        if (setsockopt(fd, level, optname, opt_buf, optval) < 0)
            std::cerr << std::strerror(errno) << '\n';
    }

    std::shared_ptr<Socket> Socket::accept(sockaddr* addr,
                                               socklen_t* addrlen)
    {
        int fd = *fd_;

    }

    void Socket::connect(const sockaddr*, socklen_t)
    {

    }

    const misc::shared_fd Socket::fd_get() const noexcept
    {
        return fd_;
    }

    bool Socket::is_ipv6() const noexcept
    {
        return ipv6_;
    }

    void Socket::ipv6_set(bool ipv6) noexcept
    {
        ipv6_ = ipv6;
    }
} // namespace http
