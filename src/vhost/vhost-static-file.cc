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
    conn.rep_ = std::make_shared<Response>(Response(req));
    event_register.register_ew<http::SendEv, http::shared_socket,
                        std::shared_ptr<Response>>(conn.sock_,
                        conn.rep_);
    conn.sock_->send(conn.rep_->rep.c_str(), conn.rep_->rep.length());
    // Call send_reponse veentwatcher with connexion struct
}
