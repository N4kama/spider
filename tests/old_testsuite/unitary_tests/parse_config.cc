#include "main.hh"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(void)
{
    try
    {
        http::ServerConfig s = http::parse_configuration("tests/configs/test1.txt", 1);
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
