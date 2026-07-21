#include "LogicSystem.h"
#include "HttpConnection.h"
#include "Varify_Client.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
#include "Status_Client.h"

bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConnection> con)
{
	if (_GetHandlers.find(url) == _GetHandlers.end()) {
		return false;
	}
	_GetHandlers[url](con);
	return true;
}

bool LogicSystem::HandlePost(std::string url, std::shared_ptr<HttpConnection> con)
{
	if (_PostHandlers.find(url) == _PostHandlers.end()) {
		return false;
	}
	_PostHandlers[url](con);
	return true;
}

void LogicSystem::RegGetHandler(std::string url, HttpHandler handler)
{
	_GetHandlers.insert(std::make_pair(url, handler));
}

void LogicSystem::RegPostHandler(std::string url, HttpHandler handler)
{
	_PostHandlers.insert(std::make_pair(url, handler));
}

LogicSystem::LogicSystem()
{
	RegGetHandler("/get_test", [](std::shared_ptr<HttpConnection> con) {
		boost::beast::ostream(con->_response.body()) << "receive get req\r\n";
		int i = 0;
		for (auto& elem : con->_get_params) {
			i++;
			boost::beast::ostream(con->_response.body())
				<< "param" << i << " key is " << elem.first<<std::endl;
			boost::beast::ostream(con->_response.body())
				<< "param" << i << " value is " << elem.second << std::endl;
		}
		});

	RegPostHandler("/get_varifycode", [](std::shared_ptr<HttpConnection> con) {
		auto bodystr = boost::beast::buffers_to_string(con->_request.body().data());
		//std::cout << "receive data is " << bodystr << std::endl;
		con->_response.set(boost::beast::http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(bodystr, src_root);
		if (!parse_success) {
			std::cout << "Failed to parse json!" << std::endl;
			root["error"] = Errids::ERR_JSON;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << jsonstr;
			return true;
		}
		if (!src_root.isMember("email")) {
			std::cout << "Failed to parse json!" << std::endl;
			root["error"] = Errids::ERR_JSON;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << jsonstr;
			return true;
		}
		auto email = src_root["email"].asString();
		std::cout << "email is " << email << std::endl;
		message::GetVarifyRsp rsp = Varify_Client::Get_instance()->GetVarifyCode(email);
		root["error"] = rsp.error();
		root["email"] = src_root["email"];
		std::string jsonstr = root.toStyledString();
		boost::beast::ostream(con->_response.body()) << jsonstr;
		return true;
		});
	
	RegPostHandler("/user_register", [](std::shared_ptr<HttpConnection> con) {
		auto body_str = boost::beast::buffers_to_string(con->_request.body().data());
		std::cout << "receive body is:" << body_str << std::endl;
		con->_response.set(boost::beast::http::field::content_type, "text/json");

		Json::Reader reader;
		Json::Value root;
		Json::Value src_root;
		bool b_parse = reader.parse(body_str, src_root);
		if (!b_parse) {
			std::cout << "error to parse json" << std::endl;
			root["error"] = Errids::ERR_JSON;
			auto resp_body = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp_body;
			return true;
		}
		std::string redis_varify_code;
		bool b_get_varify = RedisMgr::Get_instance()->Get(CODE_PREFIX + src_root["email"].asString(), redis_varify_code);
		if (!b_get_varify) {
			std::cout << "varify code not found" << std::endl;
			root["error"] = Errids::Redis_VarifyCode_NotFound;
			auto resp_body = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp_body;
			return true;
		}
		if (redis_varify_code != src_root["varify_code"].asString()) {
			std::cout << "varifycode incorrect" << std::endl;
			root["error"] = Errids::Redis_VarifyCode_Incorrect;
			auto resp_body = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp_body;
			return true;
		}
		//bool b_user_exist = RedisMgr::Get_instance()->ExistsKey(src_root["user"].asString());
		//if (b_user_exist) {
		//	std::cout << "user already exist" << std::endl;
		//	root["error"] = Errids::Redis_User_UserExist;
		//	auto resp_body = root.toStyledString();
		//	boost::beast::ostream(con->_response.body()) << resp_body;
		//	return true;
		//}

		//此处插入数据库操作代码，用户是否存在，注册成功后的增加操作。。。
		auto sqlmgr = MysqlMgr::Get_instance();
		int uid = sqlmgr->RegUserF(src_root["user"].asString(), src_root["email"].asString(), src_root["pass"].asString());
		if (uid == 0 || uid == -1) {
			std::cout << "user or email already exist" << std::endl;
			root["error"] = Errids::Mysql_User_UserExist;
			auto resp_body = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp_body;
			return true;
		}
		root["uid"] = uid;
		root["error"] = Errids::SUCCESS;
		root["email"] = src_root["email"];
		root["user"] = src_root["user"];
		root["pass"] = src_root["pass"];
		root["confirm"] = src_root["confirm"];
		root["varify_code"] = src_root["varify_code"];

		auto resp_body = root.toStyledString();
		boost::beast::ostream(con->_response.body()) << resp_body;
		return true;
		});
	RegPostHandler("/reset_pwd", [](std::shared_ptr<HttpConnection> con) {
		auto body_str = boost::beast::buffers_to_string(con->_request.body().data());
		std::cout << "receive reset_pwd data" << body_str << std::endl;
		con->_response.set(boost::beast::http::field::content_type, "text/json");

		Json::Reader jsreader;
		Json::Value src_root;
		Json::Value root;

		bool b_parse = jsreader.parse(body_str, src_root);
		if (!b_parse) {
			std::cout << "error to parse json" << std::endl;
			root["error"] = Errids::ERR_JSON;
			auto resp_body = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp_body;
			return true;
		}
		auto email = src_root["email"].asString();
		auto user = src_root["user"].asString();
		auto code = src_root["code"].asString();
		auto newpass = src_root["newpass"].asString();
		
		std::string code_get;
		bool b_codeGet = RedisMgr::Get_instance()->Get(CODE_PREFIX + email, code_get);
		if (!b_codeGet) {
			std::cout << "varify code not found" << std::endl;
			root["error"] = Errids::Redis_VarifyCode_NotFound;
			auto resp_body = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp_body;
			return true;
		}
		if (code_get != code) {
			std::cout << "varifycode incorrect" << std::endl;
			root["error"] = Errids::Redis_VarifyCode_Incorrect;
			auto resp_body = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp_body;
			return true;
		}

		bool email_vaild = MysqlMgr::Get_instance()->checkEmail(user,email);
		if (!email_vaild) {
			std::cout << "name email do not match" << std::endl;
			root["error"] = Errids::Mysql_EmailName_NotMatcth;
			auto resp_body = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp_body;
			return true;
		}

		bool uppass_vaild = MysqlMgr::Get_instance()->updatePass(user, newpass);
		if (!uppass_vaild) {
			std::cout << "passUp Failed" << std::endl;
			root["error"] = Errids::Mysql_PassUp_Failed;
			auto resp_body = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp_body;
			return true;
		}

		root["error"] = Errids::SUCCESS;
		root["user"] = src_root["user"];
		root["email"] = src_root["email"];
		root["newpass"] = src_root["newpass"];
		root["code"] = src_root["code"];
		auto resp_body = root.toStyledString();
		boost::beast::ostream(con->_response.body()) << resp_body;
		return true;
		});

	RegPostHandler("/userlogin", [](std::shared_ptr<HttpConnection> con) {
		auto body_str = boost::beast::buffers_to_string(con->_request.body().data());
		std::cout << "userlogin data" << body_str << std::endl;
		con->_response.set(boost::beast::http::field::content_type, "text/json");

		Json::Reader jsreader;
		Json::Value src_root;
		Json::Value root;

		bool b_parse = jsreader.parse(body_str, src_root);
		if (!b_parse) {
			std::cout << "error to parse json" << std::endl;
			root["error"] = Errids::ERR_JSON;
			auto resp_body = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp_body;
			return true;
		}
		auto user = src_root["user"].asString();
		auto pass = src_root["pass"].asString();
		UserInfo info;
		bool check_pass = MysqlMgr::Get_instance()->checkPass(user, pass, info);
		if (!check_pass) {
			std::cout << "error in login,pass check error" << std::endl;
			root["error"] = Errids::Mysql_Login_PassWrong;
			auto resp = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << resp;
			return true;
		}
		//std::cout << "now nownownnow" << std::endl;
		auto reply = Status_Client::Get_instance()->GetChatServer(info.uid);
		if (reply.error()) {
			std::cout << " grpc get chat server failed, error is " << reply.error() << std::endl;
			root["error"] = Errids::RPC_FAILED;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(con->_response.body()) << jsonstr;
			return true;
		}
		//std::cout << "succeed to load userinfo uid is " << info.uid << std::endl;
		std::cout << "label1: gateserver success got ip&token and reply to client" << std::endl;
		root["error"] = 0;
		root["user"] = user;
		root["uid"] = info.uid;
		root["token"] = reply.token();
		root["host"] = reply.host();
		root["port"] = reply.port();
		std::string jsonstr = root.toStyledString();
		std::cout << jsonstr << std::endl;
		boost::beast::ostream(con->_response.body())<<jsonstr;
		//std::cout << "success return logindata" << std::endl;
		return true;
		});
 }

