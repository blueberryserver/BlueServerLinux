#include "DateTime.h"
#include "SyncJobHelper.h"
#include "Job.h"
#include "IOService.h"
#include "Logger.h"

namespace BLUE_BERRY
{

SyncJobHelper::SyncJobHelper(size_t key_, Callback* post_, Job* timeOut_)
	: _hashKey(key_), _postJob(post_), _timeOutJob(timeOut_), _timeOut(false)
{
	_waitTime = DateTime::GetTickCount() + 3000 * 1000;
}

bool SyncJobHelper::checkTimeOut()
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

void SyncJobHelper::timeOutExecute()
{
	if (_timeOutJob)
	{
		_timeOutJob->onExecute();
	}
}


DEFINE_MGR(SyncJobManager)

void SyncJobManager::start()
{
	_running.store(true);

	doTimer(1000, true, &SyncJobManager::run);
}

void SyncJobManager::stop()
{
	_running.store(false);
}

void SyncJobManager::run()
{	
	if (_running.load() == false) return;
	//LOG(L_DEBUG_, "sync run", "tick", (double)DateTime::GetTickCount());


	std::lock_guard<std::recursive_mutex> guard(_mtx);
	for (auto it = _jobs.begin(); it != _jobs.end(); ++it)
	{
		if (it->second->checkTimeOut() == true)
		{
			it = _jobs.erase(it);
			if (it == _jobs.end()) break;
		}
	}
}

void SyncJobManager::addJob(SyncJobHelperPtr job_)
{	
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	_jobs.insert({ job_->_hashKey, job_ });
	LOG(L_DEBUG_, "add job", "count", (int)_jobs.size(), "key", (int)job_->_hashKey);
}

bool SyncJobManager::getPostJob(size_t hashKey_, Callback*& postJob_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);

	auto it = _jobs.find(hashKey_);
	if (it != _jobs.end())
	{
		postJob_ = it->second->_postJob;
		_jobs.erase(it);
		LOG(L_DEBUG_, "delete job", "count", (int)_jobs.size());
		return true;
	}
	return false;
}



}


