#include <gtest/gtest.h>

#include "SubProcess.hpp"

#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include <algorithm>
#include <iostream>

using namespace lutop;

void *echo_f(void *in_buf, size_t in_size, size_t *out_size) {
#if 0
    std::cout << '[' << in_size << "]:   ";
    std::for_each(
            reinterpret_cast<char *>(in_buf),
            reinterpret_cast<char *>(in_buf) + in_size,
            [](char ch) { std::cout << " " << ch; }
        );
    std::cout << std::endl;
#endif

    *out_size = in_size;
    void *res = ::malloc(in_size);
    return memcpy(res, in_buf, in_size);
}

TEST(SubProcess, FreeRun) {
    boost::asio::io_service io_service{};
    SubProcess sp{io_service, &echo_f};
}

TEST(SubProcess, Fork) {
    {
        boost::asio::io_service io_service{};
        SubProcess sp{io_service, echo_f};

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

TEST(SubProcess, Echo) {
    {
        boost::asio::io_service io_service{};
        SubProcess sp{io_service, echo_f};

        sp.fork();

        errno = 0;

        char b[] = "\xa5Hello";
        EXPECT_EQ(6, write(sp.downFd(), b, 6));
        ASSERT_EQ(0, errno);

        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 100 * 1000000;
        ASSERT_EQ(0, nanosleep(&ts, nullptr));

        char r[6];
        EXPECT_EQ(6, read(sp.upFd(), r, 6));
        ASSERT_EQ(0, errno);

        EXPECT_EQ(b[0], r[0]); EXPECT_EQ(b[1], r[1]); EXPECT_EQ(b[2], r[2]);
        EXPECT_EQ(b[3], r[3]); EXPECT_EQ(b[4], r[4]); EXPECT_EQ(b[5], r[5]);
    }

    int status;
    pid_t pid = wait(&status);

    EXPECT_GT(pid, 0);
}

TEST(SubProcess, Echo10) {
    {
        boost::asio::io_service io_service{};
        SubProcess sp{io_service, echo_f};

        sp.fork();

        errno = 0;

        for(int i = 0; i < 10; ++i) {
            char b[] = "\xa6Hello_";
            b[6] = '0' + i;
            EXPECT_EQ(7, write(sp.downFd(), b, 7));
            ASSERT_EQ(0, errno);

            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 100 * 1000000;
            ASSERT_EQ(0, nanosleep(&ts, nullptr));

            char r[7];
            EXPECT_EQ(7, read(sp.upFd(), r, 7));
            ASSERT_EQ(0, errno);

            EXPECT_EQ(b[0], r[0]); EXPECT_EQ(b[1], r[1]); EXPECT_EQ(b[2], r[2]);
            EXPECT_EQ(b[3], r[3]); EXPECT_EQ(b[4], r[4]); EXPECT_EQ(b[5], r[5]);
            EXPECT_EQ(b[6], r[6]);
        }
    }

    int status;
    pid_t pid = wait(&status);

    EXPECT_GT(pid, 0);
}
