#pragma once
#include "const.h"
#include <thread>
#include <queue>

class SqlConnection {
public:
	SqlConnection(sql::Connection* con, int64_t time) :_conn(con), _last_oper_time(time) {}

	std::unique_ptr<sql::Connection> _conn;
	int64_t _last_oper_time;
};


class MysqlPool
{
public:
	MysqlPool(const std::string url,const std::string user,const std::string pass,const std::string schema);
	void CheckThreadPro();
	void CheckThread();
	bool reconnect(int64_t timestamp);
	std::unique_ptr<SqlConnection> getConnection();
	bool retConnection(std::unique_ptr<SqlConnection> con);
	void Close() {
		_b_stop = true;
		_cond.notify_all();
	}
	~MysqlPool() {
		//Close();
		if (_check_thread.joinable()) {
			_check_thread.join();
		}
		std::lock_guard<std::mutex> lock(_mtx);
		while (!_conns.empty()) {
			_conns.pop();
		}
	}

private:
	std::string _url;
	//int64_t _port;
	std::string _user;
	std::string _pass;
	std::string _schema;
	int _poolsize;
	std::mutex _mtx;
	std::condition_variable _cond;
	std::queue<std::unique_ptr<SqlConnection>> _conns;
	std::thread _check_thread;
	std::atomic<bool> _b_stop;
	std::atomic<int> _fail_count;
};


class MysqlDAO {
public:
	MysqlDAO();
	~MysqlDAO();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	int CheckEmail(const std::string& name, const std::string& email);
	int UpdatePass(const std::string& name, const std::string& pass);
	int CheckPass(const std::string& name, const std::string& pass,UserInfo& info);

private:
	std::unique_ptr<MysqlPool> _pool;
};
