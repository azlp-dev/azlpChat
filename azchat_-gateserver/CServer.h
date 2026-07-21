#pragma once
#include "const.h"

class CServer:public std::enable_shared_from_this<CServer>
 {
public:
	CServer(boost::asio::io_context& ioc,uint16_t port);
	void start();

private:
	boost::asio::io_context& _ioc;

	boost::asio::ip::tcp::acceptor _acp;
	//boost::asio::ip::tcp::socket _sock;
};

