#include "config/config.hh"
using json = nlohmann::json;

namespace http
{
    VHostConfig::VHostConfig(const std::string& ip, const int port,
                             const std::string& server_name,
                             const std::string& root,
                             const std::string& default_file)
        : ip_(ip)
        , port_(port)
        , server_name_(server_name)
        , root_(root)
        , default_file_(default_file)
    {}

    void VHostConfig::print_VHostConfig(void)
    {
        std::cout << "IP:\t\t" << ip_ << "\n"
                  << "PORT:\t\t" << port_ << "\n"
                  << "SERVER NAME:\t" << server_name_ << "\n"
                  << "ROOT: \t\t" << root_ << "\n"
                  << "DEFAULT FILE:\t" << default_file_ << '\n';
    }

    json get_vhosts(const std::string& s)
    {
        std::ifstream ifs(s);
        if (!ifs.is_open())
            return nullptr;
        json j = json::parse(ifs);
        try
        {
            auto& arr = j.at("vhosts");
            return arr;
        } catch (const std::exception& e)
        {
            std::cerr << "Invalid json structure: no vhosts" << '\n';
        }
        return nullptr;
    }

    int check_vhost(json j)
    {
        if (j.find("ip") == j.end() || !j.find("ip")->is_string())
        {
            std::cerr << "Invalid json structure: missing ip" << '\n';
            return 0;
        }
        if (j.find("port") == j.end() || !j.find("port")->is_number())
        {
            std::cerr << "Invalid json structure: missing port" << '\n';
            return 0;
        }
        if (j.find("server_name") == j.end()
            || !j.find("server_name")->is_string())
        {
            std::cerr << "Invalid json structure: missing server_name" << '\n';
            return 0;
        }
        if (j.find("root") == j.end() || !j.find("root")->is_string())
        {
            std::cerr << "Invalid json structure: missing root" << '\n';
            return 0;
        }
        return 1;
    }

    struct ServerConfig parse_configuration(const std::string& path, int debug)
    {
        ServerConfig c = ServerConfig();
        json j = get_vhosts(path.c_str());
        for (auto vhost = j.begin(); vhost != j.end(); vhost++)
        {
            json cur = *vhost;
            if (check_vhost(cur))
            {
                std::string ip_s = cur.at("ip").get<std::string>();
                int port_i = cur.at("port").get<int>();
                std::string serv_s = cur.at("server_name").get<std::string>();
                std::string root_s = cur.at("root").get<std::string>();
                try
                {
                    std::string def_s =
                        cur.at("default_file").get<std::string>();
                    VHostConfig v =
                        VHostConfig(ip_s, port_i, serv_s, root_s, def_s);
                    c.vhosts_.emplace_back(v);
                    if (debug)
                    {
                        v.print_VHostConfig();
                    }
                } catch (const std::exception& e1)
                {
                    try
                    {
                        VHostConfig v =
                            VHostConfig(ip_s, port_i, serv_s, root_s);
                        c.vhosts_.emplace_back(v);
                        if (debug)
                        {
                            v.print_VHostConfig();
                        }
                    } catch (const std::exception& e2)
                    {
                        std::cerr << "Invalid json" << '\n';
                    }
                }
            } else
            {
                throw std::exception();
            }
        }
        return c;
    }
} // namespace http
