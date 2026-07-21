#pragma once
#include "const.h"
#include "single.h"
#include <queue>

class AsioContextPool : public Singleton<AsioContextPool>
{
	friend class Singleton<AsioContextPool>;
public:
	~AsioContextPool();
	void Stop();
	boost::asio::io_context& getContext();


private:
	AsioContextPool(size_t size = std::thread::hardware_concurrency());

	size_t _size;

	std::mutex _mtx;
	std::condition_variable _cond;
	std::vector<boost::asio::io_context> _pool;
	std::vector<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> _works;
	std::vector<std::thread> _threads;
	std::atomic<bool> _b_stop {false};
	std::atomic<int> _index{ 0 };

};

