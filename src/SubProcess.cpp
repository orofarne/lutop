#include "SubProcess.hpp"

#include <string>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace lutop {

SubProcess::subprocess_error::~subprocess_error() {
}

SubProcess::subprocess_error::subprocess_error(const char *comment)
    : runtime_error(std::string("Subprocess error [")
            + comment + " ] : " + strerror(errno)) // Not thread-safe!!!
{
}

SubProcess::SubProcess()
    : pid_(0)
{
    if(pipe(pipefd_down_.data()) < 0)
        throw subprocess_error("pipe");
    if(pipe(pipefd_up_.data()) < 0)
        throw subprocess_error("pipe");
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

    if(cpid == 0) {
        // Child process
        close(pipefd_down_[1]);
        close(pipefd_up_[0]);

        // TODO
    } else {
        // Parent process
        pid_ = cpid;
        close(pipefd_down_[0]);
        close(pipefd_up_[1]);
    }
}

}
