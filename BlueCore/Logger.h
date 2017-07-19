#pragma once
#include <unordered_map>
#include <atomic>
#include <iostream>
#include <thread>
#include "LogHelper.h"
#include "LockFreeQueue.h"

namespace BLUE_BERRY {

typedef std::unordered_map<int, LogWriter*> LogWriterMap;

class Logger /*: public asyncJob*/
{
public:
	explicit Logger(_LogType type_, _LogLevel level_, const char* name_);
	~Logger();

	void setInstanceId(int id_) { _instanceId = id_; }
	void setLogType(_LogType type_) { _type = type_; }
	void setLogLevel(_LogLevel level_) { _level = level_; }
	void addLogWriter(_LogType type_, LogWriter* writer_);

	template<typename ...ARGS>
	void write(_LogLevel level_, const std::string& file_, int line_, int thread_, const std::string& desc_, ARGS&&... args_)
	{
		auto data = new LogData(_instanceId, _no++, line_, thread_, level_, file_, desc_);
		AddJsonObject(data->_objects, args_...);

		_logs.push(data);
		//std::cout << std::this_thread::get_id() << "Logger::write" << std::endl;
	}

	void write(_LogLevel level_, const std::string& file_, int line_, int thread_, const std::string& desc_)
	{
		auto data = new LogData(_instanceId, _no++, line_, thread_, level_, file_, desc_);

		_logs.push(data);
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
	LockFreeQueue<LogData*> _logs;

public:
	//DECLARE_MGR(Logger);
	static Logger* getLogger();
	static void setLogger(Logger* mgr_);
	static void deleteLogger();
};

//EXTERN_MGR(Logger);
extern Logger* ___Logger;

#define __THREADID__ ( static_cast<int>(std::this_thread::get_id()) % 1000 )
//#define __THREADID__ ( 0 )
#define __FILENAME__ ( strrchr(__FILE__,'\\') == 0 ? __FILE__ : strrchr(__FILE__,'\\') + 1 )
#define LOG(__level__, __desc__, ...) ___Logger->write(__level__, __FILENAME__, __LINE__, __THREADID__, __desc__, ##__VA_ARGS__)




}