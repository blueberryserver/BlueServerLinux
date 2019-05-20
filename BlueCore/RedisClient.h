#pragma once
#include <vector>
#include <string>

#include "Logger.h"
#include "BufferPool.h"
#include "Session.h"
#include "RedisHelper.h"
#include "RedisConnection.h"
#include "SyncJobHelper.h"
#include "Macro.h"

namespace BLUE_BERRY
{

class RedisPool
{
	enum { MAX_SERVER_SIZE = 10, };
	struct RedisServerAddr
	{
	public:
		RedisServerAddr() {}
		RedisServerAddr(const std::string& addr_, const std::string& port_, int db_)
			: _addr(addr_), _port(port_), _dbNo(db_) {}

		std::string _addr;
		std::string _port;
		int _dbNo;
	};

public:
	RedisPool(int poolCount_)
		: _poolCount(poolCount_) {}

	bool connect(boost::asio::io_context& io_, int serverNo_, const std::string& addr_, const std::string& port_, int db_)
	{
		_io = &io_;
		if (serverNo_ > MAX_SERVER_SIZE || serverNo_ < 0) return false;

		for (int i = 0; i < _poolCount; i++)
		{
			auto client = std::make_shared< RedisConnection<Session> >(io_);
			auto result = client->connect(addr_, port_);
			if (result == true)
			{
				_connAddrInfos[serverNo_] = RedisServerAddr(addr_, port_, db_);
				_pool[serverNo_].push(client);
			}
		}
		return !_pool[serverNo_].empty();
	}


	std::shared_ptr< RedisConnection<Session> > acquire(int serverNo_)
	{
		std::shared_ptr< RedisConnection<Session> > client;
		if (_pool[serverNo_].pop(client) == true) return client;

		client = std::make_shared< RedisConnection<Session> >(*_io);
		client->connect(_connAddrInfos[serverNo_]._addr.c_str(), _connAddrInfos[serverNo_]._port);
		return client;
	}

	void release(int serverNo_, std::shared_ptr< RedisConnection<Session> > client_)
	{
		if (client_->checkConnection() == false)
		{
			client_->close();
			client_->connect(_connAddrInfos[serverNo_]._addr.c_str(), _connAddrInfos[serverNo_]._port);
		}

		if (_pool[serverNo_].push(client_) == false)
		{
			// err
		}
	}


public:
	DECLARE_MGR(RedisPool)

private:
	boost::asio::io_context* _io;
	int _poolCount;
	RedisServerAddr _connAddrInfos[MAX_SERVER_SIZE];
	LockFreeQueue< std::shared_ptr< RedisConnection<Session> >, 65536 > _pool[MAX_SERVER_SIZE];

};

class RedisClientPtr
{
public:
	RedisClientPtr(int serverNo_ = 0)
		: _serverNo(serverNo_), _con(RedisPool::getRedisPool()->acquire(serverNo_)){}
	~RedisClientPtr()
	{
		RedisPool::getRedisPool()->release(_serverNo, _con);
	}
	
	std::shared_ptr< RedisConnection<Session> > operator->() { return _con; }
	std::shared_ptr< RedisConnection<Session> > get() { return _con; }
private:
	int _serverNo;
	std::shared_ptr< RedisConnection<Session> > _con;
};

}