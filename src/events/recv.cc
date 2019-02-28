#include <events/listener.hh>
#include <events/recv.hh>
#include <events/register.hh>
#include <events/sendv.hh>
#include <vhost/connection.hh>

#include "main.hh"

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
        header = std::string("");
        body = std::string("");
        filled = 0;
    }

    int endby(std::string const& str, std::string const& end)
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

    int read_int(std::string s, size_t pos)
    {
        size_t pos_end = s.find_first_of(" \r\n", pos);
        std::string sub = s.substr(pos, pos_end);
        return std::atoi(sub.c_str());
    }

    void RecvEv::operator()()
    {
        if (filled != 0)
        {
            char c = ' ';
            if (sock_->recv(&c, 1) > 0)
            {
                body.append(1, c);
            } else
            {
                std::cerr << "Client Disconected\n";
                event_register.unregister_ew(this);
            }
            if (filled == body.length())
            {
                header += body;
                Request req = Request(header);
                Connection cnx = Connection();
                cnx.req_ = req;
                cnx.sock_ = sock_;
                http::dispatcher.dispatch_request(cnx);
                event_register.unregister_ew(this);
            }
            return;
        }
        if (filled == 0)
        {
            char c = ' ';
            if (sock_->recv(&c, 1) > 0)
            {
                header.append(1, c);
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
                    Request req = Request(header);
                    Connection cnx = Connection();
                    cnx.req_ = req;
                    cnx.sock_ = sock_;
                    http::dispatcher.dispatch_request(cnx);
                    event_register.unregister_ew(this);
                }
            }
        }
    }
} // namespace http