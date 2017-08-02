#pragma once
#include <gtest/gtest.h>
#include <iostream>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "../../BlueCore/mysqldriver/mysql_driver.h"
#include "../../BlueCore/mysqldriver/mysql_connection.h"

#include "../../BlueCore/DateTime.h"
#include "../../BlueCore/MysqlClient.h"

using namespace BLUE_BERRY;

TEST(Mysql, Connect)
{
	sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
	sql::Connection* con = driver->connect("tcp://ec2-52-79-55-103.ap-northeast-2.compute.amazonaws.com:3306", "root", "rlawoans!1234A");

	con->setSchema("doz2");

	sql::Statement* stmt = con->createStatement();
	sql::ResultSet* res = stmt->executeQuery("SELECT * FROM cw_itemtype");

	auto i = 0;
	while (res->next())
	{
		auto no = res->getInt("no");
		auto item_type = res->getInt("item_type");
		auto item_name = res->getString("item_name");
		auto disp_name = res->getString("disp_name");

		std::cout << "no: " << no << " item_type: " << item_type << " item_name: " << item_name << " dp_name: " << disp_name << std::endl;

		i++;
		if (i > 10) break;
	}

	con->close();
	delete con;
}



TEST(Mysql, Query)
{
	sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
	sql::Connection* con = driver->connect("tcp://ec2-52-79-55-103.ap-northeast-2.compute.amazonaws.com:3306", "root", "rlawoans!1234A");

	//con->setSchema("blueberry");
	//con->setAutoCommit(false);

	sql::Statement* stmt = con->createStatement();
	stmt->execute("USE blueberry");

	sql::PreparedStatement* prepStmt = con->prepareStatement("DROP TABLE IF EXISTS test");
	prepStmt->execute();

	prepStmt = con->prepareStatement("CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, label CHAR(1))");
	prepStmt->execute();

	//stmt->execute("START TRANSACTION;");

	prepStmt = con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)");
	prepStmt->setInt(1, 2);
	prepStmt->setString(2, "b");
	prepStmt->executeUpdate();

	//stmt->execute("COMMIT;");
	//stmt->execute("ROLLBACK;");


	//prepStmt = con->prepareStatement("INSERT INTO member(_name, _mail) VALUES( ?, ?)");
	//prepStmt->setString(1, "±è¼ø");
	//prepStmt->setString(2, "kimkun@nexon.co.kr");
	//prepStmt->executeUpdate();


	prepStmt = con->prepareStatement("CALL _member_find(?)");
	prepStmt->setString(1, "±è¼ø");
	auto  res = prepStmt->executeQuery();

	std::cout << "row count: " << res->rowsCount() << std::endl;

	while (res->next())
	{
		auto id = res->getInt64("_id");
		auto name = res->getString("_name");
		auto mail = res->getString("_mail");
		DateTime regDate = res->getString("_reg_date").c_str();

		std::cout << "id: " << id << " name: " << name << " mail: " << mail << regDate << std::endl;
	}

	delete prepStmt;
	con->close();
	delete con;

}


TEST(Mysql, MysqlDriver)
{
	MysqlDriver::setMysqlDriver(new MysqlDriver(4));

	// 0 -> doz2
	auto reault1 = MysqlDriver::getMysqlDriver()->connect(0, "tcp://ec2-52-79-55-103.ap-northeast-2.compute.amazonaws.com:3306", "root", "rlawoans!1234A", "doz2");

	// 1 -> blueberry
	auto reault2 = MysqlDriver::getMysqlDriver()->connect(1, "tcp://ec2-52-79-55-103.ap-northeast-2.compute.amazonaws.com:3306", "root", "rlawoans!1234A", "blueberry");


	MysqlClient conDoz2(0);
	auto preStmt = conDoz2.preparedStatment("SELECT * FROM cw_itemtype");
	{
		ResultSetPtr res(preStmt->executeQuery());

		auto i = 0;
		while (res->next())
		{
			auto no = res->getInt("no");
			auto item_type = res->getInt("item_type");
			auto item_name = res->getString("item_name");
			auto disp_name = res->getString("disp_name");

			std::cout << "no: " << no << " item_type: " << item_type << " item_name: " << item_name << " dp_name: " << disp_name << std::endl;

			i++;
			if (i > 10) break;
		}
	}

	MysqlClient conBlueberry(1);

	//conBlueberry.close();
	if (conBlueberry.checkConnection() == false)
	{
		auto reconnect = conBlueberry.reconnect();
		if (reconnect == false)
		{
			return;		
		}
	}

	try
	{

		{
			ResultSetPtr res = conBlueberry.executeQuery("SELECT * FROM test");
			while (res->next())
			{
				auto id = res->getInt("id");
				auto label = res->getString("label");

				std::cout << "id: " << id << " label: " << label << std::endl;
			}
		}
	}
	catch (sql::SQLException& e)
	{
		if (MysqlDriver::checkReconnect(e) == true)
		{
			conBlueberry.reconnect();
		}
		// error logging
		std::cout << "error msg: " << e.what() << " error code: " << e.getErrorCode() << " query: " << conBlueberry.lastQuery() << std::endl;

	}

}