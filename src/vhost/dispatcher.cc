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
            if (cnx.sock_->is_ipv6())
            {
                struct sockaddr_in6 my_addr;
                socklen_t len = sizeof(my_addr);
                getsockname(cnx.sock_->fd_get()->fd_,
                            (struct sockaddr_in6*)&my_addr, &len);
                char str[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &my_addr.sin6_addr, str, INET6_ADDRSTRLEN);
            } else
            {
                struct sockaddr_in my_addr;
                socklen_t len = sizeof(my_addr);
                getsockname(cnx.sock_->fd_get()->fd_,
                            (struct sockaddr*)&my_addr, &len);
                uint16_t port = ntohs(my_addr.sin_port);
                char str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &my_addr.sin_addr, str, INET_ADDRSTRLEN);
            }
            // search to whom vhost the request is destined
            auto vhost = (http::dispatcher.vhosts_[0]);
            cnx.req_.config_ptr =
                std::make_shared<VHostConfig>(vhost->get_conf());
            cnx.req_.get_path(); // initialise path_config (now that config_ptr
                                 // is created)
            vhost->respond(cnx.req_, cnx, 0, 0);
        } catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        return 0;
    }
} // namespace http