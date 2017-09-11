#include "../BlueCore/Logger.h"

#include "UserManager.h"


namespace BLUE_BERRY
{
DEFINE_MGR(UserManager)

UserManager::UserManager()
{
}


UserManager::~UserManager()
{
}

UserPtr UserManager::find(const std::string& sessionKey_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	auto it = _jobsBySessionKey.find(sessionKey_);
	if (it != _jobsBySessionKey.end()) return it->second;
	return UserPtr();
}

UserPtr UserManager::find(void* sessionPtr_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	auto it = _jobsBySessionPtr.find(sessionPtr_);
	if (it != _jobsBySessionPtr.end()) return it->second;
	return UserPtr();
}

UserPtr UserManager::findByUid(uint64_t uid_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	for (auto it : _jobsBySessionPtr)
	{
		if (it.second->getData().uid() == uid_) return it.second;
	}
	return UserPtr();
}

UserPtr UserManager::findByName(const char * name_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	for (auto it : _jobsBySessionPtr)
	{
		if (it.second->getData().name().compare(name_) == 0 ) return it.second;
	}
	return UserPtr();
}

void UserManager::add(UserPtr& user_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	auto sessionKey = user_->getSessionKey();
	auto sessionPtr = user_->_session.get();

	_jobsBySessionKey.insert(std::make_pair(sessionKey, user_));
	_jobsBySessionPtr.insert(std::make_pair(sessionPtr, user_));
}

void UserManager::remove(const std::string& sessionKey_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	auto it = _jobsBySessionKey.find(sessionKey_);
	if (it != _jobsBySessionKey.end())
	{
		auto sessionPtr = it->second->_session.get();

		auto it2 = _jobsBySessionPtr.find(sessionPtr);
		if (it2 != _jobsBySessionPtr.end()) _jobsBySessionPtr.erase(it2);

		_jobsBySessionKey.erase(it);
	}
	
}

void UserManager::remove(void* sessionPtr_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);

	auto it = _jobsBySessionPtr.find(sessionPtr_);
	if (it != _jobsBySessionPtr.end())
	{
		auto sessionKey = it->second->getSessionKey();

		auto it2 = _jobsBySessionKey.find(sessionKey);
		if (it2 != _jobsBySessionKey.end()) _jobsBySessionKey.erase(it2);

		_jobsBySessionPtr.erase(it);
	}
}

void UserManager::start()
{
	doTimer(1000, true, &UserManager::tick);
}

void UserManager::stop()
{
}

void UserManager::tick()
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	//check session key time out

	for (auto it = _jobsBySessionPtr.begin(); it != _jobsBySessionPtr.end(); ++it)
	{
		if (it->second->getPingTime() < DateTime::GetTickCount())
		{
			LOG(L_INFO_, "timeout", "uid", (double)it->second->_data.uid(), "name", it->second->_data.name());

			_jobsBySessionKey.erase(it->second->getSessionKey());
			it = _jobsBySessionPtr.erase(it);

			if (it == _jobsBySessionPtr.end()) break;
		}
	}
}


}