#pragma once
#include "const.h"
#include "MsgNode.h"
#include <queue>

class Cserver; 
//session对应到服务器与客户端的具体通信处理数据维护并发送数据
class CSession :public std::enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& ioc, Cserver* server);
	~CSession();

	boost::asio::ip::tcp::socket& retsock() {
		return _sock;
	}

	std::string& retsessionid() {
		return _session_id;
	}

	void setuseruid(int uid) {
		_useruid = uid;
	}

	int retuseruid() {
		return _useruid;
	}

	std::shared_ptr<CSession> sharedself() {
		return shared_from_this();
	}

	void close();

	//接收消息逻辑：封装asyncreadsome实现读取完头部消息 根据头部消息处理body 返回读取头部消息
	void start() {
		AsyncReadHEAD();
	}

	void AsyncReadHEAD(int total = HEAD_TOTAL_LEN);
	void AsyncReadBody(int total);

	//此处调用readlen函数读取足够的字符，readlen函数读取够足够的字符再进行回调，调用readfull函数时要写好回调
	void AsyncReadFull(std::size_t maxlength,std::function<void(boost::system::error_code ec,std::size_t bytes_transfered)>);
	//此处封装read——some函数读取指定的字符
	void AsyncReadLen(std::size_t currentlen, std::size_t totallen, std::function<void(boost::system::error_code ec, std::size_t bytes_transfered)> handler);

	void send(const char* msg, std::size_t len, int msgid);
	void send(std::string msg, int msgid);
	void HandleWrite(boost::system::error_code ec, std::shared_ptr<CSession> self);


private:
	boost::asio::ip::tcp::socket _sock;
	int _useruid;
	std::string _session_id;
	std::atomic<bool> _b_close;
	char _data[MAX_LENGTH];
	Cserver* server;

	std::queue<std::shared_ptr<SendNode>> _sendque;
	std::mutex _send_mtx;
	std::shared_ptr<RecvNode> _rec_node;
	std::shared_ptr<MsgNode> _head_node;
	std::atomic<bool> _b_head_parse;
	std::mutex _session_mtx;
};

class LogicSystem;
//逻辑节点用于把session和构造好的recvnode传给逻辑系统用于处理
class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(std::shared_ptr<CSession> ses, std::shared_ptr<RecvNode> recvnode)
		: _session(ses),_node(recvnode)
	{
	}

private:
	std::shared_ptr<CSession> _session;
	std::shared_ptr<RecvNode> _node;
};
