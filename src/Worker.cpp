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
    (void)c_;
    // TODO
    return nullptr;
}

}
