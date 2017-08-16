#include <iostream>
#include "../BlueCore/MemoryPool.h"
#include "../BlueCore/IOService.h"
#include "../BlueCore/Logger.h"
#include "../BlueCore/Server.h"
#include "../BlueCore/MysqlClient.h"
#include "../BlueCore/SyncJobHelper.h"
#include "../BlueCore/RedisClient.h"

#include "BlueSession.h"
#include "LoginHandler.h"


using namespace BLUE_BERRY;
int main(int argc, char *argv[])
{

	// alloc memory pool 
	MemoryPool::setMemoryPool(new MemoryPool());

	// buffer pool
	BufferPool::setBufferPool(new BufferPool());

	// create io service
	IOService::setIOService(new IOService(4));

	// create blogger
	Logger::setLogger(new Logger(T_CF_, L_INFO_, "BlueServerLog"));
	Logger::getLogger()->setLogLevel(L_INFO_);
	Logger::getLogger()->start();

	SyncJobManager::setSyncJobManager(new SyncJobManager());
	SyncJobManager::getSyncJobManager()->start();


	MysqlDriver::setMysqlDriver(new MysqlDriver(4));
	MysqlDriver::getMysqlDriver()->connect(0, "tcp://ec2-52-79-55-103.ap-northeast-2.compute.amazonaws.com:3306", "root", "rlawoans!1234A", "blueberry");

	RedisPool::setRedisPool(new RedisPool(1));
	RedisPool::getRedisPool()->connect(IOService::getIOService()->getIO(), 0, "127.0.0.1", 6379, 0);

	LOG(L_INFO_, "BlueServer Worker start", "thread count", 4);

	//server start
	auto server = new Server<BlueSession>();
	server->start(12300);

	BlueSession::setMsgHandler(new LoginHandler());


	//main thread waiting
	getchar();

	LOG(L_INFO_, "BlueServer close");

	getchar();
	server->stop();
	delete server;

	Logger::getLogger()->stop();
	IOService::deleteIOService();
    return 0;
}