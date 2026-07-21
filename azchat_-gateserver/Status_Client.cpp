#include "Status_Client.h"
#include "ConfigMgr.h"

RpcCon_Pool::RpcCon_Pool(std::string host, int port, size_t poolsize)
	:_host(host),_port(port),_poolsize(poolsize),_b_stop(false)
{
	for (int i = 0; i < poolsize; i++) {
		std::shared_ptr<grpc::Channel>
			channel = grpc::CreateChannel(host + ":" + std::to_string(port), grpc::InsecureChannelCredentials());
			//channel为逻辑的通信信道创建stub所必需
		_stubs.push(message::StatusService::NewStub(channel));
	}


}

RpcCon_Pool::~RpcCon_Pool()
{
	std::lock_guard<std::mutex> lock(_mtx);
	close();
	while (!_stubs.empty()) {
		_stubs.pop();
	}
}

void RpcCon_Pool::close()
{
	_b_stop = true;
	_cond.notify_all();
}

std::unique_ptr<message::StatusService::Stub> RpcCon_Pool::getConn()
{
	std::unique_lock<std::mutex> lock(_mtx);
	_cond.wait(lock, [this]() {
		if (_b_stop) {
			return true;
		}
		return !_stubs.empty();
		});
	if (_b_stop) {
		return nullptr;
	}
	auto context = std::move(_stubs.front());
	_stubs.pop();
	return context;
}

void RpcCon_Pool::retConn(std::unique_ptr<message::StatusService::Stub> stub)
{
	std::lock_guard<std::mutex> lock(_mtx);
	if (_b_stop) {
		return;
	}
	_stubs.push(std::move(stub));
	_cond.notify_one();
	return;
}

message::GetChatServerRsp Status_Client::GetChatServer(int uid)
{
	grpc::ClientContext context; //类似与http中的请求头
	message::GetChatServerReq req;
	message::GetChatServerRsp rsp;

	auto stub = _pool->getConn();
	req.set_uid(uid);
	grpc::Status status = stub->GetChatServer(&context, req, &rsp);
	if (status.ok()) {
		_pool->retConn(std::move(stub));
		return rsp;
	}
	else {
		rsp.set_error(Errids::RPC_FAILED);
		_pool->retConn(std::move(stub));
		return rsp;
	}

}

Status_Client::Status_Client()
{
	auto conf_mgr = ConfigMgr::Get_instance();
	auto host = (*conf_mgr)["StatusServer"]["Host"];
	auto port = (*conf_mgr)["StatusServer"]["Port"];
	
	_pool.reset(new RpcCon_Pool(host, std::stoi(port), 5));
}
