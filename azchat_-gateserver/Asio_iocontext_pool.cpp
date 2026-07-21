#include "Asio_iocontext_pool.h"

Asio_iocontext_pool::~Asio_iocontext_pool()
{
	Stop();
	std::cout << "ioc_pool destructed here " << std::endl;
}

boost::asio::io_context& Asio_iocontext_pool::GetIoContext()
{
	auto& ioc = _iocs[(_next_ioc++)%_iocs.size()];
	return ioc;
}

void Asio_iocontext_pool::Stop()
{
	for (auto& work : _works) {
		work.reset();
	}
	for (auto& ioc : _iocs) {
		ioc.stop();
	}
	for (auto& t : _threads) {
		if (t.joinable()) {
			t.join();
		}
	}
}

Asio_iocontext_pool::Asio_iocontext_pool(unsigned int size)
	:_next_ioc(0),_works(size),_iocs(size)
{
	for (int i = 0;i < size;i++) {
		_works[i] = std::shared_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(
			new boost::asio::executor_work_guard<boost::asio::io_context::executor_type>(_iocs[i].get_executor())
			);
	}

	for (int i = 0;i < size;i++) {
		_threads.emplace_back([this,i]() {
			_iocs[i].run();
			});
	}
}


