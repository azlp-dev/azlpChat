#pragma once
#include "const.h"
#include "message.pb.h"
#include "message.grpc.pb.h"
#include <queue>
#include <grpcpp/grpcpp.h>
#include <unordered_map>
#include "data.h"
class UserMgr;
class ConPool
{
public:
	ConPool(int poolsize,std::string host,std::string port);
	~ConPool();
	std::unique_ptr<message::ChatService::Stub> getStub();
	void retstub(std::unique_ptr<message::ChatService::Stub> stub);

private:
	int _poolsize;
	std::string _host;
	std::string _port;
	
	std::atomic<bool> _b_stop;
	std::mutex _mtx;
	std::condition_variable cond;
	std::queue<std::unique_ptr<message::ChatService::Stub>> _stubs;
};


class Grpc_Client: public Singleton<Grpc_Client>
{
	friend class Singleton<Grpc_Client>;
public:
	~Grpc_Client() {}

	message::AddFriendRsp NotifyAddFriend(std::string server_ip, const message::AddFriendReq& req);
	message::AuthFriendRsp NotifyAuthFriend(std::string server_ip, const message::AuthFriendReq& req);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	message::TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const message::TextChatMsgReq& req, const Json::Value& rtvalue);

private:
	Grpc_Client();

	std::unordered_map<std::string, std::unique_ptr<ConPool>> _pools;
};

