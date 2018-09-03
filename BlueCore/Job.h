#pragma once
#include <atomic>

#include "JobHelper.h"
#include "IOService.h"

namespace BLUE_BERRY {

class Job
{
public:
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

template<class _T, class _Fn, class... _ARGS>
class AsyncJob : public Job
{
	typedef std::tuple<_ARGS...> args;

public:
	_T* _obj;
	_Fn _func;
	args _args;

	explicit AsyncJob(_T* obj_, _Fn&& func_, _ARGS&&... args_)
		: _obj(obj_), _func(std::forward<_Fn>(func_)), _args(std::forward<_ARGS>(args_)...) {}

	explicit AsyncJob(_T* obj_, _Fn func_, args args_)
		: _obj(obj_), _func(func_), _args(args_) {}

	explicit AsyncJob(_Fn&& func_, _ARGS&&... args_)
		: _obj(nullptr), _func(std::forward<_Fn>(func_)), _args(std::forward<_ARGS>(args_)...) {}

	explicit AsyncJob(_Fn func_, args args_)
		: _obj(nullptr), _func(func_), _args(args_) {}

	virtual ~AsyncJob() {}

	virtual void onExecute()
	{
		if (_obj != nullptr)
		{
			doExecute(_obj, _func, _args);
		}
		else
		{
			doExecute(_func, _args);
		}

		_executedCount.fetch_add(1);
	}

	Job* clone()
	{
		if (_obj != nullptr)
		{
			return new AsyncJob(_obj, _func, _args);
		}

		return new AsyncJob(_func, _args);
	}

};



template<class _Fn, class... _ARGS >
class AsyncJobStc : public Job
{
	typedef void(*func)(_ARGS...);
	typedef std::tuple<_ARGS...> args;

public:
	_Fn _func;
	args _args;

	explicit AsyncJobStc(_Fn&& func_, _ARGS&&... args_)
		: _func(std::forward<_Fn>(func_)), _args(std::forward<_ARGS>(args_)...) {}

	explicit AsyncJobStc(_Fn func_, args args_)
		: _func(func_), _args(args_) {}

	virtual ~AsyncJobStc() {}

	virtual void onExecute()
	{
		doExecute(_func, _args);
		_executedCount.fetch_add(1);
	}

	Job* clone()
	{
		return new AsyncJobStc(_func, _args);
	}
};


template<class _Timer, class _T, class... _ARGS>
class TimerJob : public Job
{
	typedef void(_T::*memFunc)(_ARGS...);
	typedef std::tuple<_ARGS...> args;

public:
	_Timer _t;
	_T* _obj;
	memFunc _memFunc;
	args _args;

	explicit TimerJob(int repeat_, _Timer t_, _T* obj_, memFunc memFunc_, _ARGS&&... args_)
		: _t(t_), _obj(obj_), _memFunc(memFunc_), _args(std::forward<_ARGS>(args_)...) { _repeat = repeat_;}

	explicit TimerJob(int repeat_, _Timer t_, _T* obj_, memFunc memFunc_, args args_)
		: _t(t_), _obj(obj_), _memFunc(memFunc_), _args(args_) { _repeat = repeat_; }


	virtual ~TimerJob() {
		_memFunc = nullptr;
	}

	virtual void onExecute()
	{
		doExecute(_obj, _memFunc, _args);
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
		return new TimerJob(_repeat, _t, _obj, _memFunc, _args);
	}
};


template<class _Timer, class _Fn, class... _ARGS>
class TimerJobStc : public Job
{
	typedef std::tuple<_ARGS...> args;

public:
	_Timer _t;
	_Fn _func;
	args _args;

	explicit TimerJobStc(int repeat_, _Timer t_, _Fn&& func_, _ARGS&&... args_)
		: _t(t_), _func(std::forward<_Fn>(func_)), _args(std::forward<_ARGS>(args_)...) { _repeat = repeat_; }

	explicit TimerJobStc(int repeat_, _Timer t_, _Fn func_, args args_)
		: _t(t_), _func(func_), _args(args_) {	_repeat = repeat_; }

	virtual ~TimerJobStc() {
	}

	virtual void onExecute()
	{
		doExecute(_func, _args);
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
		return new TimerJobStc(_repeat, _t, _func, _args);
	}
};


}