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
static const size_t MSTL_THREAD_MAX_THRESHHOLD__ = std::thread::hardware_concurrency();
static constexpr int64_t MSTL_THREAD_MAX_IDLE_SECONDS__ = 60;

enum class THREAD_POOL_MODE {
	MODE_FIXED,  // static number
	MODE_CACHED  // dynamic number
};

class manual_thread;
class thread_pool;

struct __thread_pool_id_generator {
private:
    static uint32_t& get_id() {
        static uint32_t __pool_thread_id = 0;
        return __pool_thread_id;
    }

    static uint32_t get_new_id() {
        uint32_t& __pool_thread_id = get_id();
        return __pool_thread_id++;
    }

    static void reset_id() {
        get_id() = 0;
    }

    friend class manual_thread;
    friend class thread_pool;
};


#ifdef MSTL_PLATFORM_LINUX__
class pthread {
private:
	enum class STATE {
		Created,
		Detached,
		Joined,
		Cancelled
	};

	STATE state = STATE::Created;

	pthread_t thread = 0;
	int created = 1;

public:
	template <typename F, typename... Args>
	explicit pthread(F&& func, Args&&... args) {
		created = pthread_create(&thread, nullptr, func, this);
	}
	template <typename F, typename... Args>
	explicit pthread(const pthread_attr_t* attr, F&& func, Args&&... args) {
		created = pthread_create(&thread, attr, func, this);
	}

	~pthread() {
		if (is_created() && state == STATE::Created) {
			pthread_detach(thread);
		}
	}

	MSTL_NODISCARD bool is_created() const noexcept {
		return created == 0;
	}
	MSTL_NODISCARD bool is_joinable() const {
		return is_created() && state != STATE::Detached;
	}

	MSTL_NODISCARD bool set_name(const char* name) const {
		return pthread_setname_np(thread, name) == 0;
	}

	MSTL_NODISCARD size_t get_id() const noexcept {
		return thread;
	}
	MSTL_NODISCARD STATE get_state() const {
		return state;
	}

	static MSTL_NODISCARD pthread_t current_thread_id() {
		return pthread_self();
	}


	MSTL_NODISCARD bool join() {
		if (state != STATE::Created) return false;
		const bool res = (pthread_join(thread, nullptr) == 0);
		if (res) state = STATE::Joined;
		return res;
	}

	MSTL_NODISCARD bool detach() {
		if (state != STATE::Created) return false;
		const bool res = (pthread_detach(thread) == 0);
		if (res) state = STATE::Detached;
		return res;
	}

	MSTL_NODISCARD bool cancel() {
		if (state != STATE::Created) return false;
		const bool res = (pthread_cancel(thread) == 0);
		if (res) state = STATE::Cancelled;
		return res;
	}
};
#endif


class manual_thread {
public:
    using id_type = uint32_t;

private:
	using thread_func = _MSTL function<void(id_type)>;

	thread_func func_;
	id_type thread_id_;

public:
	explicit manual_thread(thread_func func)
    : func_(_MSTL move(func)),
    thread_id_(__thread_pool_id_generator::get_new_id()) {}

	~manual_thread() = default;

    MSTL_NODISCARD id_type get_id() const {
        return thread_id_;
    }

    void start() {
        std::thread t(func_, thread_id_);
        t.detach();
    }
};


class thread_pool {
public:
    using id_type = manual_thread::id_type;

private:
	using Task = _MSTL function<void()>;

	_MSTL unordered_map<id_type, _MSTL unique_ptr<manual_thread>> threads_map_;

	id_type init_thread_size_;
	size_t thread_threshhold_;

	_MSTL queue<Task> task_queue_;
	std::atomic_uint task_size_;
	std::atomic_uint idle_thread_size_;
	size_t task_threshhold_;

	std::mutex task_queue_mtx_;
	std::condition_variable not_full_;
	std::condition_variable not_empty_;
	std::condition_variable exit_cond_;

	std::atomic<THREAD_POOL_MODE> pool_mode_;
	std::atomic_bool is_running_;

    friend thread_pool& get_instance_thread_pool();

private:
    void thread_function(const id_type thread_id) {
        auto last = std::chrono::high_resolution_clock::now();

        for (;;) {
            Task task{};
            {
                std::unique_lock<std::mutex> lock(task_queue_mtx_);
                while (task_queue_.empty()) {
                    if (!is_running_) {
                        threads_map_.erase(thread_id);
                        exit_cond_.notify_all();
                        return;
                    }
                    if (pool_mode_ == THREAD_POOL_MODE::MODE_CACHED) {
                        if (std::cv_status::timeout == not_empty_.wait_for(lock, std::chrono::seconds(1))) {
                            auto now = std::chrono::high_resolution_clock::now();
                            const auto sub = std::chrono::duration_cast<std::chrono::seconds>(now - last);
                            if (sub.count() >= MSTL_THREAD_MAX_IDLE_SECONDS__ && threads_map_.size() > init_thread_size_) {
                                threads_map_.erase(thread_id);
                                --idle_thread_size_;
                                return;
                            }
                        }
                    }
                    else {
                        not_empty_.wait(lock);
                    }
                }

                --idle_thread_size_;
                task = task_queue_.front();
                task_queue_.pop();
                --task_size_;
                if (!task_queue_.empty()) not_empty_.notify_all();
                not_full_.notify_all();
            }
            if (task != nullptr) task();
            ++idle_thread_size_;
            last = std::chrono::high_resolution_clock::now();
        }
    }


    thread_pool()
    : init_thread_size_(0),
    thread_threshhold_(MSTL_THREAD_MAX_THRESHHOLD__),
    task_size_(0),
    idle_thread_size_(0),
    task_threshhold_(MSTL_TASK_MAX_THRESHHOLD__),
    pool_mode_(THREAD_POOL_MODE::MODE_FIXED),
    is_running_(false) {}

    ~thread_pool() {
        if(is_running_) stop();
    }

public:
    thread_pool(const thread_pool&) = delete;
    thread_pool(thread_pool&&) = delete;
    thread_pool& operator =(const thread_pool&) = delete;
    thread_pool& operator =(thread_pool&&) = delete;

    bool set_mode(const THREAD_POOL_MODE mode) {
        if (is_running_) return false;
        pool_mode_ = mode;
        return true;
    }

    bool set_task_threshhold(const size_t threshhold) {
        if (is_running_) return false;
        task_threshhold_ = threshhold;
        return true;
    }

    bool set_thread_threshhold(const size_t threshhold) {
        if (is_running_ || pool_mode_ == THREAD_POOL_MODE::MODE_FIXED) return false;
        thread_threshhold_ = threshhold > MSTL_THREAD_MAX_THRESHHOLD__
            ? MSTL_THREAD_MAX_THRESHHOLD__ : threshhold;
        return true;
    }

    MSTL_NODISCARD static size_t max_thread_size() noexcept {
        return MSTL_THREAD_MAX_THRESHHOLD__;
    }
    MSTL_NODISCARD bool running() const {
        return is_running_;
    }
    MSTL_NODISCARD THREAD_POOL_MODE mode() const {
        return pool_mode_;
    }

    bool start(const size_t init_thread_size = 3) {
        if(is_running_) return false;
        is_running_ = true;
        init_thread_size_ = init_thread_size;
        for (id_type i = 0; i < init_thread_size_; i++) {
            auto ptr = _MSTL make_unique<manual_thread>([this](const int id) { thread_function(id); });
            threads_map_.emplace(ptr->get_id(), _MSTL move(ptr));
        }
        for (id_type i = 0; i < init_thread_size_; i++) {
            threads_map_[i]->start();
            ++idle_thread_size_;
        }
        return true;
    }

    void stop() {
        if (!is_running_) return;
        is_running_ = false;
        std::unique_lock<std::mutex> lock(task_queue_mtx_);
        not_empty_.notify_all();
        exit_cond_.wait(lock, [&]()->bool { return threads_map_.empty(); });
        __thread_pool_id_generator::reset_id();
    }

	template <typename Func, typename... Args, enable_if_t<is_invocable_v<Func, Args...>, int> = 0>
	decltype(auto) submit_task(Func&& func, Args&&... args) {
		using Result = decltype(func(_MSTL forward<Args>(args)...));
		auto task = _MSTL make_shared<std::packaged_task<Result()>>(
			[func = _MSTL forward<Func>(func), args = _MSTL make_tuple(_MSTL forward<Args>(args)...)]() mutable {
				return _MSTL apply(func, args);
			}
		);
		std::future<Result> res = task->get_future();

		std::unique_lock<std::mutex> lock(task_queue_mtx_);
		if (!not_full_.wait_for(lock, std::chrono::seconds(1),
			[&]()->bool { return task_queue_.size() < task_threshhold_; })) {
			auto task_ = _MSTL make_shared<std::packaged_task<Result()>>([]() -> Result { return Result(); });
			(*task_)();
			return task_->get_future();
		}
		task_queue_.emplace([task] { (*task)(); });
		++task_size_;
		not_empty_.notify_all();
		if (pool_mode_ == THREAD_POOL_MODE::MODE_CACHED
			&& task_size_ > idle_thread_size_
			&& threads_map_.size() < thread_threshhold_) {
			auto ptr = _MSTL make_unique<manual_thread>([this](const id_type id) { thread_function(id); });
			id_type thread_id = ptr->get_id();
			threads_map_.emplace(thread_id, _MSTL move(ptr));
			threads_map_[thread_id]->start();
			++idle_thread_size_;
		}
		return res;
	}
};

inline thread_pool& get_instance_thread_pool() {
    static thread_pool instance;
    return instance;
}

MSTL_END_NAMESPACE__
#endif // MSTL_THREAD_POOL_H__
