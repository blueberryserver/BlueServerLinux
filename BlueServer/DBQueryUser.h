#pragma once
#include <atomic>
#include "cpp/common.pb.h"

namespace BLUE_BERRY
{

class DBQueryUser
{
public:
	DBQueryUser()
	: _dataCount(0), _status(0)
	{}

	// select
	bool selectData();

	// update
	bool updateData();

	// insert
	bool insertData();
	
	// delete
	bool deleteData();

	// set get function
	void setData(MSG::UserData_& data_)
	{
		_userData.CopyFrom(data_);
	}
	void getData(MSG::UserData_& data_)
	{
		data_.CopyFrom(_userData);
	}

	bool haveData()
	{
		return _dataCount > 0;
	}
private:
	// update status(cached, none)
	std::atomic<int> _status;

	int _dataCount;
	// db main data
	MSG::UserData_ _userData;

	// etc info data
};



}