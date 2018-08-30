#pragma once
#include <iostream>
#include "LogHelper.h"
#include "DateTime.h"
namespace BLUE_BERRY 
{

class ConsoleWriter : public LogWriter
{
public:
	virtual void write(const LogData* data_) override
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