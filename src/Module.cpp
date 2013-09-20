#include "Module.hpp"

namespace lutop {

Module::Module(std::string const &name)
    : name_(name)
    , status_(Status::Wait)
    , last_run_(0)
    , period_(0)
{

}

Module::~Module() throw() {

}

void
Module::start() {
    status_ = Status::Running;
    last_run_ = time(nullptr);
}

void
Module::stop() {
    status_ = Status::Wait;
}

void
Module::disable() {
    status_ = Status::Disabled;
    last_run_ = 0;
}

void
Module::enable() {
    if (status_ != Status::Disabled)
        return;
    status_ = Status::Wait;
}

void
Module::setPeriod(time_t period) {
    period_ = period;
}

}
