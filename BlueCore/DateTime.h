#pragma once
#include <string>
#include <chrono>

namespace BLUE_BERRY 
{
typedef std::chrono::duration<int64_t, std::ratio<24*60*60>> _days;
typedef std::chrono::hours _hours;
typedef std::chrono::minutes _minutes;
typedef std::chrono::seconds _seconds;
typedef std::chrono::milliseconds _milliseconds;
typedef std::chrono::microseconds _microseconds;

class DateTime
{
public:
	DateTime();
	DateTime(const std::chrono::nanoseconds time_);
	DateTime(const std::time_t time_);
	DateTime(const std::chrono::system_clock::time_point time_);
	DateTime(int year_, int month_, int day_, int hour_, int min_, int sec_, int millisec_);
	DateTime(const char* time_);
	DateTime(const DateTime& time_);
	~DateTime();

	bool operator==(const DateTime& time_);
	bool operator<(const DateTime& time_);
	bool operator>(const DateTime& time_);
	bool operator<=(const DateTime& time_);
	bool operator>=(const DateTime& time_);

	template<class T>
	DateTime operator+(const T span_)
	{
		return DateTime( _nanoSecTime + std::chrono::duration_cast<std::chrono::nanoseconds>(span_) );
	}
	template<class T>
	DateTime operator-(const T span_)
	{
		return DateTime( _nanoSecTime - std::chrono::duration_cast<std::chrono::nanoseconds>(span_) );
	}

	template<class T>
	DateTime& operator+=(const T span_)
	{
		_nanoSecTime += std::chrono::duration_cast<std::chrono::nanoseconds>(span_);
		return *this;
	}
	template<class T>
	DateTime& operator-=(const T span_)
	{
		_nanoSecTime -= std::chrono::duration_cast<std::chrono::nanoseconds>(span_);
		return *this;
	}

	_milliseconds operator-(const DateTime& time_);

	std::string format() const;
	std::string formatLocal() const;

	// static member
	static DateTime getCurrentDateTime();
	static int getGMTOff();
	static int64_t GetTickCount();
	static int64_t GetTickCountM();

	// friend 
	friend std::ostream& operator<<(std::ostream& s_, const DateTime& time_);
private:
	std::chrono::nanoseconds _nanoSecTime;

};

}