#pragma once
#include "Job.h"

namespace BLUE_BERRY 
{

template<typename _T>
static decltype(auto) makeAsyncJob(_T lamda_)
{
	return std::make_shared<AsyncJobLamda<_T>>(lamda_);
}

template<typename _Timer, typename _T>
static decltype(auto) makeTimerJob(int time_, _Timer timer_, _T lamda_)
{
	return std::make_shared<TimerJobLamda<_Timer, _T>>(time_, timer_, lamda_);
}

class JobExec
{
public:
	template<class _T>
	void doAsync(_T lamda_)
	{
		IOService::getIOService()->asyncJob(makeAsyncJob(lamda_));
	}
};


class TimerExec
{
public:
	template<typename _T>
	void doTimer(long time_, bool repeat_, _T lamda)
	{
		auto t = std::make_shared<boost::asio::deadline_timer>(IOService::getIOService()->getIO());
		t->expires_from_now(boost::posix_time::milliseconds(time_));
		if (repeat_ == false) time_ = 0;

		IOService::getIOService()->asyncWait(t, makeTimerJob((int)time_, t, lamda));
	}
};

template<typename _T>
static void asyncJob(_T lamda_)
{
	IOService::getIOService()->asyncJob(makeAsyncJob(lamda_));
}

template<typename _T>
static void asyncWait(long time_, bool repeat_, _T lamda_)
{
	auto t = std::make_shared<boost::asio::deadline_timer>(IOService::getIOService()->getIO());
	t->expires_from_now(boost::posix_time::milliseconds(time_));
	if (repeat_ == false) time_ = 0;

	IOService::getIOService()->asyncWait(t, makeTimerJob((int)time_, t, lamda_));
}

}