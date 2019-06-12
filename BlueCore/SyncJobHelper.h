#pragma once
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <memory>

#include "DateTime.h"
#include "Logger.h"
#include "Macro.h"
#include "Callback.h"
#include "JobExec.h"

namespace BLUE_BERRY
{

template<typename T>
using SharedPtr = std::shared_ptr<T>;

class Job;
class SyncJobHelper
{
public:
	explicit SyncJobHelper(size_t key_, SharedPtr<Callback> post_, SharedPtr<Job> timeOut_)
		: _hashKey(key_), _postJob(post_), _timeOutJob(timeOut_), _timeOut(false)
	{
		_waitTime = DateTime::GetTickCount() + 3000 * 1000;
	}
	// job timeout 
	int64_t _waitTime;
	// request identify
	size_t _hashKey;
	// post job
	SharedPtr<Callback> _postJob;
	// time out job
	SharedPtr<Job> _timeOutJob;
	// is timeout
	bool _timeOut;

public:
	bool checkTimeOut()
	{
		auto tick = DateTime::GetTickCount();
		if (_waitTime < tick)
		{
			_timeOut = true;
			LOG(L_DEBUG_, "timeout", "key", (int)_hashKey, "wait time", (double)_waitTime, "tick", (double)tick);
			timeOutExecute();
			return true;
		}
		return false;
	}

	virtual void timeOutExecute()
	{
		if (_timeOutJob)
		{
			_timeOutJob->onExecute();
		}
	}
};

class RedisJobHelper : public SyncJobHelper
{	
};

class DBJobHelper : public SyncJobHelper
{
};

class SyncJobManager : public TimerExec
{
public:
	DECLARE_MGR(SyncJobManager)

	void run();
public:
	void start();
	void stop();

	void addJob(SharedPtr<SyncJobHelper> job_)
	{
		std::lock_guard<std::recursive_mutex> guard(_mtx);
		_jobs.insert({ job_->_hashKey, job_ });
		LOG(L_DEBUG_, "add job", "count", (int)_jobs.size(), "key", (int)job_->_hashKey);
	}

	void addJob(size_t key_, SharedPtr<Callback> post_, SharedPtr<Job> timeOut_)
	{
		addJob(std::make_shared<SyncJobHelper>(key_, post_, timeOut_));
	}

	//template<typename _T>
	//bool getPostJob(size_t hashKey_, SharedPtr<Callback>& postJob_)
	//{
	//	std::lock_guard<std::recursive_mutex> guard(_mtx);

	//	auto it = _jobs.find(hashKey_);
	//	if (it != _jobs.end())
	//	{
	//		postJob_ = it->second->_postJob;
	//		_jobs.erase(it);
	//		LOG(L_DEBUG_, "delete job", "count", (int)_jobs.size());
	//		return true;
	//	}
	//	return false;
	//}

public:
	std::atomic<bool> _running;

	// hash key(cmd + identify), sync job
	std::unordered_map<size_t, SharedPtr<SyncJobHelper> > _jobs;
	std::recursive_mutex _mtx;

};
EXTERN_MGR(SyncJobManager)

}