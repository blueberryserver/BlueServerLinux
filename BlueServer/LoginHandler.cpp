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

namespace BLUE_BERRY
{

LoginHandler::LoginHandler()
{
	REGIST_HANDLER(MSG::CLOSED, Closed);

	REGIST_HANDLER(MSG::LOGIN_REQ, LoginReq);
	REGIST_HANDLER(MSG::PING_REQ, PingReq);
	REGIST_HANDLER(MSG::REGIST_REQ, RegistReq);
}

bool LoginHandler::execute(SessionPtr session_, unsigned short id_, char * buff_, unsigned short len_)
{
	if (id_ >= MAX_MSG_ID || _table[id_] == NULL)
		return false;

	return _table[id_](session_, buff_, len_);
}

void LoginHandler::dbSelectUser(const SessionPtr session_, const std::string name_)
{
	// db select user data	
	DBQueryUser query;
	MSG::UserData_ data;
	data.set_name(name_);
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
		MSG::LoginAns ans;
		ans.set_err(MSG::ERR_LOGIN_FAIL);
		session_->SendPacket(MSG::LOGIN_ANS, &ans);
		return;
	}

	query.getData(data);

	// update login date
	auto now = DateTime::getCurrentDateTime().formatLocal();
	data.set_login_date(now.c_str());

	auto user = std::make_shared<User>(data);
	user->setSession(session_);
	//User user(data);
	LOG(L_INFO_, "User", "key", user->getSessionKey(), "data", user->to_json());

	UserManager::getUserManager()->add(user);

	// redis caching
	{
		RedisClientPtr client;
		auto keyHGetName = client->hset("blue_server.UserData.name", data.name().c_str(), std::to_string(data.uid()).c_str());
		auto hSetPostJobName = LamdaToFuncObj([](_RedisReply reply_) -> void {
			LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.name", "reply", reply_);
		});
		SyncJobManager::getSyncJobManager()->addJob(keyHGetName, makePostJobStatic(hSetPostJobName), nullptr);


		std::string jsonStr;
		user->dump(jsonStr);
		auto keyHGetJon = client->hset("blue_server.UserData.json", std::to_string(data.uid()).c_str(), jsonStr.c_str() );
		auto hSetPostJobJson = LamdaToFuncObj([](_RedisReply reply_) -> void {
			LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.json", "reply", reply_);
		});
		SyncJobManager::getSyncJobManager()->addJob( keyHGetJon, makePostJobStatic(hSetPostJobJson), nullptr);
	}


	MSG::LoginAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	auto userData = ans.mutable_data();
	userData->CopyFrom(user->getData());
	ans.set_session_key(user->getSessionKey());

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
	auto jUser = user.to_json();
	LOG(L_INFO_, "User", "data", jUser);


	// redis caching
	{
		RedisClientPtr client;
		auto keyHGetName = client->hset("blue_server.UserData.name", data.name().c_str(), std::to_string(data.uid()).c_str());
		auto hSetPostJobName = LamdaToFuncObj([](_RedisReply reply_) -> void {
			LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.name", "reply", reply_);
		});
		SyncJobManager::getSyncJobManager()->addJob(keyHGetName, makePostJobStatic(hSetPostJobName), nullptr);


		std::string jsonStr;
		user.dump(jsonStr);
		auto keyHGetJon = client->hset("blue_server.UserData.json", std::to_string(data.uid()).c_str(), jsonStr.c_str());
		auto hSetPostJobJson = LamdaToFuncObj([](_RedisReply reply_) -> void {
			LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.json", "reply", reply_);
		});
		SyncJobManager::getSyncJobManager()->addJob(keyHGetJon, makePostJobStatic(hSetPostJobJson), nullptr);
	}


	MSG::RegistAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::REGIST_ANS, &ans);
}

void LoginHandler::redisSelectUser(SessionPtr session_, std::string name_)
{
	LOG(L_INFO_, " ");

	RedisClientPtr client;
	auto keyHGetName = client->hget("blue_server.UserData.name", name_.c_str());

	// redis key blue_server.UserData.name response 
	auto hGetPostJobName = LamdaToFuncObj([=](_RedisReply reply_) -> void {

		RedisClientPtr client;
		// no caching data check db
		if (reply_._type == REPY_NIL)
		{
			LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.name", "reply", reply_);

			// request select user data
			asyncJob(&LoginHandler::dbSelectUser, std::move(const_cast<SessionPtr&>(session_)), std::move(const_cast<std::string&>(name_)));
			return;
		}

		LOG(L_INFO_, "Redis", "hget", "blue_server.UserData.name", "reply", reply_);

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
			userData->CopyFrom(user.getData());

			session_->SendPacket(MSG::LOGIN_ANS, &ans);
		});

		SyncJobManager::getSyncJobManager()->addJob(keyHGetJson, makePostJobStatic(hGetPostJobJson), nullptr);
		/**/
	});

	SyncJobManager::getSyncJobManager()->addJob(keyHGetName, makePostJobStatic(hGetPostJobName), nullptr);
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

DEFINE_HANDLER(LoginHandler, SessionPtr, PingReq)
{
	MSG::PingReq req;
	req.ParseFromArray(body_, len_);
	LOG(L_INFO_, "recv packet", "key", req.session_key(), "time", DateTime::getCurrentDateTime().formatLocal());

	// find user by session key
	auto user = UserManager::getUserManager()->find(req.session_key());
	if (user == nullptr)
	{
		MSG::PongAns ans;
		ans.set_err(MSG::ERR_SESSIONKEY_FAIL);
		session_->SendPacket(MSG::PONG_ANS, &ans);
		return true;
	}

	// update session key time out
	user->setPingTime(DateTime::GetTickCount() + 1000 * 1000 * 60);
	if (user->getSession() == nullptr || user->getSession() != session_)
	{
		user->setSession(session_);
	}

	MSG::PongAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::PONG_ANS, &ans);
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


DEFINE_HANDLER(LoginHandler, SessionPtr, Closed)
{
	LOG(L_INFO_, " ");

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user != nullptr)
	{
		//UserManager::getUserManager()->remove(session_.get());
		user->setSession(nullptr);
	}

	session_->disconnect();
	return true;
}

}