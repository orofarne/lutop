#include <gtest/gtest.h>

#include "SubProcess.hpp"

#include <sys/types.h>
#include <sys/wait.h>

using namespace lutop;


TEST(SubProcess, FreeRun) {
    SubProcess sp{};
}

TEST(SubProcess, Fork) {
    SubProcess sp{};

    sp.fork();

    int status;
    pid_t pid = wait(&status);

    EXPECT_GT(pid, 0);
}

