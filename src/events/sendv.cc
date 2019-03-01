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
        this->count_ = this->msg_.size();
        this->is_file_ = resp->is_file;
        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);
        getsockname(socket->fd_get()->fd_, (struct sockaddr*)&my_addr, &len);
        this->port_ = ntohs(my_addr.sin_port);
    }

    void SendEv::clean_send()
    {
        int size_left = count_ - 2;
        while (size_left)
        {
            ssize_t send_nb = sock_->send(
                msg_.c_str(), (size_left >= count_) ? count_ : size_left);
            std::cout << "\n" << msg_ << "\n";
            if (send_nb == -1)
            {
                std::cerr << "Erreur lors du nb d'octets envoyé !\n";
            }
            size_left -= send_nb;
        }
        count_ = 0;
    }

    void SendEv::operator()()
    {
        if (!count_)
        {
            //event_register.register_ew<http::RecvEv, http::shared_socket>(
            //std::make_shared<http::DefaultSocket>(sock_->fd_get()));
            event_register.unregister_ew(this);
            return;
        }
        if (!is_file_)
            clean_send();
        else
        {
            int fd = sys::open_wrapper(path_.c_str(), O_RDONLY);
            auto f = std::make_shared<misc::FileDescriptor>(
                misc::FileDescriptor(fd));
            off_t p = 0;
            if (fd == -1)
            {
                clean_send();
            } else
            {
                struct stat st;
                if (sys::fstat(fd, &st) == -1)
                    std::cerr << "fstat: fail\n";
                clean_send();
                sock_->sendfile(f, p, st.st_size);
            }
        }
    }
} // namespace http