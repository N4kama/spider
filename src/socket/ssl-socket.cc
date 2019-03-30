#include "socket/ssl-socket.hh"
#include "misc/openssl/ssl.hh"
namespace http
{
    SSLSocket::SSLSocket(int domain, int type, int protocol, SSL_CTX* ssl_ctx)
        : Socket{std::make_shared<misc::FileDescriptor>(
              sys::socket(domain, type, protocol))}
        , ssl_{SSL_new(ssl_ctx), &SSL_free}
    {
        set_non_block();
        setsockopt(SOL_SOCKET, SO_REUSEPORT, 1);
        setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
        SSL_set_fd(ssl_.get(), fd_->fd_);
        SSL_accept(ssl_.get());
    }

    SSLSocket::SSLSocket(const misc::shared_fd& fd, SSL_CTX* ssl_ctx)
        : Socket{fd}
        , ssl_{SSL_new(ssl_ctx), &SSL_free}
    {
        set_non_block();
        setsockopt(SOL_SOCKET, SO_REUSEPORT, 1);
        setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
        SSL_set_fd(ssl_.get(), fd_->fd_);
        SSL_accept(ssl_.get());
    }

    ssize_t SSLSocket::recv(void* dst, size_t len)
    {
        size_t res;
        while ((res = ssl::read(ssl_.get(), dst, len)) <= 0)
            ;
        return res;
    }

    ssize_t SSLSocket::send(const void* dst, size_t len)
    {
        size_t res;
        while ((res = ssl::write(ssl_.get(), dst, len)) <= 0)
            ;
        return res;
    }

    ssize_t SSLSocket::sendfile(misc::shared_fd& in_fd, off_t& offset, size_t c)
    {
        return sys::sendfile(*fd_, *in_fd, &offset, c);
    }

    void SSLSocket::bind(const sockaddr* addr, socklen_t addrlen)
    {
        sys::bind(*fd_, addr, addrlen);
    }

    void SSLSocket::listen(int backlog)
    {
        sys::listen(*fd_, backlog);
    }

    void SSLSocket::setsockopt(int level, int optname, int optval)
    {
        sys::setsockopt(*fd_, level, optname, &optval, sizeof(int));
    }

    shared_socket SSLSocket::accept(sockaddr* addr, socklen_t* addrlen)
    {
        return std::make_shared<SSLSocket>(
            std::make_shared<misc::FileDescriptor>(
                sys::accept(*fd_, addr, addrlen)), nullptr);
    }
    void SSLSocket::connect(const sockaddr* addr, socklen_t l)
    {
        /*
        addr++;
        int okk = l;
        okk++;
        ssl::connect(ssl_.get());
        */
        sys::connect(*fd_, addr, l);
    }

    int SSLSocket::set_non_block()
    {
        return sys::fcntl_wrapper(fd_->fd_, O_NONBLOCK);
    }
} // namespace http
