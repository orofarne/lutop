#pragma once

#include "Context.hpp"

#include <string>
#include <vector>

namespace lutop {

class Manager {
    public:
        Manager();
        ~Manager() throw();

        void loadFiles(std::vector<std::string> const &files);
        void run();

    private:
        Context c_;
        std::vector<std::string> modules_;
};

}
