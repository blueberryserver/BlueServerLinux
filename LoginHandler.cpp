#include "LoginHandler.h"
#include "cpp/login.pb.h"

#include "../BlueCore/DateTime.h"
#include "../BlueCore/Packet.h"
#include "../BlueCore/Logger.h"
#include "../BlueCore/JobExec.h"
#include "../BlueCore/SyncJobHelper.h"
#include "../BlueCore/RedisClient.h"

#include "UserManager.h"
#include "DBQueryUser.h"
#include "DBQueryChar.h"
#include "DBQueryDungeon.h"

#include "ChatHandler.h"
#include "GameHandler.h"
#include "ChatChannelManager.h"

namespace BLUE_BERRY
{

DEFINE_MGR(LoginHandler)

LoginHandler::LoginHandler() : DefaultHandler()
{
	REGIST_HANDLER(MSG::LOGIN_REQ, LoginReq);
	REGIST_HANDLER(MSG::REGIST_REQ, RegistReq);
}

void LoginHandler::dbSelectUser(const SessionPtr session_, const std::string name_)
{

	MSG::UserData_ data;
	{
		// db select user data	
		DBQueryUser query;
		query.setWhere("name='%s'", name_.c_str());
		if (query.selectData() == false)
		{
			MSG::LoginAns ans;
			ans.set_err(MSG::ERR_AUTHORITY_FAIL);
			session_->SendPacket(MSG::LOGIN_ANS, &ans);
			return;
		}

		// not exist user data
		if (query.haveData() == false)
		{
			MSG::LoginAns ans;
			ans.set_err(MSG::ERR_LOGIN_FAIL);
			session_->SendPacket(MSG::LOGIN_ANS, &ans);
			return;
		}

		query.getData(data);
	}
	
	{
		// db select char data
		DBQueryChar query;
		query.setWhere("uid=%I64u", data.uid());
		if (query.selectData() == false)
		{
			MSG::LoginAns ans;
			ans.set_err(MSG::ERR_AUTHORITY_FAIL);
			session_->SendPacket(MSG::LOGIN_ANS, &ans);
			return;
		}

		std::vector<MSG::CharData_> chars;
		query.getData(chars);

		for (auto it : chars)
		{
			auto charData = data.add_chars();
			charData->CopyFrom(it);
		}
	}

	// update login date
	auto now = DateTime::getCurrentDateTime().formatLocal();
	data.set_logindate(now.c_str());

	auto user = std::make_shared<User>(data);
	user->setSession(session_);
	LOG(L_INFO_, "User", "key", user->getSessionKey(), "data", toJson(*user->getData()));

	UserManager::getUserManager()->add(user);

	// enter chat channel
	auto channel = ChatChannelManager::getChatChannelManager()->findChannel(user->getData()->groupname().c_str());
	channel->enterChannel(session_);

	// switch handler
	//session_->setMsgHandler(ChatHandler::getChatHandler());
	session_->setMsgHandler(GameHandler::getGameHandler());

	// redis caching
	{
		//RedisClientPtr client;
		//auto keyHGetName = client->hset("blue_server.UserData.name", data.name().c_str(), std::to_string(data.uid()).c_str());
		//auto hSetPostJobName = LamdaToFuncObj([](_RedisReply reply_) -> void {
		//	LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.name", "reply", reply_);
		//});
		//SyncJobManager::getSyncJobManager()->addJob(keyHGetName, makePostJobStatic(hSetPostJobName), nullptr);

		//std::string jsonStr;
		//toJson(*user->getData()).dump(jsonStr);
		//auto keyHGetJon = client->hset("blue_server.UserData.json", std::to_string(data.uid()).c_str(), jsonStr.c_str() );
		//auto hSetPostJobJson = LamdaToFuncObj([](_RedisReply reply_) -> void {
		//	LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.json", "reply", reply_);
		//});
		//SyncJobManager::getSyncJobManager()->addJob( keyHGetJon, makePostJobStatic(hSetPostJobJson), nullptr);

		//sync process by future promise using
		RedisClientPtr client;
		{
			std::future<_RedisReply> futureReply;
			if (client->hset("blue_server.UserData.name", data.name().c_str(), std::to_string(data.uid()).c_str(), std::ref(futureReply)) == true)
			{
				auto reply = futureReply.get();
				LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.name", "reply", reply);
			}
		}

		{
			std::future<_RedisReply> futureReply;
			std::string jsonStr;
			toJson(*user->getData()).dump(jsonStr);
			if (true == client->hset("blue_server.UserData.json", std::to_string(data.uid()).c_str(), jsonStr.c_str(), std::ref(futureReply)))
			{
				auto reply = futureReply.get();
				LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.json", "reply", reply);
			}
		}
	}


	MSG::LoginAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	auto userData = ans.mutable_data();
	userData->CopyFrom(*user->getData());
	ans.set_sessionkey(user->getSessionKey());

	session_->SendPacket(MSG::LOGIN_ANS, &ans);
}

void LoginHandler::dbInsertUser(SessionPtr session_, MSG::UserData_ data_)
{
	// db select user data	
	DBQueryUser query;
	query.setData(data_);

	if (query.insertData() == false)
	{
		MSG::RegistAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::REGIST_ANS, &ans);
		return;
	}

	// not exist user data
	if (query.haveData() == false)
	{
		MSG::RegistAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::REGIST_ANS, &ans);
		return;
	}
	MSG::UserData_ data;
	query.getData(data);


	User user(data);
	auto jUser = toJson(*user.getData());
	LOG(L_INFO_, "User", "data", jUser);


	// redis caching
	{
		RedisClientPtr client;
		{
			std::future<_RedisReply> futureReply;
			if (true == client->hset("blue_server.UserData.name", data.name().c_str(), std::to_string(data.uid()).c_str(), std::ref(futureReply)))
			{
				auto reply = futureReply.get();
				LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.name", "reply", reply);
			}
		}

		std::string jsonStr;
		toJson(*user.getData()).dump(jsonStr);
		{
			std::future<_RedisReply> futureReply;
			if (true == client->hset("blue_server.UserData.json", std::to_string(data.uid()).c_str(), jsonStr.c_str(), std::ref(futureReply)))
			{
				auto reply = futureReply.get();
				LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.json", "reply", reply);
			}
		}

		//auto keyHGetName = client->hset("blue_server.UserData.name", data.name().c_str(), std::to_string(data.uid()).c_str());
		//auto hSetPostJobName = LamdaToFuncObj([](_RedisReply reply_) -> void {
		//	LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.name", "reply", reply_);
		//});
		//SyncJobManager::getSyncJobManager()->addJob(keyHGetName, makePostJobStatic(hSetPostJobName), nullptr);


		//std::string jsonStr;
		//toJson(*user.getData()).dump(jsonStr);
		//auto keyHGetJon = client->hset("blue_server.UserData.json", std::to_string(data.uid()).c_str(), jsonStr.c_str());
		//auto hSetPostJobJson = LamdaToFuncObj([](_RedisReply reply_) -> void {
		//	LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.json", "reply", reply_);
		//});
		//SyncJobManager::getSyncJobManager()->addJob(keyHGetJon, makePostJobStatic(hSetPostJobJson), nullptr);
	}


	MSG::RegistAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::REGIST_ANS, &ans);
}

void LoginHandler::redisSelectUser(SessionPtr session_, std::string name_)
{
	LOG(L_INFO_, " ");

	//RedisClientPtr client;
	//auto keyHGetName = client->hget("blue_server.UserData.name", name_.c_str());

	//// redis key blue_server.UserData.name response 
	//auto hGetPostJobName = LamdaToFuncObj([=](_RedisReply reply_) -> void {

	//	RedisClientPtr client;
	//	// no caching data check db
	//	if (reply_._type == REPY_NIL)
	//	{
	//		LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.name", "reply", reply_);

	//		// request select user data
	//		asyncJob(&LoginHandler::dbSelectUser, std::move(const_cast<SessionPtr&>(session_)), std::move(const_cast<std::string&>(name_)));
	//		return;
	//	}

	//	LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.name", "reply", reply_);

	//	//*
	//	// hget user data
	//	auto uid = reply_._string;
	//	auto keyHGetJson = client->hget("blue_server.UserData.json", uid.c_str());

	//	// redis key blue_server.UserData.json response
	//	auto hGetPostJobJson = LamdaToFuncObj([=](_RedisReply reply_) -> void {
	//		// no caching data
	//		if (reply_._type == REPY_NIL)
	//		{
	//			LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.json", "reply", reply_);
	//			return;
	//		}

	//		LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.json", "reply", reply_);
	//		User user(reply_.to_json());

	//		// response
	//		MSG::LoginAns ans;
	//		ans.set_err(MSG::ERR_SUCCESS);
	//		auto userData = ans.mutable_data();
	//		userData->CopyFrom(*user.getData());

	//		session_->SendPacket(MSG::LOGIN_ANS, &ans);
	//	});

	//	SyncJobManager::getSyncJobManager()->addJob(keyHGetJson, makePostJobStatic(hGetPostJobJson), nullptr);
	//	/**/
	//});

	//SyncJobManager::getSyncJobManager()->addJob(keyHGetName, makePostJobStatic(hGetPostJobName), nullptr);

	RedisClientPtr client;
	std::future<_RedisReply> futureReply;
	if (true == client->hget("blue_server.UserData.name", name_.c_str(), std::ref(futureReply)))
	{
		auto reply = futureReply.get();
		if (reply._type == REPY_NIL)
		{
			LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.name", "reply", reply);

			// request select user data
			asyncJob(&LoginHandler::dbSelectUser, std::move(const_cast<SessionPtr&>(session_)), std::move(const_cast<std::string&>(name_)));
			return;
		}

		LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.name", "reply", reply);

		auto uid = reply._string;
		std::future<_RedisReply> futureReply2;
		if (true == client->hget("blue_server.UserData.json", uid.c_str(), std::ref(futureReply2)))
		{
			auto reply = futureReply2.get();
			if (reply._type == REPY_NIL)
			{
				LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.json", "reply", reply);
				return;
			}

			LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.json", "reply", reply);
			User user(reply.to_json());

			// response
			MSG::LoginAns ans;
			ans.set_err(MSG::ERR_SUCCESS);
			auto userData = ans.mutable_data();
			userData->CopyFrom(*user.getData());

			session_->SendPacket(MSG::LOGIN_ANS, &ans);
		}
	}
}


DEFINE_HANDLER(LoginHandler, SessionPtr, LoginReq)
{
	MSG::LoginReq req;
	req.ParseFromArray(body_, len_);

	LOG(L_INFO_, "recv packet info", "id", req.name());
	{
		asyncJob(&LoginHandler::dbSelectUser, std::move(const_cast<SessionPtr&>(session_)), std::move(const_cast<std::string&>(req.name())));
	}
	
	return true;
}

DEFINE_HANDLER(LoginHandler, SessionPtr, RegistReq)
{
	MSG::RegistReq req;
	req.ParseFromArray(body_, len_);
	LOG(L_INFO_, "recv packet info");

	// check duplication 

	// data insert
	{
		// request select user data
		asyncJob(&LoginHandler::dbInsertUser, std::move(const_cast<SessionPtr&>(session_)), std::move(const_cast<MSG::UserData_&>(req.data())));
	}
	return true;
}


}