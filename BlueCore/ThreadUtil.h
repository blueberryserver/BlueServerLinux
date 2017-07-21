#pragma once
#include <thread>
#include <boost/lexical_cast.hpp>

// casting thread id unsigned long
static int getThreadId() {
	std::string threadId = boost::lexical_cast<std::string>(std::this_thread::get_id());
	unsigned int threadNumber = 0;
	sscanf(threadId.c_str(), "%lx", &threadNumber);
	return static_cast<int>(threadNumber % 1000);
}
