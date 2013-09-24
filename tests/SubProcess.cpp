#include <gtest/gtest.h>

#include "SubProcess.hpp"

#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

using namespace lutop;


TEST(SubProcess, FreeRun) {
    boost::asio::io_service io_service{};
    SubProcess sp{io_service};
}

TEST(SubProcess, Fork) {
    {
        boost::asio::io_service io_service{};
        SubProcess sp{io_service};

        sp.fork();

        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 100 * 1000000;
        ASSERT_EQ(0, nanosleep(&ts, nullptr));
    }

    int status;
    pid_t pid = wait(&status);

    EXPECT_GT(pid, 0);
}

