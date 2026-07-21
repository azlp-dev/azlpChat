#include "Cserver.h"
#include "AsioContextPool.h"
#include "CSession.h"
#include "UserMgr.h"

Cserver::~Cserver()
{
	std::cout << "Cserver destructed..." << std::endl; 
}

Cserver::Cserver(boost::asio::io_context& ioc, unsigned short port)
	:_port(port),_maincontext(ioc)
	,_accepter(_maincontext,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),_port))
{
	std::cout << "cserver created" << std::endl;
	startacceptor();
}

void Cserver::startacceptor()
{
	auto pool = AsioContextPool::Get_instance();
	auto& ioc = pool->getContext();
	std::shared_ptr<CSession> session = std::make_shared<CSession>(ioc,this);
	_accepter.async_accept(session->retsock(), std::bind(&Cserver::handleacceptor, this, session, std::placeholders::_1));
}

void Cserver::handleacceptor(std::shared_ptr<CSession> session,boost::system::error_code ec)
{
	if (!ec) {
		session->start();
		std::lock_guard<std::mutex> lock(_mtx);
		_sessions.insert(std::make_pair(session->retsessionid(), session));
	}
	else {
		std::cout << "handle accept failed " << std::endl;
	}
	//std::cout << "tick4" << std::endl;
	startacceptor();
}

void Cserver::clearsession(std::string sessionID)
{
	{
		if (_sessions.find(sessionID) != _sessions.end()) {
			UserMgr::Get_instance()->rmvSession(_sessions[sessionID]->retuseruid());
		}
	}
	std::lock_guard<std::mutex> lock(_mtx);
	_sessions.erase(sessionID);
}
