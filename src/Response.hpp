#pragma once

#include "Value.hpp"

#include <string>

namespace lutop {

class Response {
    std::string state; // binary
    Value value; // binary
    std::string error; // text
};

}
