#include <cstdio>
#include <gtest/gtest.h>

//#include "Buffer_Test.h"
//#include "Mutex_Test.h"
//#include "Timer_Test.h"
#include "Logger_Test.h"
//#include "Chrono_Test.h"
//#include "Protobuf_Test.h"
//#include "Mysql_Test.h"
#include "Redis_Test.h"

TEST(SAMPLE, TEST1)
{
	ASSERT_EQ(true, true);
	ASSERT_NE(true, false);
}


int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

}