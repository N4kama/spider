#include "socket/socket.hh"

namespace http
{
    Socket::Socket(const misc::shared_fd& fd)
        : fd_{fd}
    {}

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
