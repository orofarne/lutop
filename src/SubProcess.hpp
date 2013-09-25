#pragma once

#include <stdexcept>
#include <array>
#include <functional>

#include <boost/asio.hpp>

namespace lutop {

class SubProcess {
    public:
        class subprocess_error : public std::runtime_error {
            public:
                virtual ~subprocess_error();
                explicit subprocess_error(const char *comment);
        };

        typedef std::function<void *(void *, size_t, size_t *)> WorkerFunc;

    public:
        SubProcess(boost::asio::io_service &io_service, WorkerFunc f);
        ~SubProcess() throw();

        void fork();

        inline int downFd() { return pipefd_down_[1]; }
        inline int upFd() { return pipefd_up_[0]; }

    private:
        std::array<int, 2> pipefd_down_;
        std::array<int, 2> pipefd_up_;
        int pid_;

        WorkerFunc worker_f_;

        // Boost::asio staff
        boost::asio::io_service &io_service_;
        boost::asio::posix::stream_descriptor up_d_;
        boost::asio::posix::stream_descriptor down_d_;

    private:
        void waitData();
};

}
