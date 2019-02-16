#include "socket/default-socket.hh"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstddef>

#include "misc/socket.hh"

namespace http
{
DefaultSocket::DefaultSocket(const misc::shared_fd& fd)
    : Socket(fd)
{
}

DefaultSocket::DefaultSocket(int domain, int type, int protocol)
    : Socket{std::make_shared<misc::FileDescriptor>(
          sys::socket(domain, type, protocol))}
{
}

void DefaultSocket::listen(int backlog)
{
    sys::listen(*fd_, backlog);
}

ssize_t DefaultSocket::recv(void *dst, size_t len)
{
    return ::recv(*fd_, dst, len, 0);
}

ssize_t DefaultSocket::send(const void *src, size_t len)
{
    return ::send(*fd_, src, len, 0);
}

ssize_t
DefaultSocket::sendfile(misc::shared_fd &in_fd, off_t &offset, size_t c)
{
    int in = *in_fd;
    return ::sendfile(*fd_, in, &offset, c);
}

void DefaultSocket::bind(const sockaddr *addr, socklen_t addrlen)
{
    if (!addr)
        return;

    int fd = *fd_;
    if (::bind(fd, addr, addrlen) < 0)
        std::cerr << std::strerror(errno) << '\n';
}

void DefaultSocket::setsockopt(int level, int optname, int optval)
{
    int fd = *fd_;
    char *opt_buf = new char[optval];
    if (::setsockopt(fd, level, optname, opt_buf, optval) < 0)
        std::cerr << std::strerror(errno) << '\n';
}

std::shared_ptr<Socket> DefaultSocket::accept(sockaddr *addr,
                                              socklen_t *addrlen)
{
    if (!addr)
        return nullptr;

    int fd = *fd_;
    if (::accept(fd, addr, addrlen) < 0)
        return nullptr;

    auto res = std::make_shared<DefaultSocket>(DefaultSocket(fd_));
    return res;
}

void DefaultSocket::connect(const sockaddr *addr, socklen_t l)
{
    int fd = *fd_;
    if (::connect(fd, addr, l) < 0)
        std::cerr << std::strerror(errno) << '\n';
}

} // namespace http
