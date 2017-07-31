#pragma once
#include <gtest/gtest.h>

#include "../../BlueCore/MemoryPool.h"
#include "../../BlueCore/BufferPool.h"
#include "../../BlueCore/IOService.h"
#include "../../BlueCore/RedisClient.h"
#include "../../BlueCore/Session.h"
#include "../../BlueCore/Logger.h"
#include "../../BlueCore/SyncJobHelper.h"


using namespace BLUE_BERRY;


void GlobalLoading()
{
	IOService::setIOService(new IOService(4));

	MemoryPool::setMemoryPool(new MemoryPool());

	BufferPool::setBufferPool(new BufferPool());

	SyncJobManager::setSyncJobManager(new SyncJobManager());
	SyncJobManager::getSyncJobManager()->start();

	//Logger::setLogger(new Logger(T_CF_, L_ALL_, "BlueTestLog"));
	Logger::setLogger(new Logger(T_CF_, L_INFO_, "BlueTestLog"));
	Logger::getLogger()->start();
	
}

void GlobalUnloading()
{
	SyncJobManager::getSyncJobManager()->stop();
	SyncJobManager::deleteSyncJobManager();

	Logger::getLogger()->stop();
	Logger::deleteLogger();

	BufferPool::deleteBufferPool();
	MemoryPool::deleteMemoryPool();

	IOService::getIOService()->stop();
	IOService::deleteIOService();
}


static void RedisReplay(_RedisReply reply_)
{
	LOG(L_INFO_, "Return", "replay", reply_);
}

static void RedisReplay2(_RedisReply reply_)
{
	LOG(L_INFO_, "Return2", "replay", reply_);
}

TEST(Redis, Simple)
{
	GlobalLoading();
	{

		RedisClient<Session> rclient(IOService::getIOService()->getIO());
		rclient.connect("52.79.55.103", 6379);
		//rclient.connect("127.0.0.1", 12300);

		LOG(L_INFO_, "connect wait");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		auto key = rclient.select(0);

		auto post = makePostJobStatic(&RedisReplay);
		auto job = std::make_shared<SyncJobHelper>(key, post, nullptr);
		SyncJobManager::getSyncJobManager()->addJob(job);


		std::this_thread::sleep_for(std::chrono::milliseconds(2000));


		auto key2 = rclient.hget("apk_hash_hash", "i1.2.5");
		auto post2 = makePostJobStatic(&RedisReplay2);
		auto job2 = std::make_shared<SyncJobHelper>(key2, post2, nullptr);
		SyncJobManager::getSyncJobManager()->addJob(job2);

		//
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		rclient.close();
	}

	GlobalUnloading();
}