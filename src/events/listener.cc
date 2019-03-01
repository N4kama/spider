#include <events/listener.hh>
#include <events/recv.hh>
#include <events/register.hh>
#include <vhost/connection.hh>

#include "callbacks.hh"

namespace http
{
    ListenerEW::ListenerEW(shared_socket socket)
        : EventWatcher(socket->fd_get()->fd_, EV_READ)
        , sock_{socket}
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
        std::shared_ptr<DefaultSocket> new_s =
            std::make_shared<http::DefaultSocket>(
                sock_->accept((struct sockaddr*)&addr, &addr_len)->fd_get());
        new_s->ipv6_set(addr.sin_family == AF_INET6);
        std::cout << "Successfully connected with client.\n";
        event_register.register_ew<http::RecvEv, http::shared_socket>(new_s);
    }
} // namespace http