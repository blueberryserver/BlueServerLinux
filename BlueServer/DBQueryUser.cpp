#include "DBQueryUser.h"
#include "../BlueCore/MysqlClient.h"
#include "../BlueCore/Logger.h"


namespace BLUE_BERRY
{

bool DBQueryUser::selectData()
{
	MysqlClient dbcon;

	try
	{
		auto pstmt = dbcon.preparedStatment("SELECT uid, name, did FROM UserData WHERE name=?");
		pstmt->setString(1, _userData.name());

		auto resultSet = pstmt->executeQuery();
		if (resultSet->next())
		{
			_userData.set_uid(resultSet->getUInt64(1));
			_userData.set_name(resultSet->getString(2));
			_userData.set_did(resultSet->getString(3));
			_dataCount++;
			return true;
		}

		return true;
	}
	catch (sql::SQLException& e)
	{
		if (MysqlDriver::checkReconnect(e) == true)
			dbcon.reconnect();
		LOG(L_INFO_, "query fail", "errorMsg", e.what(), "errorCode", e.getErrorCode(), "query", dbcon.lastQuery());
		return true;
	}

}


// update
bool DBQueryUser::updateData()
{
	MysqlClient dbcon;

	try
	{
		auto pstmt = dbcon.preparedStatment("UPDATE UserData SET name=?, did=? WHERE uid=?");
		pstmt->setString(1, _userData.name());
		pstmt->setString(2, _userData.did());
		pstmt->setUInt64(3, _userData.uid());

		auto updateRows = pstmt->executeUpdate();
		if (updateRows > 0)
		{
			// change status
			return true;
		}
		return false;
	}
	catch (sql::SQLException& e)
	{
		if (MysqlDriver::checkReconnect(e) == true)
			dbcon.reconnect();
		LOG(L_INFO_, "query fail", "errorMsg", e.what(), "errorCode", e.getErrorCode(), "query", dbcon.lastQuery());
		return true;
	}
}

// insert
bool DBQueryUser::insertData()
{
	MysqlClient dbcon;

	try
	{
		auto pstmt = dbcon.preparedStatment("INSERT INTO UserData(uid, name, did) VALUES(NULL, ?, ?)");
		pstmt->setString(1, _userData.name());
		pstmt->setString(2, _userData.did());


		auto updateRows = pstmt->executeUpdate();
		if (updateRows > 0)
		{
			// change status

			auto resultSet = dbcon.executeQuery("SELECT LAST_INSERT_ID()");
			if (resultSet->next())
			{
				_userData.set_uid(resultSet->getUInt64(1));
				_dataCount++;
			}
			
			return true;
		}
		return false;
	}
	catch (sql::SQLException& e)
	{
		if (MysqlDriver::checkReconnect(e) == true)
			dbcon.reconnect();
		LOG(L_INFO_, "query fail", "errorMsg", e.what(), "errorCode", e.getErrorCode(), "query", dbcon.lastQuery());
		return true;
	}
}

// delete
bool DBQueryUser::deleteData()
{
	MysqlClient dbcon;

	try
	{
		auto pstmt = dbcon.preparedStatment("DELETE UserData WHERE uid=?");
		pstmt->setUInt64(1, _userData.uid());

		auto updateRows = pstmt->executeUpdate();
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
		return true;
	}
}

}