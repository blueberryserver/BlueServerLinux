#pragma once
#include <atomic>
#include <stdarg.h>

namespace BLUE_BERRY
{

class DBQuery
{
public:
	DBQuery()
		: _status(0), _dataCount(0), _where("") {}

	// select
	virtual bool selectData() { return false; }

	// update
	virtual bool updateData() { return false; }

	// insert
	virtual bool insertData() { return false; }

	// delete
	virtual bool deleteData() { return false; }

	bool haveData()
	{
		return _dataCount > 0;
	}

	void setWhere(const char* format_, ...)
	{
		char buff[1024] = { 0, };
		va_list arg;
		va_start(arg, format_);
		vsprintf(buff, format_, arg);
		va_end(arg);

		_where = buff;
	}

protected:
	// update status(cached, none)
	std::atomic<int> _status;
	int _dataCount;

	std::string _where;
};



}