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
    event_register.register_ew<http::SendEv, http::shared_socket, Connection>(
        std::forward<shared_socket>(conn.sock_),
        std::forward<Connection>(conn));
}
