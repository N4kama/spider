#include <events/listener.hh>
#include <events/recv.hh>
#include <events/register.hh>
#include <vhost/connection.hh>

namespace http
{
    RecvEv::RecvEv(shared_socket socket)
        : EventWatcher(socket->fd_get()->fd_, EV_READ)
        , sock_{socket}
    {
        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);
        getsockname(socket->fd_get()->fd_, (struct sockaddr*)&my_addr, &len);
        this->port_ = ntohs(my_addr.sin_port);
    }

    void RecvEv::operator()()
    {
        new_connexion(sock_);
        event_register.unregister_ew(this);
    }
} // namespace http