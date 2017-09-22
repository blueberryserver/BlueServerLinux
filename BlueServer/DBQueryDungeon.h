#pragma once
#include "cpp/common.pb.h"

#include "DBQuery.h"

namespace BLUE_BERRY
{

	class DBQueryDungeon : public DBQuery
	{
	public:
		DBQueryDungeon()
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
		void setData(std::vector<MSG::DungeonData_> data_)
		{
			_datas = data_;
		}

		void getData(std::vector<MSG::DungeonData_>& data_)
		{
			data_ = _datas;
		}

	private:

		std::vector<MSG::DungeonData_> _datas;
	};



}