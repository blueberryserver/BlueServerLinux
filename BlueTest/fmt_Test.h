#pragma once
#include "../BlueCore/fmtlib/format.h"
#include <locale>

TEST(Fmt, Simple)
{
	//std::locale::global(std::locale("ko_KR.UTF-8"));
	setlocale(LC_ALL, "");

	std::string s = fmt::format("{0}{1}{0}", "abra", "___");
	std::cout << s << std::endl;

	std::string hangul = u8"ÇÑ±Û";
	std::cout << hangul << std::endl;
}