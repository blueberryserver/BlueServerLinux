#pragma once
#include "Job.h"

namespace BLUE_BERRY 
{

template<class _T, class... _ARGS>
static Job* makeAsyncJob(_T* ptr_, void (_T::*memFunc_)(_ARGS...), _ARGS&&... args_)
{
	auto job = new AsyncJob<_T, _ARGS...>(static_cast<_T*>(ptr_), memFunc_, std::forward<_ARGS>(args_)...);
	return job;
}

template<class... _ARGS>
static Job* makeAsyncJob(void(*func_)(_ARGS...), _ARGS&&... args_)
{
	auto job = new AsyncJobStc<_ARGS...>(func_, std::forward<_ARGS>(args_)...);
	return job;
}


template<class _Timer, class _T, class... _ARGS>
static Job* makeTimerJob(int time_, _Timer timer_, _T* ptr_, void (_T::*memFunc_)(_ARGS...), _ARGS&&... args_)
{
	auto job = new TimerJob<_Timer, _T, _ARGS...>(time_, timer_, static_cast<_T*>(ptr_), memFunc_, std::forward<_ARGS>(args_)...);
	return job;
}

template<class _Timer, class... _ARGS>
static Job* makeTimerJob(int time_, _Timer timer_, void(*func_)(_ARGS...), _ARGS&&... args_)
{
	auto job = new TimerJobStc<_Timer, _ARGS...>(time_, timer_, func_, std::forward<_ARGS>(args_)...);
	return job;
}

class JobExec
{
public:
	virtual ~JobExec() {}

	template<class _T, class... _ARGS>
	void doAsync(void (_T::*memFunc_)(_ARGS...), _ARGS&&... args_)
	{
		auto job = makeAsyncJob(static_cast<_T*>(this), memFunc_, std::forward<_ARGS>(args_)...);
		IOService::getIOService()->post(boost::bind(&Job::onExecute, job));
	}

	// static function
	template<class... _ARGS>
	void doAsync(void(*func_)(_ARGS...), _ARGS&&... args_)
	{
		auto job = makeAsyncJob(func_, std::forward<_ARGS>(args_)...);
		IOService::getIOService()->post(boost::bind(&Job::onExecute, job));
	}

};


class TimerExec
{
public:
	TimerExec() {}
	virtual ~TimerExec() {}

	template<class _T, class... _ARGS>
	void doTimer(long time_, bool repeat_, void (_T::*memFunc_)(_ARGS...), _ARGS&&... args_)
	{
		auto t = std::make_shared<boost::asio::deadline_timer>(IOService::getIOService()->getIO());
		t->expires_from_now(boost::posix_time::milliseconds(time_));
		if (repeat_ == false) time_ = 0;

		auto job = makeTimerJob(time_, t, static_cast<_T*>(this), memFunc_, std::forward<_ARGS>(args_)...);
		IOService::getIOService()->asyncWait(t, job);
	}

	// static function
	template<class... _ARGS>
	void doTimer(long time_, bool repeat_, void(*func_)(_ARGS...), _ARGS&&... args_)
	{
		auto t = std::make_shared<boost::asio::deadline_timer>(IOService::getIOService()->getIO());
		t->expires_from_now(boost::posix_time::milliseconds(time_));
		if (repeat_ == false) time_ = 0;

		auto job = makeTimerJob(time_, t, func_, std::forward<_ARGS>(args_)...);
		IOService::getIOService()->asyncWait(t, job);
	}

};

}