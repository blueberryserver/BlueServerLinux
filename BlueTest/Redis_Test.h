#pragma once
#include <gtest/gtest.h>

#include "../BlueCore/MemoryPool.h"
#include "../BlueCore/BufferPool.h"
#include "../BlueCore/IOService.h"
#include "../BlueCore/RedisClient.h"
#include "../BlueCore/RedisConnection.h"
#include "../BlueCore/Session.h"
#include "../BlueCore/Logger.h"
#include "../BlueCore/SyncJobHelper.h"


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

	RedisPool::setRedisPool(new RedisPool(1));
	RedisPool::getRedisPool()->connect(IOService::getIOService()->getIO(), 0, "127.0.0.1", "6379", 0);
	
	LOG(L_INFO_, "connect wait");
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void GlobalUnloading()
{
	RedisPool::deleteRedisPool();

	SyncJobManager::getSyncJobManager()->stop();
	SyncJobManager::deleteSyncJobManager();

	Logger::getLogger()->stop();
	Logger::deleteLogger();

	BufferPool::deleteBufferPool();
	MemoryPool::deleteMemoryPool();

	IOService::getIOService()->stop();

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	//IOService::deleteIOService();
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
	/*
	{

		RedisClientPtr rclient;

		LOG(L_INFO_, "connect wait");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		auto key = rclient->select(0);
		auto job = std::make_shared<SyncJobHelper>(key, nullptr, nullptr);
		SyncJobManager::getSyncJobManager()->addJob(job);


		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		auto replayLog = [](_RedisReply& reply_) -> void {
			LOG(L_INFO_, "Return2", "replay", reply_);
		};

		std::function<void(_RedisReply&)> f = RedisReplay2;

		auto key2 = rclient->hget("apk_hash_hash", "i1.2.5");
		auto post2 = makePostJobStatic(&RedisReplay2);

		auto job2 = std::make_shared<SyncJobHelper>(key2, post2, nullptr);
		SyncJobManager::getSyncJobManager()->addJob(job2);

		//
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	*/

	{
		//auto conn = std::make_shared< RedisConnection<Session> >(IOService::getIOService()->getIO());
		//auto result = conn->connect("127.0.0.1", "6379");

		RedisClientPtr rclient;

		std::future<_RedisReply> resultReply;
		auto r = rclient->select(1, std::ref(resultReply));

		auto reply = resultReply.get();
		{
			LOG(L_INFO_, "Future", "replay", reply);
		}

		r = rclient->hset("test_key", "f", "v", std::ref(resultReply));
		reply = resultReply.get();
		{
			LOG(L_INFO_, "Future", "replay", reply);
		}

		r = rclient->hget("test_key", "f", std::ref(resultReply));
		reply = resultReply.get();
		{
			LOG(L_INFO_, "Future", "replay", reply);
		}

	}

	GlobalUnloading();
}