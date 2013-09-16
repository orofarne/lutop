#pragma once

#include <boost/asio/io_service.hpp>

namespace lutop {

class SubProcess {
    public:
        SubProcess(boost::asio::io_service &io_service);
        ~SubProcess() throw();

    private:
        boost::asio::io_service &io_service_;
};

}
