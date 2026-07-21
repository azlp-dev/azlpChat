#pragma once
#include "const.h"
#include <queue>
#include "data.h"
class CSession;
class Cserver;
class LogicNode;

class LogicSystem: public Singleton<LogicSystem>
{
	typedef std::function<void(std::shared_ptr<CSession>,const short& msgid,const std::string& msg)> FunCallBack;
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgtoQue(std::shared_ptr<LogicNode> node);
	void setServer(std::shared_ptr<Cserver> pserver);

private:
	LogicSystem();
	void RegistFunc();
	void DealMsg();
	void GetUserByUid(const int uid,Json::Value& retvalue);
	void GetUserByName(const std::string& name, Json::Value& retvalue);
	bool GetApplyList(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);
	void DealChatTextMsg(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
		
	void LoginHandler(std::shared_ptr<CSession>, const short& msgid, const std::string& msg);
	void UserSearchHandler(std::shared_ptr<CSession>, const short& msgid, const std::string& msg);
	void AddFriendApply(std::shared_ptr<CSession>, const short& msgid, const std::string& msg);
	void AuthFriendApply(std::shared_ptr<CSession>, const short& msgid, const std::string& msg);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	bool isPureDigit(const std::string&);
	bool GetFriendList(int uid, std::vector<std::shared_ptr<UserInfo>>&);
	std::thread _work_thread;
	std::shared_ptr<Cserver> _server;
	std::queue<std::shared_ptr<LogicNode>> _msg_que;
	std::mutex _mtx;
	std::condition_variable _cond;
	bool _b_stop{false};
	std::map<short, FunCallBack> _func_map;
};

