#include "AsioContextPool.h"

AsioContextPool::~AsioContextPool()
{
	std::cout << "contextpool destructed..." << std::endl;
}

void AsioContextPool::Stop()
{
	for (int i = 0; i < _size; i++) {
		_pool[i].stop();
		_works[i].reset();
	}
	for (int i = 0; i < _size; i++) {
		if (_threads[i].joinable()) {
			_threads[i].join();
		}
	}
}

boost::asio::io_context& AsioContextPool::getContext()
{
	if (_index >= _size) {
		_index = _index % _size;
	}
	return _pool[_index++];
}

AsioContextPool::AsioContextPool(size_t size)
	:_size(size),_pool(_size)
{
	for (int i = 0; i < _size; i++) {
		_works.reserve(_size);
		_works.emplace_back(boost::asio::make_work_guard(_pool[i]));
		_threads.emplace_back([this, i]() {
			_pool[i].run();
			});
	}
}


