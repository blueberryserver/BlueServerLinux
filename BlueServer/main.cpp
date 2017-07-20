#include <iostream>
#include "../BlueCore/MemoryPool.h"
#include "../BlueCore/Server.h"
#include "../BlueCore/IOService.h"
#include "../BlueCore/Logger.h"

using namespace BLUE_BERRY;
int main(int argc, char *argv[])
{

	// alloc memory pool 
	MemoryPool::setMemoryPool(new MemoryPool());

	// create io service
	IOService::setIOService(new IOService(4));

	// create blogger
	Logger::setLogger(new Logger(T_CF_, L_ALL_, "BlueServerLog"));
	Logger::getLogger()->start();

	LOG(L_INFO_, "BlueServer Worker start", "thread count", 4);

	//server start
	Server server;
	server.start(12300);


	//main thread waiting
	getchar();

	LOG(L_INFO_, "BlueServer close");

	getchar();
	server.stop();

	Logger::getLogger()->stop();
	IOService::deleteIOService();
    return 0;
}