#include "SubProcess.hpp"

#include <string>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

namespace lutop {

SubProcess::subprocess_error::~subprocess_error() {
}

SubProcess::subprocess_error::subprocess_error(const char *comment)
    : runtime_error(std::string("Subprocess error [")
            + comment + " ] : " + strerror(errno)) // Not thread-safe!!!
{
}

SubProcess::SubProcess(boost::asio::io_service &io_service)
    : pid_(0)
    , io_service_(io_service)
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

        child();
    } else {
        // Parent process
        io_service_.notify_fork(boost::asio::io_service::fork_parent);

        pid_ = cpid;

        close(pipefd_down_[0]);
        close(pipefd_up_[1]);
    }
}

void
SubProcess::child() {
    io_service_.run();
}

}
