#pragma once
#include <string>
#include <chrono>

namespace BLUE_BERRY 
{
using namespace std::chrono;

typedef duration<int64_t, std::ratio<24*60*60>> days;

class DateTime
{
public:
	DateTime();
	DateTime(const nanoseconds time_);
	DateTime(const std::time_t time_);
	DateTime(const system_clock::time_point time_);
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
		return DateTime( _nanoSecTime + duration_cast<nanoseconds>(span_) );
	}
	template<class T>
	DateTime operator-(const T span_)
	{
		return DateTime( _nanoSecTime - duration_cast<nanoseconds>(span_) );
	}

	template<class T>
	DateTime& operator+=(const T span_)
	{
		_nanoSecTime += duration_cast<nanoseconds>(span_);
		return *this;
	}
	template<class T>
	DateTime& operator-=(const T span_)
	{
		_nanoSecTime -= duration_cast<nanoseconds>(span_);
		return *this;
	}

	milliseconds operator-(const DateTime& time_);

	std::string format() const;
	std::string formatLocal() const;

	// static member
	static DateTime getCurrentDateTime();
	static int getGMTOff();
	static int64_t GetTickCount();
	static int64_t GetTickCountM();
	static uint32_t GetTickCount32();

	// friend 
	friend std::ostream& operator<<(std::ostream& s_, const DateTime& time_);
private:
	nanoseconds _nanoSecTime;

};

}