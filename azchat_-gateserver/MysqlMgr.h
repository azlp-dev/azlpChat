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

private:
	MysqlMgr() {};

	MysqlDAO _dao;
};

