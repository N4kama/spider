#include "vhost-static-file.hh"

using namespace http;

VHostStaticFile VHostStaticFile::get_vsf(const VHostConfig& ext_conf)
{
    return VHostStaticFile(ext_conf);
}

VHostStaticFile::VHostStaticFile(const VHostConfig& ext_conf)
    : VHost(ext_conf)
{}

void VHostStaticFile::respond(const Request& req, Connection conn,
                              remaining_iterator, remaining_iterator)
{
    std::shared_ptr<DefaultSocket> new_s =
        std::make_shared<DefaultSocket>(conn.sock_->fd_get());
    new_s->set_vhost(conn.sock_->get_vhost());
    event_register.register_ew<http::SendEv, http::shared_socket,
                               std::shared_ptr<Response>>(
        new_s, std::make_shared<Response>(req));
}
