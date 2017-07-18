#pragma once
#include <atomic>
#include "JobHelper.h"

namespace BLUE_BERRY {

class Job
{
public:
	std::atomic<long> _executedCount;

	// 작업 실행
	virtual void onExecute() = 0;

	// 작업 완료 여부
	bool isComplete(long count_)
	{
		return _executedCount.compare_exchange_strong(count_, 0);
	}
	virtual Job* clone() = 0;
};

template<class _T, class... _ARGS>
class AsyncJob : public Job
{
	typedef void(_T::*memFunc)(_ARGS... );
	typedef void(*func)(_ARGS... );
	typedef std::tuple<_ARGS...> args;

public:
	_T* _obj;
	memFunc _memFunc;
	func _func;
	args _args;

	explicit AsyncJob(_T* obj_, memFunc memFunc_, _ARGS&&... args_)
		: _obj(obj_), _memFunc(memFunc_), _func(nullptr), _args(std::forward<_ARGS>(args_)...) {}

	explicit AsyncJob(_T* obj_, memFunc memFunc_, args args_)
		: _obj(obj_), _memFunc(memFunc_), _func(nullptr), _args(args_) {}

	explicit AsyncJob(func func_, _ARGS&&... args_)
		: _obj(nullptr), _memFunc(nullptr), _func(func_), _args(std::forward<_ARGS>(args_)...) {}

	explicit AsyncJob(func func_, args args_)
		: _obj(nullptr), _memFunc(nullptr), _func(func_), _args(args_) {}

	virtual void onExecute()
	{
		if (_obj != nullptr)
		{
			doExecute(_obj, _memFunc, _args);
		}
		else
		{
			doExecute(_func, _args);
		}

		_executedCount.fetch_add(1);
	}

	void setArgs(_ARGS&&... args_)
	{
		_args(std::forward<_ARGS>(args_));
	}

	Job* clone()
	{
		if (_obj != nullptr) return new AsyncJob(_obj, _memFunc, _args);
		return new AsyncJob(_func, _args);
	}

};


}