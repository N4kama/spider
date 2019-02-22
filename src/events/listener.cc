#include "events/listener.hh"
namespace http
{
    ListenerEW::ListenerEW(shared_socket socket)
        : sock_{socket}
    {
        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);
        getsockname(socket->fd_get()->fd_, (struct sockaddr *) &my_addr, &len);
        this->port_ = ntohs(my_addr.sin_port);
    }

    void ListenerEW::operator()()
    {
        new_connexion(sock_);
    }
}