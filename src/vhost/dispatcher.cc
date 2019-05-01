#include "dispatcher.hh"

#include <events/recv.hh>
#include <events/timer.hh>

#include "../main.hh"
#include "socket/ssl-socket.hh"
#include "vhost-factory.hh"
#include "config/config.hh"

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
                /*if (ssl)
                {
                    SSL_CTX_set_tlsext_servername_callback(dispatcher.get_vhost(i)->get_ctx(),
                                                           sni_cb);
                    SSL_CTX_set_tlsext_servername_arg(dispatcher.get_vhost(i)->get_ctx(),
                    static_cast<void*>(&dispatcher.get_vhost(i)->get_conf().server_name_)
                    );
                }*/
                res = dispatcher.get_vhost(i);
                break;
            }
        }
        return res;
    }

    int Dispatcher::dispatch_request(shared_socket& s)
    {
        shared_vhost v = find_vhost(s);
        std::shared_ptr<EventWatcher> ev;
        if (v->conf_get().no_ssl == 0)
        {
            s = std::make_shared<SSLSocket>(s->fd_get(), v->get_ctx());
        }
        int st = 0;
        if(toConf.to_keep_alive_ >= 0)
            st = 1;
        std::shared_ptr<TimerEW> timer = std::make_shared<TimerEW>(
            s, v, event_register.loop_get().loop, st);
        ev = event_register
                 .register_ew<http::RecvEv, shared_socket, shared_vhost>(
                     std::forward<shared_socket>(s),
                     std::forward<shared_vhost>(v),
                     std::forward<std::shared_ptr<TimerEW>>(timer));
        return 0;
    }

    /*
    **  Check if the vhost are all on different ip addreses
    **  and if one of the vhost is on 0.0.0.0 or ::
    **  it is then an error so the function returns 1
    */
    int Dispatcher::check_integrity()
    {
        for (size_t i = 0; i < vhosts_.size() - 1; i++)
        {
            VHostConfig conf1 = vhosts_[i]->conf_get();
            for (size_t j = i + 1; j < vhosts_.size(); j++)
            {
                VHostConfig conf2 = vhosts_[j]->conf_get();
                if (conf1.port_ == conf2.port_)
                    return 1;
            }
        }
        return 0;
    }

} // namespace http
