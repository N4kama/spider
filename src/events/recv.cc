#include <events/listener.hh>
#include <events/recv.hh>
#include <events/register.hh>
#include <events/sendv.hh>
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
        size_t pos_end = s.find_first_of(" \t\n", pos + 1);
        return std::atoi(s.substr(pos, pos_end).c_str());
    }

    void RecvEv::operator()()
    {
        // new_connexion(sock_);
        // req_ = fill_Request(*sock_);

        if (filled != 0)
        {
            char c = ' ';
            if (sock_->recv(&c, 1) > 0)
            {
                body.append(1, c);
            }
            if (endby(body, std::string("\r\n")))
            {
                if (body.length() != filled)
                {
                    std::cerr << "content length is not respected\n";
                    return;
                    // throw
                }
                std::string res = header + body;
                std::shared_ptr<Request> req = std::make_shared<Request>(res);
                Connection cnx =
                    Connection(sock_, req, std::make_shared<Response>(req));
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
            }
            if (endby(header, std::string("\r\n\r\n")))
            {
                auto pos = header.find("Content-Length: ");
                if (pos != std::string::npos)
                {
                    pos += 17;
                    int value = read_int(header, pos);
                    filled = value;
                } else
                {
                    Response rep_ = Response(fill_Request(header));
                    std::shared_ptr<http::SendEv> s =
                        event_register
                            .register_ew<http::SendEv, http::shared_socket,
                                         std::shared_ptr<Response>>(
                                std::make_shared<http::DefaultSocket>(
                                    sock_->fd_get()),
                                std::make_shared<http::Response>(rep_));
                    event_register.unregister_ew(this);
                }
                return;
            }
        } else
        {
            return;
        }
    }
} // namespace http