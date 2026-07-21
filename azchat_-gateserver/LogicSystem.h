#pragma once
#include "const.h"
class HttpConnection;

class LogicSystem:public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
	typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

public:
	~LogicSystem() = default;
	bool HandleGet(std::string, std::shared_ptr<HttpConnection>);
	bool HandlePost(std::string, std::shared_ptr<HttpConnection>);
	void RegGetHandler(std::string, HttpHandler);
	void RegPostHandler(std::string, HttpHandler);

private:
	LogicSystem();
	std::map<std::string, HttpHandler> _GetHandlers;
	std::map<std::string, HttpHandler> _PostHandlers;
};

