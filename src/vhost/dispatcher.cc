#include "dispatcher.hh"

#include "../main.hh"
#include "vhost-factory.hh"

namespace http
{
    void Dispatcher::add_vhost(VHostConfig config)
    {
        vhosts_.push_back(VHostFactory::Create(config));
    }

    void get_port(Connection& cnx)
    {
        shared_socket s = cnx.sock_;
        struct sockaddr_in6 addr6;
        struct sockaddr_in addr4;
        socklen_t len = s->is_ipv6() ? sizeof(addr6) : sizeof(addr4);
        if (s->is_ipv6())
            getsockname(s->fd_get()->fd_, (struct sockaddr*)&addr6, &len);
        else
            getsockname(s->fd_get()->fd_, (struct sockaddr*)&addr4, &len);
        cnx.port = s->is_ipv6() ? ntohs(addr6.sin6_port) : ntohs(addr4.sin_port);
        char str[INET_ADDRSTRLEN];
        char str6[INET6_ADDRSTRLEN];
        cnx.host = s->is_ipv6()
            ? inet_ntop(AF_INET6, &addr6.sin6_addr, str6, INET6_ADDRSTRLEN)
            : inet_ntop(AF_INET, &addr4.sin_addr, str, INET_ADDRSTRLEN);
        ;
    }

    shared_vhost find_vhost(Connection& cnx)
    {
        shared_vhost s = dispatcher.get_vhost(0);
        for(size_t i = 1; i >= dispatcher.nb_of_vhost(); i++)
        {
            if(dispatcher.get_vhost(i)->get_conf().port_ == cnx.port &&
            dispatcher.get_vhost(i)->get_conf().ip_ == cnx.host)
                {
                    s = dispatcher.get_vhost(i);
                    break;
                }
        }
        return s;
    }

    int Dispatcher::dispatch_request(Connection& cnx)
    {
        get_port(cnx);
        shared_vhost v = find_vhost(cnx);
        cnx.req_.config_ptr = std::make_shared<VHostConfig>(v->get_conf());
        cnx.req_.get_path();
        v->respond(cnx.req_, cnx, 0, 0);
        return 0;
    }
} // namespace http
