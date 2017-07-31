#pragma once
#include <thread>
#include <iostream>
#include <gtest/gtest.h>


#include "../../BlueCore/Logger.h"
#include "../../BlueCore/ThreadUtil.h"
#include "../../BlueCore/IOService.h"
#include "../../BlueCore/JobExec.h"
using namespace BLUE_BERRY;

void GlobalLoadingLoggerTest()
{
	IOService::setIOService(new IOService(4));
}

TEST(Logger, Test)
{
	GlobalLoadingLoggerTest();
	Logger::setLogger(new Logger(T_CF_, L_ALL_, "BlueTestLog"));
	Logger::getLogger()->start();

	std::thread thread1([&]()
	{
		for (auto i = 0; i < 5; i++)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			LOG(L_INFO_, "Thread1", "tid", getThreadId());
		}
		
	});

	std::thread thread2([&]()
	{
		for( auto i = 0; i < 5; i++ )
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1230));
			LOG(L_INFO_, "Thread2", "tid", getThreadId());
		}

	});

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	if (thread1.joinable() == true) thread1.join();
	if (thread2.joinable() == true) thread2.join();


	Logger::getLogger()->stop();
	Logger::deleteLogger();

	IOService::getIOService()->stop();
	IOService::deleteIOService();
}
