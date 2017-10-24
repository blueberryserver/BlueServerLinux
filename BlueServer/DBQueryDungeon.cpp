#include "DBQueryDungeon.h"
#include "../BlueCore/MysqlClient.h"
#include "../BlueCore/Logger.h"


namespace BLUE_BERRY
{

	bool DBQueryDungeon::selectData()
	{
		MysqlClient dbcon;

		try
		{
			std::string query = "SELECT uid, dungeonNo, dungeonTier, updateDate FROM DungeonData WHERE ";
			query += _where;

			auto resultSet = dbcon.executeQuery(query.c_str());
			if (resultSet->next())
			{
				MSG::DungeonData_ data;
				data.set_uid(resultSet->getUInt64(1));
				data.set_dungeonno(resultSet->getUInt(2));
				data.set_dungeontier(resultSet->getUInt(3));
				data.set_updatedate(resultSet->getString(4));

				_dataCount++;
				_datas.push_back(data);
				return true;
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


	// update
	bool DBQueryDungeon::updateData()
	{
		MysqlClient dbcon;

		try
		{
			int updateCount = 0;
			for (auto data : _datas)
			{
				auto pstmt = dbcon.preparedStatment("UPDATE DungeonData SET dungeonNo=?, dungeonTier=?, updateDate=? WHERE uid=?");
				pstmt->setUInt(1, data.dungeonno());
				pstmt->setUInt(2, data.dungeontier());
				pstmt->setString(3, data.updatedate());
				pstmt->setUInt64(4, data.uid());

				updateCount += pstmt->executeUpdate();
			}

			if (updateCount > 0)
				return true;

			insertData();
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

	// insert
	bool DBQueryDungeon::insertData()
	{
		MysqlClient dbcon;

		try
		{
			int updateCount = 0;
			for (auto data : _datas)
			{
				auto pstmt = dbcon.preparedStatment("INSERT INTO DungeonData(uid, dungeonNo, dungeonTier, updateDate) VALUES(?, ?, ?, ?)");
				pstmt->setUInt64(1, data.uid());
				pstmt->setUInt(2, data.dungeonno());
				pstmt->setUInt(3, data.dungeontier());
				pstmt->setString(4, data.updatedate());

				updateCount += pstmt->executeUpdate();
			}

			if (updateCount > 0)
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

	// delete
	bool DBQueryDungeon::deleteData()
	{
		MysqlClient dbcon;

		try
		{
			std::string query = "DELETE CharData WHERE ";
			query += _where;

			auto updateRows = dbcon.executeUpdate(query.c_str());
			if (updateRows > 0)
			{
				// change status
				_dataCount--;
				return true;
			}
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

}