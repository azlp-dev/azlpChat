#pragma once
#include "const.h"
#include <map>

class CSession;
class UserMgr: public Singleton<UserMgr>
{
	friend class Singleton<UserMgr>;
public:
	~UserMgr();
	void setSession(int uid,std::shared_ptr<CSession> session);
	std::shared_ptr<CSession> getSession(int uid);
	void rmvSession(int uid);
private:
	UserMgr();

	std::mutex _mtx;
	std::map<int, std::shared_ptr<CSession>> _map;
};

