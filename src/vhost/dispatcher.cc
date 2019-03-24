#include "dispatcher.hh"

#include <events/recv.hh>

#include "../main.hh"
#include "socket/ssl-socket.hh"

#include "vhost-factory.hh"

namespace http
{
    void Dispatcher::add_vhost(VHostConfig config)
    {
        vhosts_.push_back(VHostFactory::Create(config));
    }

    shared_vhost find_vhost(shared_socket s)
    {
        struct sockaddr_in6 addr6;
        struct sockaddr_in addr4;
        socklen_t len = s->is_ipv6() ? sizeof(addr6) : sizeof(addr4);
        if (s->is_ipv6())
            getsockname(s->fd_get()->fd_, (struct sockaddr*)&addr6, &len);
        else
            getsockname(s->fd_get()->fd_, (struct sockaddr*)&addr4, &len);
        uint16_t port =
            s->is_ipv6() ? ntohs(addr6.sin6_port) : ntohs(addr4.sin_port);
        char str[INET_ADDRSTRLEN];
        char str6[INET6_ADDRSTRLEN];
        std::string host = s->is_ipv6()
            ? inet_ntop(AF_INET6, &addr6.sin6_addr, str6, INET6_ADDRSTRLEN)
            : inet_ntop(AF_INET, &addr4.sin_addr, str, INET_ADDRSTRLEN);
        shared_vhost res = dispatcher.get_vhost(0);
        for (size_t i = 1; i < dispatcher.nb_of_vhost(); i++)
        {
            if (dispatcher.get_vhost(i)->get_conf().port_ == port
                && dispatcher.get_vhost(i)->get_conf().ip_ == host)
            {
                res = dispatcher.get_vhost(i);
                break;
            }
        }
        return res;
    }

    int Dispatcher::dispatch_request(shared_socket& s)
    {
        shared_vhost v = find_vhost(s);
        event_register.register_ew<http::RecvEv, shared_socket, shared_vhost>(
            std::forward<shared_socket>(s), std::forward<shared_vhost>(v));
        return 0;
    }
} // namespace http
