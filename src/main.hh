#pragma once
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include "config/config.hh"
#include "events/callbacks.hh"
#include "events/events.hh"
#include "events/listener.hh"
#include "events/register.hh"
#include "request/request.hh"
#include "request/response.hh"
#include "socket/default-socket.hh"
#include "vhost/dispatcher.hh"

namespace http
{
    extern http::Dispatcher dispatcher;
    int start_server(std::string arg);
    void init_ssl();
    void init_vhost();
    void
    init_listeners(std::vector<std::shared_ptr<http::ListenerEW>>& listeners,
                   int i);
    void end_loop(std::vector<std::shared_ptr<http::ListenerEW>>& listeners);

    void child_cb(EV_P_ ev_child* ec, int revents);
    void watch_child(shared_socket sock, pid_t pid,
                     std::vector<std::shared_ptr<http::ListenerEW>>& listeners);

    void
    unwatch_childs(shared_socket sock,
                   std::vector<std::shared_ptr<http::ListenerEW>>& listeners);

    pid_t
    create_child(shared_socket sock,
                 std::vector<std::shared_ptr<http::ListenerEW>>& listeners);

} // namespace http