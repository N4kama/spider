#include <events/recv.hh>
#include <events/register.hh>
#include <events/sendv.hh>
#include <fstream>
#include <misc/unistd.hh>
#include <sstream>
#include <vhost/connection.hh>

namespace http
{
    SendEv::SendEv(shared_socket socket, std::shared_ptr<Response> resp)
        : EventWatcher(socket->fd_get()->fd_, EV_WRITE)
        , sock_{socket}
    {
        this->msg_ = resp->rep;
        this->path_ = resp->file_p;
        this->count_ = resp->rep.size();
        this->is_file_ = resp->is_file;
        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);
        getsockname(socket->fd_get()->fd_, (struct sockaddr*)&my_addr, &len);
        this->port_ = ntohs(my_addr.sin_port);
    }

    void SendEv::operator()()
    {
        if (!count_)
        {
            event_register.unregister_ew(this);
            return;
        }
        if (!is_file_)
        {
            int size_left = count_;
            while (size_left)
            {
                ssize_t send_nb = sock_->send(
                    msg_.c_str(), (size_left >= count_) ? count_ : size_left);
                if (send_nb == -1)
                {
                    std::cerr << "Erreur lors du nb d'octets envoyé !\n";
                }
                size_left -= send_nb;
            }
            count_ = 0;
        } else
        {
            int fd = sys::open_wrapper(path_.c_str(), O_RDONLY);
            auto f = std::make_shared<misc::FileDescriptor>(
                misc::FileDescriptor(fd));
            off_t p = 0;
            if (fd == -1)
            {
                std::stringstream str;
                auto err = statusCode(INTERNAL_SERVER_ERROR);
                str << "<html><h1>http error: " << err.first << "</h1><h2> "
                    << err.second << "</h2></html>";
                sock_->send(str.str().c_str(), str.str().size());
            } else
            {
                struct stat st;
                if (sys::fstat(fd, &st) == -1)
                {
                    std::cerr << "fstat: fail\n";
                }
                sock_->send(msg_.c_str(), msg_.size());
                sock_->sendfile(f, p, st.st_size);
            }
        }
    }
} // namespace http