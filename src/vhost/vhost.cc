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
            SSL_CTX_set_ecdh_auto(ssl_ctx_, 1);
            SSL_CTX_set_tlsext_servername_callback(ssl_ctx_.get(), sni_cb);
            SSL_CTX_set_tlsext_servername_arg(ssl_ctx_.get(), this);
            if (SSL_CTX_use_certificate_file(
                    ssl_ctx_.get(), c.ssl_cert_.c_str(), SSL_FILETYPE_PEM)
                <= 0)
            {
                exit(EXIT_FAILURE);
            }

            if (SSL_CTX_use_PrivateKey_file(ssl_ctx_.get(), c.ssl_key_.c_str(),
                                            SSL_FILETYPE_PEM)
                <= 0)
            {
                exit(EXIT_FAILURE);
            }
            if (!SSL_CTX_check_private_key(ssl_ctx_.get()))
            {
                fprintf(stderr, "Public and private keys don't match\n");
            }
            //SSL_CTX_set_verify(ssl_ctx_.get(), SSL_VERIFY_NONE, 0);
        }
    }
    int sni_cb(SSL* s, int*, http::VHost* arg)
    {
        const char* servername =
            SSL_get_servername(s, TLSEXT_NAMETYPE_host_name);
        if (servername
            && strcmp(servername, arg->conf_get().server_name_.c_str()))
            SSL_set_SSL_CTX(s, arg->get_ctx());
        return SSL_TLSEXT_ERR_OK;
    }

} // namespace http

/*SSL_CTX_set_options(ssl_ctx_.get(), SSL_OP_SINGLE_DH_USE);
            SSL_CTX_use_certificate_file(ssl_ctx_.get(), c.ssl_cert_.c_str(),
                                                        SSL_FILETYPE_PEM);
            SSL_CTX_use_PrivateKey_file(ssl_ctx_.get(), c.ssl_key_.c_str(),
                                                        SSL_FILETYPE_PEM);
                                                         SSL_CTX_set_ecdh_auto(ctx,
            1);
*/
/* Set the key and cert */