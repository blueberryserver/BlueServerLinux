#pragma once
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <iostream>

#include "Macro.h"
#include "LogHelper.h"
#include "LockFreeQueue.h"
#include "ThreadUtil.h"
#include "JobExec.h"

namespace BLUE_BERRY {

typedef std::unordered_map<int, LogWriter*> LogWriterMap;

class Logger : public TimerExec
{
public:
	explicit Logger(_LogType type_, _LogLevel level_, const char* name_);
	~Logger();

	void setInstanceId(int id_) { _instanceId = id_; }
	void setLogType(_LogType type_) { _type = type_; }
	void setLogLevel(_LogLevel level_) { _level = level_; }
	void addLogWriter(_LogType type_, LogWriter* writer_);

	template<typename ...ARGS>
	void write(_LogLevel level_, const std::string& func_, const std::string& file_, int line_, const std::string& desc_, ARGS&&... args_)
	{
		if (_running.load() == false) return;
		if ((_level & level_) == false) return;

		auto data = new LogData(_instanceId, _no++, line_, getThreadId(), level_, func_, file_, desc_);
		AddJsonObject(data->_objects, args_...);

		if (_logs.push(data) == false)
		{
			std::cout << "Logger::write fail" << std::endl;
		}
	}

	void write(_LogLevel level_, const std::string& func_, const std::string& file_, int line_, const std::string& desc_)
	{
		if (_running.load() == false) return;
		if ((_level & level_) == false) return;

		auto data = new LogData(_instanceId, _no++, line_, getThreadId(), level_, func_, file_, desc_);

		if (_logs.push(data) == false)
		{
			std::cout << "Logger::write fail" << std::endl;
		}
	}

	void start();
	void stop();

	void workLogData();

private:
	void tick();

public:
	int _instanceId;
	int _no;
	_LogType _type;
	_LogLevel _level;
	LogWriterMap _writers;

	std::atomic<bool> _running;
	LockFreeQueue<LogData*, 65536> _logs;

public:
	DECLARE_MGR(Logger)
};

EXTERN_MGR(Logger)
#define __FILENAME__ ( strrchr(__FILE__,'\\') == 0 ? __FILE__ : strrchr(__FILE__,'\\') + 1 )
#define LOG(__level__, __desc__, ...) ___Logger->write(__level__, __func__, __FILENAME__, __LINE__, __desc__, ## __VA_ARGS__)




}