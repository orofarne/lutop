#pragma once

#include "Context.hpp"
#include "SubProcess.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include <functional>

#include <boost/asio/io_service.hpp>

namespace lutop {

class ProcessPool {
    public:
        typedef std::function<void(const Response &, const boost::system::error_code &)> RequestCallback;

    public:
        ProcessPool(boost::asio::io_service &io_service, Context &cx, unsigned int size);
        ~ProcessPool() throw();

        void process(const Request &, RequestCallback);

    private:
        unsigned int pool_size_;

        boost::asio::io_service &io_service_;
        Context c_;
};

}
