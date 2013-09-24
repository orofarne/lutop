#pragma once

#include "Module.hpp"
#include "Context.hpp"
#include "ProcessPool.hpp"

#include <string>
#include <vector>
#include <map>

#include <boost/asio.hpp>

namespace lutop {

class Manager {
    public:
        Manager(unsigned int pool_size);
        ~Manager() throw();

        void loadFiles(std::vector<std::string> const &files);
        void run();

    private:
        Context c_;
        ProcessPool pp_;
        std::vector<Module> modules_;
        std::multimap<std::string, std::string> dependencies_;

        // Boost::Asio staff
        boost::asio::io_service io_service_;
        boost::asio::deadline_timer scheduler_timer_;

    private:
        void runIter(const boost::system::error_code& error);
        void prepareModules();
        time_t startSomething();
        void startModule(const Module &m);
        void moduleCallback(const Module &m, const Response &r, const boost::system::error_code &err);
};

}
