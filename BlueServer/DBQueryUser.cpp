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
		auto pstmt = dbcon.preparedStatment("SELECT uid, name, did, platform, login_date, logout_date, reg_date, vc1, vc2, vc3, group_name, language FROM UserData WHERE name=?");
		pstmt->setString(1, _userData.name());

		auto resultSet = pstmt->executeQuery();
		if (resultSet->next())
		{
			_userData.set_uid(resultSet->getUInt64(1));
			_userData.set_name(resultSet->getString(2));
			_userData.set_did(resultSet->getString(3));
			_userData.set_platform(static_cast<MSG::PlatForm>(resultSet->getUInt(4)));
			_userData.set_login_date(resultSet->getString(5));
			_userData.set_logout_date(resultSet->getString(6));
			_userData.set_reg_date(resultSet->getString(7));
			_userData.set_vc1(resultSet->getUInt(8));
			_userData.set_vc2(resultSet->getUInt(9));
			_userData.set_vc3(resultSet->getUInt(10));
			_userData.set_group_name(resultSet->getString(11));
			_userData.set_language(resultSet->getString(12));

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
		auto pstmt = dbcon.preparedStatment("UPDATE UserData SET name=?, did=?, platform=?, login_date=?, logout_date=?, reg_date=?, vc1=?, vc2=?, vc3=?, group_name=?, language=? WHERE uid=?");
		pstmt->setString(1, _userData.name());
		pstmt->setString(2, _userData.did());
		pstmt->setUInt(3, _userData.platform());
		pstmt->setString(4, _userData.login_date());
		pstmt->setString(5, _userData.logout_date());
		pstmt->setString(6, _userData.reg_date());
		pstmt->setUInt(7, _userData.vc1());
		pstmt->setUInt(8, _userData.vc2());
		pstmt->setUInt(9, _userData.vc3());
		pstmt->setString(10, _userData.group_name());
		pstmt->setString(11, _userData.language());

		pstmt->setUInt64(12, _userData.uid());


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
		auto pstmt = dbcon.preparedStatment("INSERT INTO UserData(uid, name, did, platform, login_date, logout_date, reg_date, vc1, vc2, vc3, group_name, language) VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		pstmt->setString(1, _userData.name());
		pstmt->setString(2, _userData.did());
		pstmt->setUInt(3, _userData.platform());
		pstmt->setString(4, _userData.login_date());
		pstmt->setString(5, _userData.logout_date());
		pstmt->setString(6, _userData.reg_date());
		pstmt->setUInt(7, _userData.vc1());
		pstmt->setUInt(8, _userData.vc2());
		pstmt->setUInt(9, _userData.vc3());
		pstmt->setString(10, _userData.group_name());
		pstmt->setString(11, _userData.language());


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
		return false;
	}
}

}