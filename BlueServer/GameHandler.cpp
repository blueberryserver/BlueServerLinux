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
#include "DBQueryBattleLog.h"

#include "Battle.h"

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
	REGIST_HANDLER(MSG::BATTLELOG_REQ, BattleLogReq);
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
	std::vector<MSG::CharData_*> chars;
	for (auto i = 0; i < userData->chars_size(); i++)
	{
		auto charData = userData->mutable_chars(i);
		chars.push_back(charData);
	}

	Battle battle(chars, req.dungeonno(), req.tier());	
	battle.play();

	auto battleData = battle.getBattleData();
	// answer
	MSG::PlayDungeonAns ans;
	ans.set_err(MSG::ERR_SUCCESS);

	for(int i = 0; i < userData->chars_size(); i++)
	{
		MSG::CharData_* character = ans.add_chars();
		auto charData = userData->mutable_chars(i);
		character->CopyFrom(*charData);
	}

	int no = 0;
	for (auto it : battle.getMonsterData())
	{
		MSG::CharData_* mob = ans.add_mobs();
		mob->set_cid(0);
		mob->set_uid(0);
		mob->set_slotno(no++);
		mob->set_typeno(it->getTypeNo());
		mob->set_level(it->getLv());
		mob->set_tier(it->getTier());
	}

	for (auto it : battleData)
	{
		auto  battle = ans.add_battles();
		battle->CopyFrom(*it);
	}

	session_->SendPacket(MSG::PLAYDUNGEON_ANS, &ans);

	// db insert
	{
		MSG::DungeonPlayData_ data;
		data.set_lid(0);
		data.set_uid(userData->uid());
		for (auto it : battleData)
		{
			auto  battle = data.add_battles();
			battle->CopyFrom(*it);
		}

		for (int i = 0; i < userData->chars_size(); i++)
		{
			MSG::CharData_* character = data.add_chars();
			auto charData = userData->mutable_chars(i);
			character->CopyFrom(*charData);
		}

		int no = 0;
		for (auto it : battle.getMonsterData())
		{
			MSG::CharData_* mob = data.add_mobs();
			mob->set_cid(0);
			mob->set_uid(0);
			mob->set_slotno(no++);
			mob->set_typeno(it->getTypeNo());
			mob->set_level(it->getLv());
			mob->set_tier(it->getTier());
		}
		data.set_regdate(DateTime::getCurrentDateTime().formatLocal().c_str());

		DBQueryBattleLog query;
		query.setData({ data, });
		if (query.insertData() == false)
		{
			return true;
		}

		if (query.haveData() == false)
		{
			return true;
		}
	}
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



DEFINE_HANDLER(GameHandler, SessionPtr, BattleLogReq)
{
	MSG::BattleLogReq req;
	req.ParseFromArray(body_, len_);
	LOG(L_INFO_, "recv packet info", "lid", (double)req.lid());

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
	auto userData = user->getData();

	{
		DBQueryBattleLog query;
		query.setWhere("uid=%I64u order by lid desc limit 0, 1", userData->uid());

		if (query.selectData() == false)
		{
			MSG::BattleLogAns ans;
			ans.set_err(MSG::ERR_AUTHORITY_FAIL);
			session_->SendPacket(MSG::BATTLELOG_ANS, &ans);
			return true;
		}

		std::vector<MSG::DungeonPlayData_> data;
		query.getData(data);

		for (auto dataIt : data)
		{
			//if( dataIt.lid() != req.lid()) continue;

			LOG(L_INFO_, "start", "lid", (double)req.lid());

			Battle battle;
			auto battleCount = dataIt.battles_size();
			MSG::BattleData_::Team team = MSG::BattleData_::ENEMY;
			int turn = 0;
			for (auto i = 0; i < battleCount; ++i)
			{
				auto battleData = dataIt.mutable_battles(i);
				auto bData = new MSG::BattleData_();
				bData->CopyFrom(*battleData);
				battle.addBattleData(bData);
			}

			auto charCount = dataIt.chars_size();
			for (auto i = 0; i < charCount; ++i)
			{
				auto charData = dataIt.mutable_chars(i);
				auto battleObj = new BattleObj( charData->level(), charData->typeno(), charData->tier() );
				battle.addBattleObj(battleObj, MSG::BattleData_::ALLY);
			}

			auto mobCount = dataIt.mobs_size();
			for (auto i = 0; i < mobCount; ++i)
			{
				auto mobData = dataIt.mutable_mobs(i);
				auto battleObj = new BattleObj(mobData->level(), mobData->typeno(), mobData->tier());
				battle.addBattleObj(battleObj, MSG::BattleData_::ENEMY);
			}
			battle.replay();
		}
	}

	// answer
	MSG::BattleLogAns ans;
	ans.set_err(MSG::ERR_SUCCESS);	
	session_->SendPacket(MSG::BATTLELOG_ANS, &ans);

	return true;
}

}