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

Manager::Manager() {

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

        modules_.push_back(module_name);
        c_.loadModule(module_name.c_str(), (const char *)data, statbuf.st_size);

        if (munmap(data, statbuf.st_size))
            THROW_ERRNO("munmap");
    }
}

void
Manager::run() {

}

}
