#include "vhost-static-file.hh"

using namespace http;

VHostStaticFile VHostStaticFile::get_vsf(const VHostConfig& ext_conf)
{
    return VHostStaticFile(ext_conf);
}

VHostStaticFile::VHostStaticFile(const VHostConfig& ext_conf)
    : VHost(ext_conf)
{}

void VHostStaticFile::respond(const Request& , Connection ,
                              remaining_iterator, remaining_iterator)
{
}
