#pragma once
#include <thread>
#include <sstream>

static int64_t getThreadId() 
{
	std::stringstream ss; ss << std::this_thread::get_id();
	return std::stoull(ss.str());
}

template<typename T>
using SharedPtr = std::shared_ptr<T>;
//ex) SharedPtr<Object> ptr;

#define LOCK_R(__m__) std::lock_guard<std::recursive_mutex> guard(__m__)
#define LOCK(__m__) std::lock_guard<std::mutex> guard(__m__)