#pragma once
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
	auto job = new AsyncJob<_ARGS...>(func_, std::forward<_ARGS>(args_)...);
	return job;
}

class Worker
{
public:
	virtual ~Worker() {}

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

}