#pragma once
#include <string>
#include <vector>

namespace BLUE_BERRY
{

inline void tokenize(const std::string& str_, std::vector<std::string>& tokens_, const std::string& delimiters_ = " ")
{
	// 맨 첫 글자가 구분자인 경우 무시
	std::string::size_type lastPos = str_.find_first_not_of(delimiters_, 0);
	// 구분자가 아닌 첫 글자를 찾는다
	std::string::size_type pos = str_.find_first_of(delimiters_, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// token을 찾았으니 vector에 추가한다
		tokens_.push_back(str_.substr(lastPos, pos - lastPos));
		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = str_.find_first_not_of(delimiters_, pos);
		// 다음 구분자가 아닌 글자를 찾는다
		pos = str_.find_first_of(delimiters_, lastPos);
	}
}

}
