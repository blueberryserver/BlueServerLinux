#include "IOService.h"


namespace BLUE_BERRY
{

DEFINE_MGR(IOService)

IOService::IOService(unsigned long threadCount_)
: _worker(_ioService)
{
	if (threadCount_ == 0)
	{
		// test setting
		threadCount_ = 4;
	}

	for (unsigned long i = 0; i < threadCount_; ++i)
	{
		_pool.create_thread(boost::bind(&io_service::run, &_ioService));
	}
}

IOService::~IOService()
{
	stop();
}

}