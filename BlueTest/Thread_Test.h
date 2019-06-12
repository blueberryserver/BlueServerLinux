#pragma once
#include <thread>

TEST(Thread, cpu)
{
	std::cout << "cpu: " << std::thread::hardware_concurrency() << std::endl;
}
