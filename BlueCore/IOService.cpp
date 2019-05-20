#include "IOService.h"


namespace BLUE_BERRY
{

DEFINE_MGR(IOService)

IOService::IOService(unsigned long threadCount_)
{
	if (threadCount_ == 0)
	{
		// test setting
		threadCount_ = 4;
	}

	for (unsigned long i = 0; i < threadCount_; ++i)
	{
		_pool.create_thread([&_io = _ioService](void) {_io.run();});
	}
}

IOService::~IOService()
{
	stop();
}

}