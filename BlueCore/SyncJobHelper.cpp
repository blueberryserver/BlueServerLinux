#include "SyncJobHelper.h"
#include "Job.h"
#include "IOService.h"

namespace BLUE_BERRY
{

DEFINE_MGR(SyncJobManager)

void SyncJobManager::start()
{
	_running.store(true);

	doTimer(1000, true, 
		[this]() { this->run(); }
	);
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

}


