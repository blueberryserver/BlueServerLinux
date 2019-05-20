#pragma once
#include <thread>
#include <iostream>
#include <gtest/gtest.h>
#include <boost/asio.hpp>

#include "../BlueCore/ThreadUtil.h"
#include "../BlueCore/IOService.h"
#include "../BlueCore/JobExec.h"
using namespace BLUE_BERRY;

template<typename Clock, typename Duration>
std::ostream &operator<<(std::ostream &stream,
	const std::chrono::time_point<Clock, Duration> &time_point) {
	const time_t time = Clock::to_time_t(time_point);

	return stream << time;
}


void print(const boost::system::error_code& e_)
{
	std::cout << getThreadId() << " Hello word 1 " << std::chrono::system_clock::now() << std::endl;
}

void print2(std::shared_ptr<boost::asio::deadline_timer>& t_,  const boost::system::error_code& e_)
{
	std::cout << getThreadId() << " Hello word 2 " << std::chrono::system_clock::now() << std::endl;
}


void GlobalLoading()
{
	IOService::setIOService(new IOService(4));
}

TEST(Timer, BoostTimer)
{
	GlobalLoading();

	// chrono to_time_t time_t
	auto time_point = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(time_point);
	std::cout << now_c << std::endl;

	boost::asio::io_context io;

		
		boost::asio::deadline_timer t1(io, boost::posix_time::seconds(2));
		t1.async_wait(&print);		



		boost::asio::deadline_timer t2(io, boost::posix_time::seconds(4));
		t2.async_wait(&print);



		boost::asio::deadline_timer t3(io, boost::posix_time::seconds(6));
		t3.async_wait(&print);



		boost::asio::deadline_timer t4(io, boost::posix_time::seconds(8));
		t4.async_wait(&print);

	
	io.run();

	//std::this_thread::sleep_for(std::chrono::seconds(10));
}

class TestTimer : public TimerExec
{
public:
	virtual ~TestTimer() {}

public:
	void print(int value_)
	{
		std::cout << getThreadId() << " Hello wordl " << value_ << " " << std::chrono::system_clock::now() << std::endl;
	}

	static void print2(int value_)
	{
		std::cout << getThreadId() << " Hello wordl 2 " << value_ << " " << std::chrono::system_clock::now() << std::endl;
	}

};


TEST(Timer, DoTimer)
{
	
	auto t = std::make_shared<TestTimer>();

	// 2000 ms 간격으로 계속 호출
	t->doTimer(2000, true, 
		[t]() { t->print(1111); }
	);

	// 1000 ms 이후 호출 종료
	t->doTimer(1000, false, 
		[t]() { t->print2(2222); }
	);

	std::this_thread::sleep_for(std::chrono::seconds(10));
}

class TestObj : public JobExec
{

public:
	void print( int value_)
	{
		std::cout << getThreadId() << " TestObj " << value_ << " " << std::chrono::system_clock::now() << std::endl;
	}

	static void print2(int value_)
	{
		std::cout << getThreadId() << " TestObj 2" << value_ << " " << std::chrono::system_clock::now() << std::endl;
	}
};

TEST(aync, DoAsync)
{
	auto t = std::make_shared<TestObj>();

	// 비동기 호출
	t->doAsync(
		[t]() { t->print(100); }
	);

	t->doAsync(
		[t]() { t->print2(200); }
	);

	std::this_thread::sleep_for(std::chrono::seconds(1));
}