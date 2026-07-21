#include "MysqlMgr.h"

std::shared_ptr<UserInfo> MysqlMgr::getInfo(int uid)
{
    return _dao.GetUser(uid);
}

bool MysqlMgr::GetFriendList(int uid, std::vector<std::shared_ptr<UserInfo>>& list)
{
    return _dao.GetFriendList(uid, list);
}
