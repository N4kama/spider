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
        try
        {
            while(true)
            {

            if (filled != 0)
            {
                char c[4096];
                if (sock_->recv(&c, 4096) > 0)
                {
                    header + c;
                } else
                {
                    std::cerr << "Client Disconected\n";
                    event_register.unregister_ew(this);
                }
                if (filled == body.length())
                {
                    header += body;
                    r_.clientSocket->send(header.c_str(), header.size());
                    event_register.unregister_ew(this);
                }
                return;
            }
            if (filled == 0)
            {
                char c[4096];
                if (sock_->recv(&c, 4096) > 0)
                {
                    header + c;
                } else
                {
                    std::cerr << "Client Disconected\n";
                    event_register.unregister_ew(this);
                }
                if (endby(header, std::string("\r\n\r\n")))
                {
                    auto pos = header.find("Content-Length: ");
                    if (pos != std::string::npos)
                    {
                        pos += 16;
                        int value = read_int(header, pos);
                        std::cout << "atoi: " << value << "\n";
                        filled = value;
                    } else
                    {
                    r_.clientSocket->send(header.c_str(), header.size());
                        event_register.unregister_ew(this);
                    }
                }
            }

            }
        } catch (const std::exception& e)
        {
        }
    }
} // namespace http