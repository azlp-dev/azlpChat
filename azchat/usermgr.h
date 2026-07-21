#ifndef USERMGR_H
#define USERMGR_H
#include "singleton.h"
#include <QObject>
#include "userinfo.h"

const int PER_PAGE_NUMBER = 13;
class UserMgr :public QObject,public Singleton<UserMgr>
{
    Q_OBJECT
    friend class Singleton<UserMgr>;
public:
    ~UserMgr(){

    }

    void setuid(int uid){
        _uid = uid;
    }

    void setuname(QString name){
        _uname = name;
    }

    void setutoken(QString token){
        _utoken = token;

    }

    QString getname(){
        return _user_info->_name;
    }

    std::shared_ptr<UserInfo> GetUserInfo(){
        return _user_info;
    }

    std::shared_ptr<UserInfo> GetFriendByUid(int uid){
        auto temp = _friend_map[uid];
        return std::make_shared<UserInfo>(temp);
    }
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList(){
        return _apply_list;
    }

    std::vector<std::shared_ptr<FriendInfo>> GetFriendList(){
        return _friend_list;
    }
    bool AlreadyApply(int uid){
        for(auto apply:_apply_list){
            if(apply->_uid == uid)
                return true;
        }
        return false;
    }

    void AddApplyList(std::shared_ptr<ApplyInfo> apply){
        _apply_list.push_back(apply);
    }


    void AppendApplyList(QJsonArray array);

    void AppendFriendList(QJsonArray arry);

    int getuid(){
        return _user_info->_uid;
    }


    void SetUserInfo(std::shared_ptr<UserInfo> user_info) {
        _user_info = user_info;
    }

    bool checkUserByUid(int uid){
        auto it = _friend_map.find(uid);
        if(it == _friend_map.end()){
            return false;
        }
        return true;
    }

    void AddFriend(std::shared_ptr<AuthInfo>);
    void AddFriend(std::shared_ptr<AuthRsp>);

    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
    bool isLoadFin();
    void UpdateChatLoadedCount();

    std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();
    bool isLoadConFin();
    void UpdateConLoadedCount();
    void AppendFriendChatMsg(int,std::vector<std::shared_ptr<TextChatData>>);

private:
    UserMgr();
    int _uid{0};
    QString _uname{};
    QString _utoken{};
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    std::shared_ptr<UserInfo> _user_info;
    QMap<int,std::shared_ptr<FriendInfo>> _friend_map;
    std::vector<std::shared_ptr<FriendInfo>> _friend_list;
    int _chat_loaded;
    int _con_loaded;
};

#endif // USERMGR_H
