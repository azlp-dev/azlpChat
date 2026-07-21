#pragma once

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "single.h"
#include "ConfigMgr.h"
#include <atomic>
#include <queue>
#include <condition_variable>

class RPC_ConPool {
public:
	RPC_ConPool(std::size_t poolsize,std::string host,std::string port)
	:_poolsize(poolsize),_host(host),_port(port),_b_stop(false)
	{
		std::string addr = _host + ":" + port;
		for (size_t i = 0;i < _poolsize;i++) {
			std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(addr
				, grpc::InsecureChannelCredentials());
			_stubs.push(message::VarifyService::NewStub(channel));
		}
	}

	void Close() {
		_b_stop = true;
		_condition.notify_all();
	}

	~RPC_ConPool() {
		std::lock_guard<std::mutex> lock(_mtx);
		Close();
		while (!_stubs.empty()) {
			_stubs.pop();
		}
	}

	std::unique_ptr<message::VarifyService::Stub> Get_Stub() {
		std::unique_lock<std::mutex> lock(_mtx);
		_condition.wait(lock, [this]() {
			if (_b_stop) {
				return true;
			}
			return !_stubs.empty();
			});
		if (_b_stop) {
			return nullptr;
		}
		auto stub = std::move(_stubs.front());
		_stubs.pop();
		return stub;
	}

	void returnStub(std::unique_ptr<message::VarifyService::Stub> stub) {
		std::lock_guard<std::mutex> lock(_mtx);
		if (_b_stop) {
			return;
		}
		_stubs.push(std::move(stub));
		_condition.notify_one();
	}
private:
	std::atomic<bool> _b_stop;
	std::size_t _poolsize;
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<message::VarifyService::Stub>> _stubs;
	std::condition_variable _condition;
	std::mutex _mtx;
};

class Varify_Client :public Singleton<Varify_Client>
{
	friend class Singleton<Varify_Client>;
public:
	message::GetVarifyRsp GetVarifyCode(std::string email) {
		grpc::ClientContext context;
		message::GetVarifyReq request;
		message::GetVarifyRsp response;
		request.set_email(email);
		auto stub = _pool->Get_Stub();
		grpc::Status sta = stub->GetVarifyCode(&context,request,&response);
		if (sta.ok()) {
			_pool->returnStub(std::move(stub));
			return response;
		}
		else {
			_pool->returnStub(std::move(stub));
			response.set_error(Errids::RPC_FAILED);
			return response;
		}
		
	}
private:
	Varify_Client() {
		auto conf_mgr = ConfigMgr::Get_instance();
		std::string host = (*conf_mgr)["VarifyServer"]["Host"];
		std::string port = (*conf_mgr)["VarifyServer"]["Port"];
		_pool.reset(new RPC_ConPool(5,host,port));
	}
	std::unique_ptr<RPC_ConPool> _pool;
};

