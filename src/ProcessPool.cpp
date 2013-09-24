#include "ProcessPool.hpp"

namespace lutop {

ProcessPool::ProcessPool(boost::asio::io_service &io_service, Context &cx, unsigned int size)
    : pool_size_(size)
    , io_service_(io_service)
    , c_(cx)
{

}

ProcessPool::~ProcessPool() throw () {

}

void
ProcessPool::process(const Request &, RequestCallback) {
    // TODO
}

}
