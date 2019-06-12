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

	// sync job
	template <typename _T>
	void syncJob(_T&& handler_)
	{
		_ioService.post(_strandForJob.wrap(handler_));
	}

	// async job
	template <typename _T>
	void asyncJob(_T&& handler_)
	{
		_ioService.post(
			[handler_]()
			{
				handler_->onExecute();
			}
		);
	}

	// wait time job
	template <typename _T>
	void asyncWait(std::shared_ptr<boost::asio::deadline_timer>& t_, _T&& handler_)
	{
		t_->async_wait(_strandForTimer.wrap(
			[handler_](const boost::system::error_code& err_)
			{
				handler_->onExecute();
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
	boost::thread_group _pool{};
};

EXTERN_MGR(IOService);

}