#include "Grpc_Client.h"
#include "ConfigMgr.h"


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
	return message::AddFriendRsp();
}

Grpc_Client::Grpc_Client()
{
	auto conf = *ConfigMgr::Get_instance();

	std::stringstream ss(conf["ChatServers"]["Name"]);
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
