#pragma once
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
using namespace boost::asio;
namespace BLUE_BERRY
{

class IOService
{
public:
	IOService(unsigned long threadCount_ = 0);
	virtual ~IOService();

	static IOService* getIOService();
	static void setIOService(IOService* mgr_);
	static void deleteIOService();

	void stop();

	template <typename CompletionHandler>
	void post(BOOST_ASIO_MOVE_ARG(CompletionHandler) handler_)
	{
		_ioService.post(handler_);
	}

private:
	io_service _ioService;
	io_service::work _worker;
	boost::thread_group _pool;
};

extern IOService* ___IOService;

}