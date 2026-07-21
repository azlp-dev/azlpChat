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
    Redis_VarifyCode_NotFound = 4,
    Redis_VarifyCode_Incorrect = 5,
    Redis_User_UserExist = 6,
    Mysql_User_UserExist = 7,
    Mysql_EmailName_NotMatcth = 8,
    Mysql_PassUp_Failed = 9,
    Mysql_Login_PassWrong = 10,
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

struct UserInfo {
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
};

#define USERTOKENPREFIX  "utoken_"