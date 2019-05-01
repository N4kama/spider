
#include "main.hh"

#include <misc/addrinfo/addrinfo.hh>
#include <vector>

#include "events/timer.hh"
#include "socket/ssl-socket.hh"

namespace http
{
    // Dispatcher is routing the requests to the corresponding vhost
    http::Dispatcher dispatcher = http::Dispatcher();

    void init_ssl()
    {
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        SSL_library_init();
    }

    void init_vhost()
    {
        // fills the dispatcher with the vhosts
        for (unsigned i = 0; i < event_register.config().vhosts_.size(); i++)
        {
            dispatcher.add_vhost(event_register.config().vhosts_.at(i));
        }
    }

    pid_t
    create_child(shared_socket sock,
                 std::vector<std::shared_ptr<http::ListenerEW>>& listeners)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            std::cout << "new child:" << getpid() << "\n";

            ev_loop_fork(event_register.loop_get().loop);

            unwatch_childs(sock, listeners);
            std::shared_ptr<ListenerEW> listener =
                event_register.register_ew<ListenerEW, shared_socket>(
                    std::forward<shared_socket>(sock));
            listeners.emplace_back(listener);

            ev_run(listener->loop(), 0);
        } else if (pid > 0)
        {
            std::cout << "created child for "
                      << "\n";
            watch_child(sock, pid, listeners);
        } else
        {
            std::cout << "failed to create child for "
                      << "\n";
        }

        return pid;
    }

    void watch_child(shared_socket sock, pid_t pid,
                     std::vector<std::shared_ptr<http::ListenerEW>>& listeners)
    {
        for (std::shared_ptr<http::ListenerEW> l : listeners)
        {
            if (l->socket()->fd_get()->fd_ == sock->fd_get()->fd_)
            {
                // ev_child_stop(l->loop().get(), l->child().get());
                ev_child_init(l->child().get(), child_cb, pid, 0);
                l->child().get()->data = reinterpret_cast<void*>(&listeners);
                ev_child_start(l->loop(), l->child().get());
                break;
            }
        }
    }

    void
    unwatch_childs(shared_socket sock,
                   std::vector<std::shared_ptr<http::ListenerEW>>& listeners)
    {
        for (std::shared_ptr<http::ListenerEW> l : listeners)
        {
            if (l->socket()->fd_get()->fd_ != sock->fd_get()->fd_)
            {
                ev_child_stop(l->loop(), l->child().get());
            }
        }
    }

    void child_cb(EV_P_ ev_child* ec, int)
    {
        if (getpid() != event_register.g_master_pid())
            return;

        std::vector<std::shared_ptr<http::ListenerEW>>* listeners =
            reinterpret_cast<std::vector<std::shared_ptr<http::ListenerEW>>*>(
                ec->data);
        std::shared_ptr<http::ListenerEW> cur;
        for (std::shared_ptr<http::ListenerEW> l : *listeners)
        {
            if (l->child().get()->rpid == ec->rpid)
            {
                cur = l;
                break;
            }
        }

        std::cout << "child process " << ec->rpid << " exited with status "
                  << ec->rstatus << "\n",

            ev_child_stop(EV_A_ ec);

        pid_t pid = create_child(cur->socket(), *listeners);
        if (pid > 0)
        {
            std::cout << "respawed new child " << pid << "\n";
        } else
        {
            std::cout << "failed to recreate child: " << pid << "\n";
        }
    }

    void
    init_listeners(std::vector<std::shared_ptr<http::ListenerEW>>& listeners,
                   int i)
    {
        addrinfo info = {};
        info.ai_family = PF_UNSPEC;
        info.ai_socktype = SOCK_STREAM;
        info.ai_protocol = IPPROTO_TCP;
        info.ai_flags = AI_PASSIVE;
        addrinfo* result;
        getaddrinfo(
            event_register.config().vhosts_.at(i).ip_.c_str(),
            std::to_string(event_register.config().vhosts_.at(i).port_).c_str(),
            &info, &result);
        for (addrinfo* addr = result; addr != NULL; addr = addr->ai_next)
        {
            http::DefaultSocket server_socket = http::DefaultSocket(
                addr->ai_family, addr->ai_socktype, addr->ai_protocol);
            std::shared_ptr<DefaultSocket> sock =
                std::make_shared<DefaultSocket>(server_socket.fd_get());
            sock->bind(addr->ai_addr, addr->ai_addrlen);
            sock->listen(3000);
            sock->ipv6_set(server_socket.is_ipv6());
            create_child(sock, listeners);
        }
        freeaddrinfo(result);
    }

    void end_loop(std::vector<std::shared_ptr<http::ListenerEW>>& listeners)
    {
        for (std::shared_ptr<http::ListenerEW> l : listeners)
        {
            ev_child_stop(l->loop(), l->child().get());
        }
        for (unsigned i = 0; i < listeners.size(); i++)
        {
            event_register.unregister_ew(listeners.at(i).get());
        }
        ERR_free_strings();
        EVP_cleanup();
    }
    int start_server(std::string arg)
    {
        try
        {
            init_ssl();
            event_register.set_pid(getpid());
            http::ServerConfig config = http::parse_configuration(arg);
            event_register.set_config(config);
            init_vhost();

            // Check the integrity of the vhosts stored in dispatcher
            if (dispatcher.check_integrity())
                return 1;

            std::vector<std::shared_ptr<http::ListenerEW>> listeners =
                std::vector<std::shared_ptr<http::ListenerEW>>();

            for (unsigned i = 0; i < config.vhosts_.size(); i++)
            {
                init_listeners(listeners, i);
            }

            // run loop
            http::EventLoop event_loop = event_register.loop_get();
            event_loop();

            end_loop(listeners);

        } catch (const std::exception& e)
        {
            std::cerr << e.what() << "\n";
            return 1;
        }
        return 0;
    }

} // namespace http

int main(int argc, char* argv[])
{
    try
    {
        if (argc == 2)
            return http::start_server(argv[1]);
        else if (argc == 3)
        {
            if (argv[1][0] == '-' && argv[1][1] == 't')
            {
                try
                {
                    http::parse_configuration(argv[2]);
                    return 0;
                } catch (const std::exception& e)
                {
                    return 1;
                }
            }
            return 1;
        }
        return 1;
    } catch (const std::exception& e)
    {
        return 1;
    }
}