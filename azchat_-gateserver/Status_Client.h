#pragma once

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <atomic>
#include <mutex>
#include <queue>
#include <memory>

#include "const.h"
//建立一个状态服务客户端 gateserver访问状态服务并与客户端进行通信
//grpc: 客户端-》stub-》序列化数据-》tcp传输-》服务器反序列化-》调用服务

class RpcCon_Pool {
public:
	RpcCon_Pool(std::string host, int port, size_t poolsize);
	~RpcCon_Pool();
	void close();
	std::unique_ptr<message::StatusService::Stub> getConn();
	void retConn(std::unique_ptr<message::StatusService::Stub> stub);



private:
	
	size_t _poolsize;
	std::string _host;
	int _port;
	
	std::mutex _mtx;
	std::condition_variable _cond;
	std::atomic<bool> _b_stop;
	std::queue<std::unique_ptr<message::StatusService::Stub>> _stubs; //stub相当于远程代理负责序列化数据并收发
};


class Status_Client :public Singleton<Status_Client>
{
	friend class Singleton<Status_Client>;
public:
	~Status_Client() {};
	message::GetChatServerRsp GetChatServer(int uid);


private:
	Status_Client();

	std::unique_ptr<RpcCon_Pool> _pool;
};

