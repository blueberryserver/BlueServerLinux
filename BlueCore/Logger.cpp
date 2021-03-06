#include <thread>
#include <chrono>
#include "Logger.h"
#include "ConsoleWriter.h"
#include "FileWriter.h"
#include "IOService.h"

namespace BLUE_BERRY
{

DEFINE_MGR(Logger)

Logger::Logger(_LogType type_, _LogLevel level_, const char* name_)
	: _instanceId(0), _no(0), _type(type_), _level(level_)
{
	if( _type & T_CONSOLE_ ) addLogWriter(T_CONSOLE_, new ConsoleWriter);
	if (_type & T_FILE_) addLogWriter(T_FILE_, new FileWriter(name_, "./log"));
}


Logger::~Logger()
{
}

void Logger::addLogWriter(_LogType type_, LogWriter* writer_)
{
	auto it = _writers.find(type_);
	if ( it != _writers.end())
	{
		delete it->second;
	}

	_writers[type_] = writer_;
}

template<>
void Logger::write(_LogLevel level_, const std::string& func_, const std::string& file_, int line_, const std::string& desc_)
{
	if (_running.load() == false) return;
	if ((_level & level_) == false) return;

	auto data = new LogData(_instanceId, _no++, line_, getThreadId(), level_, func_, file_, desc_);

	//std::lock_guard<std::recursive_mutex> guard(_mtx);
	if (_logs.push(data) == false)
	{
		std::cout << "Logger::write fail" << std::endl;
	}
}

void Logger::start()
{
	doTimer(1000, true, 
		[this]() { this->tick(); }
	);
	_running.store(true);
}

void Logger::stop()
{
	_running.store(false);
}

void Logger::workLogData()
{
	//std::lock_guard<std::recursive_mutex> guard(_mtx);
	std::vector<LogData*> dataArray;
	if (_logs.popAll(dataArray) == true)
	{
		for (auto it = dataArray.begin(); it != dataArray.end(); ++it)
		{
			for (auto mit = _writers.begin(); mit != _writers.end(); ++mit)
			{
				if (mit->second == nullptr) continue;

				mit->second->write(*it);
			}

			delete *it;
		}
	}

}

void Logger::tick()
{
	workLogData();
}

}