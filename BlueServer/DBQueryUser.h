#pragma once
#include "cpp/common.pb.h"

#include "DBQuery.h"

namespace BLUE_BERRY
{

class DBQueryUser : public DBQuery
{
public:
	DBQueryUser()
	: DBQuery() {}

	// select
	virtual bool selectData() override;

	// update
	virtual bool updateData() override;

	// insert
	virtual bool insertData() override;
	
	// delete
	virtual bool deleteData() override;

	// set get function
	void setData(MSG::UserData_& data_)
	{
		_data.CopyFrom(data_);
	}
	void getData(MSG::UserData_& data_)
	{
		data_.CopyFrom(_data);
	}


private:
	// db main data
	MSG::UserData_ _data;
};



}