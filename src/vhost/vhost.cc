#include "vhost.hh"

namespace http
{
    VHost::VHost(const struct VHostConfig& c)
        : conf_{c}
    {}
} // namespace http
