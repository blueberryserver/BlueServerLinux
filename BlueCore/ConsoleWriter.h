#pragma once
#include <iostream>
#include "LogHelper.h"

namespace BLUE_BERRY 
{


class ConsoleWriter : public LogWriter
{
public:
	virtual void write(const LogData* data_)
	{
		//auto time = DateTime(data_->_time);
		//auto strTime = time.format();

		std::string log;
		//log = strTime + " ";
		log = log + ToStrLogLevel(data_->_level) + " ";
		log = log + data_->_desc + " ";

		//for (size_t i = 0; i < data_->_params.size(); i++)
		//{
		//	//log = log + data_._params[i]._type + " ";
		//	log = log + "[" + data_->_params[i]._comment + ":" + data_->_params[i]._value + "]";
		//}
		if (false == data_->_objects.empty())
			log = log + Json(data_->_objects).dump();

		std::cout << log << std::endl;
	
	}
};

}