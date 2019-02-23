/**
 * \file vhost/vhost-factory.hh
 * \brief VHostFactory
 */
#pragma once
#include "../config/config.hh"
#include "error/not-implemented.hh"
#include "../request/request.hh"
#include "connection.hh"
#include "vhost-static-file.hh"

namespace http
{
/**
     * \class VHostFactory
     * \brief Factory design pattern to create VHost.
     */
class VHostFactory
{
  public:
    /**
         * \brief Create a VHost object from a given VHostConfig.
         */
    static shared_vhost Create(VHostConfig);
};
} // namespace http
