#include "LoginHandler.h"
#include "cpp/common.pb.h"
#include "cpp/login.pb.h"

#include "../BlueCore/DateTime.h"
#include "../BlueCore/Packet.h"
#include "../BlueCore/Logger.h"
#include "../BlueCore/JobExec.h"
#include "../BlueCore/SyncJobHelper.h"
#include "../BlueCore/RedisClient.h"

#include "User.h"
#include "DBQueryUser.h"

namespace BLUE_BERRY
{

LoginHandler::LoginHandler()
{
	REGIST_HANDLER(MSG::CLOSED, Closed);
	REGIST_HANDLER(MSG::LOGIN_REQ, LoginReq);
	REGIST_HANDLER(MSG::PING_REQ, PingReq);
}

bool LoginHandler::execute(SessionPtr session_, unsigned short id_, char * buff_, unsigned short len_)
{
	if (id_ >= MAX_MSG_ID || _table[id_] == NULL)
		return false;

	return _table[id_](session_, buff_, len_);
}

void LoginHandler::dbSelectUser(const SessionPtr session_, const std::string id_)
{
	// db select user data	
	DBQueryUser query;
	MSG::UserData_ data;
	data.set_name(id_);
	query.setData(data);

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
		if (query.insertData() == false)
		{
			MSG::LoginAns ans;
			ans.set_err(MSG::ERR_LOGIN_FAIL);
			session_->SendPacket(MSG::LOGIN_ANS, &ans);
			return;
		}
	}
	query.getData(data);


	User user(data);
	auto jUser = user.to_json();
	LOG(L_INFO_, "User", "data", jUser);


	// redis caching
	{
		RedisClientPtr client;
		auto keyHGetName = client->hset("blue_server.UserData.name", data.name().c_str(), std::to_string(data.uid()).c_str());
		auto hSetPostJobName = LamdaToFuncObj([&](_RedisReply reply_) -> void {
			LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.name", "reply", reply_);
		});
		SyncJobManager::getSyncJobManager()->addJob(keyHGetName, makePostJobStatic(hSetPostJobName), nullptr);


		std::string jsonStr;
		user.dump(jsonStr);
		auto keyHGetJon = client->hset("blue_server.UserData.json", std::to_string(data.uid()).c_str(), jsonStr.c_str() );
		auto hSetPostJobJson = LamdaToFuncObj([&](_RedisReply reply_) -> void {
			LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.json", "reply", reply_);
		});
		SyncJobManager::getSyncJobManager()->addJob( keyHGetJon, makePostJobStatic(hSetPostJobJson), nullptr);
	}


	MSG::LoginAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	auto userData = ans.mutable_data();
	userData->CopyFrom(user.toProtoSerialize());

	session_->SendPacket(MSG::LOGIN_ANS, &ans);
}

void LoginHandler::redisSelectUser(SessionPtr session_, std::string id_)
{
	LOG(L_INFO_, " ");
	LOG(L_INFO_, "Session1", "usecount", (int)session_.use_count());

	RedisClientPtr client;
	auto keyHGetName = client->hget("blue_server.UserData.name", id_.c_str());

	// redis key blue_server.UserData.name response 
	auto hGetPostJobName = LamdaToFuncObj([=](_RedisReply reply_) -> void {

		RedisClientPtr client;
		// no caching data check db
		if (reply_._type == REPY_NIL)
		{
			LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.name", "reply", reply_);

			// request select user data
			asyncJob(&LoginHandler::dbSelectUser, std::move(const_cast<SessionPtr&>(session_)), std::move(const_cast<std::string&>(id_)));
			return;
		}

		LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.name", "reply", reply_);

		//LOG(L_INFO_, "Session2", "usecount", (int)session_.use_count());

		//*
		// hget user data
		auto uid = reply_._string;
		auto keyHGetJson = client->hget("blue_server.UserData.json", uid.c_str());

		// redis key blue_server.UserData.json response
		auto hGetPostJobJson = LamdaToFuncObj([=](_RedisReply reply_) -> void {
			// no caching data
			if (reply_._type == REPY_NIL)
			{
				LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.json", "reply", reply_);
				return;
			}

			LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.json", "reply", reply_);
			User user(reply_.to_json());

			// response
			MSG::LoginAns ans;
			ans.set_err(MSG::ERR_SUCCESS);
			auto userData = ans.mutable_data();
			userData->CopyFrom(user.toProtoSerialize());

			session_->SendPacket(MSG::LOGIN_ANS, &ans);
		});

		SyncJobManager::getSyncJobManager()->addJob(keyHGetJson, makePostJobStatic(hGetPostJobJson), nullptr);
		/**/
	});

	SyncJobManager::getSyncJobManager()->addJob(keyHGetName, makePostJobStatic(hGetPostJobName), nullptr);
	//LOG(L_INFO_, "Session3", "usecount", (int)session_.use_count());
}


DEFINE_HANDLER(LoginHandler, SessionPtr, LoginReq)
{
	MSG::LoginReq req;
	req.ParseFromArray(body_, len_);

	LOG(L_INFO_, "recv packet", "id", req.id());
	//LOG(L_INFO_, "Session1", "usecount", (int)session_.use_count());

	{
		// request db select job
		asyncJob(&LoginHandler::redisSelectUser, std::move(const_cast<SessionPtr&>(session_)), std::move(const_cast<std::string&>(req.id())));
	}
	//LOG(L_INFO_, "Session2", "usecount", (int)session_.use_count());
	
	return true;
}


DEFINE_HANDLER(LoginHandler, SessionPtr, PingReq)
{
	MSG::PingReq req;
	req.ParseFromArray(body_, len_);
	LOG(L_INFO_, " ");
	//LOG(L_INFO_, "recv packet", "time", DateTime::getCurrentDateTime().format());

	MSG::PongAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::PONG_ANS, &ans);
	return true;
}

DEFINE_HANDLER(LoginHandler, SessionPtr, Closed)
{
	LOG(L_INFO_, " ");
	//LOG(L_INFO_, "Session", "usecount", (int)session_.use_count());
	session_->disconnect();
}

}