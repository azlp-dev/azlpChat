#include "LogicSystem.h"
#include "Cserver.h"
#include "CSession.h"
#include "Status_Client.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
#include "UserMgr.h"
#include "Grpc_Client.h"
#include <fstream>

LogicSystem::~LogicSystem()
{
	_b_stop = true;
	_cond.notify_one();
	if (_work_thread.joinable()) {
		_work_thread.join();
	}
}

void LogicSystem::PostMsgtoQue(std::shared_ptr<LogicNode> node)
{
	std::unique_lock<std::mutex> lock(_mtx);
	_msg_que.push(node);
	if (_msg_que.size() == 1) {
		lock.unlock();
		_cond.notify_one();
	}
}

void LogicSystem::setServer(std::shared_ptr<Cserver> pserver)
{
	_server = pserver;
}

LogicSystem::LogicSystem()
{
	RegistFunc();
	_work_thread = std::thread(&LogicSystem::DealMsg, this);
}

void LogicSystem::RegistFunc()
{
	_func_map[MSG_ID::ID_CHAT_LOGIN] = std::bind(&LogicSystem::LoginHandler, this, 
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	_func_map[MSG_ID::ID_SEARCH_USER_REQ] = std::bind(&LogicSystem::UserSearchHandler, this, 
		std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
	_func_map[MSG_ID::ID_ADD_FRIEND_REQ] = std::bind(&LogicSystem::AddFriendApply, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	_func_map[MSG_ID::ID_AUTH_FRIEND_REQ] = std::bind(&LogicSystem::AuthFriendApply, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	_func_map[MSG_ID::ID_TEXT_CHAT_MSG_REQ] = std::bind(&LogicSystem::DealChatTextMsg, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void LogicSystem::DealMsg()
{
	for (;;) {
		std::unique_lock<std::mutex> lock(_mtx);
		while(_msg_que.empty()||_b_stop){
			_cond.wait(lock);
		}
		if (_b_stop) {
			while (!_msg_que.empty()) {
				std::shared_ptr<LogicNode> front = _msg_que.front();
				auto callback = _func_map.find(front->_node->_msgid);
				_msg_que.pop();
				if (callback == _func_map.end()) {
					continue;
				}
				//std::cout << "tick2" << std::endl;
				callback->second(front->_session, front->_node->_msgid, std::string(front->_node->_data, front->_node->_cur));
			}
			break;
		}
		std::shared_ptr<LogicNode> front = _msg_que.front();
		auto callback = _func_map.find(front->_node->_msgid);
		_msg_que.pop();
		if (callback == _func_map.end()) {
			continue;
		}
		callback->second(front->_session, front->_node->_msgid, std::string(front->_node->_data, front->_node->_cur));
	}
}

void LogicSystem::GetUserByUid(const int uid,Json::Value& rtvalue)
{
	rtvalue["error"] = Errids::SUCCESS;
	std::string key = USER_BASE_INFO + std::to_string(uid);
	std::string info_str = "";
	bool b_redis = RedisMgr::Get_instance()->Get(key, info_str);
	if (b_redis) {
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		auto uid = root["uid"].asInt();
		auto name = root["name"].asString();
		auto pwd = root["pwd"].asString();
		auto email = root["email"].asString();
		auto nick = root["nick"].asString();
		auto desc = root["desc"].asString();
		auto sex = root["sex"].asInt();
		auto icon = root["icon"].asString();
		std::cout << "user  uid is  " << uid << " name  is "
			<< name << " pwd is " << pwd << " email is " << email << " icon is " << icon << std::endl;

		rtvalue["uid"] = uid;
		rtvalue["pwd"] = pwd;
		rtvalue["name"] = name;
		rtvalue["email"] = email;
		rtvalue["nick"] = nick;
		rtvalue["desc"] = desc;
		rtvalue["sex"] = sex;
		rtvalue["icon"] = icon;
		return;
	}

	std::shared_ptr<UserInfo> user_info = nullptr;
	user_info = MysqlMgr::Get_instance()->getUser(uid);
	if (user_info == nullptr) {
		rtvalue["error"] = Errids::Uid_Invalid;
		return;
	}

	//将数据库内容写入redis缓存
	Json::Value redis_root;
	redis_root["uid"] = user_info->uid;
	redis_root["pwd"] = user_info->pwd;
	redis_root["name"] = user_info->name;
	redis_root["email"] = user_info->email;
	redis_root["nick"] = user_info->nick;
	redis_root["desc"] = user_info->desc;
	redis_root["sex"] = user_info->sex;
	redis_root["icon"] = user_info->icon;

	RedisMgr::Get_instance()->Set(key, redis_root.toStyledString());

	//返回数据
	rtvalue["uid"] = user_info->uid;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	rtvalue["nick"] = user_info->nick;
	rtvalue["desc"] = user_info->desc;
	rtvalue["sex"] = user_info->sex;
	rtvalue["icon"] = user_info->icon;
}

void LogicSystem::GetUserByName(const std::string & name,Json::Value& rtvalue)
{
	rtvalue["error"] = Errids::SUCCESS;
	std::string key = USER_BASE_INFO + name;
	std::string info_str = "";
	bool b_redis = RedisMgr::Get_instance()->Get(key, info_str);
	if (b_redis) {
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		auto uid = root["uid"].asInt();
		auto name = root["name"].asString();
		auto pwd = root["pwd"].asString();
		auto email = root["email"].asString();
		auto nick = root["nick"].asString();
		auto desc = root["desc"].asString();
		auto sex = root["sex"].asInt();
		auto icon = root["icon"].asString();
		std::cout << "user  uid is  " << uid << " name  is "
			<< name << " pwd is " << pwd << " email is " << email << " icon is " << icon << std::endl;

		rtvalue["uid"] = uid;
		rtvalue["pwd"] = pwd;
		rtvalue["name"] = name;
		rtvalue["email"] = email;
		rtvalue["nick"] = nick;
		rtvalue["desc"] = desc;
		rtvalue["sex"] = sex;
		rtvalue["icon"] = icon;
		return;
	}

	std::shared_ptr<UserInfo> user_info = nullptr;
	user_info = MysqlMgr::Get_instance()->getUser(name);
	if (user_info == nullptr) {
		std::cout << "debug2" << std::endl;		
		rtvalue["error"] = Errids::Uid_Invalid;
		return;
	}

	//将数据库内容写入redis缓存
	Json::Value redis_root;
	redis_root["uid"] = user_info->uid;
	redis_root["pwd"] = user_info->pwd;
	redis_root["name"] = user_info->name;
	redis_root["email"] = user_info->email;
	redis_root["nick"] = user_info->nick;
	redis_root["desc"] = user_info->desc;
	redis_root["sex"] = user_info->sex;
	redis_root["icon"] = user_info->icon;

	RedisMgr::Get_instance()->Set(key, redis_root.toStyledString());

	//返回数据
	rtvalue["uid"] = user_info->uid;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	rtvalue["nick"] = user_info->nick;
	rtvalue["desc"] = user_info->desc;
	rtvalue["sex"] = user_info->sex;
	rtvalue["icon"] = user_info->icon;
}

bool LogicSystem::GetApplyList(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list)
{
	return MysqlMgr::Get_instance()->GetApplyList(to_uid, list, 0, 10);
}

void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, const short& msgid, const std::string& msg)
{
	Json::Reader reader;
	Json::Value root;
	Json::Value retvalue;
	reader.parse(msg,root);
	auto rsp = Status_Client::Get_instance()->Login(root["uid"].asInt(), root["token"].asString());

	Defer defer([this, &retvalue, session]() {
		auto rsp = retvalue.toStyledString();
		session->send(rsp, MSG_ID::ID_CHAT_LOGIN_RSP);
		});
	std::string uid_key = USERTOKENPREFIX + root["uid"].asString();
	std::string token_value = "";
	bool b_redis = RedisMgr::Get_instance()->Get(uid_key, token_value);
	if (!b_redis||(token_value!=root["token"].asString())) {
		retvalue["error"] = Errids::TokenInvalid;
		return;
	}

	retvalue["error"] = rsp.error();
	if (retvalue["error"] != Errids::SUCCESS) {
		return;
	}
	std::string base_key = USER_BASE_INFO + root["uid"].asString();
	std::shared_ptr<UserInfo> userinfo = std::make_shared<UserInfo>();
	bool success= GetBaseInfo(base_key, root["uid"].asInt(),userinfo);
	if (!success) {
		retvalue["error"] = Errids::Uid_Invalid;
		return;
	}
	retvalue["uid"] = userinfo->uid;
	retvalue["token"] = root["token"].asString();
	retvalue["pwd"] = userinfo->pwd;
	retvalue["name"] = userinfo->name;
	retvalue["email"] = userinfo->email;
	retvalue["nick"] = userinfo->nick;
	retvalue["desc"] = userinfo->desc;
	retvalue["sex"] = userinfo->sex;
	retvalue["icon"] = userinfo->icon;

	//获取好友列表
	std::vector<std::shared_ptr<UserInfo>> friend_list;
	bool b_friendlist = GetFriendList(userinfo->uid, friend_list);
	for (std::shared_ptr<UserInfo> friend_ele : friend_list) {
		Json::Value obj1;
		obj1["name"] = friend_ele->name;
		obj1["uid"] = friend_ele->uid;
		obj1["nick"] = friend_ele->nick;
		obj1["icon"] = friend_ele->icon;
		obj1["sex"] = friend_ele->sex;
		obj1["desc"] = friend_ele->desc;
		obj1["back"] = friend_ele->back;
		retvalue["friend_list"].append(obj1);
	}

	std::string count_s = RedisMgr::Get_instance()->HGet(LOGIN_COUNT, SELF_SERVER);
	int count = 0;
	if (!count_s.empty()) {
		count = std::stoi(count_s);
	}
	count++;
	count_s = std::to_string(count);
	auto b = RedisMgr::Get_instance()->HSet(LOGIN_COUNT, SELF_SERVER, count_s);
	session->setuseruid(userinfo->uid);
	std::string ipkey = USERIPPREFIX + std::to_string(userinfo->uid);
	RedisMgr::Get_instance()->Set(ipkey, SELF_SERVER);
	std::cout << ipkey << std::endl;
	UserMgr::Get_instance()->setSession(root["uid"].asInt(), session);

	std::vector<std::shared_ptr<ApplyInfo>> apply_list;
	bool b_list = GetApplyList(root["uid"].asInt(), apply_list);
	if (b_list) {
		for (std::shared_ptr<ApplyInfo> apply : apply_list) {
			std::cout << "debug4" << std::endl;
			Json::Value obj;
			obj["name"] = apply->_name;
			obj["uid"] = apply->_uid;
			obj["icon"] = apply->_icon;
			obj["nick"] = apply->_nick;
			obj["sex"] = apply->_sex;
			obj["desc"] = apply->_desc;
			obj["status"] = apply->_status;
			retvalue["apply_list"].append(obj);
		}
	}
}

void LogicSystem::UserSearchHandler(std::shared_ptr<CSession> session, const short& msgid, const std::string& msg)
{
	Json::Reader reader;
	Json::Value root;
	Json::Value retvalue;
	reader.parse(msg, root);

	Defer defer([this, &retvalue, session]() {
		auto rsp = retvalue.toStyledString();
		session->send(rsp, MSG_ID::ID_SEARCH_USER_RSP);
		//std::cout << "debug1" << std::endl;
		});
	auto search_str = root["uid"].asString();
	if (search_str.length() > 10)
	{
		retvalue["error"] = Errids::Uid_Invalid;
		return;
	}
	if (isPureDigit(search_str)) {
		int uid = std::stoi(search_str);
		GetUserByUid(uid,retvalue);
	}
	else {
		GetUserByName(search_str,retvalue);
	}
	return;
}

void LogicSystem::AddFriendApply(std::shared_ptr<CSession> session, const short& msgid, const std::string& msg)
{
	Json::Reader reader;
	Json::Value root;
	Json::Value retvalue;
	reader.parse(msg, root);

	Defer defer([this, &retvalue, session]() {
		auto rsp = retvalue.toStyledString();
		session->send(rsp, MSG_ID::ID_ADD_FRIEND_RSP);
		//std::cout << "debug1" << std::endl;
		});

	auto uid = root["uid"].asInt();
	auto applyname = root["name"].asString();
	auto bakname = root["backname"].asString();
	auto touid = root["touid"].asInt();
	std::cout << "user add uid is  " << uid << " applyname  is "
		<< applyname << " bakname is " << bakname << " touid is " << touid << std::endl;
	  
	//{
	//	std::ofstream ofs("test.txt", std::ios::binary);
	//	ofs << msg;
	//	ofs.close();
	//}
	// 
	bool b_sql = MysqlMgr::Get_instance()->AddFriendApply(uid, touid);
	std::string tostr = USERIPPREFIX + std::to_string(touid);
	std::cout << tostr << std::endl;
	std::string toip = "";
	bool b_redis = RedisMgr::Get_instance()->Get(tostr, toip);
	std::cout << toip << std::endl;
	if (!b_redis) {
		return;
	}
	if (toip == SELF_SERVER) {
		auto session = UserMgr::Get_instance()->getSession(touid);
		std::cout << "enter redis search" << std::endl;
		if (session) {
			//在内存中则直接发送通知对方
			Json::Value  notify;
			notify["error"] = Errids::SUCCESS;
			notify["applyuid"] = uid;
			notify["name"] = applyname;
			notify["desc"] = "";
			notify["icon"] = "";
			notify["nick"] = "";
			notify["sex"] = 0;
			std::string return_str = notify.toStyledString();
			session->send(return_str, MSG_ID::ID_NOTIFY_ADD_FRIEND_REQ);
			std::cout << "send notify" << std::endl;
			return;
		}
	}
	std::string base_key = USER_BASE_INFO + std::to_string(uid);
	std::string info_str = "";
	std::shared_ptr<UserInfo> userinfo = nullptr;
	bool b_info = GetBaseInfo(base_key, uid, userinfo);
	
	message::AddFriendReq req;
	req.set_applyuid(uid);
	req.set_touid(touid);
	req.set_name(applyname);
	req.set_desc("");
	if (b_info) {
		req.set_icon(userinfo->icon);
		req.set_nick(userinfo->nick); 
		req.set_sex(userinfo->sex);
	}
	Grpc_Client::Get_instance()->NotifyAddFriend(toip, req);

}

void LogicSystem::AuthFriendApply(std::shared_ptr<CSession> session, const short& msgid, const std::string& msg)
{
	Json::Reader reader;
	Json::Value root;
	Json::Value retvalue;
	reader.parse(msg, root);

	//Defer defer([this, &retvalue, session]() {
	//	auto rsp = retvalue.toStyledString();
	//	session->send(rsp, MSG_ID::ID_AUTH_FRIEND_RSP);
	//	//std::cout << "debug1" << std::endl;
	//	});

	auto fromuid = root["fromuid"].asInt();
	auto touid = root["touid"].asInt();
	auto back = root["back"].asString();

	retvalue["error"] = Errids::SUCCESS;
	auto userInfo = std::make_shared<UserInfo>();
	std::string base_key = USER_BASE_INFO + std::to_string(touid);
	bool b_getinfo = GetBaseInfo(base_key, fromuid, userInfo);
	
	Defer defer2([this, &retvalue, session]() {
		auto str = retvalue.toStyledString();
		session->send(str, MSG_ID::ID_AUTH_FRIEND_RSP);
		});
	
	if (!b_getinfo) {
		retvalue["error"] = Errids::Uid_Invalid;
		return;
	}
	retvalue["name"] = userInfo->name;
	retvalue["nick"] = userInfo->nick;
	retvalue["icon"] = userInfo->icon;
	retvalue["sex"] = userInfo->sex;
	retvalue["uid"] = touid;
	MysqlMgr::Get_instance()->AuthFriendApply(fromuid, touid);
	MysqlMgr::Get_instance()->AddFriend(fromuid,touid,back);
	
	std::string base_key1 = USERIPPREFIX + std::to_string(touid);
	std::string peerserver = "";
	bool b_ip = RedisMgr::Get_instance()->Get(base_key1, peerserver);
	if (!b_ip) {
		return;
	}
	if (peerserver == SELF_SERVER) {
		auto session = UserMgr::Get_instance()->getSession(touid);
		if (session) {
			Json::Value notify;
			notify["error"] = Errids::SUCCESS;
			notify["fromuid"] = fromuid;
			notify["touid"] = touid;
			std::string basekey = USER_BASE_INFO + std::to_string(fromuid);
			std::shared_ptr<UserInfo> userinfo_ = std::make_shared<UserInfo>();
			bool b_info = GetBaseInfo(basekey, fromuid, userinfo_);
			if (b_info) {
				notify["name"] = userinfo_->name;
				notify["nick"] = userinfo_->nick;
				notify["icon"] = userinfo_->icon;
				notify["sex"] = userinfo_->sex;
			}
			else {
				notify["error"] = Errids::Uid_Invalid;
				return;
			}
			session->send(notify.toStyledString(), MSG_ID::ID_NOTIFY_AUTH_FRIEND_REQ);
			//std::cout << "#debug2" << std::endl;
		}
		return;
	}
	else {
		message::AuthFriendReq req;
		req.set_fromuid(fromuid);
		req.set_touid(touid);
		Grpc_Client::Get_instance()->NotifyAuthFriend(peerserver, req);
		return;
	}
}

bool LogicSystem::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
	userinfo = std::make_shared<UserInfo>();
	std::string infostr = "";
	bool success = RedisMgr::Get_instance()->Get(base_key, infostr);
	if (success) {
		Json::Reader reader;
		Json::Value root;
		reader.parse(infostr, root);
		userinfo->uid = root["uid"].asInt();
		userinfo->name = root["name"].asString();
		userinfo->pwd = root["pwd"].asString();
		userinfo->email = root["email"].asString();
		userinfo->nick = root["nick"].asString();
		userinfo->desc = root["desc"].asString();
		userinfo->sex = root["sex"].asInt();
		userinfo->icon = root["icon"].asString();
		return true;
	}
	else {
		std::shared_ptr<UserInfo> sel_info = MysqlMgr::Get_instance()->getInfo(uid);
		if (sel_info == nullptr) {
			return false;
		}
		userinfo = sel_info;

		Json::Value redis_root;
		redis_root["uid"] = sel_info->uid;
		redis_root["uid"] = uid;
		redis_root["pwd"] = userinfo->pwd;
		redis_root["name"] = userinfo->name;
		redis_root["email"] = userinfo->email;
		redis_root["nick"] = userinfo->nick;
		redis_root["desc"] = userinfo->desc;
		redis_root["sex"] = userinfo->sex;
		redis_root["icon"] = userinfo->icon;

		RedisMgr::Get_instance()->Set(base_key, redis_root.toStyledString());
		return true;
	}
}

bool LogicSystem::isPureDigit(const std::string& str)
{
	for (char ch : str) {
		if (!std::isdigit(ch)) {
			return false;
		}
	}
	return true;
}

bool LogicSystem::GetFriendList(int uid, std::vector<std::shared_ptr<UserInfo>>& list)
{
	return MysqlMgr::Get_instance()->GetFriendList(uid, list);
}

void LogicSystem::DealChatTextMsg(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data) {
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);

	auto uid = root["fromuid"].asInt();
	auto touid = root["touid"].asInt();

	const Json::Value  arrays = root["text_array"];

	Json::Value  rtvalue;
	rtvalue["error"] = Errids::SUCCESS;
	rtvalue["text_array"] = arrays;
	rtvalue["fromuid"] = uid;
	rtvalue["touid"] = touid;

	Defer defer([this, &rtvalue, session]() {
		std::string return_str = rtvalue.toStyledString();
		session->send(return_str, ID_TEXT_CHAT_MSG_RSP);
		});


	//查询redis 查找touid对应的server ip
	auto to_str = std::to_string(touid);
	auto to_ip_key = USERIPPREFIX + to_str;
	std::string to_ip_value = "";
	bool b_ip = RedisMgr::Get_instance()->Get(to_ip_key, to_ip_value);
	if (!b_ip) {
		return;
	}

	//直接通知对方有认证通过消息
	if (to_ip_value == SELF_SERVER) {
		auto session = UserMgr::Get_instance()->getSession(touid);
		if (session) {
			//在内存中则直接发送通知对方
			std::string return_str = rtvalue.toStyledString();
			session->send(return_str, ID_NOTIFY_TEXT_CHAT_MSG_REQ);
		}

		return;
	}


	message::TextChatMsgReq text_msg_req;
	text_msg_req.set_fromuid(uid);
	text_msg_req.set_touid(touid);
	for (const auto& txt_obj : arrays) {
		auto content = txt_obj["content"].asString();
		auto msgid = txt_obj["msgid"].asString();
		std::cout << "content is " << content << std::endl;
		std::cout << "msgid is " << msgid << std::endl;
		auto* text_msg = text_msg_req.add_textmsg();
		text_msg->set_msgid(msgid);
		text_msg->set_msgcontent(content);
	}


	
	Grpc_Client::Get_instance()->NotifyTextChatMsg(to_ip_value, text_msg_req, rtvalue);
}

