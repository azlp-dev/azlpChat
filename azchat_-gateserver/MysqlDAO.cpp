#include "MysqlDAO.h"
#include "ConfigMgr.h"
//std::string _url;
////int64_t _port;
//std::string _user;
//std::string _pass;
//std::string _schema;
//int _poolsize;
//std::mutex _mtx;
//std::condition_variable _cond;
//std::queue<SqlConnection> _conns;
//std::thread _check_thread;
//std::atomic<bool> _b_stop;
//std::atomic<int> _fail_count;

MysqlPool::MysqlPool(const std::string url, const std::string user, const std::string pass, const std::string schema)
	:_url(url),_user(user),_pass(pass),_schema(schema),_poolsize(5),_b_stop(false),_fail_count(0)
{
	try {
		for (int i = 0; i < _poolsize; i++) {
			auto* sqldriver = sql::mysql::get_driver_instance();
			auto* con = sqldriver->connect(_url, _user, _pass);
			con->setSchema(_schema);
			auto now = std::chrono::system_clock::now().time_since_epoch();
			int64_t current_time = std::chrono::duration_cast<std::chrono::seconds>(now).count();
			_conns.push(std::make_unique<SqlConnection>(con, current_time));
		}
		_check_thread = std::thread([this]() {
			while (!_b_stop) {
				CheckThreadPro();
				std::this_thread::sleep_for(std::chrono::seconds(60));
			}
			});
		//_check_thread.detach();
	}
	catch (sql::SQLException& e) {
		std::cout << "sqlconnection pool init failed " << std::endl;
	}
}

void MysqlPool::CheckThreadPro()
{
	int check_num = 0;
	int finished = 0;
	{
		std::lock_guard<std::mutex> lock(_mtx);
		check_num = _conns.size();
	}
	auto now = std::chrono::system_clock::now().time_since_epoch();
	int64_t currenttime = std::chrono::duration_cast<std::chrono::seconds>(now).count();
	while (finished < check_num) {
		std::unique_ptr<SqlConnection> con;
		{
			std::lock_guard<std::mutex> lock(_mtx);
			if (_conns.empty()) {
				break;
			}
			con = std::move(_conns.front());
			_conns.pop();
		}
		bool healthy = true;
		if (currenttime - con->_last_oper_time > 5) {
			try {
				std::unique_ptr<sql::Statement> stmt(con->_conn->createStatement());
				stmt->executeQuery("SELECT 1");
				con->_last_oper_time = currenttime;
			}
			catch (sql::SQLException& e) {
				std::cout << "found bad connection ,start to reconnect" << std::endl;
				_fail_count++;
				healthy = false;
			}

		}
		if (healthy) {
			std::lock_guard<std::mutex> lock(_mtx);
			_conns.push(std::move(con));
			_cond.notify_one();
		}
		finished++;
	}
	while (_fail_count>0) {
		_fail_count--;
		auto b_res = reconnect(currenttime);
		if (b_res) {
			continue;
		}
		else {
			break;
		}
	}
}

void MysqlPool::CheckThread()
{
	
}

bool MysqlPool::reconnect(int64_t timestamp)
{
	try {
		auto sqldriver = sql::mysql::get_driver_instance();
		auto* con = sqldriver->connect(_url, _user, _pass);
		con->setSchema(_schema);
		auto newcon = std::make_unique<SqlConnection>(con,timestamp);
		{
			std::lock_guard<std::mutex> lock(_mtx);
			_conns.push(std::move(newcon));
		}
		std::cout << "reconnect seccuss" << std::endl;
		return true;
	}
	catch (sql::SQLException& e) {
		std::cout << "reconnect failed." << std::endl;
		return false;
	}
}

std::unique_ptr<SqlConnection> MysqlPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mtx);
	_cond.wait(lock, [this]() {
		if (_b_stop) {
			return true;
		}
		return !_conns.empty();
		});
	if (_b_stop) {
		return nullptr;
	}
	auto con = std::unique_ptr<SqlConnection>(std::move(_conns.front()));
	_conns.pop();
	return con;
}

bool MysqlPool::retConnection(std::unique_ptr<SqlConnection> con)
{
	std::lock_guard<std::mutex> lock(_mtx);
	if (_b_stop) {
		return true;
	}
	_conns.push(std::move(con));
	_cond.notify_one();
	return true;
}

MysqlDAO::MysqlDAO()
{
	auto cfg = ConfigMgr::Get_instance();
	std::string host = (*cfg)["MySql"]["Host"];
	std::string port = (*cfg)["MySql"]["Port"];
	std::string pass = (*cfg)["MySql"]["Pass"];
	std::string schema = (*cfg)["MySql"]["Schema"];
	std::string user = (*cfg)["MySql"]["User"];
	std::string url = host + ":" + port;
	_pool.reset(new MysqlPool(url, user, pass, schema));
}

MysqlDAO::~MysqlDAO()
{
	_pool->Close();
}

int MysqlDAO::RegUser(const std::string& name,const std::string& email,const std::string& pwd)
{
	auto con = _pool->getConnection();
	try {
		if (con == nullptr) {
			//_pool->retConnection(std::move(con));
			return 0;
		}
		std::unique_ptr<sql::PreparedStatement> stmt(con->_conn->
			prepareStatement("CALL reg_user(?,?,?,@result)"));
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, pwd);
		stmt->execute();
		std::unique_ptr<sql::Statement> stmtresult(con->_conn->
			createStatement());
		std::unique_ptr < sql::ResultSet > res(stmtresult->executeQuery("select @result as result"));
		if (res->next()) {
			int result = res->getInt("result");
			std::cout << " result : " << result << std::endl;
			_pool->retConnection(std::move(con));
			return result;
		}
		_pool->retConnection(std::move(con));
		return 0;
	}
	catch (sql::SQLException& e) {
		_pool->retConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

int MysqlDAO::CheckEmail(const std::string& name, const std::string& email)
{
	auto con = _pool->getConnection();
	try {
		if (con == nullptr) {
			//_pool->retConnection(std::move(con));
			return false;
		}
		std::unique_ptr<sql::PreparedStatement> stmt(con->_conn->prepareStatement("SELECT email FROM user WHERE name = ?"));
		stmt->setString(1, name);

		std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery());
		while (rs->next()) {
			if (email != rs->getString("email")) {
				std::cout << "check email wrong" << std::endl;
				_pool->retConnection(std::move(con));
				return false;
			}
			_pool->retConnection(std::move(con));
			return true;
		}
	}
	catch (const sql::SQLException& e) {
		_pool->retConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

int MysqlDAO::UpdatePass(const std::string& name, const std::string& pass)
{
	auto con = _pool->getConnection();
	if (con == nullptr) {
		//_pool->retConnection(std::move(con));
		return false;
	}
	try {
		std::unique_ptr<sql::PreparedStatement> stmt(con->_conn->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
		stmt->setString(1, pass);
		stmt->setString(2, name);

		int num = stmt->executeUpdate();
		std::cout << "change " << num << " rows" << std::endl;
		_pool->retConnection(std::move(con));
		return true;
	}
	catch (const sql::SQLException& e) {
		_pool->retConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

int MysqlDAO::CheckPass(const std::string& name, const std::string& pass,UserInfo& info)
{
	auto con = _pool->getConnection();
	if (con == nullptr) {
		return false;
	}
	try {
		std::unique_ptr<sql::PreparedStatement> stmt(con->_conn->prepareStatement("SELECT * FROM user WHERE name = ?"));
		stmt->setString(1, name);
		std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery());
		sql::SQLString passget = "";
		while (rs->next()) {
			passget = rs->getString("pwd");
			std::cout << "pass : " << pass << std::endl;
			break;
		}
		if (pass != passget) {
			std::cout << "用户输入的密码不正确" <<name<< " "<< pass << std::endl;
			_pool->retConnection(std::move(con));
			return false;
		}
		info.uid = rs->getInt("uid");
		info.name = name;
		info.email = rs->getString("email");
		info.pwd = pass;
		_pool->retConnection(std::move(con));
		return true;
	}
	catch (const sql::SQLException& e) {
		_pool->retConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

