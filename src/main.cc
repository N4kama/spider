#include <iostream>
#include <iterator>

#include "parsing/json.hh"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

int parsing_json(std::string arg)
{
    try
    {
        Config c = Config();
        nlohmann::json j = c.get_json(arg);
        nlohmann::json j2 = c.get_vhosts(j);
        c.parse_json(j2);
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
        ;
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, opt_desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << opt_desc << "\n";
            return 0;
        }
        else if (vm.count("-t"))
        {
            return parsing_json(vm["-t"].as<std::string>());
        }
        else
            return parsing_json(argv[1]);
    }
    catch(const std::exception& e)
    {
        return 1;
    }
}
