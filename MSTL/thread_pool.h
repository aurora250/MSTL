#ifndef MSTL_THREAD_POOL_H__
#define MSTL_THREAD_POOL_H__
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <future>
#include "queue.hpp"
#include "functional.hpp"
#include "unordered_map.hpp"
MSTL_BEGIN_NAMESPACE__

static constexpr size_t MSTL_TASK_MAX_THRESHHOLD__ = INT32_MAX_SIZE;
static constexpr int64_t MSTL_THREAD_MAX_IDLE_SECONDS__ = 60;
static const size_t MSTL_THREAD_MAX_THRESHHOLD__ = std::thread::hardware_concurrency();

enum class THREAD_POOL_MODE {
	MODE_FIXED,  // static number
	MODE_CACHED  // dynamic number
};

static uint32_t __pool_generate_id = 0;

class manual_thread {
public:
    using id_type        = uint32_t;

private:
	using thread_func    = _MSTL function<void(id_type)>;

	thread_func func_;
	id_type thread_id_;

public:
	manual_thread(thread_func);
	~manual_thread() = default;

	MSTL_NODISCARD id_type get_id() const;
	void start();
};

class thread_pool;
thread_pool& get_instance_thread_pool();


class thread_pool {
private:
	using Task      = _MSTL function<void()>;
    using id_type   = typename manual_thread::id_type;

	_MSTL unordered_map<id_type, _MSTL unique_ptr<manual_thread>> threads_;

	id_type init_thread_size_;
	size_t thread_size_thresh_hold_;

	_MSTL queue<Task> task_queue_;
	_MSTL queue<Task> finished_queue_;
	std::atomic_uint task_size_;
	std::atomic_uint idle_thread_size_;
	size_t task_queue_max_thresh_hold_;

	std::mutex task_queue_mtx_;
	std::condition_variable not_full_;
	std::condition_variable not_empty_;
	std::condition_variable exit_cond_;

	_MSTL THREAD_POOL_MODE pool_mode_;
	std::atomic_bool is_running_;

    friend thread_pool& get_instance_thread_pool();

private:
	void thread_function(id_type);

	thread_pool();
	~thread_pool();

public:
    thread_pool(const thread_pool&) = delete;
    thread_pool(thread_pool&&) = delete;
    thread_pool& operator =(const thread_pool&) = delete;
    thread_pool& operator =(thread_pool&&) = delete;

	bool set_mode(THREAD_POOL_MODE);
	bool set_taskque_max_thresh_hold(size_t);
	bool set_thread_size_thresh_hold(size_t);
	MSTL_NODISCARD size_t max_thread_size() noexcept;
	MSTL_NODISCARD bool running() const;
    MSTL_NODISCARD THREAD_POOL_MODE mode() const;

	void start(id_type = 2);
    void stop();

	template <typename Func, typename... Args, enable_if_t<is_invocable_v<Func, Args...>, int> = 0>
	decltype(auto) submit_task(Func&& func, Args&&... args) {
		using Result = decltype(func(args...));
		auto task = _MSTL make_shared<std::packaged_task<Result()>>(
			[func = _MSTL forward<Func>(func), args = _MSTL make_tuple(_MSTL forward<Args>(args)...)]() mutable {
				return _MSTL apply(func, args);
			}
		);
		std::future<Result> res = task->get_future();

		std::unique_lock<std::mutex> lock(task_queue_mtx_);
		if (!not_full_.wait_for(lock, std::chrono::seconds(1),
			[&]()->bool { return task_queue_.size() < task_queue_max_thresh_hold_; })) {
			auto task_ = _MSTL make_shared<std::packaged_task<Result()>>([]() -> Result { return Result(); });
			(*task_)();
			return task_->get_future();
		}
		task_queue_.emplace([task] { (*task)(); });
		++task_size_;
		not_empty_.notify_all();
		if (pool_mode_ == THREAD_POOL_MODE::MODE_CACHED
			&& task_size_ > idle_thread_size_
			&& threads_.size() < thread_size_thresh_hold_) {
			auto ptr = _MSTL make_unique<manual_thread>([this](const id_type id) { thread_function(id); });
			id_type thread_id = ptr->get_id();
			threads_.emplace(thread_id, _MSTL move(ptr));
			threads_[thread_id]->start();
			++idle_thread_size_;
		}
		return res;
	}
};

MSTL_END_NAMESPACE__
#endif // MSTL_THREAD_POOL_H__
