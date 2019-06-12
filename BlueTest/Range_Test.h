#pragma once

#include <array>
#include <range/v3/all.hpp>
using namespace ranges;

TEST(RANGE, sort)
{
	std::vector<int> v{ 4,5,6,7,1,2,3, 9 , 10 };
	sort(v);
	std::cout << view::all(v) << std::endl;
}


TEST(RANGE, view)
{
	// count
	{
		std::vector<int> v{ 1,1,2,2,3,3,3,4,4,4,5,5,5,6,6,6 };
		auto vc = count(v, 6);
		std::cout << vc << std::endl;

		std::array<int, 16> a{ 1,1,2,2,3,3,3,4,4,4,5,5,5,6,6,6 };
		auto ac = count(a, 6);
		std::cout << ac << std::endl;
	}

	// count_if
	{
		auto isSix = [](int i) -> bool { return i == 6; };
		std::vector<int> v{ 1,1,2,2,3,3,3,4,4,4,5,5,5,6,6,6 };
		auto vc = count_if(v, isSix);
		std::cout << vc << std::endl;

		std::array<int, 16> a{ 1,1,2,2,3,3,3,4,4,4,5,5,5,6,6,6 };
		auto ac = count_if(a, isSix);
		std::cout << ac << std::endl;
	}

	// find, find_if, find_if_not
	{
		std::vector<int> v{ 1,1,2,2,3,3,3,4,4,4,5,5,5,6,6,6 };
		auto isSix = [](int i) -> bool { return i == 6; };

		{
			auto it = find(v, 6);
			std::cout << *it << std::endl;
		}
		{
			auto it = find(v, 10);
			if (it == end(v))
			{
				std::cout << "didn't not found 10" << std::endl;
			}
		}
		{
			auto it = find_if(v, isSix);
			if (it != end(v))
			{
				std::cout << *it << std::endl;
			}
		}

		{
			auto it = find_if_not(v, isSix);
			for (; it != end(v); it++)
			{
				std::cout << *it << std::endl;
			}
		}

	}



	{
		std::vector<int> vi{ 1,2,3,4,5,6,7,8,9,10 };
		auto rng = vi | view::remove_if([](int i) {return i % 2 == 1; })
			| view::transform([](int i) {return std::to_string(i); });

		std::cout << rng << std::endl;
	}

	{
		int sum = accumulate(view::ints(1)
			| view::transform([](int i) {return i * i; })
			| view::take(10), 0);

		std::cout << sum << std::endl;
	}

	{
		std::vector<int> vi =
			view::for_each(view::ints(1, 10), [](int i) {
			return yield_from(view::repeat_n(i, i));
				});

		std::cout << view::all(vi) << std::endl;
	}

	{
		auto vi = view::ints(1)
			| view::take(10);

		std::cout << vi << std::endl;
	}
}