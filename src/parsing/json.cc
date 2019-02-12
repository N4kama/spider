#include "json.hh"

Vhost::Vhost(const std::string &ip, const int port,
             const std::string &server_name, const std::string &root,
             const std::string &default_file)
    : ip_(ip), port_(port), server_name_(server_name),
      root_(root), default_file_(default_file){};

void Vhost::print(void)
{
    std::cout << "IP:\t\t" << ip_ << "\n"
              << "PORT:\t\t" << port_ << "\n"
              << "SERVER NAME:\t" << server_name_ << "\n"
              << "ROOT: \t\t" << root_ << "\n"
              << "DEFAULT FILE:\t" << default_file_ << '\n';
}

nlohmann::json Config::getJson(const std::string &s)
{
    std::ifstream ifs(s);
    if (!ifs.is_open())
        return nullptr;
    nlohmann::json j;
    j << ifs;
    //j.parse(ifs);
    return j;
}

nlohmann::json Config::getVHosts(nlohmann::json j)
{
    try
    {
        auto &arr = j.at("vhosts");
        return arr;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Invalid json structure: no vhosts" << '\n';
    }
}

void Config::parse_json(nlohmann::json j)
{
    for (auto vhost = j.begin(); vhost != j.end(); vhost++)
    {
        nlohmann::json cur = *vhost;
        try
        {
            std::string def = cur.at("default_file").get<std::string>();
            try
            {
                Vhost v = Vhost(cur.at("ip").get<std::string>(),
                                cur.at("port").get<int>(),
                                cur.at("server_name").get<std::string>(),
                                cur.at("root").get<std::string>(), def);
                vhosts_.emplace_back(v);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Invalid json structure: invalid vhost" << '\n';
            }
        }
        catch (const std::exception &e)
        {
            try
            {
                Vhost v = Vhost(cur.at("ip").get<std::string>(),
                                cur.at("port").get<int>(),
                                cur.at("server_name").get<std::string>(),
                                cur.at("root").get<std::string>());
                vhosts_.emplace_back(v);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Invalid json structure: invalid vhost" << '\n';
            }
        }
    }
}