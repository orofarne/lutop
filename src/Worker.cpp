#include "Worker.hpp"

namespace lutop {

Worker::Worker(Context &c)
    : c_(c)
{

}

Worker::~Worker() throw() {

}

void *
Worker::operator()(void *in_buf, size_t in_size, size_t *out_buf) {
    c_.setString("__request", reinterpret_cast<char *>(in_buf), in_size);


    return nullptr;
}

}
