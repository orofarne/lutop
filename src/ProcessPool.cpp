#include "ProcessPool.hpp"

#include <algorithm>

#include "Worker.hpp"

namespace lutop {

ProcessPool::ProcessPool(boost::asio::io_service &io_service, Context &cx, unsigned int size, unsigned int max_count)
    : pool_size_(size)
    , max_count_(max_count)
    , procs_(pool_size_)
    , counts_(pool_size_)
    , in_use_(pool_size_)
    , io_service_(io_service)
    , c_(cx)
{
    std::fill(counts_.begin(), counts_.end(), 0);
    std::fill(in_use_.begin(), in_use_.end(), false);
}

ProcessPool::~ProcessPool() throw () {

}

void
ProcessPool::process(std::shared_ptr<Request> r, RequestCallback callback) {
    auto proc = getProc();

    if(proc) {
        process(proc, r, callback);
    }
    else {
        task_queue_.push(
            std::tuple<std::shared_ptr<Request>, RequestCallback>(r, callback)
        );
    }
}

void
ProcessPool::process(std::shared_ptr<SubProcess> proc, std::shared_ptr<Request> r, RequestCallback callback) {
    // TODO
}

std::shared_ptr<SubProcess>
ProcessPool::getProc() {
    for(int i = 0; i < pool_size_; ++i) {
        if(in_use_[i])
            continue;

        auto proc = procs_[i];
        if(!proc || counts_[i] >= max_count_)
            proc.reset(new SubProcess{io_service_, Worker{c_}});

        return proc;
    }
    return nullptr;
}

}
