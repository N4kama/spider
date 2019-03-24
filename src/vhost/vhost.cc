#include "vhost.hh"

#include <iostream>
#include <memory>

namespace http
{
    VHost::VHost(const struct VHostConfig& c)
        : conf_{c}
        , ssl_ctx_{nullptr, SSL_CTX_free}

    {
        if (c.no_ssl == 0)
        {
            ssl_ctx_ = {SSL_CTX_new(SSLv23_server_method()), SSL_CTX_free};
            SSL_CTX_set_options(ssl_ctx_.get(), SSL_OP_SINGLE_DH_USE);
            SSL_CTX_use_certificate_file(ssl_ctx_.get(), c.ssl_cert_.c_str(),
                                                        SSL_FILETYPE_PEM);
            SSL_CTX_use_PrivateKey_file(ssl_ctx_.get(), c.ssl_key_.c_str(),
                                                        SSL_FILETYPE_PEM);
        }
        
    }
} // namespace http
