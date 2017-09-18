#pragma once
#include <vector>
namespace BLUE_BERRY
{


struct Buff 
{
	enum BuffType
	{
		None = 0,
		Atk_Plus = 1,		// ���ݷ� ����
		Def_Plus = 2,		// ���� ����
		Hp_Plus = 3,		// ü�� ����

		Atk_Percent = 101,	// ���ݷ� �ۼ�Ʈ ����
		Def_Percent = 102,	// ���� �ۼ�Ʈ ����
		Hp_Percent = 103,	// ü�� �ۼ�Ʈ ����
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