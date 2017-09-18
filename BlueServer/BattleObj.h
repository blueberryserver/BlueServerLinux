#pragma once
#include <vector>
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

class BattleObj
{
public:

	BattleObj( int lv_, int typeNo_, int atk_, int def_, int hp_, int range_ )
		: _lv(lv_), _typeNo(typeNo_), _atk(atk_), _def(def_), _hp(hp_), _range(range_)
	{}

	virtual ~BattleObj()
	{
		_buffs.clear();
	}

	void addBuff(std::vector<Buff>&& buffs_)
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
	int getAtk() { return _finalAtk; }
	int getDef() { return _finalDef; }
	int getHp() { return _finalHp; }
	int getCurHp() { return _curHp; }


	bool isAlive() { return _curHp > 0; }

private:
	int _lv;
	int _typeNo;
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


}