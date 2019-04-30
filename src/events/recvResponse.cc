#include <events/listener.hh>
#include <events/recvResponse.hh>
#include <events/register.hh>
#include <events/sendv.hh>
#include <iostream>
#include <string>
#include <vhost/connection.hh>

#include "main.hh"

namespace http
{
    static std::string getline(std::string& s)
    {
        size_t delim = s.find_first_of("\r\n") + 2;
        std::string res = s.substr(0, delim);
        s = s.substr(delim);
        return res;
    }

    struct serv_to_prox_Resp fill_serv_prox_Resp(shared_socket& sock_)
    {
        struct serv_to_prox_Resp rep;
        // get request line and headers in a string;
        std::string header = "";
        while (true)
        {
            try
            {
                const size_t buf_size = 256;
                char c[buf_size] = "";
                int nb = sock_->recv(&c, buf_size);
                if (nb > 0)
                {
                    header.append(c);
                    if (header.find("\r\n\r\n") != std::string::npos)
                        break;
                } else
                {
                    return rep;
                }
            } catch (int e)
            {
                continue;
            }
        }
        //// parse string into the struct
        // parsing requestLine
        rep.requestLine = getline(header);
        // parsing headers
        std::string line = getline(header);
        while (line.size() && line.at(0) != '\r')
        {
            // creating pairs of two string separated by a semicolon and a
            // space
            size_t sep_idx = line.find_first_of(':', 0);
            rep.headers.emplace(
                std::string(line.begin(), line.begin() + sep_idx),
                std::string(line.begin() + sep_idx + 2, line.end() - 2));

            line = getline(header);
        }
        if (header.length())
        {
            rep.msg_piece = header;
        }
        return rep;
    }

    RecvResponse::RecvResponse(shared_socket socket, Request r)
        : EventWatcher(socket->fd_get()->fd_, EV_READ)
        , sock_{socket}
        , r_{r}
    {
        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);
        getsockname(socket->fd_get()->fd_, (struct sockaddr*)&my_addr, &len);
        this->port_ = ntohs(my_addr.sin_port);
        header = std::string("");
        body = std::string("");
        filled = 0;
    }

    int RecvResponse::endby(std::string const& str, std::string const& end)
    {
        if (str.length() >= end.length())
        {
            return (
                0
                == str.compare(str.length() - end.length(), end.length(), end));
        } else
        {
            return 0;
        }
    }

    int RecvResponse::read_int(std::string s, size_t pos)
    {
        size_t pos_end = s.find_first_of(" \r\n", pos);
        std::string sub = s.substr(pos, pos_end);
        return std::atoi(sub.c_str());
    }

    void RecvResponse::operator()()
    {
        struct serv_to_prox_Resp rep = fill_serv_prox_Resp(sock_);

        // add/remove/modify headers according to config
        modify_headers(r_, rep);

        // send ~updated~ values from the rep struct:
        r_.clientSocket->send(rep.requestLine.c_str(),
                              rep.requestLine.length());
        for (auto tuple : rep.headers)
        {
            r_.clientSocket->send(tuple.first.c_str(), tuple.first.length());
            r_.clientSocket->send(": ", 2);
            r_.clientSocket->send(tuple.second.c_str(),
                                  tuple.second.length());
            r_.clientSocket->send("\r\n", 2);
        }
        r_.clientSocket->send("\r\n", 2);
        r_.clientSocket->send(rep.msg_piece.c_str(), rep.msg_piece.length());

        // send the rest of the server response to client through proxy
        while (true)
        {
            try
            {
                const size_t buf_size = 1024;
                char c[buf_size] = {0};
                int nb = sock_->recv(&c, buf_size);
                if (nb > 0)
                {
                    r_.clientSocket->send(c, buf_size);
                } else
                {
                    event_register.unregister_ew(this);
                    return;
                }
            } catch (int e)
            {
                continue;
            }
        }
    }
    void modify_headers(Request& r, struct serv_to_prox_Resp& res)
    {
        //delete
        for (auto it : r.config_ptr->proxy_remove_header_)
            res.headers.erase(it);
        //add
        for (auto it : r.config_ptr->proxy_set_header_)
            res.headers[it.first] = it.second;
    }
} // namespace http