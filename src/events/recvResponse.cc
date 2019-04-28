#include <events/listener.hh>
#include <events/recvResponse.hh>
#include <events/register.hh>
#include <events/sendv.hh>
#include <vhost/connection.hh>

#include "main.hh"

namespace http
{
    RecvResponse::RecvResponse(shared_socket socket, Request r)
        : EventWatcher(socket->fd_get()->fd_, EV_READ)
        , sock_{socket}
        , r_{r}
    {
        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);
        getsockname(socket->fd_get()->fd_, (struct sockaddr*)&my_addr, &len);
        this->port_ = ntohs(my_addr.sin_port);
        header = std::string("");
        body = std::string("");
        filled = 0;
    }

    int RecvResponse::endby(std::string const& str, std::string const& end)
    {
        if (str.length() >= end.length())
        {
            return (
                0
                == str.compare(str.length() - end.length(), end.length(), end));
        } else
        {
            return 0;
        }
    }

    int RecvResponse::read_int(std::string s, size_t pos)
    {
        size_t pos_end = s.find_first_of(" \r\n", pos);
        std::string sub = s.substr(pos, pos_end);
        return std::atoi(sub.c_str());
    }

    void RecvResponse::operator()()
    {
        while (true)
        {
            try
            {
                const size_t buf_size = 1024;
                char c[buf_size] = {0};
                int nb = sock_->recv(&c, buf_size);
                if (nb > 0)
                {
                    r_.clientSocket->send(c, buf_size);
                } else
                {
                    event_register.unregister_ew(this);
                    return;
                }
            } catch (int e)
            {
                continue;
            }
        }
    } // namespace http
} // namespace http