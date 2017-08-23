#pragma once
#include <mutex>
#include <unordered_map>
#include <boost/filesystem.hpp>

#include "Macro.h"
#include "JobExec.h"
#include "json11.hpp"

using namespace json11;
using namespace boost;

namespace BLUE_BERRY
{

class JsonFileLoader : public TimerExec
{
public:
	JsonFileLoader();
	virtual ~JsonFileLoader();

	void start();
	void stop();

	bool insert(const char* jsonFilePath_);
	Json get(const char* name_);

private:
	void tick();
	bool load(filesystem::path& path_, Json& jsonData_);

public:
	std::recursive_mutex _mtx;
	std::unordered_map<std::string, Json> _jsonDatas;

public:
	DECLARE_MGR(JsonFileLoader)
};


EXTERN_MGR(JsonFileLoader)
}