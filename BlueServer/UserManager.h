#pragma once
#include <unordered_map>
#include <mutex>
#include "User.h"

namespace BLUE_BERRY
{

class UserManager
{
public:
	UserManager();
	~UserManager();





private:

	// hash key(cmd + identify), sync job
	std::unordered_map<uint64_t, UserPtr > _jobs;
	std::recursive_mutex _mtx;
};


}