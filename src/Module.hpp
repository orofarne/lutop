#pragma once

#include <string>

namespace lutop {

class Module {
    public:
        enum class Status {
            Wait,
            Running,
            Disabled
        };

    public:
        explicit Module(std::string const &name);
        ~Module() throw();

        inline std::string const & name() { return name_; }
        inline Status status() { return status_; }
        inline time_t lastRun() { return last_run_; }
        inline time_t period() { return period_; }
        inline time_t nextRun() {return lastRun() + period(); }

        void start();
        void stop();
        void disable();
        void enable();
        void setPeriod(time_t period);

    public:
        std::string name_;
        Status status_;
        time_t last_run_;
        time_t period_;
};

}
