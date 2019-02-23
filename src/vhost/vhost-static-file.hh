/**
 * \file vhost/vhost-static-file.hh
 * \brief VHostStaticFile declaration.
 */

#pragma once

#include "../config/config.hh"
#include "../request/request.hh"
#include "connection.hh"

namespace http
{
    /**
     * \class VHostStaticFile
     * \brief VHost serving static files.
     */
    using remaining_iterator = int; //SHOULD BE CHANGED BECAUSE IS FALSE !
    class VHostStaticFile
    {
    public:
        friend class VHostFactory;
        virtual ~VHostStaticFile() = default;
        static VHostStaticFile get_vsf(const VHostConfig& ext_conf);
    private:
        /**
         * \brief Constructor called by the factory.
         *
         * \paralllo darble t la ? ALLO am config VHostConfig virtual host configuration.
         * alor le darble
         */
        VHostConfig conf_;
        explicit VHostStaticFile(const VHostConfig& ext_conf);

    public:
        /**
         * \brief Send response.
         *
         * \param req Request.
         * \param conn Connection.
         * \param begin remaining_iterator of received data.
         * \param end remaining_iterator of received data.
         *
         * Note that these iterators will only be useful starting from SRPS.
         */
        void respond(const struct Request&, struct Connection, remaining_iterator,
                     remaining_iterator);
    };
    using shared_vhost = std::shared_ptr<VHostStaticFile>;

} // namespace http
