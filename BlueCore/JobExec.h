#pragma once
#include "Job.h"

namespace BLUE_BERRY 
{

template<class _T>
static Job* makeAsyncJob(_T lamda_)
{
	auto job = new AsyncJobLamda<_T>(lamda_);
	return job;
}

template<class _Timer, class _T>
static Job* makeTimerJob(int time_, _Timer timer_, _T lamda_)
{
	auto job = new TimerJobLamda<_Timer, _T>(time_, timer_, lamda_);
	return job;
}

class JobExec
{
public:
	virtual ~JobExec() {}

	template<class _T>
	void doAsync(_T lamda_)
	{
		auto job = makeAsyncJob(lamda_);
		IOService::getIOService()->asyncJob(job);
	}

};


class TimerExec
{
public:
	TimerExec() {}
	virtual ~TimerExec() {}

	template<class _T>
	void doTimer(long time_, bool repeat_, _T lamda)
	{
		auto t = std::make_shared<boost::asio::deadline_timer>(IOService::getIOService()->getIO());
		t->expires_from_now(boost::posix_time::milliseconds(time_));
		if (repeat_ == false) time_ = 0;

		auto job = makeTimerJob((int)time_, t, lamda);
		IOService::getIOService()->asyncWait(t, job);
	}
};

template<class _T>
static void asyncJob(_T lamda_)
{
	IOService::getIOService()->asyncJob(new AsyncJobLamda<_T>(lamda_));
}

}