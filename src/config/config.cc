#include "config/config.hh"
using json = nlohmann::json;

namespace http
{
    VHostConfig::VHostConfig(std::string& ip, int port,
                             std::string& server_name, std::string& root,
                             std::string& ssl_cert, std::string& ssl_key,
                             size_t header_max_size, size_t uri_max_size,
                             size_t payload_max_size, std::string& default_file)
        : ip_(ip)
        , port_(port)
        , server_name_(server_name)
        , root_(root)
        , ssl_cert_(ssl_cert)
        , ssl_key_(ssl_key)
        , header_max_size_(header_max_size)
        , uri_max_size_(uri_max_size)
        , payload_max_size_(payload_max_size)
        , default_file_(default_file)
    {
        if (ssl_cert == "" || ssl_key == "")
            no_ssl = 1;
    }

    void VHostConfig::print_VHostConfig(void)
    {
        std::cout << "IP:\t\t" << ip_ << "\n"
                  << "PORT:\t\t" << port_ << "\n"
                  << "SERVER NAME:\t" << server_name_ << "\n"
                  << "ROOT: \t\t" << root_ << "\n"
                  << "DEFAULT FILE:\t" << default_file_ << '\n'
                  << "HEADER MAX SIZE:\t" << header_max_size_ << '\n';
    }

    json get_vhosts(const std::string& s)
    {
        json j;
        try
        {
            std::ifstream ifs(s);
            if (!ifs.is_open())
                return nullptr;
            j = json::parse(ifs);
        } catch (const std::exception& e)
        {
            std::cerr << "bad json\n";
        }
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

    struct ServerConfig parse_configuration(const std::string& path)
    {
        ServerConfig c = ServerConfig();
        json j = get_vhosts(path.c_str());
        for (auto vhost = j.begin(); vhost != j.end(); vhost++)
        {
            json cur = *vhost;
            if (check_vhost(cur))
            {
                std::string ip_s = cur.at("ip").get<std::string>();
                if (ip_s == "")
                {
                    std::cerr << "ip is empty\n";
                    throw std::exception();
                }
                int port_i = cur.at("port").get<int>();
                std::string serv_s = cur.at("server_name").get<std::string>();
                if (serv_s == "")
                {
                    std::cerr << "server name is empty\n";
                    throw std::exception();
                }
                std::string root_s = cur.at("root").get<std::string>();
                if (root_s == "")
                {
                    std::cerr << "root is empty\n";
                    throw std::exception();
                }

                size_t header_max_size_i;
                try
                {
                    header_max_size_i = cur.at("header_max_size").get<size_t>();
                } catch (const std::exception& e)
                {
                    header_max_size_i = 0;
                }
                size_t uri_max_size_i;
                try
                {
                    uri_max_size_i = cur.at("uri_max_size").get<size_t>();
                } catch (const std::exception& e)
                {
                    uri_max_size_i = 0;
                }
                std::string ssl_cert_i;
                try
                {
                    ssl_cert_i = cur.at("ssl_cert").get<std::string>();
                } catch (const std::exception& e)
                {
                    ssl_cert_i = "";
                }
                std::string ssl_key_i;
                try
                {
                    ssl_key_i = cur.at("ssl_key").get<std::string>();
                } catch (const std::exception& e)
                {
                    ssl_key_i = "";
                }
                size_t payload_max_size_i;
                try
                {
                    payload_max_size_i =
                        cur.at("payload_max_size").get<size_t>();
                } catch (const std::exception& e)
                {
                    payload_max_size_i = 0;
                }

                std::string def_s;
                try
                {
                    def_s = cur.at("default_file").get<std::string>();
                    if (def_s == "")
                    {
                        def_s = std::string("index.html");
                    }
                } catch (const std::exception& e)
                {
                    def_s = std::string("index.html");
                }

                VHostConfig v =
                    VHostConfig(ip_s, port_i, serv_s, root_s, ssl_cert_i,
                                ssl_key_i, header_max_size_i, uri_max_size_i,
                                payload_max_size_i, def_s);
                c.vhosts_.emplace_back(v);
            } else
            {
                std::cerr << "Invalid json" << '\n';
                throw std::exception();
            }
        }
        if (c.vhosts_.size() == 0)
        {
            std::cerr << "Invalid json" << '\n';
            throw std::exception();
        }
        return c;
    }
} // namespace http
