#pragma once
#include <iostream>
#include "LogHelper.h"
#include "DateTime.h"
namespace BLUE_BERRY 
{


static std::string getDateTime(std::time_t time_)
{
	char time[2048];
	std::strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", std::localtime(&time_));
	return std::string(time);
}

class ConsoleWriter : public LogWriter
{
public:
	virtual void write(const LogData* data_)
	{
		auto now = DateTime(data_->_time).formatLocal();

		std::string log;
		log = now + " ";
		log = log + std::to_string(data_->_thread) + " ";
		log = log + ToStrLogLevel(data_->_level) + " ";
		log = log + "[" + data_->_func + "] ";
		log = log + data_->_desc + " ";

		if (false == data_->_objects.empty())
			log = log + Json(data_->_objects).dump();

		std::cout << log << std::endl;
	
	}
};

}