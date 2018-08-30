#pragma once
#include <boost/optional.hpp>

boost::optional<int> testOption(int i)
{
	if (i > 100) return 100;
	return {};
}

TEST(Boost, Optional)
{
	auto val = testOption(100);
	if (val) ASSERT_EQ(*val, 100);
}