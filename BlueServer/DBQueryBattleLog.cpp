#include "DBQueryBattleLog.h"
#include <google/protobuf/util/json_util.h>
#include "../BlueCore/MysqlClient.h"
#include "../BlueCore/Logger.h"

#include <strstream>

namespace BLUE_BERRY
{

	
bool DBQueryBattleLog::selectData()
{
	MysqlClient dbcon;

	try
	{
		std::string query = "SELECT lid, uid, battleLog, regDate FROM BattleLog WHERE ";
		query += _where;

		auto resultSet = dbcon.executeQuery(query.c_str());
		while(resultSet->next())
		{
			MSG::DungeonPlayData_ data;
			{
				//select blob data
				auto blobData = resultSet->getBlob(3);

				blobData->seekg(0, std::ios::end);
				auto length = blobData->tellg();
				blobData->seekg(0, std::ios::beg);

				char* buff = new char[length];
				blobData->read(buff, length);

				std::string err;
				auto jdata = Json::parse(buff, err);
				google::protobuf::util::JsonStringToMessage(jdata.dump(), &data);
				delete[] buff;
			}

			data.set_lid(resultSet->getUInt64(1));
			data.set_uid(resultSet->getUInt64(2));
			data.set_regdate(resultSet->getString(4));

			_dataCount++;
			_datas.push_back(data);
		}

		return true;
	}
	catch (sql::SQLException& e)
	{
		if (MysqlDriver::checkReconnect(e) == true)
			dbcon.reconnect();
		LOG(L_INFO_, "query fail", "errorMsg", e.what(), "errorCode", e.getErrorCode(), "query", dbcon.lastQuery());
		return false;
	}

}

// insert
bool DBQueryBattleLog::insertData()
{
	MysqlClient dbcon;

	try
	{
		int insertCount = 0;
		for (auto data : _datas)
		{
			auto pstmt = dbcon.preparedStatment("INSERT INTO BattleLog(lid, uid, battleLog, regDate) VALUES(NULL, ?, ?, ?)");
			pstmt->setUInt64(1, data.uid());

			std::string out;
			google::protobuf::util::MessageToJsonString(data, &out);
			std::istringstream is(out);		
			pstmt->setBlob(2, &is);

			pstmt->setString(3, data.regdate());

			auto updateRows = pstmt->executeUpdate();
			if (updateRows > 0)
			{
				// change status
				auto resultSet = dbcon.executeQuery("SELECT LAST_INSERT_ID()");
				if (resultSet->next())
				{
					data.set_lid(resultSet->getUInt64(1));
					_dataCount++;
					insertCount += updateRows;
				}
			}
		}

		if (insertCount > 0)
			return true;
		return false;
	}
	catch (sql::SQLException& e)
	{
		if (MysqlDriver::checkReconnect(e) == true)
			dbcon.reconnect();
		LOG(L_INFO_, "query fail", "errorMsg", e.what(), "errorCode", e.getErrorCode(), "query", dbcon.lastQuery());
		return false;
	}
}

bool DBQueryBattleLog::deleteData()
{
	return false;
}


}