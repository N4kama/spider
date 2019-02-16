#include "main.hh"
/*
#include <boost/program_options.hpp>
namespace po = boost::program_options;

int parsing_json(std::string arg, int debug)
{
    try
    {
        http::ServerConfig s = http::parse_configuration(arg, debug);
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
        opt_desc.add_options()
            ("help,h", "Displays options")
            (",t", po::value<std::string>(),
            "Takes a json as argument and checks its validity")
            ("debug,d", "debug option, prints info")
        ;
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
            return parsing_json(vm["-t"].as<std::string>(), debug);
        }
        else
            return parsing_json(argv[1], debug);
    }
    catch(const std::exception& e)
    {
        return 1;
    }
}
*/

#include <request/request.hh>
#include <request/response.hh>
#include <misc/fd.hh>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main()
{
    char *localip;
    hostent *localHost;
    struct sockaddr_in adr;
    auto sock = http::DefaultSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    localHost = gethostbyname("hello.html");
    localip = inet_ntoa(*(struct in_addr *)*localHost->h_addr_list);

    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = inet_addr(localip);
    adr.sin_port = htons(1234);

    sock.bind((sockaddr *)&adr, sizeof(adr));

    auto r = http::Request();
    r.method = "get";
    r.status = 100;
    r.url = "hello.html";

    http_get(r, sock);
    return 1;
}