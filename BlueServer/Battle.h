#pragma once
#include "BattleObj.h"
#include "cpp/common.pb.h"

namespace BLUE_BERRY
{

class Battle
{
public:
	Battle();
	Battle(std::vector<MSG::CharData_*>& chars_, int dungeonNo_, int tier_);
	virtual ~Battle();

	void addBattleObj(BattleObj* obj_, MSG::BattleData_::Team team_);

	void addBattleData(MSG::BattleData_* battleData_);

	// dungeon play
	void play();

	void replay();

	const std::vector< MSG::BattleData_* >& getBattleData() { return _battleDatas; }

	const std::vector<BattleObj*>& getMonsterData() { return _enemy; }

	int getReward() { return _reward; }

	MSG::BattleData_::Team winner();
private:
	bool isGameOver(MSG::BattleData_::Team team_);

private:
	int _dungeonNo;
	int _tier;
	int _reward;
	// �÷��̾� ĳ����
	std::vector<BattleObj*> _ally;


	// ���� ĳ����
	std::vector<BattleObj*> _enemy;


	// ���� �÷��� �����丮
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