#include "IOService.h"


namespace BLUE_BERRY
{

IOService* ___IOService = nullptr;
IOService* IOService::getIOService()
{
	return ___IOService;
}
void IOService::setIOService(IOService* mgr_)
{
	___IOService = mgr_;
}
void IOService::deleteIOService()
{
	delete ___IOService;
}

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

void IOService::stop()
{
	_ioService.reset();
	_ioService.stop();
	_pool.join_all();
}


}