#ifndef USERINFO_H
#define USERINFO_H
#include <QString>
#include <QJsonArray>
#include <QJsonValueRef>
#include <QJsonObject>
#include "global.h"
struct TextChatData{
    TextChatData(QString msgid,QString msgcontent,int fromuid,int touid)
        :_msg_id(msgid),_msg_content(msgcontent),_fromuid(fromuid),_touid(touid)
    {

    }

    QString _msg_id;
    QString _msg_content;
    int _fromuid;
    int _touid;
};

struct TextChatMsg{
    TextChatMsg(int fromuid,int touid,QJsonArray array)
        :_from_uid(fromuid),_to_uid(touid)
    {
        for(auto msg_data:array){
            auto jsobj = msg_data.toObject();
            auto content = jsobj["content"].toString();
            auto msgid = jsobj["msgid"].toString();
            _vec.push_back(std::make_shared<TextChatData>(msgid,content,fromuid,touid));
        }
    }

    int _from_uid;
    int _to_uid;
    std::vector<std::shared_ptr<TextChatData>> _vec;
};

class AddFriendApply {
public:
    AddFriendApply(int from_uid, QString name, QString desc,
                   QString icon, QString nick, int sex)
        :_from_uid(from_uid),_name(name),_desc(desc),_icon(icon),_nick(nick),_sex(sex)
    {

    }
    int _from_uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int     _sex;
};
struct AuthInfo {
    AuthInfo(int uid, QString name,
             QString nick, QString icon, int sex):
        _uid(uid), _name(name), _nick(nick), _icon(icon),
        _sex(sex), _thread_id(0){}

    //void SetChatDatas(std::vector<std::shared_ptr<TextChatData>> _chat_datas);
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    int _thread_id;
    std::vector<std::shared_ptr<TextChatData>> _chat_datas;
};

struct ApplyInfo {
    ApplyInfo(int uid, QString name, QString desc,
              QString icon, QString nick, int sex, int status)
        :_uid(uid),_name(name),_desc(desc),
        _icon(icon),_nick(nick),_sex(sex),_status(status){}

    ApplyInfo(std::shared_ptr<AddFriendApply> addinfo)
        :_uid(addinfo->_from_uid),_name(addinfo->_name),
        _desc(addinfo->_desc),_icon(addinfo->_icon),
        _nick(addinfo->_nick),_sex(addinfo->_sex),
        _status(0)
    {}
    void SetIcon(QString head){
        _icon = head;
    }
    int _uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int _sex;
    int _status;
};

struct AuthRsp {
    AuthRsp(int peer_uid, QString peer_name,
            QString peer_nick, QString peer_icon, int peer_sex)
        :_uid(peer_uid),_name(peer_name),_nick(peer_nick),
        _icon(peer_icon),_sex(peer_sex),_thread_id(0)
    {

    }


   // void SetChatDatas(std::vector<std::shared_ptr<TextChatData>> _chat_datas);
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    int _thread_id;
    std::vector<std::shared_ptr<TextChatData>> _chat_datas;
};

class SearchInfo{
public:
    int uid;
    int sex;
    QString name;
    QString icon;
    QString nick;
    QString desc;

    SearchInfo(int uid,int sex,QString name,QString nick,QString desc,QString icon);
};

struct FriendInfo{
    FriendInfo(int uid,QString name,QString nick,QString icon,int sex,QString desc,QString back,QString last_msg)
        :_uid(uid),_name(name),_nick(nick),_icon(icon),_sex(sex),_desc(desc),_back(back),_last_msg(last_msg)
    {
        _icon = heads[2];
    }
    FriendInfo(std::shared_ptr<AuthInfo> auth)
        :_uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),_icon(auth->_icon),_sex(auth->_sex)
        ,_desc(""),_back(""),_last_msg("")
    {
        _icon = heads[2];
    }
    FriendInfo(std::shared_ptr<AuthRsp> authrsp)
        :_uid(authrsp->_uid),_name(authrsp->_name),_nick(authrsp->_nick),_icon(authrsp->_icon),_sex(authrsp->_sex)
        ,_desc(""),_back(""),_last_msg("")
    {
        _icon = heads[2];
    }

    void appendmsg(std::vector<std::shared_ptr<TextChatData>>);

public:
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _desc;
    QString _back;
    QString _last_msg;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};

struct UserInfo {
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = "", QString desc=""):
        _uid(uid),_name(name),_nick(nick),_icon(icon),_sex(sex),_desc(desc),_last_msg(""){}

    UserInfo(std::shared_ptr<AuthInfo> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _icon(auth->_icon),_sex(auth->_sex),_desc(""),_last_msg(""){}

    // UserInfo(std::shared_ptr<SearchInfo> si)
    //     :_uid(si->uid),_name(si->name),_nick(si->nick),_icon(si->icon),_sex(si->sex),_desc(si->desc),_last_msg("")
    // {

    // }
    UserInfo(int uid, QString name, QString icon):
        _uid(uid), _name(name), _icon(icon),_nick(_name),
        _sex(0),_desc(""),_last_msg(""){

    }

    UserInfo(std::shared_ptr<AuthRsp> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _icon(auth->_icon),_sex(auth->_sex),_desc(""),_last_msg(""){}

    UserInfo(std::shared_ptr<SearchInfo> search_info):
        _uid(search_info->uid),_name(search_info->name),_nick(search_info->nick),
        _icon(search_info->icon),_sex(search_info->sex), _desc(search_info->desc),_last_msg(""){

    }

    UserInfo(std::shared_ptr<FriendInfo> friend_info)
        :_uid(friend_info->_uid),_name(friend_info->_name),_nick(friend_info->_nick),
        _icon(friend_info->_icon),_sex(friend_info->_sex),_desc(friend_info->_desc),_last_msg("")
    {
        _chat_msgs = friend_info->_chat_msgs;
    }
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _desc;
    QString _last_msg;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};


#endif // USERINFO_H
