#include "SubProcess.hpp"

namespace lutop {

SubProcess::SubProcess(boost::asio::io_service &io_service)
    : io_service_(io_service)
{

}

SubProcess::~SubProcess() throw() {

}

}
