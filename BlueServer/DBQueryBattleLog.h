#pragma once
#include "cpp/common.pb.h"

#include "DBQuery.h"
namespace BLUE_BERRY 
{

class DBQueryBattleLog : public DBQuery
{
public:
	DBQueryBattleLog()
		: DBQuery()
	{

	}

	// select
	virtual bool selectData() override;

	// insert
	virtual bool insertData() override;

	// delete
	virtual bool deleteData() override;

	void setData(std::vector<MSG::DungeonPlayData_> data_)
	{
		_datas = data_;
	}

	void getData(std::vector<MSG::DungeonPlayData_>& data_)
	{
		data_ = _datas;
	}

private:

	std::vector<MSG::DungeonPlayData_> _datas;
};


}