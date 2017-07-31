#pragma once
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <memory>

#include "Macro.h"
#include "Callback.h"
#include "JobExec.h"

namespace BLUE_BERRY
{

class Job;
class SyncJobHelper
{
public:
	explicit SyncJobHelper(size_t key_, Callback* post_, Job* timeOut_);
	// job timeout 
	int64_t _waitTime;
	// request identify
	size_t _hashKey;
	// post job
	Callback* _postJob;
	// time out job
	Job* _timeOutJob;
	// is timeout
	bool _timeOut;

public:
	bool checkTimeOut();
	virtual void timeOutExecute();
};
DECLARE_SMART_PTR(SyncJobHelper)

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
	void addJob(SyncJobHelperPtr job_);
	bool getPostJob(size_t hashKey_, Callback*& postJob_);

public:
	std::atomic<bool> _running;

	// hash key(cmd + identify), sync job
	std::unordered_map<size_t, SyncJobHelperPtr> _jobs;
	std::recursive_mutex _mtx;

};
EXTERN_MGR(SyncJobManager)

}