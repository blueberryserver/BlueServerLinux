#include <string.h>

#include "DateTime.h"
#include "StringUtil.h"

namespace BLUE_BERRY
{

DateTime::DateTime() : _nanoSecTime(0) {}
DateTime::DateTime(const std::chrono::nanoseconds time_) : _nanoSecTime(time_) {}
DateTime::DateTime(const std::time_t time_)
{
	auto tp = std::chrono::system_clock::from_time_t(time_);
	_nanoSecTime = tp.time_since_epoch();
}
DateTime::DateTime(const std::chrono::system_clock::time_point time_)
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
	auto tp = std::chrono::system_clock::from_time_t(t);
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
	auto tp = std::chrono::system_clock::from_time_t(t);
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

_milliseconds DateTime::operator-(const DateTime & time_)
{
	return std::chrono::duration_cast<_milliseconds>(_nanoSecTime - time_._nanoSecTime);
}

std::string DateTime::format() const
{
	std::chrono::system_clock::time_point tp( std::chrono::duration_cast<_seconds>(_nanoSecTime));
	std::time_t time = std::chrono::system_clock::to_time_t(tp);
	tm* gmt = gmtime(&time);

	char buff[64] = { 0, };
	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d",
		gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
	return std::string(buff);
}

std::string DateTime::formatLocal() const
{
	std::chrono::system_clock::time_point tp(std::chrono::duration_cast<_seconds>(_nanoSecTime));
	std::time_t time = std::chrono::system_clock::to_time_t(tp);
	tm* gmt = localtime(&time);

	char buff[64] = { 0, };
	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d",
		gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
	return std::string(buff);
}

DateTime DateTime::getCurrentDateTime()
{
	return DateTime(std::chrono::system_clock::now());
}


int DateTime::getGMTOff()
{
	std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	return (int)( localtime(&t)->tm_gmtoff / 3600 );
}

int64_t DateTime::GetTickCount()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int64_t DateTime::GetTickCountM()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

std::ostream& operator<<(std::ostream& s_, const DateTime& time_)
{
	return s_ << time_.format();
}
}