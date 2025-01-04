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

static const size_t MSTL_TASK_MAX_THRESHHOLD__ = INT32_MAX;
static const size_t MSTL_THREAD_MAX_THRESHHOLD__ = std::thread::hardware_concurrency();
static const MSTL_LONG_LONG_TYPE__ MSTL_THREAD_MAX_IDLE_SECONDS__ = 60;

enum class POOL_MODE {
	MODE_FIXED,  // static number
	MODE_CACHED  // dynamic number
};

class Thread__ {
public:
	typedef std::function<void(int)> ThreadFunc;

	Thread__(ThreadFunc func);
	~Thread__();

	MSTL_NODISCARD int get_id() const;
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
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator =(const ThreadPool&) = delete;
	ThreadPool& operator =(ThreadPool&&) = delete;

	void set_mode(POOL_MODE mode);
	void set_taskque_max_thresh_hold(size_t threshHold);
	void set_thread_size_thresh_hold(size_t threshHold);
	MSTL_NODISCARD size_t max_thread_size();
	void start(unsigned int initThreadSize = 2);

	template <typename Func, typename... Args>
	decltype(auto) submit_task(Func&& func, Args&&... args) {
		using Result = decltype(func(args...));
		auto task = std::make_shared<std::packaged_task<Result()>>(
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
		std::future<Result> res = task->get_future();

		std::unique_lock<std::mutex> lock(task_queue_mtx_);
		if (not not_full_.wait_for(lock, std::chrono::seconds(1),
			[&]()->bool { return task_queue_.size() < task_queue_max_thresh_hold_; })) {
#ifdef MSTL_STATE_DEBUG__
			sout << "Task queue is full, submit failed\n";
#endif
			auto task = std::make_shared<std::packaged_task<Result()>>([]() -> Result { return Result(); });
			(*task)();
			return task->get_future();
		}
		task_queue_.emplace([task]() { (*task)(); });
		task_size_++;
		not_empty_.notify_all();
		if (pool_mode_ == POOL_MODE::MODE_CACHED
			&& task_size_ > idle_thread_size_
			&& threads_.size() < thread_size_thresh_hold_) {
#ifdef MSTL_STATE_DEBUG__
			sout << ">>> creat new thread\n";
#endif
			auto ptr = std::make_shared<Thread__>(std::bind(&ThreadPool::thread_function, this, std::placeholders::_1));
			int threadid = ptr->get_id();
			threads_.emplace(threadid, std::move(ptr));
			threads_[threadid]->start();
			idle_thread_size_++;
		}
		return res;
	}
private:
	void thread_function(int threadid);
	MSTL_NODISCARD bool running() const;
private:
	typedef std::function<void()> Task;

	//std::
	unordered_map<int, std::shared_ptr<Thread__>> threads_;

	size_t init_thread_size_;
	size_t thread_size_thresh_hold_;

	queue<Task> task_queue_;
	queue<Task> finished_queue_;
	std::atomic_uint task_size_;
	std::atomic_uint idle_thread_size_;
	size_t task_queue_max_thresh_hold_;

	std::mutex task_queue_mtx_;
	std::condition_variable not_full_;
	std::condition_variable not_empty_;
	std::condition_variable exit_cond_;

	POOL_MODE pool_mode_;
	std::atomic_bool is_running_;

	Output aux_output_;
};

MSTL_END_NAMESPACE__
#endif // MSTL_THREAD_POOL_H__
