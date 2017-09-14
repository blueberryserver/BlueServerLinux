#include "GameHandler.h"
#include "cpp/game.pb.h"
#include "cpp/SimTable.pb.h"

#include "../BlueCore/Logger.h"
#include "../BlueCore/SyncJobHelper.h"
#include "../BlueCore/RedisClient.h"
#include "../BlueCore/JsonFileLoader.h"

#include "UserManager.h"
#include "DBQueryUser.h"
#include "DBQueryChar.h"

namespace BLUE_BERRY
{

DEFINE_MGR(GameHandler)

GameHandler::GameHandler()
	: ChatHandler()
{
	REGIST_HANDLER(MSG::CREATECHAR_REQ, CreateCharReq);
	REGIST_HANDLER(MSG::PLAYDUNGEON_REQ, PlayDungeonReq);
	REGIST_HANDLER(MSG::LEVELUPCHAR_REQ, LevelupCharReq);
	REGIST_HANDLER(MSG::TIERUPCHAR_REQ, TierupCharReq);
}

DEFINE_HANDLER(GameHandler, SessionPtr, CreateCharReq)
{
	MSG::CreateCharReq req;
	req.ParseFromArray(body_, len_);

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user == nullptr)
	{
		MSG::CreateCharAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::CREATECHAR_ANS, &ans);
		return true;
	}

	// update time out
	user->setPingTime(DateTime::GetTickCount() + std::chrono::duration_cast<_microseconds>(_minutes(2)).count());

	// get table
	auto table = JsonFileLoader::getJsonFileLoader()->get("SimTable.json");

	// char slot check
	auto userData = user->getData();

	bool changeVc = false;
	if (userData->chars_size() == 0)
	{
		// 무료 캐릭터 생성
	}
	else if (userData->chars_size() == 1)
	{
		// 비용 처리
		MSG::Product product; json2Proto(table["Product"], product);

		bool find = false; MSG::Product::ProductTable* productTable = nullptr;
		for (auto i = 0; i < product.data_size(); i++)
		{
			auto data = product.mutable_data(i);
			if (data->productcode() == "CreateChar" &&
				data->charno() == (int)req.charno())
			{
				productTable = data;
				find = true;
				break;
			}
		}
		if (find == false)
		{
			MSG::CreateCharAns ans;
			ans.set_err(MSG::ERR_ARGUMENT_FAIL);
			session_->SendPacket(MSG::CREATECHAR_ANS, &ans);
			return true;
		}

		if (productTable->vctype() == "VC1")
		{
			userData->set_vc1(userData->vc1() - productTable->value());
			changeVc = true;
		}
		else if (productTable->vctype() == "VC2")
		{
			userData->set_vc2(userData->vc2() - productTable->value());
			changeVc = true;
		}
		else if (productTable->vctype() == "VC3")
		{
			userData->set_vc3(userData->vc3() - productTable->value());
			changeVc = true;
		}
		else
		{

		}
	}
	else
	{
		// 더이상 캐릭터 생성 못함
		MSG::CreateCharAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::CREATECHAR_ANS, &ans);
		return true;
	}
	

	// search character table 
	MSG::Char character; json2Proto(table["Char"], character);
	bool find = false; MSG::Char::CharacterTable* characterTable = nullptr;
	for (auto i = 0; i < character.data_size(); i++)
	{
		auto data = character.mutable_data(i);
		if (data->no() == (int)req.charno())
		{
			characterTable = data;
			find = true;
			break;
		}
	}
	
	if (find == false)
	{
		MSG::CreateCharAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::CREATECHAR_ANS, &ans);
		return true;

	}

	MSG::CharData_ charData;
	charData.set_uid(userData->uid());
	charData.set_slotno(userData->chars_size());
	charData.set_typeno(req.charno());
	charData.set_level(1);
	charData.set_tier(0);
	charData.set_regdate(DateTime::getCurrentDateTime().formatLocal().c_str());

	if(changeVc == true)
	{
		DBQueryUser query;
		query.setData(*userData);
		if (query.updateData() == false)
		{
			MSG::CreateCharAns ans;
			ans.set_err(MSG::ERR_ARGUMENT_FAIL);
			session_->SendPacket(MSG::CREATECHAR_ANS, &ans);
			return true;
		}
	}

	{
		// db insert char data
		DBQueryChar query;
		query.setData({ charData, });

		if (query.insertData() == false)
		{
			MSG::CreateCharAns ans;
			ans.set_err(MSG::ERR_ARGUMENT_FAIL);
			session_->SendPacket(MSG::CREATECHAR_ANS, &ans);
			return true;
		}

		if (query.haveData() == false)
		{
			MSG::CreateCharAns ans;
			ans.set_err(MSG::ERR_ARGUMENT_FAIL);
			session_->SendPacket(MSG::CREATECHAR_ANS, &ans);
			return true;
		}

		std::vector<MSG::CharData_> charArray;
		query.getData(charArray);
		for (auto it : charArray)
		{
			auto addChar = userData->add_chars();
			addChar->CopyFrom(it);
			charData.CopyFrom(it);
		}
	}

	// answer
	MSG::CreateCharAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	auto mutableChar = ans.mutable_char_();
	mutableChar->CopyFrom(charData);
	session_->SendPacket(MSG::CREATECHAR_ANS, &ans);

	if (changeVc == true)
	{
		MSG::CurrencyNot notify;
		notify.set_vc1(userData->vc1());
		notify.set_vc2(userData->vc2());
		notify.set_vc3(userData->vc3());
		session_->SendPacket(MSG::CURRENCY_NOT, &notify);
	}
	return true;
}


DEFINE_HANDLER(GameHandler, SessionPtr, PlayDungeonReq)
{
	MSG::PlayDungeonReq req;
	req.ParseFromArray(body_, len_);

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user == nullptr)
	{
		MSG::PlayDungeonAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::PLAYDUNGEON_ANS, &ans);
		return true;
	}

	// update time out
	user->setPingTime(DateTime::GetTickCount() + std::chrono::duration_cast<_microseconds>(_minutes(2)).count());

	// get table
	auto table = JsonFileLoader::getJsonFileLoader()->get("SimTable.json");

	// slot no
	auto userData = user->getData();

	// 데이터
	// 던전 : 넘버, 티어(Level), 보상, 몬스터1레벨, 몬스터2레벨, 몬스터3레벨
	int dungeonLevel = 0;
	int dungeonReward = 0;
	int dungeonMobALevel = 0;
	int dungeonMobBLevel = 0;
	int dungeonMobCLevel = 0;
	auto dungeonDatas = table["Dungeon"]["Data"].array_items();
	for (auto it : dungeonDatas)
	{
		if (req.dungeonno() == it["No"].int_value())
		{
			dungeonLevel = it["Level"].int_value();
			dungeonReward = it["Reward"].int_value();
			dungeonMobALevel = it["MobALv"].int_value();
			dungeonMobBLevel = it["MobBLv"].int_value();
			dungeonMobCLevel = it["MobCLv"].int_value();
			break;
		}
	}
	// 던전 티어 : 티어, 버프타입1, 버프값1, 버프타입2, 버프값2, 버프타입3, 버프값3
	struct TierBuff
	{
		int tier;
		std::string tierBuff1Type;
		std::string tierBuff1Value;
		std::string tierBuff2Type;
		std::string tierBuff2Value;
		std::string tierBuff3Type;
		std::string tierBuff3Value;
	};
	TierBuff dungeonTierBuffs[3] = { 0, "", };
	/*int dungeonTier = 0;
	std::string dungeonTierBuff1Type = 0;
	std::string dungeonTierBuff1Value = 0;
	std::string dungeonTierBuff2Type = 0;
	std::string dungeonTierBuff2Value = 0;
	std::string dungeonTierBuff3Type = 0;
	std::string dungeonTierBuff3Value = 0;*/
	auto dungeonTierDatas = table["DungeonTier"]["Data"].array_items();
	for (auto it : dungeonTierDatas)
	{
		if (dungeonLevel == it["Tier"].int_value())
		{
			TierBuff& dungeonTierBuff = dungeonTierBuffs[it["Tier"].int_value()];
			dungeonTierBuff.tier = it["Tier"].int_value();
			dungeonTierBuff.tierBuff1Type = it["Buff1Type"].string_value();
			dungeonTierBuff.tierBuff1Value = it["Buff1Value"].string_value();
			dungeonTierBuff.tierBuff2Type = it["Buff2Type"].string_value();
			dungeonTierBuff.tierBuff2Value = it["Buff2Value"].string_value();
			dungeonTierBuff.tierBuff3Type = it["Buff3Type"].string_value();
			dungeonTierBuff.tierBuff3Value = it["Buff3Value"].string_value();
			break;
		}
	}

	// 캐릭터 스탯 : 레벨, 공격, 방어, 체력
	struct SlotStat
	{
		int level;
		int attack;
		int defence;
		int hp;
	};
	SlotStat charSlotStats[3] = { 0, };
	for (int i = 0; i < userData->chars_size(); i++)
	{
		auto charData = userData->mutable_chars(i);
		auto charArray = table["Char"]["Data"].array_items();
		std::string charName;
		for (auto it : charArray)
		{
			if (it["No"].int_value() == (int)charData->typeno())
			{
				charName = it["Name"].string_value();
				break;
			}
		}

		std::string StatTableName;
		StatTableName += charName + "Stat";

		auto stat = table[StatTableName.c_str()];
		auto levelArray = stat["Data"].array_items();
		for (auto it : levelArray)
		{
			if (it["Level"] == (int)charData->level())
			{
				SlotStat& charSlotStat = charSlotStats[(int)charData->slotno()];
				charSlotStat.level = it["Level"].int_value();
				charSlotStat.attack = it["Atk"].int_value();
				charSlotStat.defence = it["Def"].int_value();
				charSlotStat.hp = it["Hp"].int_value();
				break;
			}
		}
	}
	// 캐릭터 티어 : 티어, 버프타입1, 버프값1, 버프타입2, 버프값2, 버프타입3, 버프값3

	/*int charTier1 = 0;
	std::string charTier1Buff1Type = 0;
	std::string charTier1Buff1Value = 0;
	std::string charTier1Buff2Type = 0;
	std::string charTier1Buff2Value = 0;
	std::string charTier1Buff3Type = 0;
	std::string charTier1Buff3Value = 0;
	int charTier2 = 0;
	std::string charTier2Buff1Type = 0;
	std::string charTier2Buff1Value = 0;
	std::string charTier2Buff2Type = 0;
	std::string charTier2Buff2Value = 0;
	std::string charTier2Buff3Type = 0;
	std::string charTier2Buff3Value = 0;
	int charTier3 = 0;
	std::string charTier3Buff1Type = 0;
	std::string charTier3Buff1Value = 0;
	std::string charTier3Buff2Type = 0;
	std::string charTier3Buff2Value = 0;
	std::string charTier3Buff3Type = 0;
	std::string charTier3Buff3Value = 0;*/
	TierBuff charTierBuffs[3] = { 0, "", };
	for (int i = 0; i < userData->chars_size(); i++)
	{
		auto charData = userData->mutable_chars(i);
		auto charTierArray = table["CharTier"]["Data"].array_items();
		for (auto it : charTierArray)
		{
			if (it["Tier"] == (int)charData->tier())
			{
				TierBuff& charTierBuff = charTierBuffs[(int)charData->slotno()];
				charTierBuff.tier = it["Tier"].int_value();
				charTierBuff.tierBuff1Type = it["Buff1Type"].string_value();
				charTierBuff.tierBuff1Value = it["Buff1Value"].string_value();
				charTierBuff.tierBuff2Type = it["Buff2Type"].string_value();
				charTierBuff.tierBuff2Value = it["Buff2Value"].string_value();
				charTierBuff.tierBuff3Type = it["Buff3Type"].string_value();
				charTierBuff.tierBuff3Value = it["Buff3Value"].string_value();
				break;
			}
		}
	}
	// 몬스터 스탯 : 레벨, 공격, 방어, 체력
	/*int mobSlot1StatLevel = 0;
	int mobSlot1StatAttack = 0;
	int mobSlot1StatDefence = 0;
	int mobSlot1StatHp = 0;
	int mobSlot2StatLevel = 0;
	int mobSlot2StatAttack = 0;
	int mobSlot2StatDefence = 0;
	int mobSlot2StatHp = 0;
	int mobSlot3StatLevel = 0;
	int mobSlot3StatAttack = 0;
	int mobSlot3StatDefence = 0;
	int mobSlot3StatHp = 0;*/
	SlotStat mobSlotStats[3] = { 0, };
	auto mobAStatArray = table["MobAStat"]["Data"].array_items();
	for (auto it : mobAStatArray)
	{
		if (it["Level"].int_value() == dungeonMobALevel)
		{
			SlotStat& mobSlotStat = mobSlotStats[0];
			mobSlotStat.level = it["Level"].int_value();
			mobSlotStat.attack = it["Atk"].int_value();
			mobSlotStat.defence = it["Def"].int_value();
			mobSlotStat.hp = it["Hp"].int_value();
			break;
		}
	}
	auto mobBStatArray = table["MobBStat"]["Data"].array_items();
	for (auto it : mobBStatArray)
	{
		if (it["Level"].int_value() == dungeonMobBLevel)
		{
			SlotStat& mobSlotStat = mobSlotStats[1];
			mobSlotStat.level = it["Level"].int_value();
			mobSlotStat.attack = it["Atk"].int_value();
			mobSlotStat.defence = it["Def"].int_value();
			mobSlotStat.hp = it["Hp"].int_value();
			break;
		}
	}
	auto mobCStatArray = table["MobCStat"]["Data"].array_items();
	for (auto it : mobCStatArray)
	{
		if (it["Level"].int_value() == dungeonMobCLevel)
		{
			SlotStat& mobSlotStat = mobSlotStats[2];
			mobSlotStat.level = it["Level"].int_value();
			mobSlotStat.attack = it["Atk"].int_value();
			mobSlotStat.defence = it["Def"].int_value();
			mobSlotStat.hp = it["Hp"].int_value();
			break;
		}
	}

	// 메모리, DB
	// 유저 캐릭터 (CharData_) : 슬롯, 타입, 레벨, 티어
	// 유저 던전 (DungeonData_) : 넘버, 티어, 몬스터 캐릭터(CharData_)

	// 시뮬
	MSG::PlayDungeonAns ans;
	int allyHpSlots[3] = { charSlotStats[0].hp, charSlotStats[1].hp, charSlotStats[2].hp };
	int enemyHpSlots[3] = { mobSlotStats[0].hp, mobSlotStats[1].hp, mobSlotStats[2].hp };
	MSG::BattleData__Team turn = MSG::BattleData__Team_ALLY;
	int allyTurnSlot = 0;
	int enemyTurnSlot = 0;
	bool endFlag = false;
	int turnCount = 0;

	while (endFlag == false)
	{
		// 공격 방어
		// 총 공격 = max((공격자 공격 + 공격자 공격 버프(+)) * 공격자  공격 버프(%) - (방어자 방어력 + 방어자 방어력 버프(+)) * 방어자 방어력 버프(%), 0)
		// 방어자 체력 = (방어자 체력 + 방어자 체력 버프(+)) * 방어자 체력 버프(%) - 총 공격

		SlotStat attackerSlotStat;
		SlotStat defenderSlotStat;
		TierBuff attackerTierBuff;
		TierBuff defenderTierBuff;
		if (turn == MSG::BattleData__Team_ALLY)
		{
			attackerSlotStat = charSlotStats[allyTurnSlot];
			attackerTierBuff = charTierBuffs[allyTurnSlot];
			defenderSlotStat = mobSlotStats[enemyTurnSlot];
			defenderTierBuff = dungeonTierBuffs[enemyTurnSlot];
		}
		else
		{
			attackerSlotStat = mobSlotStats[enemyTurnSlot];
			attackerTierBuff = dungeonTierBuffs[enemyTurnSlot];
			defenderSlotStat = charSlotStats[allyTurnSlot];
			defenderTierBuff = charTierBuffs[allyTurnSlot];
		}

		float buffAtkValue = 0.0f;
		float buffDefValue = 0.0f;
		float buffHpValue = 0.0f;
		float buffAtkRate = 1.0f;
		float buffDefRate = 1.0f;
		float buffHpRate = 1.0f;

		/*if (attackerTierBuff.tierBuff1Type == "Atk")
		{
			buffAtk += attackerTierBuff.
		}
		else if (attackerTierBuff.tierBuff1Type == "Def")
		{

		}
		else if (attackerTierBuff.tierBuff1Type == "Hp")
		{

		}*/

		/*if (attackerTierBuff.tierBuff1Type == "Atk")
		{
		buffAtk += attackerTierBuff.
		}
		else if (attackerTierBuff.tierBuff1Type == "Def")
		{

		}
		else if (attackerTierBuff.tierBuff1Type == "Hp")
		{

		}*/

		/*if (attackerTierBuff.tierBuff1Type == "Atk")
		{
		buffAtk += attackerTierBuff.
		}
		else if (attackerTierBuff.tierBuff1Type == "Def")
		{

		}
		else if (attackerTierBuff.tierBuff1Type == "Hp")
		{

		}*/

		int totalAttack = std::max(((float)attackerSlotStat.attack + buffAtkValue) * buffAtkRate - ((float)defenderSlotStat.defence + buffDefValue) * buffDefRate, 0.0f);
		int resultHp = ((float)defenderSlotStat.hp + buffHpValue) * buffHpRate - totalAttack;

		if (turn == MSG::BattleData__Team_ALLY)
		{
			resultHp = std::max(resultHp, 0);
			enemyHpSlots[enemyTurnSlot] = resultHp;
			MSG::BattleData_* battle = ans.add_battles();
			battle->set_srcno(allyTurnSlot);
			battle->set_team(turn);
			MSG::BattleData_::Attack* target = battle->add_targets();
			target->set_no(enemyTurnSlot);
			target->set_damage(totalAttack);
			target->set_result(0 < resultHp ? MSG::BattleData__AttackResult_ALIVE : MSG::BattleData__AttackResult_DEAD);
		}
		else
		{
			resultHp = std::max(resultHp, 0);
			allyHpSlots[allyTurnSlot] = resultHp;
			MSG::BattleData_* battle = ans.add_battles();
			battle->set_srcno(enemyTurnSlot);
			battle->set_team(turn);
			MSG::BattleData_::Attack* target = battle->add_targets();
			target->set_no(allyTurnSlot);
			target->set_damage(totalAttack);
			target->set_result(0 < resultHp ? MSG::BattleData__AttackResult_ALIVE : MSG::BattleData__AttackResult_DEAD);
		}

		// 체력 체크
		endFlag = endFlag || (allyHpSlots[0] <= 0 && allyHpSlots[1] <= 0 && allyHpSlots[2] <= 0) || (enemyHpSlots[0] <= 0 && enemyHpSlots[1] <= 0 && enemyHpSlots[2] <= 0);
		// 턴 횟수 체크
		endFlag = endFlag || (10 <= turnCount);
		turnCount++;
		// 공격자 팀 선택
		turn = turn == MSG::BattleData__Team_Team_MAX ? MSG::BattleData__Team_Team_MIN : (MSG::BattleData__Team)(turn + 1);
		// 아군 슬롯 선택
		allyTurnSlot = (allyTurnSlot + 1) % 3;
		// 적군 슬롯 선택
		enemyTurnSlot = (enemyTurnSlot + 1) % 3;
	}

	for (int i = 0; i < userData->chars_size(); i++)
	{
		MSG::CharData_* character = ans.add_chars();
		auto charData = userData->mutable_chars(i);
		character->CopyFrom(*charData);
	}
	int no = 0;
	int type = 3;
	for (auto it : mobSlotStats)
	{
		MSG::CharData_* mob = ans.add_mobs();
		mob->set_cid(0);
		mob->set_uid(0);
		mob->set_slotno(no++);
		mob->set_typeno(type++);
		mob->set_level(it.level);
	}

	// answer
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::PLAYDUNGEON_ANS, &ans);
	return true;
}

DEFINE_HANDLER(GameHandler, SessionPtr, LevelupCharReq)
{
	MSG::LevelupCharReq req;
	req.ParseFromArray(body_, len_);

	LOG(L_INFO_, "recv packet info", "slotno", (int)req.slotno());

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user == nullptr)
	{
		MSG::LevelupCharAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::LEVELUPCHAR_ANS, &ans);
		return true;
	}

	// update time out
	user->setPingTime(DateTime::GetTickCount() + std::chrono::duration_cast<_microseconds>(_minutes(2)).count());

	// get table
	auto table = JsonFileLoader::getJsonFileLoader()->get("SimTable.json");

	// slot no
	auto userData = user->getData();
	if (userData->chars_size() < (int)req.slotno() + 1)
	{
		MSG::LevelupCharAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::LEVELUPCHAR_ANS, &ans);
		return true;
	}

	auto charData = userData->mutable_chars(req.slotno());

	MSG::Char character; json2Proto(table["Char"], character);
	MSG::Char::CharacterTable* characterTable = nullptr;
	for (auto i = 0; i < character.data_size(); i++)
	{
		auto data = character.mutable_data(i);
		if (data->no() == (int)charData->typeno())
		{
			characterTable = data;
			break;
		}
	}

	std::string StatTableName;
	StatTableName += characterTable->name() + "Stat";

	//auto stat = table[StatTableName.c_str()];
	MSG::WarriorStat stat; json2Proto(table[StatTableName.c_str()], stat);
	int needGold = 0;
	for (auto i = 0; i < stat.data_size(); i++)
	{
		auto data = stat.mutable_data(i);
		if (data->level() == (int)charData->level())
		{
			needGold = data->exp();
			break;
		}
	}

	// 
	if (needGold == 0 || (int)userData->vc1() < needGold)
	{
		MSG::LevelupCharAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::LEVELUPCHAR_ANS, &ans);
		return true;
	}

	// 비용 감소
	userData->set_vc1(userData->vc1() - needGold);

	// 캐릭터 레벨 변경
	charData->set_level(charData->level() + 1);


	{
		DBQueryUser query;
		query.setData(*userData);
		if (query.updateData() == false)
		{
			MSG::LevelupCharAns ans;
			ans.set_err(MSG::ERR_ARGUMENT_FAIL);
			session_->SendPacket(MSG::LEVELUPCHAR_ANS, &ans);
			return true;
		}
	}

	{
		DBQueryChar query;
		query.setData({ *charData, });
		if (query.updateData() == false)
		{
			MSG::LevelupCharAns ans;
			ans.set_err(MSG::ERR_ARGUMENT_FAIL);
			session_->SendPacket(MSG::LEVELUPCHAR_ANS, &ans);
			return true;
		}
	}

	// answer
	MSG::LevelupCharAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	auto mchar = ans.mutable_char_();
	mchar->CopyFrom(*charData);
	session_->SendPacket(MSG::LEVELUPCHAR_ANS, &ans);

	{
		MSG::CurrencyNot notify;
		notify.set_vc1(userData->vc1());
		notify.set_vc2(userData->vc2());
		notify.set_vc3(userData->vc3());
		session_->SendPacket(MSG::CURRENCY_NOT, &notify);
	}
	return true;
}

DEFINE_HANDLER(GameHandler, SessionPtr, TierupCharReq)
{
	MSG::TierupCharReq req;
	req.ParseFromArray(body_, len_);
	LOG(L_INFO_, "recv packet info", "slotno", (int)req.slotno());

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user == nullptr)
	{
		MSG::TierupCharAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::TIERUPCHAR_ANS, &ans);
		return true;
	}

	// update time out
	user->setPingTime(DateTime::GetTickCount() + std::chrono::duration_cast<_microseconds>(_minutes(2)).count());

	// get table
	auto table = JsonFileLoader::getJsonFileLoader()->get("SimTable.json");

	// slot no
	auto userData = user->getData();
	if (userData->chars_size() < (int)req.slotno() + 1)
	{
		MSG::TierupCharAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::TIERUPCHAR_ANS, &ans);
		return true;
	}

	auto charData = userData->mutable_chars(req.slotno());

	if (charData->level() < 20)
	{
		MSG::TierupCharAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::TIERUPCHAR_ANS, &ans);
		return true;
	}

	MSG::CharTier tier; json2Proto(table["CharTier"], tier);
	MSG::CharTier::CharTierTable* charTierTable = nullptr;
	int tierUpCost = 200;
	for (auto i = 0; i < tier.data_size(); i++)
	{
		auto data = tier.mutable_data(i);
		if (data->tier() == charData->tier())
		{
			charTierTable = data;
		}
	}

	if ((int)userData->vc2() < tierUpCost)
	{
		MSG::LevelupCharAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::TIERUPCHAR_ANS, &ans);
		return true;
	}

	// 비용 감소
	userData->set_vc2(userData->vc2() - tierUpCost);

	// 캐릭터 레벨 초기화 
	charData->set_level(1);

	// 티어 변경
	charData->set_tier(charData->tier() + 1);

	{
		DBQueryUser query;
		query.setData(*userData);
		if (query.updateData() == false)
		{
			MSG::TierupCharAns ans;
			ans.set_err(MSG::ERR_ARGUMENT_FAIL);
			session_->SendPacket(MSG::TIERUPCHAR_ANS, &ans);
			return true;
		}
	}

	{
		DBQueryChar query;
		query.setData({ *charData, });
		if (query.updateData() == false)
		{
			MSG::TierupCharAns ans;
			ans.set_err(MSG::ERR_ARGUMENT_FAIL);
			session_->SendPacket(MSG::TIERUPCHAR_ANS, &ans);
			return true;
		}
	}

	// answer
	MSG::TierupCharAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	auto mchar = ans.mutable_char_();
	mchar->CopyFrom(*charData);
	session_->SendPacket(MSG::TIERUPCHAR_ANS, &ans);


	{
		MSG::CurrencyNot notify;
		notify.set_vc1(userData->vc1());
		notify.set_vc2(userData->vc2());
		notify.set_vc3(userData->vc3());
		session_->SendPacket(MSG::CURRENCY_NOT, &notify);
	}
	return true;
}


}