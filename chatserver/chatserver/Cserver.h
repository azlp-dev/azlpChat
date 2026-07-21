#pragma once
#include "const.h"

class CSession;
class Cserver //public Singleton<Cserver>
{
	//friend class Singleton<Cserver>;
public:
	~Cserver();
	Cserver(boost::asio::io_context& ioc, unsigned short port);
	void clearsession(std::string sessionID);


private:
	
	void startacceptor();
	void handleacceptor(std::shared_ptr<CSession> session, boost::system::error_code ec);

	unsigned short _port;
	boost::asio::io_context& _maincontext;
	boost::asio::ip::tcp::acceptor _accepter;
	std::map<std::string, std::shared_ptr<CSession>> _sessions;
	std::mutex _mtx;
};

