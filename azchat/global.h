#ifndef GLOBAL_H
#define GLOBAL_H

#include <functional>
#include <memory>
#include <iostream>
#include <QNetworkReply>
#include <QJsonObject>
#include <QSettings>
#include <QDir>
#include <QPixmap>
class QWidget;
/******************************************************************************
 *
 * @brief      用于刷新窗口stylesheet的可调用对象
 *
 * @date       2026/04/13
 *****************************************************************************/
extern std::function<void(QWidget*)> polish;
extern QString gate_url_prefix;
extern std::function<QString(QString)> Enc_sha256;

struct ServerInfo{
    QString host;
    QString port;
    int uid;
    QString token;
};

enum Reqids{
    ID_GET_VARIFY_CODE = 1001,
    ID_REQUEST_REG_USER = 1002,
    ID_PassWord_Reset = 1003,
    ID_USER_LOGIN = 1004,
    ID_CHAT_LOGIN = 1005,
    ID_CHAT_LOGIN_RSP = 1006,
    ID_SEARCH_USER_REQ = 1007, //用户搜索请求
    ID_SEARCH_USER_RSP = 1008, //搜索用户回包
    ID_ADD_FRIEND_REQ = 1009,  //添加好友申请
    ID_ADD_FRIEND_RSP = 1010, //申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
    ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ  = 1017,  //文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP  = 1018,  //文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
};

enum Moudles{
    REGISTER_MOD = 0,
    RESET_MOD = 1,
    LOGIN_MOD = 2,
};

enum Errids{
    SUCCESS = 0,
    ERR_JSON = 1,
    ERR_NETWORK = 2,
};

enum ErrTips{
    NoErrTip = 0,
    UserNameErr = 1,
    EmailErr = 2,
    PassErr = 3,
    ConfirmErr = 4,
    CodeErr = 5,
};

enum ClickedLbState{
    Normal = 0,
    Selected = 1,
};

enum ChatUIMode{
    SearchMode, //搜索模式
    ChatMode, //聊天模式
    ContactMode, //联系模式
    SettingsMode, //设置模式
};

enum ListItemType{
    CHAT_USER_ITEM, //聊天用户
    CONTACT_USER_ITEM, //联系人用户
    SEARCH_USER_ITEM, //搜索到的用户
    ADD_USER_TIP_ITEM, //提示添加用户
    INVALID_ITEM,  //不可点击条目
    GROUP_TIP_ITEM, //分组提示条目
    LINE_ITEM,  //分割线
    APPLY_FRIEND_ITEM, //好友申请
};

enum class chat_role{
    self,
    other,
};


//测试资源

const std::vector<QString> msgs = {
    "hello world !",
    "nice to meet u",
    "New year，new life",
    "You have to love yourself",
    "My love is written in the wind ever since the whole world is you",
    "Life goes on"
};

const std::vector<QString> heads = {
    ":/res/head_1.jpg",
    ":/res/head_2.jpg",
    ":/res/head_3.jpg",
    ":/res/head_4.jpg",
    ":/res/head_5.jpg",
    ":/res/head_6.jpg"
};

const std::vector<QString> names = {
    "llfc",
    "zack",
    "golang",
    "cpp",
    "java",
    "nodejs",
    "python",
    "rust"
};

struct msgInfo{
    QString msgFlag;
    QString content;
    QPixmap pix;
};

enum class ChatMsgType {
    TEXT = 0,
    PIC = 1,
    FILE = 2
};

const int MIN_APPLY_LABEL_ED_LEN = 40;
const int  tip_offset = 5;
const QString add_prefix = "添加标签 ";
#define USERTOKENPREFIX  "utoken_"
#endif // GLOBAL_H
