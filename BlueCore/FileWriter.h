#pragma once
#include <fstream>
#include "LogHelper.h"
namespace BLUE_BERRY
{
#define _5MB 1048576 * 5

class FileWriter : public LogWriter
{
public:
	explicit FileWriter(const char* name_, const char* path_, size_t fileSize_ = _5MB);
	virtual ~FileWriter();
	virtual void write(const LogData* data_) override;

private:
	bool createFile();
	void checkFileSize();
	void writeJson(const LogData* data_);

private:
	// 최대 파일 크기
	size_t _maxFileSize;
	size_t _curFileSize;

	short _fileNo;
	std::string _name;
	std::string _path;
	std::ofstream _file;
};

}