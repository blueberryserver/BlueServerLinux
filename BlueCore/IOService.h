#pragma once
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "Macro.h"

using namespace boost::asio;
namespace BLUE_BERRY
{

class IOService
{
public:
	IOService(unsigned long threadCount_ = 0);
	virtual ~IOService();

	template <typename CompletionHandler>
	void post(BOOST_ASIO_MOVE_ARG(CompletionHandler) handler_)
	{
		_ioService.post(handler_);
	}

	template <typename CompletionHandler>
	void asyncWait(std::shared_ptr<boost::asio::deadline_timer>& t_, BOOST_ASIO_MOVE_ARG(CompletionHandler) handler_)
	{
		auto h = [handler_](const boost::system::error_code& err_)
		{
			handler_->onExecute();
			delete handler_;
		};

		t_->async_wait(h);
	}



	void stop()
	{
		_ioService.reset();
		_ioService.stop();
		_pool.join_all();
	}


	io_service& getIO()
	{
		return _ioService;
	}

	DECLARE_MGR(IOService)

private:
	io_service _ioService;
	io_service::work _worker;
	boost::thread_group _pool;
};

EXTERN_MGR(IOService);

}