#include <iostream>
#include "../BlueCore/Server.h"
#include "../BlueCore/IOService.h"
#include "../BlueCore/Logger.h"

using namespace BLUE_BERRY;
int main(int argc, char *argv[])
{

	IOService::setIOService(new IOService(4));

	Logger::setLogger(new Logger(T_CF_, L_ALL_, "BlueServerLog"));
	Logger::getLogger()->start();

	LOG(L_INFO_, "BlueServer Worker start", "thread count", 4);

	//Server server;
	//server.start();

	//main thread waiting
	getchar();

	LOG(L_INFO_, "BlueServer close");

	getchar();

	Logger::getLogger()->stop();
	IOService::deleteIOService();
    return 0;
}