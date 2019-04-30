#include <events/listener.hh>
#include <events/recv.hh>
#include <events/register.hh>
#include <vhost/connection.hh>
#include "../main.hh"

namespace http
{
    ListenerEW::ListenerEW(shared_socket socket, TimeoutConfig toCon)
        : EventWatcher(socket->fd_get()->fd_, EV_READ)
        , sock_{socket}
        , toConfig_{toCon}
    {
        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);
        getsockname(socket->fd_get()->fd_, (struct sockaddr*)&my_addr, &len);
        this->port_ = ntohs(my_addr.sin_port);
    }

    void ListenerEW::operator()()
    {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        shared_socket sock = sock_->accept((struct sockaddr*)&addr, &addr_len);
        sock->ipv6_set(sock_->is_ipv6());
        std::cout << "Successfully connected with client.\n";
        http::dispatcher.dispatch_request(sock, toConfig_);
    }
} // namespace http