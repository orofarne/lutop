#include "SubProcess.hpp"

#include "msglen.h"

#include <string>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BLOCK_SIZE 1024

namespace lutop {

SubProcess::subprocess_error::~subprocess_error() {
}

SubProcess::subprocess_error::subprocess_error(const char *comment)
    : runtime_error(std::string("Subprocess error [")
            + comment + " ] : " + strerror(errno)) // Not thread-safe!!!
{
}

SubProcess::SubProcess(boost::asio::io_service &io_service, WorkerFunc f)
    : pid_(0)
    , worker_f_(f)
    , io_service_(io_service)
    , up_d_(io_service_)
    , down_d_(io_service_)
{
    if(pipe(pipefd_down_.data()) < 0)
        throw subprocess_error("pipe");
    if(fcntl(pipefd_down_[1], F_SETFL, O_NONBLOCK) < 0)
        throw subprocess_error("fcntl");

    if(pipe(pipefd_up_.data()) < 0)
        throw subprocess_error("pipe");
    if(fcntl(pipefd_up_[0], F_SETFL, O_NONBLOCK) < 0)
        throw subprocess_error("fcntl");
}

SubProcess::~SubProcess() throw() {
    for(int fd : pipefd_down_) {
        close(fd);
    }
    for(int fd : pipefd_up_) {
        close(fd);
    }

    if(pid_ > 0) {
        if(kill(pid_, SIGKILL) < 0 && errno != ESRCH) {
            // log it?...
        }

        // Not good place for waitpid :-(
    }
}

void
SubProcess::fork() {
    pid_t cpid = ::fork();

    if(cpid < 0)
        throw subprocess_error("fork");

    io_service_.notify_fork(boost::asio::io_service::fork_prepare);
    if(cpid == 0) {
        // Child process
        io_service_.notify_fork(boost::asio::io_service::fork_child);

        close(pipefd_down_[1]);
        close(pipefd_up_[0]);

        up_d_.assign(pipefd_up_[1]);
        down_d_.assign(pipefd_down_[0]);

        waitData();
    } else {
        // Parent process
        io_service_.notify_fork(boost::asio::io_service::fork_parent);

        pid_ = cpid;

        close(pipefd_down_[0]);
        close(pipefd_up_[1]);
    }
}

void
SubProcess::waitData() {
    size_t len = BLOCK_SIZE;
    size_t offset = 0;
    std::shared_ptr<char> p{
        reinterpret_cast<char *>(::malloc(len)),
        [](char *ptr) { if(ptr) ::free(ptr); }
    };
    if(p.get() == nullptr)
        throw std::runtime_error("no memory");

    for(;;) {
        size_t n = down_d_.read_some(boost::asio::buffer(p.get() + offset, len - offset));

        char *error = nullptr;
        size_t msg_len = ::msgpackclen_buf_read(p.get(), n, &error);
        if(error) {
            std::ostringstream msg;
            msg << "Error [msgpackclen_buf_read]: " << error;
            ::free(error);
            throw std::runtime_error{msg.str()};
        }

        offset += n;

        if(msg_len == 0) {
            if(n == len - offset) {
                len += BLOCK_SIZE;
                char *ptr = reinterpret_cast<char *>(::realloc(p.get(), len));
                p.reset(ptr);
                if(p.get() == nullptr)
                    throw std::runtime_error("no memory");
            }
        }
        else {
            size_t out_size = 0;
            void *out_ptr = worker_f_(
                    reinterpret_cast<void *>(p.get()), msg_len, &out_size
                    );
            std::shared_ptr<void> out_p{
                out_ptr,
                [](void *ptr) { if(ptr) ::free(ptr); }
            };

            // send answer
            size_t m = 0;
            while(m < out_size)
                m += up_d_.write_some(boost::asio::buffer(out_ptr, out_size));

            // move
            ::memmove(p.get(), p.get() + msg_len, offset - msg_len);
            offset = 0;
        }
    }
}

}
