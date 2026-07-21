#include "Grpc_Client.h"
#include "ConfigMgr.h"
#include "UserMgr.h"

ConPool::ConPool(int poolsize, std::string host, std::string port)
	:_poolsize(poolsize),_b_stop(false),_host(host),_port(port)
{
	for (int i = 0; i < _poolsize; i++) {
		std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(_host + ":" + _port, grpc::InsecureChannelCredentials());
		_stubs.push(message::ChatService::NewStub(channel));
	}
}

ConPool::~ConPool()
{
	std::lock_guard<std::mutex> lock(_mtx);
	_b_stop = false;
	cond.notify_all();
	while (!_stubs.empty()) {
		_stubs.pop();
	}
}

std::unique_ptr<message::ChatService::Stub> ConPool::getStub()
{
	std::unique_lock<std::mutex> lock(_mtx);
	cond.wait(lock, [this]() {
		return !_stubs.empty() || _b_stop;
		});
	if (_b_stop) {
		return nullptr;
	}
	auto stub = std::move(_stubs.front());
	_stubs.pop();
	return stub;
}

void ConPool::retstub(std::unique_ptr<message::ChatService::Stub> stub)
{
	std::lock_guard<std::mutex> lock(_mtx);
	if (_b_stop) {
		return;
	}
	_stubs.push(std::move(stub));
	cond.notify_one();
	return;
}

message::AddFriendRsp Grpc_Client::NotifyAddFriend(std::string server_ip, const message::AddFriendReq& req)
{
	message::AddFriendRsp rsp;
	rsp.set_error(Errids::SUCCESS);
	Defer defer1([&req, &rsp]() {
		//rsp.set_error(Errids::SUCCESS);
		rsp.set_applyuid(req.applyuid());
		rsp.set_touid(req.touid());
		});

	auto it = _pools.find(server_ip);
	if (it == _pools.end()) {
		return rsp;
	}
	auto& pool = it->second;
	auto stub = pool->getStub();
	grpc::ClientContext context;
	grpc::Status status = stub->NotifyAddFriend(&context, req, &rsp);
	Defer defer2([&stub, this, &pool]() {
		pool->retstub(std::move(stub));
		});
	if (!status.ok()) {
		rsp.set_error(Errids::RPC_FAILED);
		return rsp;
	}

	return rsp;
}

message::AuthFriendRsp Grpc_Client::NotifyAuthFriend(std::string server_ip, const message::AuthFriendReq& req)
{
	message::AuthFriendRsp rsp;
	rsp.set_error(Errids::SUCCESS);
	Defer defer1([&req, &rsp]() {
		//rsp.set_error(Errids::SUCCESS);
		
		rsp.set_fromuid(req.fromuid());
		rsp.set_touid(req.touid());
		});

	auto it = _pools.find(server_ip);
	if (it == _pools.end()) {
		return rsp;
	}
	auto& pool = it->second;
	auto stub = pool->getStub();
	grpc::ClientContext context;
	grpc::Status status = stub->NotifyAuthFriend(&context, req, &rsp);
	Defer defer2([&stub, this, &pool]() {
		pool->retstub(std::move(stub));
		});
	if (!status.ok()) {
		rsp.set_error(Errids::RPC_FAILED);
		return rsp;
	}

	return rsp;
}

bool Grpc_Client::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
	return false;
}

message::TextChatMsgRsp Grpc_Client::NotifyTextChatMsg(std::string server_ip, const message::TextChatMsgReq& req, const Json::Value& rtvalue)
{
	message::TextChatMsgRsp rsp;
	rsp.set_error(Errids::SUCCESS);

	Defer defer([&rsp, &req]() {
		rsp.set_fromuid(req.fromuid());
		rsp.set_touid(req.touid());
		for (const auto& text_data : req.textmsg()) {
			message::TextChatData* new_msg = rsp.add_textmsg();
			new_msg->set_msgid(text_data.msgid());
			new_msg->set_msgcontent(text_data.msgcontent());
		}

		});

	auto find_iter = _pools.find(server_ip);
	if (find_iter == _pools.end()) {
		return rsp;
	}

	auto& pool = find_iter->second;
	grpc::ClientContext context;
	auto stub = pool->getStub();
	grpc::Status status = stub->TextChatMsg(&context, req, &rsp);
	Defer defercon([&stub, this, &pool]() {
		pool->retstub(std::move(stub));
		});

	if (!status.ok()) {
		rsp.set_error(Errids::RPC_FAILED);
		return rsp;
	}

	return rsp;
}

Grpc_Client::Grpc_Client()
{
	auto usermgr = UserMgr::Get_instance();
	auto conf = *ConfigMgr::Get_instance();

	std::stringstream ss(conf["PeerServer"]["servers"]);
	std::vector<std::string> words;
	std::string word;
	while (std::getline(ss, word, ',')) {
		words.push_back(word);
	}
	for (int i = 0; i < words.size(); i++) {
		std::string host = conf[words[i]]["Host"];
		std::string port = conf[words[i]]["Port"];
		
		_pools[conf[words[i]]["Name"]] = std::make_unique<ConPool>(5, host, port); 
	}

}
