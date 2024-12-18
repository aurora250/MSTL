#ifndef MSTL_THREAD_POOL_H__
#define MSTL_THREAD_POOL_H__
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <chrono>
#include <future>
#include "basiclib.h"
#include "queue.hpp"
#include "hash_map.hpp"
#include "threadsafe_print.h"

MSTL_BEGIN_NAMESPACE__

enum class POOL_MODE {
	MODE_FIXED,  // static number
	MODE_CACHED  // dynamic number
};

class __thread {
public:
	typedef std::function<void(int)> ThreadFunc;

	__thread(ThreadFunc func);
	~__thread();

	int getId() const;
	void start();
private:
	ThreadFunc func_;
	static int generateId_;
	int threadId_;
};

class ThreadPool {
public:
	ThreadPool();
	~ThreadPool();

	void set_mode(POOL_MODE mode);
	void set_taskque_max_thresh_hold(size_t threshHold);
	void set_thread_size_thresh_hold(size_t threshHold);
	size_t max_thread_size();
	void start(unsigned int initThreadSize = 2);

	template <typename Func, typename... Args>
	decltype(auto) submitTask(Func&& func, Args&&... args) {
		using Result = decltype(func(args...));
		auto task = std::make_shared<std::packaged_task<Result()>>(
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
		std::future<Result> res = task->get_future();

		std::unique_lock<std::mutex> lock(taskQueMtx_);
		if (not notFull_.wait_for(lock, std::chrono::seconds(1),
			[&]()->bool { return taskQueue_.size() < taskQueMaxThreshHold_; })) {
			sout << "Task queue is full, submit failed\n";
			auto task = std::make_shared<std::packaged_task<Result()>>([]() -> Result { return Result(); });
			(*task)();
			return task->get_future();
		}
		taskQueue_.emplace([task]() { (*task)(); });
		taskSize_++;
		notEmpty_.notify_all();
		if (poolMode_ == POOL_MODE::MODE_CACHED
			&& taskSize_ > idleThreadSize_
			&& threads_.size() < threadSizeThreshHold_) {
			sout << ">>> creat new thread\n";
			auto ptr = std::make_shared<__thread>(std::bind(&ThreadPool::thread_function, this, std::placeholders::_1));
			int threadid = ptr->getId();
			threads_.emplace(threadid, std::move(ptr));
			threads_[threadid]->start();
			idleThreadSize_++;
		}
		return res;
	}

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator =(const ThreadPool&) = delete;
private:
	void thread_function(int threadid);
	bool checkRunningState() const;
private:
	typedef std::function<void()> Task;

	//std::
	unordered_map<int, std::shared_ptr<__thread>> threads_;

	size_t initThreadSize_;
	size_t threadSizeThreshHold_;

	queue<Task> taskQueue_;
	std::atomic_uint taskSize_;
	std::atomic_uint idleThreadSize_;
	size_t taskQueMaxThreshHold_;

	std::mutex taskQueMtx_;
	std::condition_variable notFull_;
	std::condition_variable notEmpty_;
	std::condition_variable exitCond_;

	POOL_MODE poolMode_;
	std::atomic_bool isRunning_;

	Output aux_output_;
};

MSTL_END_NAMESPACE__
#endif // MSTL_THREAD_POOL_H__
