#include "StatusServerImpl.h"
#include "ConfigMgr.h"
#include <boost/uuid.hpp>
#include "RedisMgr.h"

std::string unique_string_gen() {
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	std::string ret = boost::uuids::to_string(uuid);
	return ret;
}

StatusServerImpl::StatusServerImpl()
{
	auto conf_mgr = ConfigMgr::Get_instance();
	auto cfg = *conf_mgr;
	chatserver ser;
	std::stringstream ss(cfg["ChatServers"]["Name"]);
	std::vector<std::string> words;
	std::string word;
	while (std::getline(ss, word, ',')) {
		words.push_back(word);
	}
	for (int i = 0; i < words.size(); i++) {
		ser.host = cfg[words[i]]["Host"];
		ser.port = cfg[words[i]]["Port"];
		ser.name = cfg[words[i]]["Name"];
		_server_map[ser.name] = ser;
	}
}	

grpc::Status StatusServerImpl::GetChatServer(grpc::ServerContext * context, const message::GetChatServerReq * req, message::GetChatServerRsp * rsp)
{
	const auto& server = getMinServer();
	rsp->set_host(server.host);
	rsp->set_port(server.port);
	rsp->set_error(Errids::SUCCESS);
	rsp->set_token(unique_string_gen());
	insertToken(req->uid(), rsp->token());
	return grpc::Status::OK;
}

grpc::Status StatusServerImpl::Login(grpc::ServerContext* context, const message::LoginReq* req, message::LoginRsp* rsp)
{
	auto uid = req->uid();
	auto token = req->token();

	std::string tokenkey = USERTOKENPREFIX + std::to_string(uid);
	std::string tokenvalue = "";

	bool success = RedisMgr::Get_instance()->Get(tokenkey, tokenvalue);
	if (!success) {
		rsp->set_error(Errids::Redis_Invaild_Uids);
		return grpc::Status::OK;
	}
	if (tokenvalue != token) {
		rsp->set_error(Errids::Redis_Invaild_Token);
		return grpc::Status::OK;
	}
	rsp->set_error(Errids::SUCCESS);
	rsp->set_uid(uid);
	rsp->set_token(tokenvalue);
	return grpc::Status::OK;
}

void StatusServerImpl::insertToken(int uid,std::string token)
{
	std::string uids = std::to_string(uid);
	std::string tokenkey = USERTOKENPREFIX + uids;
	RedisMgr::Get_instance()->Set(tokenkey,token);
	return;
}

chatserver StatusServerImpl::getMinServer()
{
	std::lock_guard<std::mutex> lock(_server_mtx);
	chatserver minServer = _server_map.begin()->second;
	auto countstr = RedisMgr::Get_instance()->HGet(LOGIN_COUNT, minServer.name);
	if (countstr.empty()) {
		minServer.con_count = INT_MAX;
	}
	else {
		minServer.con_count = std::stoi(countstr);
	}
	for (auto& ser : _server_map) {
		auto& server = ser.second;
		if (server.name == minServer.name) {
			continue;
		}
		auto count_str = RedisMgr::Get_instance()->HGet(LOGIN_COUNT, server.name);
		if (count_str.empty()) {
			server.con_count = INT_MAX;
		}else{
			server.con_count = std::stoi(count_str);
			if (server.con_count < minServer.con_count) {
				minServer = server;  
			}
		}
	}
	return minServer;
}
