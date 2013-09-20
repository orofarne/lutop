#pragma once

#include "Module.hpp"
#include "Context.hpp"

#include <string>
#include <vector>

#include <boost/asio.hpp>

namespace lutop {

class Manager {
    public:
        Manager();
        ~Manager() throw();

        void loadFiles(std::vector<std::string> const &files);
        void run();

    private:
        Context c_;
        std::vector<Module> modules_;

        // Boost::Asio staff
        boost::asio::io_service io_service_;
        boost::asio::steady_timer scheduler_timer_;

    private:
        void runIter(const boost::system::error_code& error);
        void prepareModules();
        time_t startSomething();
};

}
