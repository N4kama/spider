<<<<<<< HEAD
#include <iostream>

=======
#include "main.hh"

namespace po = boost::program_options;

int dispatch(std::string arg, int debug)
{
    try
    {
        // Dispatcher is routing the requests to the corresponding vhost
        http::Dispatcher pat = http::Dispatcher();

        // Init one VHOST
        http::ServerConfig config = http::parse_configuration(arg, debug);
        http::VHostConfig vhost = config.vhosts_.at(0);

        // adding the vhost(s) to the list
        pat.add_vhost(vhost);

        /* Init socket */
        http::DefaultSocket server_socket =
            http::DefaultSocket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        int len = sizeof(addr);

        /* Init address */
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(vhost.ip_.c_str());
        addr.sin_port = htons(vhost.port_);

        /* Experimental event */
        // permet de faire pls action en meme temps
        // struct ev_loop *loop = ev_default_loop(EVBACKEND_EPOLL);
        // struct ev_loop *loop = ev_default_loop(0);

        /* Bind socket with address */
        struct sockaddr* addr_not_in = (struct sockaddr*)&addr;
        server_socket.bind(addr_not_in, len);
        if (-1 == server_socket.set_non_block())
        {
            std::cerr << "aieaieouille\n";
        }

        server_socket.listen(30);
        // socklen_t* s_len = (socklen_t*)&len;
        event_register.register_ew<http::ListenerEW, http::shared_socket>(
            std::make_shared<http::DefaultSocket>(server_socket.fd_get()));
        http::EventLoop event_loop = event_register.loop_get();
        while (1)
        {
            event_loop();
            // ev_run(evloop->loop, 0);
        }
        /*
        while (1)
        {
            // while eventloop
            http::shared_socket new_s = server_socket.accept(addr_not_in,
        s_len); http::ListenerEW l_EW(new_s); // Create a ListenerEW from a
        listener socket. if (false) // Each time we receive a new connection
            {
                l_EW(); // Start accepting connections on listener socket.
                std::shared_ptr<http::EventWatcher> ew =
        event_register.register_ew(1);// Register an eventwatcher to handle the
        connection if (false) // Once event is finished
                {
                    event_register.unregister_ew(ew.get());
                }
            }
            // http::fill_Request(new_s);
            // TO COMPLETE
        }
        // DEBUG TESTING:
        */
    } catch (const std::exception& e)
    {
        return 1;
    }
    return 0;
}
>>>>>>> [EVENTS] test event in main

int main(int argc, char* argv[])
{
    try
    {
<<<<<<< HEAD
        argc = argc;
        argv = argv;
        return 0;
    }
    catch (const std::exception &e)
=======
        po::options_description opt_desc("Options");
        opt_desc.add_options()("help,h", "Displays options")(
            ",t", po::value<std::string>(),
            "Takes a json as argument and checks its validity")(
            "debug,d", "debug option, prints info");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, opt_desc), vm);
        po::notify(vm);

        int debug = 0;
        if (vm.count("debug"))
            debug = 1;
        if (vm.count("help"))
        {
            std::cout << opt_desc << "\n";
            return 0;
        } else if (vm.count("-t"))
        {
            return dispatch(vm["-t"].as<std::string>(), debug);
        } else
            return dispatch(argv[1], debug);
    } catch (const std::exception& e)
>>>>>>> [EVENTS] test event in main
    {
        return 1;
    }
}