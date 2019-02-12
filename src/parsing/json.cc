#include "json.hh"

Vhost::Vhost(const std::string &ip, const int port,
             const std::string &server_name, const std::string &root,
             const std::string &default_file)
    : ip_(ip), port_(port), server_name_(server_name),
      root_(root), default_file_(default_file){};

void Vhost::print(void)
{
    std::cout << "IP:\t\t" << ip_ << "\n"
              << "PORT:\t\t " << port_ << "\n"
              << "SERVER NAME:\t" << server_name_ << "\n"
              << "ROOT: \t\t" << root_ << "\n"
              << "DEFAULT FILE:\t" << default_file_
              << '\n';
}
nlohmann::json Config::getJson(const std::string &s)
{
    std::ifstream ifs(s);
    if (!ifs.is_open())
        return nullptr;
    nlohmann::json j;
    j << ifs;
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
    std::cout << "size array = " << j.size() << "\n";
    for (auto vhost = j.begin(); vhost != j.end(); vhost++)
    {
        try
        {
            nlohmann::json cur = *vhost;
            Vhost v = Vhost(cur.at("ip").dump(), std::stoi(cur.at("port").dump()),
                            cur.at("server_name").dump(), cur.at("root").dump());
            vhosts_.emplace_back(v);
            v.print();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Invalid json structure: no vhosts" << '\n';
        }
    }
}