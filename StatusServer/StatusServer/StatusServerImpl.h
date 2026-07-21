#pragma once
#include <grpcpp/grpcpp.h>
#include "const.h"
#include "message.pb.h"
#include "message.grpc.pb.h"
#include <vector>
#include <map>


class StatusServerImpl:public message::StatusService::Service
{
public:
	StatusServerImpl();
	grpc::Status GetChatServer(grpc::ServerContext* context,const message::GetChatServerReq* req,message::GetChatServerRsp* rsp)
		override;
	grpc::Status Login(grpc::ServerContext* context, const message::LoginReq* req, message::LoginRsp* rsp);
	void insertToken(int uid,std::string token);
	chatserver getMinServer();

private:
	std::map<std::string,chatserver> _server_map;
	std::mutex _server_mtx;
	int _server_index{0};
};

