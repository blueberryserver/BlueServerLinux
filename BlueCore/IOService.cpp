#include "IOService.h"

namespace BLUE_BERRY
{

DEFINE_MGR(IOService)

IOService::IOService(unsigned long threadCount_)
{
	if (threadCount_ == 0)
	{
		// defaul setting(cpu count * 2)
		threadCount_ = std::thread::hardware_concurrency() * 2;
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