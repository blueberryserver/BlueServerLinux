#pragma once
#include <gtest/gtest.h>
#include <thread>

#include "../BlueCore/MemoryPool.h"
#include "../BlueCore/BufferPool.h"
#include "../BlueCore/IOService.h"
#include "../BlueCore/LockFreeQueue.h"


using namespace BLUE_BERRY;

class Header
{
public:
	short _len;
	short _id;
};

TEST(Buffer, BufferHelper)
{
	MemoryPool::setMemoryPool(new MemoryPool());
	BufferPool::setBufferPool(new BufferPool());
	IOService::setIOService(new IOService(4));


	{
		LockFreeQueue<BufferHelperPtr> lfqueue;

		std::thread t([&](void) {

			//std::vector<BufferHelperPtr> v;
			for (int i = 0; i < 50000; i++)
			{
				BufferHelperPtr packet(new BufferHelper(4 + sizeof(Header)));

				auto header = packet->getHeader<Header>();
				header->_id = 1;
				header->_len = (short)0xfefe;

				// 패킷 바디 생성
				//auto body = packet->getBody<Header>();

				lfqueue.push(packet);

				if (i % 7482 == 0)
				{
					std::vector<BufferHelperPtr> v;
					if (lfqueue.popAll(v) == true)
					{
						v.clear();
					}
				}
			}

		});



		std::thread t1([&](void) {
			std::vector<BufferHelperPtr> v;
			for (int i = 0; i < 50000; i++)
			{
				BufferHelperPtr packet(new BufferHelper(4 + sizeof(Header)));

				auto header = packet->getHeader<Header>();
				header->_id = 1;
				header->_len = (short)0xeeee;

				// 패킷 바디 생성
				//auto body = packet->getBody<Header>();

				lfqueue.push(packet);

				if (i % 2346 == 0)
				{
					std::vector<BufferHelperPtr> v;
					if (lfqueue.popAll(v) == true)
					{
						v.clear();
					}
				}
			}
		});


		std::thread t2([&](void) {
			for (int i = 0; i < 50000; i++)
			{
				BufferHelperPtr packet(new BufferHelper(4 + sizeof(Header)));

				auto header = packet->getHeader<Header>();
				header->_id = 1;
				header->_len = 4;

				// 패킷 바디 생성
				//auto body = packet->getBody<Header>();

				lfqueue.push(packet);

				if (i % 2134 == 0)
				{
					std::vector<BufferHelperPtr> v;
					if (lfqueue.popAll(v) == true)
					{
						v.clear();
					}
				}

			}
		});

		std::thread t3([&](void) {
			for (int i = 0; i < 50000; i++)
			{
				BufferHelperPtr packet(new BufferHelper(4 + sizeof(Header)));

				auto header = packet->getHeader<Header>();
				header->_id = 1;
				header->_len = 4;

				// 패킷 바디 생성
				//auto body = packet->getBody<Header>();

				lfqueue.push(packet);

				if (i % 5679 == 0)
				{
					std::vector<BufferHelperPtr> v;
					if (lfqueue.popAll(v) == true)
					{
						v.clear();
					}
				}

			}
		});



		t.join();
		t1.join();
		t2.join();
		t3.join();
	}
}

