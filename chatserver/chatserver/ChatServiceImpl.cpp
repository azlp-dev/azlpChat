#include "ChatServiceImpl.h"
#include "UserMgr.h"
#include "CSession.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"

ChatServiceImpl::ChatServiceImpl()
{}

ChatServiceImpl::~ChatServiceImpl()
{}

bool ChatServiceImpl::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
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

::grpc::Status ChatServiceImpl::NotifyAddFriend(::grpc::ServerContext* context, const::message::AddFriendReq* request, ::message::AddFriendRsp* reply)
{
	auto touid = request->touid();
	auto session = UserMgr::Get_instance()->getSession(touid);

	Defer defer([request, reply]() {
		reply->set_error(Errids::SUCCESS);
		reply->set_applyuid(request->applyuid());
		reply->set_touid(request->touid());
		});

	//用户不在内存中则直接返回
	if (session == nullptr) {
		return grpc::Status::OK;
	}

	//在内存中则直接发送通知对方
	Json::Value  rtvalue;
	rtvalue["error"] = Errids::SUCCESS;
	rtvalue["applyuid"] = request->applyuid();
	rtvalue["name"] = request->name();
	rtvalue["desc"] = request->desc();
	rtvalue["icon"] = request->icon();
	rtvalue["sex"] = request->sex();
	rtvalue["nick"] = request->nick();

	std::string return_str = rtvalue.toStyledString();

	session->send(return_str, ID_NOTIFY_ADD_FRIEND_REQ);

	return grpc::Status::OK;

}

::grpc::Status ChatServiceImpl::ReplyAddFriend(::grpc::ServerContext* context, const::message::ReplyFriendReq* request, ::message::ReplyFriendRsp* response)
{
    return ::grpc::Status();
}

::grpc::Status ChatServiceImpl::SendChatMsg(::grpc::ServerContext* context, const::message::SendChatMsgReq* request, ::message::SendChatMsgRsp* response)
{
    return ::grpc::Status();
}

::grpc::Status ChatServiceImpl::NotifyAuthFriend(::grpc::ServerContext* context, const::message::AuthFriendReq* request, ::message::AuthFriendRsp* response)
{
	auto fromuid = request->fromuid();
	auto touid = request->touid();
	auto session = UserMgr::Get_instance()->getSession(touid);

	response->set_error(Errids::SUCCESS);
	Defer defer1([response, fromuid, touid]() {
		response->set_fromuid(fromuid);
		response->set_touid(touid);
		});
	if (session == nullptr) {
		return grpc::Status::OK;
	}
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
		return grpc::Status::OK;
	}
	session->send(notify.toStyledString(), MSG_ID::ID_NOTIFY_AUTH_FRIEND_REQ);
	return grpc::Status::OK;
}

::grpc::Status ChatServiceImpl::TextChatMsg(::grpc::ServerContext* context, const::message::TextChatMsgReq* request, ::message::TextChatMsgRsp* response)
{
	auto touid = request->touid();
	auto session = UserMgr::Get_instance()->getSession(touid);
	response->set_error(Errids::SUCCESS);

	//用户不在内存中则直接返回
	if (session == nullptr) {
		return grpc::Status::OK;
	}

	//在内存中则直接发送通知对方
	Json::Value  rtvalue;
	rtvalue["error"] = Errids::SUCCESS;
	rtvalue["fromuid"] = request->fromuid();
	rtvalue["touid"] = request->touid();

	//将聊天数据组织为数组
	Json::Value text_array;
	for (auto& msg : request->textmsg()) {
		Json::Value element;
		element["content"] = msg.msgcontent();
		element["msgid"] = msg.msgid();
		text_array.append(element);
	}
	rtvalue["text_array"] = text_array;

	std::string return_str = rtvalue.toStyledString();

	session->send(return_str, ID_NOTIFY_TEXT_CHAT_MSG_REQ);
	return grpc::Status::OK;
}
