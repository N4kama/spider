#include "dispatcher.hh"

#include "../main.hh"
#include "vhost-factory.hh"

namespace http
{
    void Dispatcher::add_vhost(VHostConfig config)
    {
        vhosts_.push_back(VHostFactory::Create(config));
    }

    int Dispatcher::dispatch_request(Connection& cnx)
    {
        // Now we consider there is only one vhost
        try
        {
            uint16_t port = 0;
            std::string host;
            struct in_addr addr;
            int domain = cnx.sock_->is_ipv6() ? AF_INET6 : AF_INET;
            if (cnx.sock_->is_ipv6())
            {
                struct sockaddr_in6 my_addr;
                socklen_t len = sizeof(my_addr);
                getsockname(cnx.sock_->fd_get()->fd_,
                            (struct sockaddr*)&my_addr, &len);
                port = ntohs(my_addr.sin6_port);
                char str[INET6_ADDRSTRLEN];
                host = inet_ntop(AF_INET6, &my_addr.sin6_addr, str,
                                 INET6_ADDRSTRLEN);
            } else
            {
                struct sockaddr_in my_addr;
                socklen_t len = sizeof(my_addr);
                getsockname(cnx.sock_->fd_get()->fd_,
                            (struct sockaddr*)&my_addr, &len);
                port = ntohs(my_addr.sin_port);
                char str[INET_ADDRSTRLEN];
                host =
                    inet_ntop(AF_INET, &my_addr.sin_addr, str, INET_ADDRSTRLEN);
            }
            for (unsigned i = 0; i < http::dispatcher.vhosts_.size(); i++)
            {
                auto vhost = (http::dispatcher.vhosts_[i]);
                inet_pton(domain, vhost->get_conf().ip_.c_str(), &addr);
                char temp_host[INET6_ADDRSTRLEN];
                inet_ntop(domain, &addr, temp_host, sizeof(temp_host));
                if ((temp_host == host) && port == vhost->get_conf().port_)
                {
                    cnx.req_.config_ptr =
                        std::make_shared<VHostConfig>(vhost->get_conf());
                    cnx.req_.get_path();
                    vhost->respond(cnx.req_, cnx, 0, 0);
                    break;
                }
            }
        } catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        return 0;
    }
} // namespace http