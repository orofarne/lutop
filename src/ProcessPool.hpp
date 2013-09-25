#pragma once

#include "Context.hpp"
#include "SubProcess.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include <tuple>
#include <queue>
#include <memory>
#include <functional>

#include <boost/asio/io_service.hpp>

namespace lutop {

class ProcessPool {
    public:
        typedef std::function<void(std::shared_ptr<Response>, const boost::system::error_code &)> RequestCallback;

    public:
        ProcessPool(boost::asio::io_service &io_service, Context &cx, unsigned int size, unsigned int max_count);
        ~ProcessPool() throw();

        void process(std::shared_ptr<Request> r, RequestCallback callback);

    private:
        unsigned int pool_size_;
        unsigned int max_count_;
        std::vector<std::shared_ptr<SubProcess>> procs_;
        std::vector<unsigned int> counts_;
        std::vector<bool> in_use_;
        std::queue<std::tuple<std::shared_ptr<Request>, RequestCallback>>  task_queue_;

        boost::asio::io_service &io_service_;
        Context c_;

    private:
        void process(std::shared_ptr<SubProcess> proc, std::shared_ptr<Request> r, RequestCallback callback);
        std::shared_ptr<SubProcess> getProc();
};

}
