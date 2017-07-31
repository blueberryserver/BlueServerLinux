#include "MysqlClient.h"

namespace BLUE_BERRY 
{

DEFINE_MGR(MysqlDriver)
MysqlDriver::~MysqlDriver()
{
	for (int i = 0; i < DB_SERVER_MAX; i++)
	{

		std::vector<sql::Connection*> temp;
		if (_pool[i].popAll(temp) == true)
		{
			for (auto it : temp)
			{
				(it)->close();
				delete it;
			}
		}
	}
}

bool MysqlDriver::connect(int serverNo_, const char * addr_, const char * user_, const char * pw_, const char* database_)
{
	if (serverNo_ > DB_SERVER_MAX || serverNo_ < 0) return false;

	for (int i = 0; i < _poolCount; i++)
	{
		//"tcp://127.0.0.1:3306", "root", "1234"
		auto con = _driver->connect(addr_, user_, pw_);
		_connAddrInfos[serverNo_] = DBServerAddr(addr_, user_, pw_, database_);

		if (con != nullptr)
		{
			con->setSchema(database_);
			_pool[serverNo_].push(con);
		}
	}
	return !_pool[serverNo_].empty();
}

sql::Connection* MysqlDriver::acquire( int serverNo_ )
{
	if (_driver == nullptr) return nullptr;

	sql::Connection* con = nullptr;
	if (_pool[serverNo_].pop(con) == true) return con;
	return nullptr;
}

void MysqlDriver::release(int serverNo_ , sql::Connection* con_)
{
	if (_pool[serverNo_].push(con_) == false)
	{
		// err
	}
}

void MysqlDriver::reConnect(int serverNo_, sql::Connection*& con_)
{
	con_->close();
	con_ = _driver->connect(_connAddrInfos[serverNo_]._addr, _connAddrInfos[serverNo_]._user, _connAddrInfos[serverNo_]._pw);
	con_->setSchema(_connAddrInfos[serverNo_]._dbName);
}

}