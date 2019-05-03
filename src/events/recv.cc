#include <events/listener.hh>
#include <events/recv.hh>
#include <events/register.hh>
#include <events/sendv.hh>
#include <vhost/connection.hh>

#include "main.hh"

namespace http
{
    RecvEv::RecvEv(shared_socket socket, shared_vhost vhost,
                   std::shared_ptr<TimerEW> timer)
        : EventWatcher(socket->fd_get()->fd_, EV_READ)
        , sock_{socket}
        , vhost_{vhost}
        , ka_timer_{timer}
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

    void RecvEv::sendit()
    {
        Request req = Request(header);
        req.clientSocket = sock_;
        req.config_ptr = std::make_shared<VHostConfig>(vhost_->get_conf());
        req.get_path();
        event_register.register_ew<http::SendEv, http::shared_socket,
                                   shared_vhost, std::shared_ptr<Response>>(
            std::forward<shared_socket>(sock_),
            std::forward<shared_vhost>(vhost_),
            std::make_shared<Response>(req));
        //ka_timer_->unregister_timer_watcher();
        event_register.unregister_ew(this);
    }

    void RecvEv::operator()()
    {
        if (sock_->killed())
        {
            event_register.unregister_ew(this);
            std::cout << "timeout!\n";
            return;
        }
        try
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
                }
                if (filled == body.length())
                {
                    header += body;
                    if(toConf.to_transaction_ >= 0)
                        transaction_timer_->unregister_timer_watcher();
                    //iciiiiiiiiiiiiiiiiiiiiii dernier bit
                    sendit();
                }
                return;
            }
            if (filled == 0)
            {
                char c[4096] = {0};
                if (sock_->recv(&c, 1) > 0)
                {
                    if (header.size() == 0)
                    {
                        if (toConf.to_keep_alive_ >= 0)
                            ka_timer_->unregister_timer_watcher();
                        if(toConf.to_transaction_ >= 0)
                            transaction_timer_ = std::make_shared<TimerEW>(
                                sock_, vhost_, event_register.loop_get().loop, 2);
                    }
                    if (sock_->is_ssl())
                        header += c;
                    else
                        header.append(1, *c);
                } else
                {
                    if (!sock_->is_ssl())
                    {
                        event_register.unregister_ew(this);
                    }
                    std::cerr << "Client Disconected\n";
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
                        sendit();
                    }
                }
            }
        } catch (const std::exception& e)
        {}
    }
} // namespace http
