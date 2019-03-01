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
    event_register.register_ew<http::SendEv, http::shared_socket,
                               std::shared_ptr<Response>>(
        std::make_shared<DefaultSocket>(conn.sock_->fd_get()),
        std::make_shared<Response>(req));
}
