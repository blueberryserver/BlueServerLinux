#include <iostream>
#include <fstream>
#include "JsonFileLoader.h"
#include "DateTime.h"
#include "ThreadUtil.h"

namespace BLUE_BERRY
{
DEFINE_MGR(JsonFileLoader)

JsonFileLoader::JsonFileLoader()
{
}


JsonFileLoader::~JsonFileLoader()
{
}

void JsonFileLoader::start()
{
	doTimer(
		duration_cast<seconds>(minutes(1)).count(), 
		true, 
		[this]() {this->tick(); }
	);
}

void JsonFileLoader::stop()
{

}

bool JsonFileLoader::insert(const char * jsonFilePath_)
{
	filesystem::path p(jsonFilePath_);
	system::error_code ec;
	if (false == exists(p, ec))
	{
		std::cout << "path not exists " << p.c_str() << " err: " << ec << std::endl;
		return false;
	}

	Json jsonData;
	if (load(p, jsonData) == false)
		return false;

	std::lock_guard<std::recursive_mutex> guard(_mtx);
	_jsonDatas.insert(std::make_pair( p.string(), jsonData ));
	return true;
}

Json JsonFileLoader::get(const char* name_)
{
	LOCK_R(_mtx);
	for (auto it : _jsonDatas)
	{
		auto path = it.first;
		filesystem::path p(path);
		system::error_code ec;
		if (false == exists(p, ec))
		{
			std::cout << "path not exists " << p.c_str() << " err: " << ec << std::endl;
			continue;
		}

		if (strcmp(p.filename().c_str(), name_) == 0) return it.second;
	}
	return Json();
}

void JsonFileLoader::tick()
{
	LOCK_R(_mtx);
	for (auto it : _jsonDatas)
	{
		auto path = it.first;
		filesystem::path p(path);
		system::error_code ec;
		if (false == exists(p, ec))
		{
			std::cout << "path not exists " << p.c_str() << " err: " << ec << std::endl;
			continue;
		}
		
		if (load(p, it.second) == false)
		{
			// logging
			//std::cout << "load fail " << p.filename().c_str() << std::endl;
			continue;
		}
		//std::cout << "json file load " << p.filename().c_str() << std::endl;
	}
}

bool JsonFileLoader::load(filesystem::path& path_, Json& jsonData_)
{
	std::ifstream file(path_.c_str(), std::ios::in);
	if (file.fail() == true)
	{
		std::cout << "file open fail " << path_.c_str() << std::endl;
		return false;
	}

	if (file.is_open() == false)
	{
		return false;
	}

	std::string str;
	file.seekg(0, std::ios::end);
	int size = (int)file.tellg();
	str.resize(size);

	file.seekg(0, std::ios::beg);
	file.read(&str[0], size);
	std::string err;
	jsonData_ = Json::parse(str.c_str(), err);
	if (err.empty() == false)
	{
		std::cout << "file parsing fail " << err << std::endl;
		return false;
	}

	return true;
}

}