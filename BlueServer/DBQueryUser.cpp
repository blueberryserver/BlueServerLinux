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
		std::string query = "SELECT uid, name, did, platform, loginDate, logoutDate, regDate, vc1, vc2, vc3, groupName, language FROM UserData WHERE ";
		query += _where;

		auto resultSet = dbcon.executeQuery(query.c_str());
		if (resultSet->next())
		{
			_data.set_uid(resultSet->getUInt64(1));
			_data.set_name(resultSet->getString(2));
			_data.set_did(resultSet->getString(3));
			_data.set_platform(static_cast<MSG::PlatForm>(resultSet->getUInt(4)));
			_data.set_logindate(resultSet->getString(5));
			_data.set_logoutdate(resultSet->getString(6));
			_data.set_regdate(resultSet->getString(7));
			_data.set_vc1(resultSet->getUInt(8));
			_data.set_vc2(resultSet->getUInt(9));
			_data.set_vc3(resultSet->getUInt(10));
			_data.set_groupname(resultSet->getString(11));
			_data.set_language(resultSet->getString(12));

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
		return false;
	}

}


// update
bool DBQueryUser::updateData()
{
	MysqlClient dbcon;

	try
	{
		auto pstmt = dbcon.preparedStatment("UPDATE UserData SET name=?, did=?, platform=?, loginDate=?, logoutDate=?, regDate=?, vc1=?, vc2=?, vc3=?, groupName=?, language=? WHERE uid=?");
		pstmt->setString(1, _data.name());
		pstmt->setString(2, _data.did());
		pstmt->setUInt(3, _data.platform());
		pstmt->setString(4, _data.logindate());
		pstmt->setString(5, _data.logoutdate());
		pstmt->setString(6, _data.regdate());
		pstmt->setUInt(7, _data.vc1());
		pstmt->setUInt(8, _data.vc2());
		pstmt->setUInt(9, _data.vc3());
		pstmt->setString(10, _data.groupname());
		pstmt->setString(11, _data.language());

		pstmt->setUInt64(12, _data.uid());


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
		return false;
	}
}

// insert
bool DBQueryUser::insertData()
{
	MysqlClient dbcon;

	try
	{
		auto pstmt = dbcon.preparedStatment("INSERT INTO UserData(uid, name, did, platform, loginDate, logoutDate, regDate, vc1, vc2, vc3, groupName, language) VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		pstmt->setString(1, _data.name());
		pstmt->setString(2, _data.did());
		pstmt->setUInt(3, _data.platform());
		pstmt->setString(4, _data.logindate());
		pstmt->setString(5, _data.logoutdate());
		pstmt->setString(6, _data.regdate());
		pstmt->setUInt(7, _data.vc1());
		pstmt->setUInt(8, _data.vc2());
		pstmt->setUInt(9, _data.vc3());
		pstmt->setString(10, _data.groupname());
		pstmt->setString(11, _data.language());


		auto updateRows = pstmt->executeUpdate();
		if (updateRows > 0)
		{
			// change status

			auto resultSet = dbcon.executeQuery("SELECT LAST_INSERT_ID()");
			if (resultSet->next())
			{
				_data.set_uid(resultSet->getUInt64(1));
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
		return false;
	}
}

// delete
bool DBQueryUser::deleteData()
{
	MysqlClient dbcon;

	try
	{
		auto pstmt = dbcon.preparedStatment("DELETE UserData WHERE uid=?");
		pstmt->setUInt64(1, _data.uid());

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
		return false;
	}
}

}