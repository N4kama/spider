#include "vhost-static-file.hh"

namespace http
{
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
        // En rapport avec connexion.hh qui doit etre coder...

        http::DefaultSocket sock = DefaultSocket(conn.sock_->fd_get());
        conn.sock_->~Socket();
        request_server(req, sock); // Il faut coder connexion si c'est pas deja
                                   // fait et cree l'attribut socket_
    }
} // namespace http