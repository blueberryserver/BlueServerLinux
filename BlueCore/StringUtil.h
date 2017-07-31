#pragma once
#include <string>
#include <vector>

namespace BLUE_BERRY
{

inline void tokenize(const std::string& str_, std::vector<std::string>& tokens_, const std::string& delimiters_ = " ")
{
	// �� ù ���ڰ� �������� ��� ����
	std::string::size_type lastPos = str_.find_first_not_of(delimiters_, 0);
	// �����ڰ� �ƴ� ù ���ڸ� ã�´�
	std::string::size_type pos = str_.find_first_of(delimiters_, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// token�� ã������ vector�� �߰��Ѵ�
		tokens_.push_back(str_.substr(lastPos, pos - lastPos));
		// �����ڸ� �پ�Ѵ´�.  "not_of"�� �����϶�
		lastPos = str_.find_first_not_of(delimiters_, pos);
		// ���� �����ڰ� �ƴ� ���ڸ� ã�´�
		pos = str_.find_first_of(delimiters_, lastPos);
	}
}

}
