#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/write.hpp>
#include <iostream>
#include <map>
#include <functional>
#include <unordered_map>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <thread>

#include <hiredis.h>
#include "single.h"
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>


enum Reqids {
    ID_GET_VARIFY_CODE = 1001,
    ID_REQUEST_REG_USER = 1002,
    ID_PassWord_Reset = 1003,
    ID_USER_LOGIN = 1004,
};

enum Moudles {
    REGISTER_MOD = 0,
    RESET_MOD = 1,
    LOGIN_MOD = 2,
};

enum Errids {
    SUCCESS = 0,
    ERR_JSON = 1,
    ERR_NETWORK = 2,
    RPC_FAILED = 3,
    Uid_Invalid = 11,
    Redis_VarifyCode_NotFound = 4,
    Redis_VarifyCode_Incorrect = 5,
    Redis_User_UserExist = 6,
    Mysql_User_UserExist = 7,
    Mysql_EmailName_NotMatcth = 8,
    Mysql_PassUp_Failed = 9,
    Mysql_Login_PassWrong = 10,
    TokenInvalid = 11,
};

enum MSG_ID {
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

    ID_TEXT_CHAT_MSG_REQ = 1017,  //文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP = 1018,  //文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
};
#define CODE_PREFIX "code_"

//raii 思想 用于在释放时执行指定操作
class Defer {
public:
    Defer(std::function<void()> func) :_func(func) {}
    ~Defer() {
        _func();
    }

    std::function<void()> _func;
};

#define SELF_SERVER "chatserver1"
#define CODE_PREFIX "code_"
#define USERTOKENPREFIX  "utoken_"
#define USERIPPREFIX  "uip_"

#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define LOCK_COUNT "lockcount"

//此处定义消息结构
#define MAX_MSG_ID 9999
#define MAX_LENGTH 1024*2
#define HEAD_ID_LEN 2
#define HEAD_DATA_LEN 2
#define HEAD_TOTAL_LEN 4
#define MAX_RECV_QUE 1000
#define MAX_SAND_QUE 1000

#define USERTOKENPREFIX  "utoken_"