#include "RedisMgr.h"
#include "ConfigMgr.h"

//bool RedisMgr::Connect(const std::string host, int port)
//{
//    con = redisConnect(host.c_str(), port);
//
//    if (_connect == NULL) {
//        std::cout << "redis connect failed" << std::endl;
//        //redisFree(_connect);
//        return false;
//    }
//    if (_connect->err != 0) {
//        std::cout << "redis connect failed" << std::endl;
//        redisFree(_connect);
//        return false;
//    }
//    std::cout << "redis connect success" << std::endl;
//    return true;
//}

bool RedisMgr::Close()
{
    //redisFree(con);
    //return true;
    _conpool->close();
    _conpool->clearPool();
    return true;
}

//bool RedisMgr::Auth(const std::string psw)
//{
//    auto* con = _conpool->getCon();
//    auto* reply = (redisReply*)redisCommand(con, "AUTH %s", psw.c_str());
//
//    if (reply == NULL) {
//        std::cout << "Auth failed " << std::endl;
//        freeReplyObject(reply);
//        return false;
//    }
//    if (reply->type == REDIS_REPLY_ERROR) {
//        std::cout << "Auth failed " << std::endl;
//        freeReplyObject(reply);
//        return false;
//    }
//
//    std::cout << "Auth seccuss " << std::endl;
//    freeReplyObject(reply);
//    return true;
//}

bool RedisMgr::Set(const std::string& key,const std::string& value)
{
    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return false;
    }

    auto* reply = (redisReply*)redisCommand(con, "SET %s %s", key.c_str(), value.c_str());

    if (reply == NULL) {
        std::cout << "redis command [SET" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        //redisFree(con);
        return false;
    }
    if (!(reply->type == REDIS_REPLY_STATUS && 
            ((strcmp(reply->str,"ok") == 0) || (strcmp(reply->str,"OK") == 0 ))
        ))
    {

        std::cout << "redis command [SET" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        //redisFree(con);
        return false;
    }

    std::cout << "redis command execute success" << std::endl;
    freeReplyObject(reply);
    _conpool->retCon(con);
     
    //redisFree(con);
    return true;
}

bool RedisMgr::Get(const std::string& key, std::string& value)
{
    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return false;
    }

    auto* reply = (redisReply*)redisCommand(con, "GET %s", key.c_str());
    if (reply == NULL) {
        std::cout << "redis command [GET" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        //redisFree(con);
        return false;
    }
    if ((reply->type == REDIS_REPLY_NIL) || (reply->type == REDIS_REPLY_ERROR)) {
        std::cout << "redis command [GET" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        //redisFree(con);
        return false;
    }

    value = reply->str;
    std::cout << "redis command execute seccuss" << std::endl;
    freeReplyObject(reply);
    _conpool->retCon(con);
    //redisFree(con);
    return true;
}

bool RedisMgr::LPush(const std::string& key,const std::string& value)
{
    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return false;
    }

    auto* reply = (redisReply*)redisCommand(con, "LPUSH %s %s", key.c_str(), value.c_str());
    if (reply == NULL) {
        std::cout << "redis command [LPush" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        return false;
    }
    if ((reply->type != REDIS_REPLY_INTEGER) || (reply->integer <= 0) || (reply->type == REDIS_REPLY_ERROR)) {
        std::cout << "redis command [GET" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        return false;
    }
    std::cout << "redis command execute seccuss" << std::endl;
    freeReplyObject(reply);
    _conpool->retCon(con);

    return true;
}

bool RedisMgr::RPush(const std::string& key,const std::string& value)
{
    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return false;
    }

    auto* reply = (redisReply*)redisCommand(con, "RPUSH %s %s", key.c_str(), value.c_str());
    if (reply == NULL) {
        std::cout << "redis command [LPush" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);

        return false;
    }
    if ((reply->type != REDIS_REPLY_INTEGER) || (reply->integer <= 0) || (reply->type == REDIS_REPLY_ERROR)) {
        std::cout << "redis command [GET" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);

        return false;
    }
    std::cout << "redis command execute seccuss" << std::endl;
    freeReplyObject(reply);
    _conpool->retCon(con);

    return true;
}

bool RedisMgr::LPop(const std::string& key, std::string& value)
{
    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return false;
    }

    auto* reply = (redisReply*)redisCommand(con, "LPOP %s", key.c_str());
    if (reply == NULL) {
        std::cout << "redis command [LPop" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);

        return false;
    }
    if ((reply->type == REDIS_REPLY_NIL) || (reply->type == REDIS_REPLY_ERROR) || (reply->type != REDIS_REPLY_STRING)) {
        std::cout << "redis command [LPop" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);

        return false;
    }
    value = reply->str;
    std::cout << "redis command execute seccuss" << std::endl;
    freeReplyObject(reply);
    _conpool->retCon(con);

    return true;
}

bool RedisMgr::RPop(const std::string& key, std::string& value)
{
    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return false;
    }

    auto* reply = (redisReply*)redisCommand(con, "RPOP %s", key.c_str());
    if (reply == NULL) {
        std::cout << "redis command [LPop" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        return false;
    }
    if ((reply->type == REDIS_REPLY_NIL) || (reply->type == REDIS_REPLY_ERROR) || (reply->type != REDIS_REPLY_STRING)) {
        std::cout << "redis command [LPop" << key << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        return false;
    }
    value = reply->str;
    std::cout << "redis command execute seccuss" << std::endl;
    freeReplyObject(reply);
    _conpool->retCon(con);
    return true;
}

bool RedisMgr::HSet(const std::string& key,const std::string& hkey,const std::string value)
{
    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return false;
    }

    auto* reply = (redisReply*)redisCommand(con, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());

    if (reply == NULL) {
        std::cout << "redis command [Hset" << key << " " << hkey << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "redis command [Hset" << key << " " << hkey << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        return false;
    }
    // 返回1表示新增字段 返回0表示覆盖
    std::cout << "redis command execute seccuss" << std::endl;
    freeReplyObject(reply);
    _conpool->retCon(con);
    return true;
}

bool RedisMgr::HSet(const char* key, const char* hkey, const char* value, size_t value_length)
{
    const char* argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = strlen(argv[0]);

    argv[1] = key;
    argvlen[1] = strlen(key);

    argv[2] = hkey;
    argvlen[2] = strlen(hkey);

    argv[3] = value;
    argvlen[3] = value_length;
    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return false;
    }

    auto* reply = (redisReply*)redisCommandArgv(con, 4, argv, argvlen);

    if (reply == NULL) {
        std::cout << "redis command [Hset" << key << " " << hkey << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "redis command [Hset" << key << " " << hkey << " ]execute failed" << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        return false;
    }
    // 返回1表示新增字段 返回0表示覆盖
    std::cout << "redis command execute seccuss" << std::endl;
    freeReplyObject(reply);
    _conpool->retCon(con);
    return true;
}

std::string RedisMgr::HGet(const std::string& key, const std::string& hkey)
{
    const char* argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();

    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return "";
    }

    auto* reply = (redisReply*)redisCommandArgv(con, 3, argv, argvlen);
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(reply);
        _conpool->retCon(con);
        std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
        return "";
    }
    std::string value = reply->str;
    freeReplyObject(reply);
    _conpool->retCon(con);
    std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
    return value;
}

bool RedisMgr::Del(const std::string& key)
{
    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return false;
    }

    auto* reply = (redisReply*)redisCommand(con, "DEL %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        return false;
    }
    std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
    freeReplyObject(reply);
    _conpool->retCon(con);
    return true;
}

bool RedisMgr::ExistsKey(const std::string& key)
{
    auto* con = _conpool->getCon();
    if (con == nullptr) {
        return false;
    }

    auto* reply = (redisReply*)redisCommand(con, "exists %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
        std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
        freeReplyObject(reply);
        _conpool->retCon(con);
        return false;
    }
    std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
    freeReplyObject(reply);
    _conpool->retCon(con);
    return true;
}

RedisMgr::~RedisMgr()
{
}

RedisMgr::RedisMgr()
{
    auto conf_mgr = ConfigMgr::Get_instance();
    auto host = (*conf_mgr)["Redis"]["Host"];
    auto port = (*conf_mgr)["Redis"]["Port"];
    auto pwd = (*conf_mgr)["Redis"]["Pass"];
    _conpool.reset(new RedisConPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));
}

/*
    size_t _pool_size;
    int _time_counter;
    std::atomic<bool> _b_stop;
    std::atomic<int> _failure_count;
    std::mutex _mtx;
    std::condition_variable _cond;
    const char* _host;
    const int _port;
    const char* _pwd;
    std::queue<redisContext*> _queue;
    std::thread _chaeck_thread;
*/

RedisConPool::RedisConPool(int poolsize, std::string host, int port, std::string pwd)
    :_pool_size(poolsize),_time_counter(0),_b_stop(false),_failure_count(0),_host(host),_port(port),_pwd(pwd)
{
    for (int i = 0;i < _pool_size;i++) {
        auto context = redisConnect(_host.c_str(), _port);
        if (context == nullptr || context->err != 0) {
            std::cout << "rediscontext create failed in connectionpool create " << std::endl;
            redisFree(context);
            continue;
        }
        auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd.c_str());
        if (reply == NULL || reply->type == REDIS_REPLY_ERROR) {
            std::cout << "Auth failed,in connectionpool create " << std::endl;
            freeReplyObject(reply);    //释放redisReply占用的内存
            redisFree(context);
            continue;
        }
        std::cout << "redisConnection " << i << " create success ,in connectionpool create" << std::endl;
        freeReplyObject(reply);
        _queue.push(context);
    }
    
    _chaeck_thread = std::thread(
        [this]() {
            while (!_b_stop) {
                _time_counter++;
                if (_time_counter >= 60) {
                    checkThread();
                    _time_counter = 0;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    );
}

RedisConPool::~RedisConPool()
{
    if (_chaeck_thread.joinable()) {
        _chaeck_thread.join();
    }
}

redisContext* RedisConPool::getCon()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _cond.wait(lock,[this]() {
        if (_b_stop) {
            return true;
        }
        return !_queue.empty();
        });
    if (_b_stop) {
        return nullptr;
    }
    auto* con = _queue.front();
    _queue.pop();
    return con;
}

redisContext* RedisConPool::getNonblockCon()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_b_stop) {
        return nullptr;
    }
    if (_queue.empty()) {
        return nullptr;
    }
    auto* con = _queue.front();
    _queue.pop();
    return con;
}

void RedisConPool::retCon(redisContext* toret)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_b_stop) {
        redisFree(toret);//防止内存泄露
        return;
    }
    _queue.push(toret);
    _cond.notify_one();
}

void RedisConPool::clearPool()
{
    std::lock_guard<std::mutex> lock(_mtx);
    while (!_queue.empty()) {
        auto* con = _queue.front();
        redisFree(con);
        _queue.pop();
    }
}

void RedisConPool::close()
{
    //std::lock_guard<std::mutex> lock(_mtx);   会引发死锁
    _b_stop = true;
    _cond.notify_all();
    _chaeck_thread.join();
}

bool RedisConPool::reConnect()
{
    auto* context = redisConnect(_host.c_str(), _port);
    if (context == nullptr || context->err != 0) {
        std::cout << "redisContext create failed,in reconnect process" << std::endl;
        redisFree(context);
        return false;
    }
    auto* reply = (redisReply*)redisCommand(context, "AUTH %s", _pwd.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_ERROR) {
        std::cout << "redisConnection create failed in reconnect process" << std::endl;
        freeReplyObject(reply);
        redisFree(context);
        return false;
    }

    freeReplyObject(reply);
    std::cout << "Auth success,reconnect success " << std::endl;
    retCon(context);
    return true;
}

void RedisConPool::checkThread()
{
    size_t current_poolsize;
    {
        std::lock_guard<std::mutex> lock(_mtx);
        current_poolsize = _queue.size();
    }
    for (int i = 0;i < current_poolsize;i++) {
        auto* con = getNonblockCon();
        if (con == nullptr) {
            break;
        }
        auto* reply = (redisReply*)redisCommand(con, "PING");
        try {
            if (con->err != 0) {
                std::cout << "found wrong connection "<<con->err <<" trying to reconnect " << std::endl;
                freeReplyObject(reply);
                redisFree(con);
                _failure_count++;
                continue;
            }
            if (reply == nullptr || reply->type == REDIS_REPLY_ERROR) {
                std::cout << "found useless connection trying to reconnect" << std::endl;
                freeReplyObject(reply);
                redisFree(con);
                _failure_count++;
                continue;
            }   
            //检查通过，将连接返回队列
            freeReplyObject(reply);
            retCon(con);
        }
        catch (const std::exception& e) {
            //出现异常时直接释放出连接并尝试重连
            freeReplyObject(reply);
            redisFree(con);
            _failure_count++;
        }

    }
    while (_failure_count > 0) {
        auto res = reConnect();
        if (res) {
            _failure_count--;
        }
        else {
            //直接退出防止出现阻塞
            break;
        }
    }
}

void RedisConPool::checkThreadPro()
{

}
