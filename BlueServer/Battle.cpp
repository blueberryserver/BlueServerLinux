#include <random>
#include <algorithm>

#include "../BlueCore/JsonFileLoader.h"
#include "../BlueCore/Logger.h"

#include "cpp/SimTable.pb.h"

#include "Battle.h"
namespace BLUE_BERRY
{

BLUE_BERRY::Battle::Battle(std::vector<MSG::CharData_*>& chars_, int dungeonNo_, int tier_)
{
	auto table = JsonFileLoader::getJsonFileLoader()->get("SimTable.json");

	// ally 
	MSG::Char character; json2Proto(table["Char"], character);
	MSG::Char::CharacterTable* characterTable = nullptr;
	for (auto it : chars_)
	{
		characterTable = nullptr;
		for (auto i = 0; i < character.data_size(); i++)
		{
			auto data = character.mutable_data(i);
			if (data->no() == it->typeno())
			{
				characterTable = data;
				break;
			}
		}

		if(characterTable == nullptr) continue;

		std::string StatTableName;
		StatTableName += characterTable->name() + "Stat";

		MSG::WarriorStat stat; json2Proto(table[StatTableName.c_str()], stat);
		MSG::WarriorStat::WarriorStatTable* statTable = nullptr;
		for (auto i = 0; i < stat.data_size(); i++)
		{
			auto data = stat.mutable_data(i);
			if (data->level() == it->level())
			{
				statTable = data;
				break;
			}
		}

		if (statTable == nullptr) continue;

		MSG::CharTier tier; json2Proto(table["CharTier"], tier);
		MSG::CharTier::CharTierTable* charTierTable = nullptr;
		for (auto i = 0; i < tier.data_size(); i++)
		{
			auto data = tier.mutable_data(i);
			if (data->tier() == it->tier())
			{
				charTierTable = data;
				break;
			}
		}

		BattleObj* obj = new BattleObj(statTable->level(), characterTable->no(), statTable->atk(), statTable->def(), statTable->hp(), characterTable->attackrange());
		_ally.push_back(obj);

		if (charTierTable == nullptr) continue;
		obj->addBuff(
		{ 
			{ String2BuffType(charTierTable->buff1type(), charTierTable->buff1value()), String2BuffValue(charTierTable->buff1value()) },
			{ String2BuffType(charTierTable->buff2type(), charTierTable->buff2value()), String2BuffValue(charTierTable->buff2value()) },
			{ String2BuffType(charTierTable->buff3type(), charTierTable->buff3value()), String2BuffValue(charTierTable->buff3value()) },
		}
		);
	}

	// enemy
	MSG::Dungeon dungeon; json2Proto(table["Dungeon"], dungeon);
	MSG::Dungeon::DungeonTable* dungeonTable = nullptr;
	for (auto i = 0; i < dungeon.data_size(); i++)
	{
		auto data = dungeon.mutable_data(i);
		if (data->no() == dungeonNo_)
		{
			dungeonTable = data;
			break;
		}
	}

	if (dungeonTable == nullptr) return;


	MSG::MobAStat mobAStat; json2Proto(table["MobAStat"], mobAStat);
	MSG::MobAStat::MobAStatTable* mobAStatTable = nullptr;
	for (auto i = 0; i < mobAStat.data_size(); i++)
	{
		auto data = mobAStat.mutable_data(i);
		if (data->level() == dungeonTable->mobalv())
		{
			mobAStatTable = data;
			break;
		}
	}

	MSG::MobAStat mobBStat; json2Proto(table["MobBStat"], mobBStat);
	MSG::MobAStat::MobAStatTable* mobBStatTable = nullptr;
	for (auto i = 0; i < mobBStat.data_size(); i++)
	{
		auto data = mobBStat.mutable_data(i);
		if (data->level() == dungeonTable->mobblv())
		{
			mobBStatTable = data;
			break;
		}
	}

	MSG::MobAStat mobCStat; json2Proto(table["MobCStat"], mobCStat);
	MSG::MobAStat::MobAStatTable* mobCStatTable = nullptr;
	for (auto i = 0; i < mobCStat.data_size(); i++)
	{
		auto data = mobCStat.mutable_data(i);
		if (data->level() == dungeonTable->mobclv())
		{
			mobCStatTable = data;
			break;
		}
	}

	{
		if (mobAStatTable != nullptr)
		{
			characterTable = nullptr;
			for (auto i = 0; i < character.data_size(); i++)
			{
				auto data = character.mutable_data(i);
				if (data->name() == "MobA" )
				{
					characterTable = data;
					break;
				}
			}

			if (characterTable != nullptr)
			{
				BattleObj* obj = new BattleObj(mobAStatTable->level(), characterTable->no(), mobAStatTable->atk(), mobAStatTable->def(), mobAStatTable->hp(), characterTable->attackrange());
				_enemy.push_back(obj);
			}
		}

		if (mobBStatTable != nullptr)
		{
			characterTable = nullptr;
			for (auto i = 0; i < character.data_size(); i++)
			{
				auto data = character.mutable_data(i);
				if (data->name() == "MobB")
				{
					characterTable = data;
					break;
				}
			}

			if (characterTable != nullptr)
			{
				BattleObj* obj = new BattleObj(mobAStatTable->level(), characterTable->no(), mobBStatTable->atk(), mobBStatTable->def(), mobBStatTable->hp(), characterTable->attackrange());
				_enemy.push_back(obj);
			}
		}

		if (mobCStatTable != nullptr)
		{
			characterTable = nullptr;
			for (auto i = 0; i < character.data_size(); i++)
			{
				auto data = character.mutable_data(i);
				if (data->name() == "MobC")
				{
					characterTable = data;
					break;
				}
			}

			if (characterTable != nullptr)
			{
				BattleObj* obj = new BattleObj(mobAStatTable->level(), characterTable->no(), mobCStatTable->atk(), mobCStatTable->def(), mobCStatTable->hp(), characterTable->attackrange());
				_enemy.push_back(obj);
			}
		}

		MSG::DungeonTier tier; json2Proto(table["DungeonTier"], tier);
		MSG::DungeonTier::DungeonTierTable* dungeonTierTable = nullptr;
		for (auto i = 0; i < tier.data_size(); i++)
		{
			auto data = tier.mutable_data(i);
			if (data->tier() == tier_)
			{
				dungeonTierTable = data;
				break;
			}
		}

		if (dungeonTierTable != nullptr)
		{
			for (auto it : _enemy)
			{
				it->addBuff({
					{ String2BuffType(dungeonTierTable->buff1type(), dungeonTierTable->buff1value()), String2BuffValue(dungeonTierTable->buff1value()) },
					{ String2BuffType(dungeonTierTable->buff2type(), dungeonTierTable->buff2value()), String2BuffValue(dungeonTierTable->buff2value()) },
					{ String2BuffType(dungeonTierTable->buff3type(), dungeonTierTable->buff3value()), String2BuffValue(dungeonTierTable->buff3value()) },
				}
				);
			}
		}
	}

}

Battle::~Battle()
{
	for (auto it : _ally)
	{
		delete it;
	}

	for (auto it : _enemy)
	{
		delete it;
	}

	for (auto it : _battleDatas)
	{
		delete it;
	}
}

void Battle::play()
{
	LOG(L_INFO_, "start");

	// 랜덤 시드 설정
	std::random_device rd;
	std::mt19937 g(rd());


	// 배틀 오브젝트 hp 초기화
	for (auto it : _ally)
	{
		it->initHp();
	}

	for (auto it : _enemy)
	{
		it->initHp();
	}


	// 전투 시작
	// 최대 루프 횟수 20회
	int maxTurnCount = 30;
	// 공격 팀(아군 부터 공격 시작)
	MSG::BattleData_::Team attackTeam = MSG::BattleData_::ALLY;

	std::vector<BattleObj*>* attackTeamObjs = nullptr;
	std::vector<BattleObj*>* defenceTeamObjs = nullptr;

	for (int turn = 0; turn < maxTurnCount; ++turn)
	{
		LOG(L_INFO_, "battle", "turn", turn, "attack team", MSG::BattleData_::Team_Name(attackTeam));
		if (attackTeam == MSG::BattleData_::ALLY)
		{			
			attackTeamObjs = &_ally;
			defenceTeamObjs = &_enemy;
		}
		else
		{
			attackTeamObjs = &_enemy;
			defenceTeamObjs = &_ally;
		}


		{
			int teamSlotIndex = -1;
			for (auto it : *attackTeamObjs)
			{
				teamSlotIndex++;
				// 죽은 객체는 건너뜀
				if (it->isAlive() == false) continue;

				MSG::BattleData_* battleData = new MSG::BattleData_();;
				battleData->set_srcno(teamSlotIndex);
				battleData->set_team(attackTeam);

				auto range = it->getRange();

				LOG(L_INFO_, "attack", "slot", teamSlotIndex, "range", range);

				// 단일 대상 공격
				int targetSlot = -1;
				std::vector<int> temp;
				int slotIndex = 0;
				for (auto emIt : *defenceTeamObjs)
				{
					if (emIt->isAlive() == true) temp.push_back(slotIndex);

					slotIndex++;
				}

				if( temp.empty() == true ) continue;

				std::shuffle(temp.begin(), temp.end(), g);
				targetSlot = *temp.begin();

				BattleObj* target = nullptr;
				target = (*defenceTeamObjs)[targetSlot];
				if (target != nullptr)
				{
					// 데미지 계산
					int damage = std::max(it->getAtk() - target->getDef(), 0);
					target->changeHp(-damage);

					auto attackInfo = battleData->add_targets();
					attackInfo->set_no(targetSlot);
					attackInfo->set_damage(damage);
					if (target->isAlive()) attackInfo->set_result(MSG::BattleData_::ALIVE);
					else attackInfo->set_result(MSG::BattleData_::DEAD);

					LOG(L_INFO_, "defence", "slot1", targetSlot, "damage", damage, "hp", target->getCurHp(), "result", (int)target->isAlive());
				}


				BattleObj* target2 = nullptr;
				int target2Slot = -1;
				if (range == 2)
				{
					if (targetSlot == 1)
					{
						// 1 or -1
						auto randValue = (g() % 2);
						if (randValue == 0) randValue = -1;

						target2Slot = targetSlot + randValue;
						if (target2Slot > (*defenceTeamObjs).size() - 1)
						{
							target2Slot = targetSlot - randValue;
						}
						target2 = (*defenceTeamObjs)[target2Slot];

					}
					else
					{
						target2Slot = 1;
						target2 = (*defenceTeamObjs)[target2Slot];
					}

					if (target2 != nullptr && target2->isAlive() == true)
					{
						// 데미지 계산
						int damage = std::max(it->getAtk() - target2->getDef(), 0);
						target2->changeHp(-damage);

						auto attackInfo = battleData->add_targets();
						attackInfo->set_no(target2Slot);
						attackInfo->set_damage(damage);
						if (target2->isAlive()) attackInfo->set_result(MSG::BattleData_::ALIVE);
						else attackInfo->set_result(MSG::BattleData_::DEAD);

						LOG(L_INFO_, "defence", "slot2", target2Slot, "damage", damage, "hp", target2->getCurHp(), "result", (int)target2->isAlive());
					}
				}

				_battleDatas.push_back(battleData);

				if (attackTeam == MSG::BattleData_::ENEMY)
					if (isGameOver(MSG::BattleData_::ALLY) == true) break;
				else
					if (isGameOver(MSG::BattleData_::ENEMY) == true) break;

			}

			if (attackTeam == MSG::BattleData_::ENEMY)
			{
				attackTeam = MSG::BattleData_::ALLY;
			}
			else
			{
				attackTeam = MSG::BattleData_::ENEMY;
			}
		}

		if (isGameOver(attackTeam) == true) break;
	}

	auto winnerTeam = winner();
	LOG(L_INFO_, "battle", "result", MSG::BattleData_::Team_Name(winnerTeam));
}

bool Battle::isGameOver( MSG::BattleData_::Team team_)
{
	if (team_ == MSG::BattleData_::ALLY)
	{
		for (auto it : _ally)
		{
			if (it->isAlive() == true) return false;
		}
	}
	else
	{
		for (auto it : _enemy)
		{
			if (it->isAlive() == true) return false;
		}
	}
	return true;
}


MSG::BattleData_::Team Battle::winner()
{
	for (auto it : _enemy)
	{
		if (it->isAlive() == true) return MSG::BattleData_::ENEMY;
	}

	for (auto it : _ally)
	{
		if (it->isAlive() == true) return MSG::BattleData_::ALLY;
	}
	return MSG::BattleData_::ENEMY;
}


}