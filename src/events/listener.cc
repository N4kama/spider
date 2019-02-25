#include <events/listener.hh>
#include <vhost/connection.hh>

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
        sock_->accept((struct sockaddr *)&addr, &addr_len);
        std::cout << "Successfully connected with client.\n";
        //new_connexion(sock_);
    }
} // namespace http