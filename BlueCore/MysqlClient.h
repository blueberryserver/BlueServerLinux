#pragma once
#include "Macro.h"
#include "LockFreeQueue.h"
#include "mysqldriver/mysql_driver.h"
#include "mysqldriver/mysql_connection.h"
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

namespace BLUE_BERRY
{

	
class MysqlDriver
{
	enum { DB_SERVER_MAX = 10, };

	struct DBServerAddr
	{
	public:
		DBServerAddr() {}
		DBServerAddr(const char* addr_, const char* user_, const char* pw_, const char* database_)
			: _addr(addr_), _user(user_), _pw(pw_), _dbName(database_) {}
		std::string _addr;
		std::string _user;
		std::string _pw;
		
		std::string _dbName;
	};

public:
	MysqlDriver(int poolCount_) 
		: _poolCount(poolCount_), _driver(sql::mysql::get_driver_instance()) {}
	~MysqlDriver();

	bool connect(int serverNo_, const char* addr_, const char* user_, const char* pw_, const char* database_ = "");

	sql::Connection* acquire(int serverNo_);
	void release(int serverNo_, sql::Connection* con_);

	void reConnect(int serverNo_, sql::Connection*& con_);
public:
	DECLARE_MGR(MysqlDriver)


public:
	static bool checkReconnect(sql::SQLException& e_)
	{
		// 2006 server has gone away
		// 2013 lost connection to mysql during query
		if (e_.getErrorCode() == 2006 || e_.getErrorCode() == 2013)
			return true;
		return false;
	}


private:
	int _poolCount;
	sql::mysql::MySQL_Driver* _driver;
	DBServerAddr _connAddrInfos[DB_SERVER_MAX];
	LockFreeQueue<sql::Connection*, 65536> _pool[DB_SERVER_MAX];

};
EXTERN_MGR(MysqlDriver)


typedef std::shared_ptr<sql::PreparedStatement>  PreparedStatementPtr;
typedef std::shared_ptr<sql::Statement>  StatementPtr;

typedef std::shared_ptr<sql::ResultSet>  ResultSetPtr;

class MysqlClient
{
public:
	MysqlClient(int serverNo_ = 0)
		: _serverNo(serverNo_), _con(MysqlDriver::getMysqlDriver()->acquire(serverNo_))
	{}


	~MysqlClient()
	{
		MysqlDriver::getMysqlDriver()->release(_serverNo, _con);
	}
	
	// check connection
	bool checkConnection()
	{
		return _con->isValid();
	}

	bool checkClosed()
	{
		return _con->isClosed();
	}

	// reconnect
	bool reconnect()
	{
		if (_con->reconnect() == false)
		{
			MysqlDriver::getMysqlDriver()->reConnect(_serverNo, _con);
			return true;
		}
		return true;
	}

	void close()
	{
		_con->close();
	}

	std::string schema()
	{
		return std::string(_con->getSchema());
	}

	std::string debugString()
	{
		return std::string(_con->getWarnings()->getMessage());
	}

	std::string lastQuery()
	{
		return _query;
	}

	ResultSetPtr executeQuery(const char* statement_)
	{
		_query = statement_;
		auto stmt = StatementPtr(_con->createStatement());
		if(stmt != nullptr ) return ResultSetPtr(stmt->executeQuery(statement_));
		return ResultSetPtr();
	}

	int executeUpdate(const char* statement_)
	{
		_query = statement_;
		auto stmt = StatementPtr(_con->createStatement());
		if (stmt != nullptr) return stmt->executeUpdate(statement_);
		return 0;
	}

	PreparedStatementPtr preparedStatment(const char* statement_)
	{
		_query = statement_;
		return PreparedStatementPtr(_con->prepareStatement(statement_));
	}

private:

	// auto commit
	// rollback
	// commit
	// query string
	// preparedstement pooling


	
private:
	int _serverNo;
	std::string _query;
	sql::Connection* _con;
};


}