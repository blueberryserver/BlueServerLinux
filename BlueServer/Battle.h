#pragma once
#include "BattleObj.h"
#include "cpp/common.pb.h"

namespace BLUE_BERRY
{

class Battle
{
public:
	Battle(std::vector<MSG::CharData_*>& chars_, int dungeonNo_, int tier_);
	virtual ~Battle();

	// dungeon play
	void play();

	const std::vector< MSG::BattleData_* >& getBattleData() { return _battleDatas; }

	const std::vector<BattleObj*>& getMonsterData() { return _enemy; }

private:
	bool isGameOver(MSG::BattleData_::Team team_);
	MSG::BattleData_::Team winner();

private:
	// 플레이어 캐릭터
	std::vector<BattleObj*> _ally;


	// 몬스터 캐릭터
	std::vector<BattleObj*> _enemy;


	// 전투 플레이 히스토리
	std::vector< MSG::BattleData_* > _battleDatas;
};


static Buff::BuffType String2BuffType(const std::string& typeStr_, const std::string& valueStr_ )
{
	if (typeStr_ == "Atk")
	{
		if (valueStr_.rfind("%") >= 0)
			return Buff::Atk_Percent;
		return Buff::Atk_Plus;
	}
	else if (typeStr_ == "Def")
	{
		if (valueStr_.rfind("%") >= 0)
			return Buff::Def_Percent;
		return Buff::Def_Plus;
	}
	else if (typeStr_ == "Hp")
	{
		if (valueStr_.rfind("%") >= 0)
			return Buff::Hp_Percent;
		return Buff::Hp_Plus;
	}
	return Buff::None;
}

static int String2BuffValue(const std::string& str_)
{
	if (str_.empty() == true) return 0;

	if (str_.rfind("%") >= 0)
	{
		//마지막 문자 제거
		auto str = str_.substr(0, str_.length() - 1);
		return std::atoi(str.c_str());
	}

	return std::atoi(str_.c_str());
}


//template< typename T>
//static decltype(auto) findJsonData(T& t_, int no_)
//{
//	for (auto i = 0; i < t_.data_size(); i++)
//	{
//		auto data = t_.mutable_data(i);
//		if (data->no() == no_)
//		{
//			return data;
//		}
//	}
//	return nullptr;
//}

}