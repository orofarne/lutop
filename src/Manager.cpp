#include "Manager.hpp"

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <sstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

// Warning! Not thread-safe!
#define THROW_ERRNO(comment) { \
        std::ostringstream msg; \
        msg << "Error " << errno << " " << comment << ": " << strerror(errno); \
        throw std::runtime_error(msg.str()); \
    }

namespace lutop {

Manager::Manager()
    : scheduler_timer_(io_service_)
{

}

Manager::~Manager() throw() {

}

void
Manager::loadFiles(std::vector<std::string> const &files) {
    namespace fs = boost::filesystem;

    for(std::string const &file : files) {
        int fd = open(file.c_str(), O_RDONLY);
        if(fd < 0)
            THROW_ERRNO("open");

        struct stat statbuf;
        if ( fstat(fd, &statbuf) < 0 )
            THROW_ERRNO("fstat");

        void *data = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if (data == MAP_FAILED)
            THROW_ERRNO("mmap");

        fs::path p(file);
        std::string filename = p.filename().native();
        std::string module_name = filename.substr(0, filename.length() - 4);

        if(module_name == "__init__") {
            c_.load((const char *)data, statbuf.st_size);
        }
        else {
            modules_.push_back(Module(module_name));
            c_.loadModule(module_name.c_str(), (const char *)data, statbuf.st_size);
        }

        if (munmap(data, statbuf.st_size))
            THROW_ERRNO("munmap");
    }
}

void
Manager::run() {
    prepareModules();

    io_service_.dispatch(
            std::bind(&Manager::runIter, this, std::placeholders::_1)
            );

    io_service_.run();
}

void
Manager::runIter(const boost::system::error_code& error) {
    if(error) {
        throw std::runtime_error(error.message());
    }

    // TODO
}

void
Manager::prepareModules() {
    for(Module &m : modules_) {
        auto m_table = c_[m.name().c_str()]->asTable();
        // Disabled
        if(m_table->get("disabled"))
            m.disable();

        // Periodic
        bool is_num = false;
        time_t t = m_table->get("periodic")->asInteger(&is_num);
        if(is_num)
            m.setPeriod(t);
    }
}

time_t
Manager::startSomething() {

}

}
