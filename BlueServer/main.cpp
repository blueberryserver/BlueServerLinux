#include <iostream>
#include "../BlueCore/MemoryPool.h"
#include "../BlueCore/IOService.h"
#include "../BlueCore/Logger.h"
#include "../BlueCore/Server.h"
#include "../BlueCore/MysqlClient.h"
#include "../BlueCore/SyncJobHelper.h"
#include "../BlueCore/RedisClient.h"
#include "../BlueCore/JsonFileLoader.h"

#include "BlueSession.h"
#include "UserManager.h"

#include "DefaultHandler.h"
#include "LoginHandler.h"
#include "ChatHandler.h"


using namespace BLUE_BERRY;
int main(int argc, char *argv[])
{
	// alloc memory pool 
	MemoryPool::setMemoryPool(new MemoryPool());

	// buffer pool
	BufferPool::setBufferPool(new BufferPool());

	JsonFileLoader::setJsonFileLoader(new JsonFileLoader());
	if (JsonFileLoader::getJsonFileLoader()->insert("../../../json/config.json") == false)
		return 0;

	auto config = JsonFileLoader::getJsonFileLoader()->get("config.json");

	auto jsonstr = config.dump();
	auto threadCount = config["server_info"]["io_thread"].int_value();
	auto tcpPort = config["server_info"]["tcp_port"].int_value();

	auto logLevel = config["server_info"]["logger_info"]["level"].string_value();
	auto logType = config["server_info"]["logger_info"]["type"].string_value();
	auto logName = config["server_info"]["logger_info"]["name"].string_value();

	// create io service
	IOService::setIOService(new IOService(threadCount));

	JsonFileLoader::getJsonFileLoader()->start();

	// create logger
	Logger::setLogger(new Logger(T_CF_, L_INFO_, logName.c_str()));
	Logger::getLogger()->setLogLevel(L_INFO_);
	Logger::getLogger()->start();

	SyncJobManager::setSyncJobManager(new SyncJobManager());
	SyncJobManager::getSyncJobManager()->start();

	auto dbPool = config["mysql"]["pool"].int_value();
	MysqlDriver::setMysqlDriver(new MysqlDriver(dbPool));

	auto dbConInfo = config["mysql"]["connect_info"].array_items();
	for (auto it : dbConInfo)
	{
		auto connectAddr = it["connect_addr"].string_value();
		auto database = it["database"].string_value();
		auto user = it["user"].string_value();
		auto pw = it["pw"].string_value();
		auto regionNo = it["region_no"].int_value();
		//MysqlDriver::getMysqlDriver()->connect(0, "tcp://ec2-52-79-55-103.ap-northeast-2.compute.amazonaws.com:3306", "root", "rlawoans!1234A", "blueberry");
		MysqlDriver::getMysqlDriver()->connect(regionNo, connectAddr.c_str(), user.c_str(), pw.c_str(), database.c_str());

		LOG(L_INFO_, "Mysql Info", "addr", connectAddr, "database", database, "region", regionNo);
	}

	auto redisPool = config["redis"]["pool"].int_value();
	RedisPool::setRedisPool(new RedisPool(redisPool));

	auto redisConInfo = config["redis"]["connect_info"].array_items();
	for (auto it : redisConInfo)
	{
		auto connectAddr = it["connect_addr"].string_value();
		auto databaseNo = it["database_no"].int_value();
		auto port = it["port"].int_value();
		auto regionNo = it["region_no"].int_value();

		RedisPool::getRedisPool()->connect(IOService::getIOService()->getIO(), regionNo, connectAddr.c_str(), (short)port, databaseNo);
		//RedisPool::getRedisPool()->connect(IOService::getIOService()->getIO(), 0, "127.0.0.1", 6379, 0);

		LOG(L_INFO_, "Redis Info", "addr", connectAddr, "port", port, "database", databaseNo, "region", regionNo);
	}

	LOG(L_INFO_, "BlueServer Worker start", "thread count", threadCount, "port", tcpPort);
	LOG(L_INFO_, "Logger Info", "name", logName, "level", logLevel, "type", logType);

	//server start
	auto server = new Server<BlueSession>();
	server->start((short)tcpPort);

	DefaultHandler::setDefaultHandler(new DefaultHandler());
	LoginHandler::setLoginHandler(new LoginHandler());
	ChatHandler::setChatHandler(new ChatHandler());

	UserManager::setUserManager(new UserManager());
	UserManager::getUserManager()->start();

	//main thread waiting
	getchar();

	LOG(L_INFO_, "BlueServer close");

	getchar();
	server->stop();

	UserManager::deleteUserManager();

	delete server;

	Logger::getLogger()->stop();
	IOService::deleteIOService();

	MemoryPool::deleteMemoryPool();
    return 0;
}