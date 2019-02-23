#include "vhost-factory.hh"
using namespace http;

shared_vhost VHostFactory::Create(VHostConfig vc)
{
    return std::make_shared<VHostStaticFile>(vc);
}