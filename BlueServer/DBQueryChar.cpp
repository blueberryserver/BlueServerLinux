#include "DBQueryChar.h"
#include "../BlueCore/MysqlClient.h"
#include "../BlueCore/Logger.h"


namespace BLUE_BERRY
{


bool DBQueryChar::selectData()
{
	MysqlClient dbcon;

	try
	{
		std::string query = "SELECT cid, uid, slotNo, typeNo, level, tier, regDate FROM CharData WHERE ";
		query += _where;

		auto resultSet = dbcon.executeQuery(query.c_str());
		while(resultSet->next())
		{
			MSG::CharData_ data;
			data.set_cid(resultSet->getUInt64(1));
			data.set_uid(resultSet->getUInt64(2));

			data.set_slotno(resultSet->getUInt(3));
			data.set_typeno(resultSet->getUInt(4));
			data.set_level(resultSet->getUInt(5));
			data.set_tier(resultSet->getUInt(6));
			data.set_regdate(resultSet->getString(7));

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


// update
bool DBQueryChar::updateData()
{
	MysqlClient dbcon;

	try
	{
		int updateCount = 0;
		for (auto data : _datas)
		{
			auto pstmt = dbcon.preparedStatment("UPDATE CharData SET slotNo=?, typeNo=?, level=?, tier=?, regDate=? WHERE cid=?");
			pstmt->setUInt(1, data.slotno());
			pstmt->setUInt(2, data.typeno());
			pstmt->setUInt(3, data.level());
			pstmt->setUInt(4, data.tier());
			pstmt->setString(5, data.regdate());
			pstmt->setUInt64(6, data.cid());

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

// insert
bool DBQueryChar::insertData()
{
	MysqlClient dbcon;

	try
	{
		int insertCount = 0;
		for (auto data : _datas)
		{
			auto pstmt = dbcon.preparedStatment("INSERT INTO CharData(cid, uid, slotNo, typeNo, level, tier, regDate) VALUES(NULL, ?, ?, ?, ?, ?, ?)");
			pstmt->setUInt64(1, data.uid());
			pstmt->setUInt(2, data.slotno());
			pstmt->setUInt(3, data.typeno());
			pstmt->setUInt(4, data.level());
			pstmt->setUInt(5, data.tier());
			pstmt->setString(6, data.regdate());

			auto updateRows = pstmt->executeUpdate();
			if (updateRows > 0)
			{
				// change status

				auto resultSet = dbcon.executeQuery("SELECT LAST_INSERT_ID()");
				if (resultSet->next())
				{
					data.set_cid(resultSet->getUInt64(1));
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

// delete
bool DBQueryChar::deleteData()
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
			_dataCount -= updateRows;
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