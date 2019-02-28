#include "main.hh"

#include <vector>

namespace po = boost::program_options;

namespace http
{
    /* Dispatcher is routing the requests to the corresponding vhost */
    http::Dispatcher dispatcher = http::Dispatcher();

    int start_server(std::string arg, int debug)
    {
        try
        {
            /* Init one VHOST */
            http::ServerConfig config = http::parse_configuration(arg, debug);

            for (unsigned i = 0; i < config.vhosts_.size(); i++)
            {
                dispatcher.add_vhost(config.vhosts_.at(i));
            }

            for (unsigned i = 0; i < config.vhosts_.size(); i++)
            {
                /* Init addrinfo */
                addrinfo info = {};
                info.ai_family = PF_UNSPEC;
                info.ai_socktype = SOCK_STREAM;
                info.ai_protocol = IPPROTO_TCP;
                info.ai_flags = AI_PASSIVE;
                addrinfo* result;

                getaddrinfo(config.vhosts_.at(i).ip_.c_str(),
                            std::to_string(config.vhosts_.at(i).port_).c_str(),
                            &info, &result);
                for (addrinfo* addr = result; addr != NULL; addr = addr->ai_next)
                {
                    /* Init socket */
                    http::DefaultSocket server_socket = http::DefaultSocket(
                        addr->ai_family, addr->ai_socktype, addr->ai_protocol);
                    /* Bind socket with address */
                    server_socket.bind(addr->ai_addr, addr->ai_addrlen);
                    if (-1 == server_socket.set_non_block())
                    {
                        std::cerr << "can't set the socket to non blocking\n";
                    }

                    server_socket.listen(30);

                    // socklen_t* s_len = (socklen_t*)&len;
                    std::shared_ptr<http::ListenerEW> listener =
                        event_register
                            .register_ew<http::ListenerEW, http::shared_socket>(
                                std::make_shared<http::DefaultSocket>(
                                    server_socket.fd_get()));
                }
                freeaddrinfo(result);
            }

            http::EventLoop event_loop = event_register.loop_get();
            ev_signal signal_watcher;
            event_loop.register_sigint_watcher(&signal_watcher);
            event_loop();

        } catch (const std::exception& e)
        {
            return 1;
        }
        return 0;
    }

} // namespace http

int main(int argc, char* argv[])
{
    try
    {
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
            return http::start_server(vm["-t"].as<std::string>(), debug);
        } else
            return http::start_server(argv[1], debug);
    } catch (const std::exception& e)
    {
        return 1;
    }
}