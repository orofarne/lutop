#pragma once

#include <stdexcept>
#include <array>

#include <boost/asio/io_service.hpp>

namespace lutop {

class SubProcess {
    public:
        class subprocess_error : public std::runtime_error {
            public:
                virtual ~subprocess_error();
                explicit subprocess_error(const char *comment);
        };

    public:
        SubProcess(boost::asio::io_service &io_service);
        ~SubProcess() throw();

        void fork();

        inline int downFd() { return pipefd_down_[1]; }
        inline int upFd() { return pipefd_up_[0]; }

    private:
        std::array<int, 2> pipefd_down_;
        std::array<int, 2> pipefd_up_;
        int pid_;

        boost::asio::io_service &io_service_;

    private:
        void child();
};

}
