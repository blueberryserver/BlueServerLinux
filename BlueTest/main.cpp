#include <cstdio>
#include <gtest/gtest.h>

#include "Buffer_Test.h"

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