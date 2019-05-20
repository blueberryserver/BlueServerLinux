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
		_ioService.post(_strandForJob.wrap(handler_));
	}

	template <typename CompletionHandler>
	void asyncJob(BOOST_ASIO_MOVE_ARG(CompletionHandler) handler_)
	{
		_ioService.post(
			[handler_]()
			{
				handler_->onExecute();
				delete handler_;
			}
		);
	}

	template <typename CompletionHandler>
	void asyncWait(std::shared_ptr<boost::asio::deadline_timer>& t_, BOOST_ASIO_MOVE_ARG(CompletionHandler) handler_)
	{
		t_->async_wait(_strandForTimer.wrap(
			[handler_](const boost::system::error_code& err_)
			{
				handler_->onExecute();
				delete handler_;
			})
		);
	}

	void stop()
	{
		_ioService.reset();
		_ioService.stop();
		_pool.join_all();
	}


	io_context& getIO()
	{
		return _ioService;
	}

	DECLARE_MGR(IOService)

private:
	io_context _ioService{};
	io_context::strand _strandForJob{ _ioService };
	io_context::strand _strandForTimer{ _ioService };
	io_context::work _worker{ _ioService };
	boost::thread_group _pool;
};

EXTERN_MGR(IOService);

}