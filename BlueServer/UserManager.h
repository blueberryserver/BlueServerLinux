#pragma once
#include <unordered_map>
#include <mutex>
#include "../BlueCore/JobExec.h"
#include "../BlueCore/Macro.h"

#include "User.h"

namespace BLUE_BERRY
{


class UserManager : public TimerExec
{
public:
	UserManager();
	virtual ~UserManager();

	UserPtr find(const std::string& sessionKey_);
	UserPtr find(void* sessionPtr_);

	void add(UserPtr& user_);
	void remove(const std::string& sessionKey_);
	void remove(void* sessionPtr_);

	void start();
	void stop();


private:
	void tick();

private:
	std::recursive_mutex _mtx;
	// void*, user ptr
	std::unordered_map<void*, UserPtr > _jobsBySessionPtr;

	// session eky user ptr
	std::unordered_map<std::string, UserPtr > _jobsBySessionKey;

public:
	DECLARE_MGR(UserManager)
	
};
EXTERN_MGR(UserManager);

}