#include "HttpConnection.h"
#include "LogicSystem.h"

HttpConnection::HttpConnection(boost::asio::io_context& ioc)
	:_sock(ioc)
{

}

void HttpConnection::start()
{
	auto self = shared_from_this();
	boost::beast::http::async_read(_sock, _buffer, _request,
		[self](boost::system::error_code ec, size_t bt) {
			try {
				if (ec) {
					std::cout << "http read error: " << ec.what() << std::endl;
					return;
				}
				boost::ignore_unused(bt);
				self->HandleRequest();
				self->CheckDeadLine(); 
			}
			catch (const std::exception e) {
				std::cout << "Exception is " << e.what() << std::endl;
				return;
			}
		});
}


void HttpConnection::CheckDeadLine()
{
	auto self = shared_from_this();
	_deadline.async_wait([self](boost::system::error_code ec) {
		if (!ec) {
			self->_sock.close();
		}
		});
}

void HttpConnection::WriteResponse()
{
	auto self = shared_from_this();
	_response.content_length(_response.body().size());
	boost::beast::http::async_write(_sock, _response
		, [self](boost::system::error_code ec, size_t bt) {
			if (!ec) {
				self->_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
				self->_deadline.cancel();
			}
		});
}

unsigned char ToHex(unsigned char x) {
	return x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x) {
	unsigned char y;
	if (x >= 'A' && x<= 'Z') {
		y = x - 'A' + 10;
	}
	else if (x >= 'a' && x <= 'z') {
		y = x - 'a' + 10;
	}
	else if(x >= '0' && x<= '9'){
		y = x - '0';
	}
	else {
		assert(0);
	}
	return y;
}

std::string UrlDecode(std::string str) {
	std::string temp = "";
	size_t length = str.length();
	for (int i = 0; i < length; i++) {
		if (str[i] == '+') {
			temp += ' ';
		}
		else if (str[i] == '%') {
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			temp += high * 16 + low;
		}
		else {
			temp += str[i];
		}	
	}
	return temp;
}

std::string UrlEncode(std::string& str) {
	std::string temp = "";
	size_t length = str.length();
	for(int i = 0; i < length; i++) {
		if (isalnum((unsigned char)str[i]) ||
			str[i] == '_' ||
			str[i] == '-' ||
			str[i] == '~'  ) {
			temp += str[i];
		}
		else if (str[i] == ' ') {
			temp += '+';
		}
		else {
			temp += '%';
			temp += ToHex((unsigned char)str[i] >> 4);
			temp += ToHex((unsigned char)str[i] & 0x0f);
		}
	}
	return temp;
}

void HttpConnection::HandleRequest()
{
	auto self = shared_from_this();
	_response.version(_request.version());
	_response.keep_alive(false);
	PreParseGetParam();
	if (_request.method() == boost::beast::http::verb::get) {
		bool success = LogicSystem::Get_instance()->HandleGet(_get_url, self);
		if (!success) {
			_response.result(boost::beast::http::status::not_found);
			_response.set(boost::beast::http::field::content_type, "text/plain");
			boost::beast::ostream(_response.body())<<"url not found\r\n";
			WriteResponse();
			return;
		}
		_response.result(boost::beast::http::status::ok);
		_response.set(boost::beast::http::field::server, "GateServer");
		WriteResponse();
		return;
	}
	if (_request.method() == boost::beast::http::verb::post) {
		bool success = LogicSystem::Get_instance()->HandlePost(_request.target(), self);
		if (!success) {
			_response.result(boost::beast::http::status::not_found);
			_response.set(boost::beast::http::field::content_type, "text/plain");
			boost::beast::ostream(_response.body()) << "url not found\r\n";
			WriteResponse();
			return;
		}
		_response.result(boost::beast::http::status::ok);
		_response.set(boost::beast::http::field::server, "GateServer");
		WriteResponse();
		return;
	}
}

void HttpConnection::PreParseGetParam()
{
	auto uri = _request.target();
	auto query_pos = uri.find('?');
	if (query_pos == std::string::npos) {
		_get_url = uri;
		return;
	}
	_get_url = uri.substr(0, query_pos);
	std::string sub_string = uri.substr(query_pos + 1);
	std::string key;
	std::string value;
	size_t pos = 0;
	while ((pos = sub_string.find('&')) != std::string::npos) {
		auto pair_string = sub_string.substr(0, pos);
		size_t eq_pos = 0;
		if ((eq_pos = pair_string.find('=')) != std::string::npos) {
			key = UrlDecode(pair_string.substr(0, eq_pos));
			value = UrlDecode(pair_string.substr(eq_pos+1));
			_get_params[key] = value;
		}
		sub_string.erase(0, pos + 1);
	}
	if (!sub_string.empty()) {
		size_t eq_pos = 0;
		if ((eq_pos = sub_string.find('=')) != std::string::npos) {
			key = UrlDecode(sub_string.substr(0, eq_pos));
			value = UrlDecode(sub_string.substr(eq_pos+1));
			_get_params[key] = value;
		}
	}
	return;
}
