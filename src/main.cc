#include "main.hh"
#include <boost/program_options.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <request/response.hh>
#include <sys/epoll.h>
#include <misc/fd.hh>
#include "events/register.hh"
#include "events/listener.hh"
#include "socket/socket.hh"


namespace po = boost::program_options;
using namespace http;
EventWatcherRegistry event_register;

int dispatch(std::string arg, int debug)
{
    try
    {
        //Init one VHOST
        ServerConfig config = parse_configuration(arg, debug);
        VHostConfig vhost_config = config.vhosts_.at(0);
        //Add the new vhost to the VHOST list in the dispatcher
        dispatcher.add_vhost(vhost_config);

        //init socket
        DefaultSocket server_socket = DefaultSocket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;

        //init address
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(vhost_config.ip_.c_str());
        addr.sin_port = htons(vhost_config.port_);

        //bind socket with address
        struct sockaddr *addr_not_in = (struct sockaddr *)&addr;
        server_socket.bind(addr_not_in, sizeof(addr));
        server_socket.listen(30);
        //socklen_t *s_len = (socklen_t *)&len;

        //event handling
        //EventLoop e_loop = EventLoop();
        //std::shared_ptr<struct FileDescriptor> s_fd = std::make_shared<FileDescriptor>(server_socket.fd_get());
        //std::shared_ptr<DefaultSocket> s_server = std::make_shared<DefaultSocket>(s_fd);
        //ListenerEW e_server = ListenerEW(s_server);
        //e_loop.register_watcher(&e_server);

        //event loop
        //int res;
        //while (1)
        //{
        //    res = epoll_wait();
        //    for (int n = 0; n < res; n++)
        //    {
                /*
                if ()
                {
                    http::shared_socket new_s = server_socket.accept(addr_not_in, s_len);
                    http::DefaultSocket sock = http::DefaultSocket(new_s->fd_get());
                }
                else
                {
                }
                */
        //    }
        //}
        //DEBUG TESTING:
    }
    catch (const std::exception &e)
    {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    try
    {
        po::options_description opt_desc("Options");
        opt_desc.add_options()("help,h", "Displays options")(",t", po::value<std::string>(),
                                                             "Takes a json as argument and checks its validity")("debug,d", "debug option, prints info");
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
        }
        else if (vm.count("-t"))
        {
            return dispatch(vm["-t"].as<std::string>(), debug);
        }
        else
            return dispatch(argv[1], debug);
    }
    catch (const std::exception &e)
    {
        return 1;
    }
}