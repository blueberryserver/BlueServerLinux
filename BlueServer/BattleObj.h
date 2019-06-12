#pragma once
#include <vector>
#include "../BlueCore/JsonFileLoader.h"

namespace BLUE_BERRY
{


struct Buff 
{
	enum BuffType
	{
		None = 0,
		Atk_Plus = 1,		// 공격력 증감
		Def_Plus = 2,		// 방어력 증감
		Hp_Plus = 3,		// 체력 증감

		Atk_Percent = 101,	// 공격력 퍼센트 증감
		Def_Percent = 102,	// 방어력 퍼센트 증감
		Hp_Percent = 103,	// 체력 퍼센트 증감
	};

	BuffType _type;
	int _value;
};

static Buff::BuffType String2BuffType(const std::string& typeStr_, const std::string& valueStr_)
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

class BattleObj
{
public:

	BattleObj( int lv_, int typeNo_, int tier_/*, int atk_, int def_, int hp_, int range_ */)
		: _lv(lv_), _typeNo(typeNo_), _tier(tier_)/*, _atk(atk_), _def(def_), _hp(hp_), _range(range_)*/
	{
		auto table = JsonFileLoader::getJsonFileLoader()->get("SimTable.json");

		MSG::Char character; json2Proto(table["Char"], character);
		MSG::Char::CharacterTable* characterTable = nullptr;
		characterTable = nullptr;
		for (auto i = 0; i < character.data_size(); i++)
		{
			auto data = character.mutable_data(i);
			if (data->no() == _typeNo)
			{
				characterTable = data;
				break;
			}
		}

		std::string StatTableName;
		StatTableName += characterTable->name() + "Stat";

		MSG::WarriorStat stat; json2Proto(table[StatTableName.c_str()], stat);
		MSG::WarriorStat::WarriorStatTable* statTable = nullptr;
		for (auto i = 0; i < stat.data_size(); i++)
		{
			auto data = stat.mutable_data(i);
			if (data->level() == _lv)
			{
				statTable = data;
				break;
			}
		}

		if (statTable != nullptr)
		{
			_atk = statTable->atk();
			_def = statTable->def();
			_hp = statTable->hp();
		}

		if (characterTable != nullptr)
		{
			_range = characterTable->attackrange();
		}

		if (_typeNo <= 2)
		{
			MSG::CharTier tier; json2Proto(table["CharTier"], tier);
			MSG::CharTier::CharTierTable* charTierTable = nullptr;
			for (auto i = 0; i < tier.data_size(); i++)
			{
				auto data = tier.mutable_data(i);
				if (data->tier() == _tier)
				{
					charTierTable = data;
					break;
				}
			}

			if (charTierTable != nullptr)
			{
				_buffs.push_back({ String2BuffType(charTierTable->buff1type(), charTierTable->buff1value()), String2BuffValue(charTierTable->buff1value()) });
				_buffs.push_back({ String2BuffType(charTierTable->buff2type(), charTierTable->buff2value()), String2BuffValue(charTierTable->buff2value()) });
				_buffs.push_back({ String2BuffType(charTierTable->buff3type(), charTierTable->buff3value()), String2BuffValue(charTierTable->buff3value()) });
			}


		}
		else
		{
			MSG::DungeonTier tier; json2Proto(table["DungeonTier"], tier);
			MSG::DungeonTier::DungeonTierTable* dungeonTierTable = nullptr;
			for (auto i = 0; i < tier.data_size(); i++)
			{
				auto data = tier.mutable_data(i);
				if (data->tier() == _tier)
				{
					dungeonTierTable = data;
					break;
				}
			}

			if (dungeonTierTable != nullptr)
			{
				_buffs.push_back({ String2BuffType(dungeonTierTable->buff1type(), dungeonTierTable->buff1value()), String2BuffValue(dungeonTierTable->buff1value()) });
				_buffs.push_back({ String2BuffType(dungeonTierTable->buff2type(), dungeonTierTable->buff2value()), String2BuffValue(dungeonTierTable->buff2value()) });
				_buffs.push_back({ String2BuffType(dungeonTierTable->buff3type(), dungeonTierTable->buff3value()), String2BuffValue(dungeonTierTable->buff3value()) });
			}
		}

		calcStat();		
	}

	virtual ~BattleObj()
	{
		_buffs.clear();
	}

	void addBuff(const std::vector<Buff>& buffs_)
	{
		for (auto it : buffs_)
			_buffs.push_back(it);

		calcStat();
	}

	void calcStat()
	{
		auto atk = _atk;
		auto def = _def;
		auto hp = _hp;
		// calc plus 
		for (auto it : _buffs)
		{
			if (it._type == Buff::Atk_Plus)
			{
				atk += it._value;
			}
			else if (it._type == Buff::Def_Plus)
			{
				def += it._value;
			}
			else if (it._type == Buff::Hp_Plus)
			{
				hp += it._value;
			}
		}

		auto atkPercent = 0;
		auto defPercent = 0;
		auto hpPercent = 0;

		// calc percent
		for (auto it : _buffs)
		{
			if (it._type == Buff::Atk_Percent)
			{
				atkPercent += it._value;
			}
			else if (it._type == Buff::Def_Percent)
			{
				defPercent += it._value;
			}
			else if (it._type == Buff::Hp_Percent)
			{
				hpPercent += it._value;
			}
		}

		_finalAtk = atk + (atk * atkPercent) / 100;
		_finalDef = def + (def * defPercent) / 100;
		_finalHp = hp + (hp * hpPercent) / 100;
	}

	void initHp() { calcStat(); _curHp = _finalHp; }
	void changeHp(int value_) { _curHp += value_; }

	int getLv() { return _lv; }
	int getTypeNo() { return _typeNo; }
	int getRange() { return _range; }
	int getTier() { return _tier;  }
	int getAtk() { return _finalAtk; }
	int getDef() { return _finalDef; }
	int getHp() { return _finalHp; }
	int getCurHp() { return _curHp; }


	bool isAlive() { return _curHp > 0; }

private:
	unsigned int _lv;
	unsigned int _typeNo;
	unsigned int _tier;
	// default stat
	int _atk;
	int _def;
	int _hp;

	int _finalAtk;
	int _finalDef;
	int _finalHp;

	// attack range
	int _range;

	// item add stat

	// buff
	std::vector<Buff> _buffs;


	// current hp
	int _curHp;
};

DECLARE_SMART_PTR(BattleObj);


}