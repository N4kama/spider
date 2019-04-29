#include "config/config.hh"
using json = nlohmann::json;

namespace http
{
    VHostConfig::VHostConfig(std::string& ip, int port,
                             std::string& server_name, std::string& root,
                             std::string& ssl_cert, std::string& ssl_key,
                             size_t header_max_size, size_t uri_max_size,
                             size_t payload_max_size, std::string& default_file
                             , bool auto_index)
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
        , auto_index_(auto_index)
    {
        if (ssl_cert == "" || ssl_key == "")
            no_ssl = 1;
    }

    TimeoutConfig::TimeoutConfig(double to_ka, double to_tran, double to_thr_val, double to_thr_time)
    : to_keep_alive_(to_ka)
    , to_transaction_(to_tran)
    , to_throughput_val_(to_thr_val)
    , to_throughput_time_(to_thr_time)
    {}  

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

    json get_timeouts(const std::string& s)
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
            auto& obj = j.at("timeout");
            return obj;
        } catch (const std::exception& e)
        {
            return nullptr;
        }
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
        /*
        if (j.find("root") == j.end() || !j.find("root")->is_string())
        {
            std::cerr << "Invalid json structure: missing root" << '\n';
            return 0;
        }
        */
        return 1;
    }

    struct TimeoutConfig set_TimeoutConfig(const std::string& str)
    {
        json j = get_timeouts(str.c_str());
        if (j != nullptr)
        {
            double to_keep_alive;
            try
            {
                to_keep_alive = j.at("keep_alive").get<double>();
            } catch (const std::exception& e)
            {
                to_keep_alive = -1;
            }

            double to_transaction;
            try
            {
                to_transaction = j.at("transaction").get<double>();
            } catch (const std::exception& e)
            {
                to_transaction = -1;
            }

            double to_throughput_val;
            try
            {
                to_throughput_val = j.at("throughput_val").get<double>();
            } catch (const std::exception& e)
            {
                to_throughput_val = -1;
            }

            double to_throughput_time;
            try
            {
                to_throughput_time = j.at("throughput_time").get<double>();
            } catch (const std::exception& e)
            {
                to_throughput_time = -1;
            }

            return TimeoutConfig(to_keep_alive, to_transaction, to_throughput_val, to_throughput_time);
        }
        return TimeoutConfig(-1, -1, -1, -1);
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

                std::string root_s;
                try
                {
                    root_s = cur.at("root").get<std::string>();
                }
                catch(const std::exception& e)
                {
                    root_s = "";
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

                if ((ssl_cert_i == "" && ssl_key_i != "")
                    || (ssl_cert_i != "" && ssl_key_i == ""))
                {
                    std::cerr << "Invalid json" << '\n';
                    throw std::exception();
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

                bool auto_index_i;
                try
                {
                    auto_index_i = cur.at("auto_index").get<bool>();
                }
                catch(const std::exception& e)
                {
                    auto_index_i = false;
                }

                VHostConfig v =
                    VHostConfig(ip_s, port_i, serv_s, root_s, ssl_cert_i,
                                ssl_key_i, header_max_size_i, uri_max_size_i,
                                payload_max_size_i, def_s, auto_index_i);
                c.vhosts_.emplace_back(v);
            } else
            {
                std::cerr << "Invalid json" << '\n';
                throw std::exception();
            }
        }

        c.timeoutConf_ = set_TimeoutConfig(path);

        if (c.vhosts_.size() == 0)
        {
            std::cerr << "Invalid json" << '\n';
            throw std::exception();
        }
        return c;
    }
} // namespace http
