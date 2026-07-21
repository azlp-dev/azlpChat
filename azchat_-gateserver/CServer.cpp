#include "CServer.h"
#include "HttpConnection.h"
#include "Asio_iocontext_pool.h"
CServer::CServer(boost::asio::io_context& ioc, uint16_t port):_ioc(ioc)
,_acp(ioc,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port))//,_sock(ioc)
{
	std::cout << "GateServer started" << std::endl;
}

void CServer::start()
{
	auto self = shared_from_this();
	auto& ioc = Asio_iocontext_pool::Get_instance()->GetIoContext();
	auto con = std::make_shared<HttpConnection>(ioc);
	_acp.async_accept(con->Get_Sock(), [self,con](boost::system::error_code ec) {
		try {
			if (ec) {
				self->start();
				return;
			}
			//auto con = std::make_shared<HttpConnection>(std::move(self->_sock));
			con->start();
			self->start();
		}
		catch (const std::exception e) {
			std::cout << "Exception is " << e.what() << std::endl;
		}
		});
}
