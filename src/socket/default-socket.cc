#include "socket/default-socket.hh"

// rayou includes
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <iostream>
// end of rayou includes

#include "misc/socket.hh"

namespace http
{
    DefaultSocket::DefaultSocket(const misc::shared_fd& fd)
        : Socket{fd}
    {}

    DefaultSocket::DefaultSocket(int domain, int type, int protocol)
        : Socket{std::make_shared<misc::FileDescriptor>(
              sys::socket(domain, type, protocol))}
    {}

    void DefaultSocket::listen(int backlog)
    {
        sys::listen(*fd_, backlog);
    }

    ssize_t DefaultSocket::recv(void* dst, size_t len)
    {
        return sys::recv(*fd_, dst, len, 0);
    }

    ssize_t DefaultSocket::send(const void* src, size_t len)
    {
        return sys::send(*fd_, src, len, 0);
    }

    ssize_t DefaultSocket::sendfile(misc::shared_fd& in_fd, off_t& offset,
                                    size_t c)
    {
        return sys::sendfile(*fd_, *in_fd, &offset, c);
    }

    void DefaultSocket::bind(const sockaddr* addr, socklen_t addrlen)
    {
        sys::bind(*fd_, addr, addrlen);
    }

    void DefaultSocket::setsockopt(int level, int optname, int optval)
    {
        char* opt_buf = new char[optval];
        sys::setsockopt(*fd_, level, optname, opt_buf, optval);
    }

    std::shared_ptr<Socket> DefaultSocket::accept(sockaddr* addr,
                                                  socklen_t* addrlen)
    {
        return std::make_shared<DefaultSocket>(
            std::make_shared<misc::FileDescriptor>(
                sys::accept(*fd_, addr, addrlen)));
    }

    void DefaultSocket::connect(const sockaddr* addr, socklen_t l)
    {
        sys::connect(*fd_, addr, l);
    }

    int DefaultSocket::set_non_block()
    {
        return sys::fcntl_wrapper(fd_->fd_, O_NONBLOCK);
    }
} // namespace http