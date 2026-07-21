#pragma once
#include "const.h"
#include <queue>
#include <memory>

class RedisConPool {
	friend class RedisMgr;
public:
	RedisConPool(int poolsize, std::string host, int port, std::string pwd);
	~RedisConPool();
	redisContext* getCon();
	redisContext* getNonblockCon();
	void retCon(redisContext* toret);



private:
	void clearPool();
	void close();
	bool reConnect();
	void checkThread();
	void checkThreadPro();

	size_t _pool_size;
	int _time_counter;
	std::atomic<bool> _b_stop;
	std::atomic<int> _failure_count;
	std::mutex _mtx;
	std::condition_variable _cond;
	std::string _host;
	const int _port;
	std::string _pwd;
	std::queue<redisContext*> _queue;
	std::thread _chaeck_thread;
};

class RedisMgr :public Singleton<RedisMgr> 
{
	friend class Singleton<RedisMgr>;
	
public:
	bool Connect(const std::string host,int port);
	bool Close();
	bool Auth(const std::string psw);
	bool Set(const std::string& key,const std::string& value);
	bool Get(const std::string& key,std::string& value);
	bool LPush(const std::string& key,const std::string& value);
	bool RPush(const std::string& key,const std::string& value);
	bool LPop(const std::string& key, std::string& value);
	bool RPop(const std::string& key, std::string& value);
	bool HSet(const std::string& key,const std::string& hkey,const std::string value);
	bool HSet(const char* key, const char* hkey, const char* value, size_t value_length);
	std::string HGet(const std::string& key, const std::string& hkey);
	bool Del(const std::string& key);
	bool ExistsKey(const std::string& key);
	bool Hdel(const std::string& key, const std::string hkey);
	~RedisMgr();
	
private:
	RedisMgr();

	//redisContext* _connect;
	//redisReply* _reply;
	std::unique_ptr<RedisConPool> _conpool;

};

