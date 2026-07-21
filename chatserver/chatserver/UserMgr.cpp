#include "UserMgr.h"

UserMgr::~UserMgr()
{
	std::lock_guard<std::mutex> lock(_mtx);
	_map.clear();
}

void UserMgr::setSession(int uid,std::shared_ptr<CSession> session)
{
	std::lock_guard<std::mutex> lock(_mtx);
	if (session == nullptr) {
		return;
	}
	_map[uid] = session;
}

std::shared_ptr<CSession> UserMgr::getSession(int uid)
{
	std::lock_guard<std::mutex> lock(_mtx);
	if (_map.find(uid) != _map.end()) {
		return _map[uid];
	}
	return nullptr;
}

void UserMgr::rmvSession(int uid)
{
	{
		std::lock_guard<std::mutex> lock(_mtx);
		_map.erase(uid);
	}
}

UserMgr::UserMgr()
{
	std::lock_guard<std::mutex> lock(_mtx);
}
