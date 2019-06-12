#pragma once
#include "BattleObj.h"
#include "cpp/common.pb.h"

namespace BLUE_BERRY
{

class Battle
{
public:
	Battle() = default;
	Battle(std::vector<MSG::CharData_*>& chars_, unsigned int dungeonNo_, int tier_);
	virtual ~Battle();

	void addBattleObj(BattleObjPtr obj_, MSG::BattleData_::Team team_);

	void addBattleData(MSG::BattleData_* battleData_);

	// dungeon play
	void play();

	void replay();

	const std::vector< MSG::BattleData_* >& getBattleData() { return _battleDatas; }

	const std::vector<BattleObjPtr>& getMonsterData() { return _enemy; }

	int getReward() { return _reward; }

	MSG::BattleData_::Team winner();
private:
	bool isGameOver(MSG::BattleData_::Team team_);

private:
	unsigned int _dungeonNo;
	unsigned int _tier;
	int _reward;
	// 플레이어 캐릭터
	std::vector<BattleObjPtr> _ally;


	// 몬스터 캐릭터
	std::vector<BattleObjPtr> _enemy;


	// 전투 플레이 히스토리
	std::vector< MSG::BattleData_* > _battleDatas;
};

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