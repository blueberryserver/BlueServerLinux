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
	REGIST_HANDLER(MSG::LOGIN_REQ, LoginReq);
	REGIST_HANDLER(MSG::PING_REQ, PingReq);
}

bool LoginHandler::execute(SessionPtr session_, unsigned short id_, char * buff_, unsigned short len_)
{
	if (id_ >= MAX_MSG_ID || _table[id_] == NULL)
		return false;

	return _table[id_](session_, buff_, len_);
}

void LoginHandler::dbSelectUser(SessionPtr session_, std::string id_)
{
	// db select	
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


	MSG::LoginAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	auto userData = ans.mutable_data();
	userData->CopyFrom(user.toProtoSerialize());

	session_->SendPacket(MSG::LOGIN_ANS, &ans);
}

void LoginHandler::redisSelectUser(SessionPtr session_, std::string id_)
{
	RedisClientPtr client;
	auto keyHGetName = client->hget("blue_server.UserData.name", id_.c_str());
	auto hGetPostJobName = LamdaToFuncObj([&](_RedisReply reply_) -> void {

		RedisClientPtr client;
		// no caching data
		if (reply_._type == REPY_NIL)
		{
			LOG(L_INFO_, "UserData.Name", "ErrorMsg", "no caching data");
			return;
		}

		LOG(L_INFO_, "UserData.Name", "data", reply_);

		auto uid = reply_._string;
		auto keyHGetJson = client->hget("blue_server.UserData.json", uid.c_str());

		auto hGetPostJobJson = LamdaToFuncObj([&](_RedisReply reply_) -> void {
			// no caching data
			if (reply_._type == REPY_NIL)
			{
				LOG(L_INFO_, "UserData.Json", "ErrorMsg", "no caching data");
				return;
			}

			LOG(L_INFO_, "UserData.Json", "data", reply_);
			User user(reply_.to_json());

			MSG::LoginAns ans;
			ans.set_err(MSG::ERR_SUCCESS);
			auto userData = ans.mutable_data();
			userData->CopyFrom(user.toProtoSerialize());

			session_->SendPacket(MSG::LOGIN_ANS, &ans);
		});

		SyncJobManager::getSyncJobManager()->addJob(
			std::make_shared<SyncJobHelper>(keyHGetJson, makePostJobStatic(hGetPostJobJson), nullptr));
	});

	SyncJobManager::getSyncJobManager()->addJob(
		std::make_shared<SyncJobHelper>(keyHGetName, makePostJobStatic(hGetPostJobName), nullptr));
}


DEFINE_HANDLER(LoginHandler, SessionPtr, LoginReq)
{
	MSG::LoginReq req;
	req.ParseFromArray(body_, len_);

	LOG(L_INFO_, "LoginReq ", "id", req.id());


	{
		// request db select job
		auto asyncJob = makeAsyncJob(&LoginHandler::redisSelectUser, std::move(session_), std::move(const_cast<std::string&>(req.id())));
		IOService::getIOService()->post(boost::bind(&Job::onExecute, asyncJob));
	}


	/*
	{
		// request db select job
		auto asyncJob = makeAsyncJob(&LoginHandler::dbSelectUser, std::move(session_), std::move(const_cast<std::string&>(req.id())));
		IOService::getIOService()->post(boost::bind(&Job::onExecute, asyncJob));
	}
	*/
	
	return true;
}


DEFINE_HANDLER(LoginHandler, SessionPtr, PingReq)
{
	MSG::PingReq req;
	req.ParseFromArray(body_, len_);

	LOG(L_INFO_, "PingReq ", "time", DateTime::getCurrentDateTime().format());

	MSG::PongAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::PONG_ANS, &ans);
	return true;
}

}