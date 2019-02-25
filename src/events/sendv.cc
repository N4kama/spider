#include <events/recv.hh>
#include <events/register.hh>
#include <events/sendv.hh>
#include <vhost/connection.hh>

namespace http
{
    SendEv::SendEv(shared_socket socket, std::string msg, ssize_t count)
        : EventWatcher(socket->fd_get()->fd_, EV_WRITE)
        , sock_{socket}
    {
        this->msg_ = msg;
        this->count_ = count;
        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);
        getsockname(socket->fd_get()->fd_, (struct sockaddr*)&my_addr, &len);
        this->port_ = ntohs(my_addr.sin_port);
    }

    void SendEv::operator()()
    {
        if (!count_)
        {
            event_register.unregister_ew(this);
            return;
        }
        int size_left = count_;
        while (size_left)
        {
            ssize_t send_nb = sock_->send(msg_.c_str(), (size_left >= count_) ? count_ : size_left);
            if (send_nb == -1)
            {
                std::cerr << "Erreur lors du nb d'octets envoyé !\n";
            }
            size_left -= send_nb;
        }
        count_ = 0;
    }
} // namespace http