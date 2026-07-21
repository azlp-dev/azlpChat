#pragma once
#include "const.h"
#include "MysqlDAO.h"

class MysqlMgr: public Singleton<MysqlMgr> 
{
	friend class Singleton<MysqlMgr>;

public:
	~MysqlMgr() {};

	bool RegUserF(const std::string& name,const std::string& email,const std::string& pass) {
		return _dao.RegUser(name, email, pass);
	}
	bool checkEmail(const std::string& name,const std::string& email) {
		return _dao.CheckEmail(name, email);
	}
	bool updatePass(const std::string& name, const std::string& pass) {
		return _dao.UpdatePass(name, pass);
	}
	bool checkPass(const std::string& name, const std::string& pass,UserInfo& info) {
		return _dao.CheckPass(name, pass,info);
	}
	
	std::shared_ptr<UserInfo> getUser(const int uid) {
		return _dao.GetUser(uid);
	}
	std::shared_ptr<UserInfo> getUser(const std::string& name) {
		return _dao.GetUser(name);
	}

	bool AddFriendApply(const int& from, const int& to) {
		return _dao.AddFriendApply(from, to);
	}

	bool GetApplyList(int uid, std::vector<std::shared_ptr<ApplyInfo>>& list, int begin, int limit) {
		return _dao.GetApplyList(uid,list, begin, limit);
	}

	bool AuthFriendApply(int fromuid, int touid) {
		return _dao.AuthFriendApply(fromuid, touid);
	}

	bool AddFriend(int fromuid, int touid, std::string back) {
		return _dao.AddFriend(fromuid, touid, back);
	}
	std::shared_ptr<UserInfo> getInfo(int uid);

	bool GetFriendList(int uid, std::vector<std::shared_ptr<UserInfo>>& list);
private:
	MysqlMgr() {};

	MysqlDAO _dao;
};

