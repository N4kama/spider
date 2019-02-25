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
        char buffer[4096];
        ssize_t read;

        read = sock_->recv(buffer, 4096);

        if (read < 0)
        {
            std::cerr << "read error";
            return;
        }
        if (read == 0)
        {
            event_register.unregister_ew(this);
            std::cerr << "peer might closing\n";
            return;
        } else
        {
            printf("message:%s\n", buffer);
        }

        // Send message bach to the client
        sock_->send("<http><h1>DBZ <<< NARUTO\n</h1>hehehe sex</http>", 48);
        bzero(buffer, read);
    }
} // namespace http