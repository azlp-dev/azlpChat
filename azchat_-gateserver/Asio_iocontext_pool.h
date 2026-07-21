#pragma once
#include "const.h"

class Asio_iocontext_pool : public Singleton<Asio_iocontext_pool>
{
	friend Singleton<Asio_iocontext_pool>;
	using work_ptr = std::shared_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>;
public:
	~Asio_iocontext_pool();
	Asio_iocontext_pool(const Asio_iocontext_pool&) = delete;
	Asio_iocontext_pool& operator=(const Asio_iocontext_pool&) = delete;
	boost::asio::io_context& GetIoContext();
	void Stop();
private:
	Asio_iocontext_pool(unsigned int size = std::thread::hardware_concurrency());

	std::vector<std::thread> _threads;
	std::vector<boost::asio::io_context> _iocs;
	std::vector<work_ptr> _works;
	std::size_t _next_ioc;
};

