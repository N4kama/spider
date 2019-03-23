#include "vhost.hh"
#include <memory>
#include <iostream>

namespace http
{
    VHost::VHost(const struct VHostConfig& c)
        : conf_{c}
        , ssl_ctx_{nullptr, SSL_CTX_free}
    {
    }
} // namespace http

