#pragma once
#include <gtest/gtest.h>
#include <thread>
#include <mutex>


TEST(Mutex, Lock)
{
	std::mutex mtx;
	std::thread thread1( [&]()
	{
		for (auto i = 0; i < 10; i++)
		{
			mtx.lock();
			std::cout << std::this_thread::get_id() << " console write thread1 count: " << i << std::endl;
			mtx.unlock();
		}
	});


	std::thread thread2([&]()
	{
		for (auto i = 0; i < 20; i++)
		{
			mtx.lock();
			std::cout << std::this_thread::get_id() << " console write thread2 count: " << i << std::endl;
			mtx.unlock();
		}
	});

	std::thread thread3([&]()
	{
		for (auto i = 0; i < 5; i++)
		{
			mtx.lock();
			std::cout << std::this_thread::get_id() << " console write thread3 count: " << i << std::endl;
			mtx.unlock();
		}
	});

	if (thread1.joinable() == true) thread1.join();
	if (thread2.joinable() == true) thread2.join();
	if (thread3.joinable() == true) thread3.join();
}


TEST(Mutex, TryLock)
{
	std::mutex mtx;
	std::thread thread1([&]()
	{
		for (auto i = 0; i < 10; i++)
		{
			while(mtx.try_lock() == false)
			{
				std::cout << std::this_thread::get_id() << " wait try lock thread1 " << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			std::cout << std::this_thread::get_id() << " console write thread1 count: " << i << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			mtx.unlock();
		}
	});


	std::thread thread2([&]()
	{
		for (auto i = 0; i < 20; i++)
		{
			while (mtx.try_lock() == false)
			{
				std::cout << std::this_thread::get_id() << " wait try lock thread2 " << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
			std::cout << std::this_thread::get_id() << " console write thread2 count: " << i << std::endl;
			mtx.unlock();
		}
	});

	std::thread thread3([&]()
	{
		for (auto i = 0; i < 5; i++)
		{
			while (mtx.try_lock() == false)
			{
				std::cout << std::this_thread::get_id() << " wait try lock thread3 " << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
			std::cout << std::this_thread::get_id() << " console write thread3 count: " << i << std::endl;
			mtx.unlock();
		}
	});

	if (thread1.joinable() == true) thread1.join();
	if (thread2.joinable() == true) thread2.join();
	if (thread3.joinable() == true) thread3.join();
}

TEST(Mutex, LockGuard)
{

	std::mutex mtx;
	std::thread thread1([&]()
	{
		for (auto i = 0; i < 10; i++)
		{
			std::lock_guard<std::mutex> guard(mtx);
			std::cout << std::this_thread::get_id() << " console write thread1 count: " << i << std::endl;

		}
	});


	std::thread thread2([&]()
	{
		for (auto i = 0; i < 20; i++)
		{
			std::lock_guard<std::mutex> guard(mtx);
			std::cout << std::this_thread::get_id() << " console write thread2 count: " << i << std::endl;
		}
	});

	std::thread thread3([&]()
	{
		for (auto i = 0; i < 5; i++)
		{
			std::lock_guard<std::mutex> guard(mtx);
			std::cout << std::this_thread::get_id() << " console write thread3 count: " << i << std::endl;
		}
	});

	if (thread1.joinable() == true) thread1.join();
	if (thread2.joinable() == true) thread2.join();
	if (thread3.joinable() == true) thread3.join();
}

TEST(Mutex, RecursiveLock)
{
	std::recursive_mutex mtx;
	std::thread thread1([&]()
	{
		for (auto i = 0; i < 10; i++)
		{
			std::lock_guard<std::recursive_mutex> guard(mtx);
			std::cout << std::this_thread::get_id() << " console write thread1 count: " << i << std::endl;
			{
				std::lock_guard<std::recursive_mutex> guard(mtx);
				std::cout << std::this_thread::get_id() << " nested thread1 count: " << i << std::endl;
			}

		}
	});


	std::thread thread2([&]()
	{
		for (auto i = 0; i < 20; i++)
		{
			std::lock_guard<std::recursive_mutex> guard(mtx);
			std::cout << std::this_thread::get_id() << " console write thread2 count: " << i << std::endl;
			{
				std::lock_guard<std::recursive_mutex> guard(mtx);
				std::cout << std::this_thread::get_id() << " nested thread2 count: " << i << std::endl;
			}
		}
	});

	std::thread thread3([&]()
	{
		for (auto i = 0; i < 5; i++)
		{
			std::lock_guard<std::recursive_mutex> guard(mtx);
			std::cout << std::this_thread::get_id() << " console write thread3 count: " << i << std::endl;
			{
				std::lock_guard<std::recursive_mutex> guard(mtx);
				std::cout << std::this_thread::get_id() << " nested thread3 count: " << i << std::endl;
			}
		}
	});

	if (thread1.joinable() == true) thread1.join();
	if (thread2.joinable() == true) thread2.join();
	if (thread3.joinable() == true) thread3.join();
}

TEST(Mutex, TryLockFor)
{
	std::timed_mutex mtx;
	std::thread thread1([&]()
	{
		for (auto i = 0; i < 10; i++)
		{
			if(mtx.try_lock_for(std::chrono::microseconds(5)) == false)
			{
				std::cout << std::this_thread::get_id() << " try lock fail thread1 " << std::endl;
				continue;
			}
			std::cout << std::this_thread::get_id() << " console write thread1 count: " << i << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			mtx.unlock();
		}
	});


	std::thread thread2([&]()
	{
		for (auto i = 0; i < 20; i++)
		{
			if (mtx.try_lock_for(std::chrono::microseconds(5)) == false)
			{
				std::cout << std::this_thread::get_id() << " try lock fail thread2 " << std::endl;
				continue;
			}
			std::cout << std::this_thread::get_id() << " console write thread2 count: " << i << std::endl;
			mtx.unlock();
		}
	});

	std::thread thread3([&]()
	{
		for (auto i = 0; i < 5; i++)
		{
			if (mtx.try_lock_for(std::chrono::microseconds(5)) == false)
			{
				std::cout << std::this_thread::get_id() << " try lock fail thread3 " << std::endl;
				continue;
			}
			std::cout << std::this_thread::get_id() << " console write thread3 count: " << i << std::endl;
			mtx.unlock();
		}
	});

	if (thread1.joinable() == true) thread1.join();
	if (thread2.joinable() == true) thread2.join();
	if (thread3.joinable() == true) thread3.join();
}