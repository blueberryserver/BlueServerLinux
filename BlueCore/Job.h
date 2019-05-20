#pragma once
#include <atomic>

#include "JobHelper.h"
#include "IOService.h"

namespace BLUE_BERRY {

class Job
{
public:
	// for repeated job
	std::atomic<long> _executedCount;
	int _repeat;
public:
	Job() : _executedCount(0), _repeat(0) {}
	virtual ~Job() {}

	// 작업 실행
	virtual void onExecute() = 0;

	// 작업 완료 여부
	bool isComplete(long count_)
	{
		return _executedCount.compare_exchange_strong(count_, 0);
	}
	virtual Job* clone() = 0;
};

template<class _T>
class AsyncJobLamda : public Job
{
public:
	_T _lamda;

	explicit AsyncJobLamda(_T lamda_)
		: _lamda(lamda_) {}

	virtual ~AsyncJobLamda() = default;

	virtual void onExecute()
	{
		_lamda();
		_executedCount.fetch_add(1);
	}

	Job* clone()
	{
		return new AsyncJobLamda(_lamda);
	}

};

template<class _Timer, class _T>
class TimerJobLamda : public Job
{
public:
	_Timer _t;
	_T _lamda;
	explicit TimerJobLamda(int repeat_, _Timer t_, _T lamda_)
		: _t(t_), _lamda(lamda_) 
	{
		_repeat = repeat_;
	}

	virtual ~TimerJobLamda() = default;

	virtual void onExecute()
	{
		_lamda();
		_executedCount.fetch_add(1);

		if (_repeat > 0)
		{
			_t->expires_from_now(boost::posix_time::milliseconds(_repeat));
			auto job = clone();
			IOService::getIOService()->asyncWait(_t, job);
		}
	}

	Job* clone()
	{
		return new TimerJobLamda(_repeat, _t, _lamda);
	}
};

}