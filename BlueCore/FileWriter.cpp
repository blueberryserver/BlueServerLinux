
#include <chrono>
#include <ctime>
#include <stdio.h>
#include <boost/filesystem.hpp>

#include <iostream>
using namespace boost::filesystem;

#include "FileWriter.h"
#include "json11.hpp"
#include "DateTime.h"
using namespace json11;

namespace BLUE_BERRY
{

static std::string getTime()
{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	char time[2048];
	std::strftime(time, sizeof(time), "%H_%M", std::localtime(&now));
	return std::string(time);
}

void FileWriter::checkFileSize()
{
	if (_curFileSize > _maxFileSize)
	{
		_file.flush();
		_file.close();

		createFile();
	}
}

FileWriter::FileWriter(const char* name_, const char* path_, size_t fileSize_)
	:_maxFileSize(fileSize_), _curFileSize(0), _fileNo(1), _name(name_), _path(path_)
{
	createFile();
}


FileWriter::~FileWriter()
{

}

void FileWriter::write(const LogData* data_)
{
	writeJson(data_);
}

bool FileWriter::createFile()
{
	path p(_path.c_str());

	if (false == exists(p))
	{
		if (false == create_directory(p))
		{
			return false;
		}
	}
	std::string fileName;
	fileName += _path;
	fileName += "/";
	fileName += _name;
	fileName += getTime();
	fileName += std::to_string(_fileNo++);
	fileName += ".json";

	_file.open(fileName);

	// check failbit streams error state
	if (_file.fail() == true)
	{
		return false;
	}

	return true;
}

void FileWriter::writeJson(const LogData* data_)
{
	auto now = DateTime(data_->_time).formatLocal();

	Json::object jMap;
	jMap["id"] = std::to_string(data_->_id);
	jMap["no"] = std::to_string(data_->_no);
	jMap["date"] = now;
	jMap["file"] = data_->_file;
	jMap["func"] = data_->_func;
	jMap["line"] = std::to_string(data_->_line);
	jMap["level"] = std::to_string(data_->_level);
	jMap["desc"] = data_->_desc;


	jMap["param"]= data_->_objects;

	Json Log = jMap;
	std::string strdump = Log.dump();
	strdump = strdump + "\r\n";

	_curFileSize += strdump.size();

	_file << strdump;
	_file.flush();

	checkFileSize();
}

}
