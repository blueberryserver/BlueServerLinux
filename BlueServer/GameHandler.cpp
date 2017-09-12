#include "GameHandler.h"
#include "cpp/game.pb.h"

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
		auto productTable = table["Product"];
		auto productArray = productTable["Data"].array_items();

		Json Product; bool find = false;
		for (auto it : productArray)
		{
			if (it["ProductCode"].string_value() == "CreateChar" &&
				it["CharNo"].int_value() == (int)req.charno())
			{
				Product = it;
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

		auto vcType = Product["VCType"].string_value();
		auto price = Product["Value"].int_value();
		if (vcType == "VC1")
		{
			userData->set_vc1(userData->vc1() - price);
			changeVc = true;
		}
		else if (vcType == "VC2")
		{
			userData->set_vc2(userData->vc2() - price);
			changeVc = true;
		} 
		else if (vcType == "VC3")
		{
			userData->set_vc3(userData->vc3() - price);
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
	auto charTable = table["Char"];
	auto charArray = charTable["Data"].array_items();

	Json Char; bool find = false;
	for (auto it : charArray)
	{
		if (it["No"].int_value() == (int)req.charno())
		{
			Char = it;
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

	// answer
	MSG::PlayDungeonAns ans;
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
	int needGold = 0;
	for (auto it : levelArray)
	{
		if (it["Level"] == (int)charData->level())
		{
			needGold = it["Exp"].int_value();
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

	auto tierArray = table["CharTier"]["Data"].array_items();

	int tierUpCost = 200;
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