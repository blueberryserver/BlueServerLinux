#pragma once
#include "cpp/common.pb.h"

#include "DBQuery.h"
namespace BLUE_BERRY
{

class DBQueryChar : public DBQuery
{
public:
	DBQueryChar()
		: DBQuery()
	{
		_datas.clear();
	}

	// select
	virtual bool selectData() override;

	// update
	virtual bool updateData() override;

	// insert
	virtual bool insertData() override;

	// delete
	virtual bool deleteData() override;

	// set get function
	void setData(std::vector<MSG::CharData_> data_)
	{
		_datas = data_;
	}

	void getData(std::vector<MSG::CharData_>& data_)
	{
		data_ = _datas;
	}

private:

	std::vector<MSG::CharData_> _datas;
};


}