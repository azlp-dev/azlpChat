#pragma once
#include "const.h"
class LogicSystem;


//处理http请求的类，超时检查，处理请求，发回响应
class HttpConnection: public std::enable_shared_from_this<HttpConnection>
{
	friend class LogicSystem;
public:
	HttpConnection(boost::asio::io_context& ioc);
	void start();
	boost::asio::ip::tcp::socket& Get_Sock() {
		return _sock;
	}

private:
	void CheckDeadLine();
	void WriteResponse();
	void HandleRequest();
	void PreParseGetParam();


	boost::asio::ip::tcp::socket _sock;
	boost::beast::flat_buffer _buffer;
	boost::beast::http::request<boost::beast::http::dynamic_body> _request;
	boost::beast::http::response<boost::beast::http::dynamic_body> _response;
	boost::asio::steady_timer _deadline{
		_sock.get_executor(),std::chrono::seconds(60)
	};
	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;

};

