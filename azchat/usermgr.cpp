#include "usermgr.h"
#include <QJsonArray>

void UserMgr::AppendApplyList(QJsonArray array)
{
    //qDebug()<<"*debug2"<<Qt::endl;
    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue &value : array) {
        //qDebug()<<"*debug3"<<Qt::endl;
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto status = value["status"].toInt();
        auto info = std::make_shared<ApplyInfo>(uid, name,
                                                desc, icon, nick, sex, status);
        _apply_list.push_back(info);
    }
}

void UserMgr::AppendFriendList(QJsonArray arry)
{
    for (const QJsonValue &value : arry) {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto back = value["back"].toString();
        auto info = std::make_shared<FriendInfo>(uid, name,
                                                nick, icon, sex, desc, back,"");
        _friend_map.insert(uid,info);
        _friend_list.push_back(info);
    }
}

void UserMgr::AddFriend(std::shared_ptr<AuthInfo> info)
{
    auto friendInfo = std::make_shared<FriendInfo>(info);
    _friend_map[info->_uid] = friendInfo;
}

void UserMgr::AddFriend(std::shared_ptr<AuthRsp> info)
{
    auto friendInfo = std::make_shared<FriendInfo>(info);
    _friend_map[info->_uid] = friendInfo;
}

std::vector<std::shared_ptr<FriendInfo>> UserMgr::GetChatListPerPage()
{
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    int begin = _chat_loaded;
    int end = begin + PER_PAGE_NUMBER;
    if(begin>=_friend_list.size()){
        return friend_list;
    }
    if(end>_friend_list.size()){
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin()+begin,_friend_list.end());
        return friend_list;
    }
    friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin()+begin,_friend_list.begin()+end);
    return friend_list;
}

bool UserMgr::isLoadFin()
{
    if(_chat_loaded>=_friend_list.size()){
        return true;
    }
    return false;
}

void UserMgr::UpdateChatLoadedCount()
{
    int begin = _chat_loaded;
    int end = begin + PER_PAGE_NUMBER;
    if(begin>= _friend_list.size()){
        return;
    }
    if(end>=_friend_list.size()){
        _chat_loaded = _friend_list.size();
    }
    _chat_loaded = end;
}

std::vector<std::shared_ptr<FriendInfo> > UserMgr::GetConListPerPage()
{
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    int begin = _con_loaded;
    int end = begin + PER_PAGE_NUMBER;
    if(begin>=_friend_list.size()){
        return friend_list;
    }
    if(end>_friend_list.size()){
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin()+begin,_friend_list.end());
        return friend_list;
    }
    friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin()+begin,_friend_list.begin()+end);
    return friend_list;
}

bool UserMgr::isLoadConFin()
{
    if(_con_loaded>=_friend_list.size()){
        return true;
    }
    return false;
}

void UserMgr::UpdateConLoadedCount()
{
    int begin = _con_loaded;
    int end = begin + PER_PAGE_NUMBER;
    if(begin>= _friend_list.size()){
        return;
    }
    if(end>=_friend_list.size()){
        _con_loaded = _friend_list.size();
    }
    _con_loaded = end;
}

void UserMgr::AppendFriendChatMsg(int friendid, std::vector<std::shared_ptr<TextChatData>> msgs)
{
    auto finditer = _friend_map.find(friendid);
    if(finditer==_friend_map.end()){
        return;
    }
    finditer.value()->appendmsg(msgs);

}

UserMgr::UserMgr()
    :_user_info(nullptr),_chat_loaded(0),_con_loaded(0)
{}
