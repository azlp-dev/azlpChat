#include "CSession.h"
#include <boost/uuid.hpp>
#include "Cserver.h"
#include "LogicSystem.h"

CSession::CSession(boost::asio::io_context& ioc, Cserver* server)
	:_sock(ioc),_b_close(false),server(server),_b_head_parse(false),_useruid(-1)
{
	_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
	boost::uuids::uuid temp = boost::uuids::random_generator()();
	_session_id = boost::uuids::to_string(temp);
}

CSession::~CSession()
{
	std::cout << "Csession destructed" << std::endl;
}

void CSession::close()
{
	std::lock_guard<std::mutex> lock(_session_mtx);
	if (_b_close) {
		return;
	}
	_b_close = true;
	_sock.close();
}

void CSession::AsyncReadHEAD(int total)
{
	auto self = sharedself();
	AsyncReadFull(total, [self, this,total](boost::system::error_code ec, std::size_t bttransfered) {
		try {
			if (ec) {
				std::cout << "read head error:" << ec.what() << std::endl;
				close();
				server->clearsession(_session_id);
				return;
			}
			if (bttransfered < total) {
				std::cout << "readhead error read:" << bttransfered << std::endl;
				close();
				server->clearsession(_session_id);
				return;
			}
			_head_node->clear();
			memcpy(_head_node->_data, _data, HEAD_TOTAL_LEN);
			//下面进行头部信息的字节序转换和读取，调用readbody
			short mesgid;
			short msglen;
			memcpy(&mesgid, _head_node->_data, HEAD_ID_LEN);
			memcpy(&msglen, _head_node->_data+HEAD_ID_LEN, HEAD_DATA_LEN);
			mesgid = boost::asio::detail::socket_ops::network_to_host_short(mesgid);
			msglen = boost::asio::detail::socket_ops::network_to_host_short(msglen);
			if (mesgid > MAX_MSG_ID) {
				std::cout << "error in msg id:" << mesgid << std::endl;
				close();
				server->clearsession(_session_id);
				return;
			}
			if (msglen > MAX_LENGTH) {
				std::cout << "error in msg len:" << msglen << std::endl;
				close();
				server->clearsession(_session_id);
				return;
			}

			_rec_node = std::make_shared<RecvNode>(mesgid, msglen);
			std::cout << "msg id: " << mesgid << std::endl;
			AsyncReadBody(msglen);
		}
		catch (const std::exception& e) {
			std::cout << "exception:" << e.what() << std::endl;
		}
		});
}

void CSession::AsyncReadBody(int total)
{
	
	auto self = sharedself();
	AsyncReadFull(total, [self, this, total](boost::system::error_code ec, std::size_t bt) {
		try {
			if (ec) {
				std::cout << "read body error" << std::endl;
				close();
				server->clearsession(_session_id);
				return;
			}
			if (bt < total) {
				std::cout << "read body error,error in len:" << total << " " << bt << std::endl;
				close();
				server->clearsession(_session_id);
				return;
			}
			memcpy(_rec_node->_data, _data, bt);
			std::cout << "recv msg:" << _data << std::endl;
			_rec_node->_cur = bt;
			auto logicnode = std::make_shared<LogicNode>(self, _rec_node);
			LogicSystem::Get_instance()->PostMsgtoQue(logicnode);
			AsyncReadHEAD();
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		});
}

void CSession::AsyncReadFull(std::size_t maxlength, std::function<void(boost::system::error_code ec, std::size_t bytes_transfered)> handler)
{
	::memset(_data, 0, MAX_LENGTH);
	AsyncReadLen(0,maxlength, handler);
}

void CSession::AsyncReadLen(std::size_t currentlen, std::size_t totallen, std::function<void(boost::system::error_code ec, std::size_t bytes_transfered)> handler)
{
	auto self = sharedself();
	_sock.async_read_some(boost::asio::buffer(_data + currentlen, totallen - currentlen),
		[self, this, handler, currentlen, totallen](boost::system::error_code ec, std::size_t bttransfered) {
			if (ec) {
				handler(ec, currentlen + bttransfered);
				return;
			}
			if (currentlen + bttransfered >= totallen) {
				handler(ec, totallen);
				return;
			}
			AsyncReadLen(currentlen + bttransfered, totallen, handler);
		});
}

void CSession::send(const char* msg, std::size_t len, int msgid)
{
	auto self = sharedself();
	std::lock_guard<std::mutex> lock(_send_mtx);
	try {
		int size = _sendque.size();
		if (size > MAX_SAND_QUE) {
			std::cout << "error : send que full" << std::endl;
			return;
		}
		_sendque.push(std::make_shared<SendNode>(msg, msgid, len));
		if (size > 0) {
			return;
		}
		auto& msgnode = _sendque.front();
		boost::asio::async_write(_sock, boost::asio::
			buffer(msgnode->_data, msgnode->_total), std::bind(&CSession::HandleWrite, this, std::placeholders::_1, self));
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

void CSession::send(std::string msg, int msgid)
{
	auto size = msg.length();
	send(msg.c_str(), size, msgid);
}

void CSession::HandleWrite(boost::system::error_code ec, std::shared_ptr<CSession> self)
{
	try {
		auto self = sharedself();
		if (!ec) {
			std::lock_guard<std::mutex> lock(_send_mtx);
			_sendque.pop();
			if (!_sendque.empty()) {
				boost::asio::async_write(_sock, boost::asio::buffer(_sendque.front()->_data, _sendque.front()->_total)
					, std::bind(&CSession::HandleWrite, this, std::placeholders::_1, self));
			}
		}
		else {
			std::cout << "write error!" << std::endl;
			close();
			server->clearsession(_session_id);
			return;
		}
	}
	catch (const std::exception& e) {
		std::cout << e.what()<<std::endl;
		return;
	}
}

