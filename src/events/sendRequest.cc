#include "sendRequest.hh"

#include <events/recv.hh>
#include <events/register.hh>
#include <fstream>
#include <misc/unistd.hh>
#include <netdb.h>
#include <socket/socket.hh>
#include <socket/ssl-socket.hh>
#include <sstream>
#include <events/recvResponse.hh>
#include <vhost/connection.hh>
namespace http
{
    SendRequest::SendRequest(const Request& r, shared_socket sock)
        : EventWatcher(sock->fd_get()->fd_, EV_WRITE)
    {
        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);
        getsockname(sock->fd_get()->fd_, (struct sockaddr*)&my_addr, &len);
        r_ = r;
        sock_ = sock;
        init_msg();
        count_ = msg_.size();
    }

    void SendRequest::init_msg()
    {
        msg_ = std::string(r_.method);
        msg_.append(" " + r_.uri + " " + r_.http_version + "\r\n");
        for (auto iter : r_.headers)
        {
            msg_.append(iter.first + ": " + iter.second + "\r\n");
        }
        msg_.append("\r\n" + r_.message_body + "\r\n");
    }

    void SendRequest::operator()()
    {
        std::string serverAddr = r_.headers.find("Host")->second;
        sockaddr_in sin;
        sin.sin_addr.s_addr = inet_addr(serverAddr.c_str());
        if (sin.sin_addr.s_addr == INADDR_NONE)
        {
            struct hostent* phost = gethostbyname(serverAddr.c_str());
            if ((phost) && (phost->h_addrtype == AF_INET))
                sin.sin_addr = *(in_addr*)(phost->h_addr);
        }
        sin.sin_family = AF_INET;
        sin.sin_port = htons(80);
        if(sock_->is_ssl() == 0 && connect(sock_->fd_get()->fd_,(sockaddr*)&sin, sizeof(sin)) < 0)
        {
            std::cout << "proxy can't connect to server (yet)";
        }
        if(sock_->is_ssl() == 1)
        {
            sock_->connect((sockaddr*)&sin, sizeof(sin));
        }

        // Sending
        int size_left = count_ - 2;
        while (size_left)
        {
            ssize_t send_nb = sock_->send(
                msg_.c_str(), (size_left >= count_) ? count_ : size_left);
            std::cout << "\n" << msg_ << "\n";
            if (send_nb == -1)
            {
                std::cerr << "sendrequest - Erreur lors du nb d'octets envoyé !\n";
            }
            size_left -= send_nb;
        }
        //receiving

        //recois sur sock_
        //envoi sur r_.clientSocket


        event_register.register_ew<http::RecvResponse, http::shared_socket,
                                     http::Request>(
                            std::forward<shared_socket>(sock_),
                            std::forward<Request>(r_));

        event_register.unregister_ew(this);
    }
} // namespace http