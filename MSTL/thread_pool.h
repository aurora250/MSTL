#ifndef MSTL_THREAD_POOL_H__
#define MSTL_THREAD_POOL_H__
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <future>
#include "queue.hpp"
#include "functional.hpp"
#include "unordered_map.hpp"
MSTL_BEGIN_NAMESPACE__

static const size_t MSTL_TASK_MAX_THRESHHOLD__ = INT32_MAX;
static const size_t MSTL_THREAD_MAX_THRESHHOLD__ = std::thread::hardware_concurrency();
static const MSTL_LLT MSTL_THREAD_MAX_IDLE_SECONDS__ = 60;

enum class POOL_MODE {
	MODE_FIXED,  // static number
	MODE_CACHED  // dynamic number
};

class Thread__ {
public:
	using ThreadFunc = MSTL::function<void(int)>;

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
		auto task = MSTL::make_shared<std::packaged_task<Result()>>(
			std::bind(MSTL::forward<Func>(func), MSTL::forward<Args>(args)...));
		std::future<Result> res = task->get_future();

		std::unique_lock<std::mutex> lock(task_queue_mtx_);
		if (not not_full_.wait_for(lock, std::chrono::seconds(1),
			[&]()->bool { return task_queue_.size() < task_queue_max_thresh_hold_; })) {
			auto task = MSTL::make_shared<std::packaged_task<Result()>>([]() -> Result { return Result(); });
			(*task)();
			return task->get_future();
		}
		task_queue_.emplace([task]() { (*task)(); });
		task_size_++;
		not_empty_.notify_all();
		if (pool_mode_ == POOL_MODE::MODE_CACHED
			&& task_size_ > idle_thread_size_
			&& threads_.size() < thread_size_thresh_hold_) {
			auto ptr = MSTL::make_unique<Thread__>(std::bind(
				&ThreadPool::thread_function, this, std::placeholders::_1));
			int threadid = ptr->get_id();
			threads_.emplace(threadid, MSTL::move(ptr));
			threads_[threadid]->start();
			idle_thread_size_++;
		}
		return res;
	}
private:
	void thread_function(int threadid);
	MSTL_NODISCARD bool running() const;
private:
	using Task = function<void()>;

	unordered_map<int, MSTL::unique_ptr<Thread__>> threads_;

	uint32_t init_thread_size_;
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
};

MSTL_END_NAMESPACE__
#endif // MSTL_THREAD_POOL_H__
