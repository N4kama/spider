/**
 * \file vhost/vhost-static-file.hh
 * \brief VHostStaticFile declaration.
 */

#pragma once
#include "events/register.hh"
#include "events/sendv.hh"
#include "vhost.hh"
//include respond

namespace http
{
    /**
     * \class VHostStaticFile
     * \brief VHost serving static files.
     */
    class VHostStaticFile : public VHost
    {
    public:
        friend class VHostFactory;
        virtual ~VHostStaticFile() = default;
        static VHostStaticFile get_vsf(const VHostConfig& ext_conf);
        explicit VHostStaticFile(const VHostConfig&);

    private:
        /**
         * \brief Constructor called by the factory.
         *
         * \param config VHostConfig virtual host configuration.
         */
        VHostConfig conf_;

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
        void respond(const Request&, struct Connection, remaining_iterator,
                     remaining_iterator) final;
    };
} // namespace http
