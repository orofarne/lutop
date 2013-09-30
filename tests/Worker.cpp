#include <gtest/gtest.h>

#include "Worker.hpp"

using namespace lutop;

TEST(Worker, Create) {
    Context c{};
    Worker w{c};
}
