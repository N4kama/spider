#include <events/recv.hh>
#include <events/register.hh>
#include <events/sendv.hh>
#include <vhost/connection.hh>

namespace http
{
    SendEv::SendEv(shared_socket socket, std::shared_ptr<Response> resp)
        : EventWatcher(socket->fd_get()->fd_, EV_WRITE)
        , sock_{socket}
    {
        this->msg_ = resp->rep;
        this->count_ = resp->rep.size();
        this->is_file_ = resp->is_file;
        struct sockaddr_in my_addr;
        socklen_t len = sizeof(my_addr);
        getsockname(socket->fd_get()->fd_, (struct sockaddr*)&my_addr, &len);
        this->port_ = ntohs(my_addr.sin_port);
    }

    void SendEv::operator()()
    {
        sock_->send(msg_.c_str(), msg_.length());
        event_register.unregister_ew(this);
        return;
    }
} // namespace http