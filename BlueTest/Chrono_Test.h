#pragma once
#include <iostream>
#include <chrono>
#include <gtest/gtest.h>
#include <ctime>
#include <thread>

#include "../BlueCore/DateTime.h"
using namespace BLUE_BERRY;

TEST(Chrono, Simple)
{
	
	
	std::chrono::hours h(1);
	std::chrono::minutes m = h;
	std::cout << h.count() << std::endl;
	std::cout << m.count() << std::endl;

	// 날짜 표시 타입 정의
	typedef std::chrono::duration<int64_t, std::ratio<86400>>  days;
	days d(1);
	std::chrono::seconds s = d;


	std::cout << d.count() << std::endl;
	std::cout << s.count() << std::endl;

	std::chrono::hours h2 = d;
	std::cout << h2.count() << std::endl;


	// 초를 분으로 변경
	std::chrono::seconds s2(350);
	std::chrono::minutes m2 = std::chrono::duration_cast<std::chrono::minutes>(s2);

	std::chrono::seconds rest = s2 - m2;
	std::cout << m2.count() << " second: " << rest.count() << std::endl;


	std::cout << " 120 second: " << std::chrono::duration_cast<_microseconds>(_minutes(2)).count() << std::endl;

}

TEST(Chrono, Now)
{
	// 현재 시간 포인트
	auto tp = std::chrono::system_clock::now();	// static method call


	std::chrono::nanoseconds nanosec = tp.time_since_epoch();	// 1970-01-01 00:00:00 nano sec 리턴
	std::chrono::seconds     sec = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()); // 1970-01-01 00:00:00 sec 리턴

	std::cout << nanosec.count() << " second: " << sec.count() << std::endl;


	std::time_t t = std::chrono::system_clock::to_time_t(tp);
	std::string s = ctime(&t);					// 2. struct time_t => char*
	std::cout << s << std::endl;


	tm* time = gmtime(&t);

	char buff[64] = { 0, };
	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d %s %d",
		time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec, time->tm_zone, time->tm_gmtoff / 3600);

	std::cout << buff << std::endl;


	tm* ltime = localtime(&t);

	char buff2[64] = { 0, };
	sprintf(buff2, "%04d-%02d-%02d %02d:%02d:%02d %s %d",
		ltime->tm_year + 1900, ltime->tm_mon + 1, ltime->tm_mday, ltime->tm_hour, ltime->tm_min, ltime->tm_sec, ltime->tm_zone, time->tm_gmtoff / 3600);

	std::cout << buff2 << std::endl;

}


TEST(DateTime, Simple)
{
	DateTime t = DateTime::getCurrentDateTime();
	std::cout << "now: " << t.format() << " local: " << t.formatLocal() << " " << DateTime::getGMTOff() << std::endl;

	t += _hours(1);
	std::cout << "after 1hours: " << t << " local: " << t.formatLocal() << std::endl;


	t -= _minutes(30);
	std::cout << "before 30minutes : " << t << " local: " << t.formatLocal() << std::endl;


	t = DateTime::getCurrentDateTime();
	t -= _days(30);
	std::cout << "before 1mounths : " << t << " local: " << t.formatLocal() << std::endl;

	auto t2 = DateTime::getCurrentDateTime();
	std::this_thread::sleep_for(std::chrono::milliseconds(1235));
	std::cout << " estimate time: " << (DateTime::getCurrentDateTime() - t2).count() << std::endl;
}

TEST(Chrono, 32)
{
	auto tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	std::cout << "tick :" << DateTime::GetTickCount32() << " " << DateTime::GetTickCountM() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1200));
	std::cout << "tick :" << DateTime::GetTickCount32() << " " << DateTime::GetTickCountM() << std::endl;

	auto ticksec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	std::cout << "tick nano :" << std::make_shared<DateTime>(std::chrono::system_clock::now().time_since_epoch())->format() << std::endl;
	std::cout << "tick sec :" << std::make_shared<DateTime>(ticksec)->format() << std::endl;

	ASSERT_EQ(tick, ticksec);
}