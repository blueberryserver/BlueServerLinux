#include <string.h>
#include "fmtlib/format.h"
#include "DateTime.h"
#include "StringUtil.h"

namespace BLUE_BERRY
{

DateTime::DateTime() : _nanoSecTime(0) {}
DateTime::DateTime(const nanoseconds time_) : _nanoSecTime(time_) {}
DateTime::DateTime(const std::time_t time_)
{
	auto tp = system_clock::from_time_t(time_);
	_nanoSecTime = tp.time_since_epoch();
}
DateTime::DateTime(const system_clock::time_point time_)
{
	_nanoSecTime = time_.time_since_epoch();
}

DateTime::DateTime(int year_, int month_, int day_, int hour_, int min_, int sec_, int millisec_)
{
	tm time;
	memset(&time, 0, sizeof(time));
	time.tm_year = year_ - 1900;
	time.tm_mon = month_ - 1;
	time.tm_mday = day_;
	time.tm_hour = hour_;
	time.tm_min = min_;
	time.tm_sec = sec_;
	auto t = mktime(&time);
	auto tp = system_clock::from_time_t(t);
	_nanoSecTime = tp.time_since_epoch();
}

DateTime::DateTime(const char* time_)
{
	tm time;
	memset(&time, 0, sizeof(time));

	// format %Y-%m-%d %H:%M:%S
	std::vector<std::string> tokens;
	tokenize(std::string(time_), tokens);
	if (tokens.size() == 2)
	{
		// date
		std::vector<std::string> dates;
		tokenize(tokens[0], dates, "-");
		if (dates.size() == 3)
		{
			time.tm_year = std::stoi(dates[0]) - 1900;
			time.tm_mon = std::stoi(dates[1]) - 1;
			time.tm_mday = std::stoi(dates[2]);
		}
		// time
		std::vector<std::string> times;
		tokenize(tokens[1], times, ":");
		if (times.size() == 3)
		{
			time.tm_hour = std::stoi(times[0]);
			time.tm_min = std::stoi(times[1]);
			time.tm_sec = std::stoi(times[2]);
		}
	}
	auto t = mktime(&time);
	auto tp = system_clock::from_time_t(t);
	_nanoSecTime = tp.time_since_epoch();

}


DateTime::DateTime(const DateTime& time_) : _nanoSecTime(time_._nanoSecTime)
{

}

DateTime::~DateTime()
{
}


bool DateTime::operator==(const DateTime& time_)
{
	return _nanoSecTime == time_._nanoSecTime;
}
bool DateTime::operator<(const DateTime& time_)
{
	return _nanoSecTime < time_._nanoSecTime;
}
bool DateTime::operator>(const DateTime& time_)
{
	return _nanoSecTime > time_._nanoSecTime;
}
bool DateTime::operator<=(const DateTime& time_)
{
	return _nanoSecTime <= time_._nanoSecTime;
}
bool DateTime::operator>=(const DateTime& time_)
{
	return _nanoSecTime >= time_._nanoSecTime;
}

milliseconds DateTime::operator-(const DateTime & time_)
{
	return duration_cast<milliseconds>(_nanoSecTime - time_._nanoSecTime);
}

std::string DateTime::format() const
{
	system_clock::time_point tp( duration_cast<seconds>(_nanoSecTime));
	std::time_t time = system_clock::to_time_t(tp);
	tm* t = gmtime(&time);

	return fmt::format("{0:04}-{1:02}-{2:02} {3:02}:{4:02}:{5:02}",
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

std::string DateTime::formatLocal() const
{
	system_clock::time_point tp(duration_cast<seconds>(_nanoSecTime));
	std::time_t time = system_clock::to_time_t(tp);
	tm* t = localtime(&time);

	return fmt::format("{0:04}-{1:02}-{2:02} {3:02}:{4:02}:{5:02}", 
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

DateTime DateTime::getCurrentDateTime()
{
	return DateTime(system_clock::now());
}


int DateTime::getGMTOff()
{
	std::time_t t = system_clock::to_time_t(system_clock::now());
	return (int)( localtime(&t)->tm_gmtoff / 3600 );
}

int64_t DateTime::GetTickCount()
{
	return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

int64_t DateTime::GetTickCountM()
{
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

uint32_t DateTime::GetTickCount32()
{
	auto tick = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	tick <<= 32; tick >>= 32;
	return static_cast<uint32_t>(tick);
}


std::ostream& operator<<(std::ostream& s_, const DateTime& time_)
{
	return s_ << time_.format();
}
}