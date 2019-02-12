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

int Config::check_vhost(nlohmann::json j)
{
    auto ip_it = j.find("ip");
    auto port_it = j.find("port");
    auto serv_it = j.find("server_name");
    auto root_it = j.find("root");
    return (ip_it != j.end() && port_it != j.end() &&
            serv_it != j.end() && root_it != j.end());
}

void Config::parse_json(nlohmann::json j)
{
    for (auto vhost = j.begin(); vhost != j.end(); vhost++)
    {
        nlohmann::json cur = *vhost;
        auto it_ip = j.find("ip");
        std::string ip_s = cur.at("default_file").get<std::string>();
        int port_i = cur.at("default_file").get<int>();
        std::string serv_s = cur.at("default_file").get<std::string>();
        std::string root_s = cur.at("default_file").get<std::string>();
        std::string ef_s = cur.at("default_file").get<std::string>();
    }
}
