#include "thread_pool.h"
MSTL_BEGIN_NAMESPACE__

uint32_t __thread_pool_id_generator::__pool_thread_id = 0;

manual_thread::manual_thread(thread_func func)
	: func_(_MSTL move(func)), thread_id_(__thread_pool_id_generator::get_new_id()) {}

void manual_thread::start() {
	std::thread t(func_, thread_id_);
	t.detach();
}
manual_thread::id_type manual_thread::get_id() const {
	return thread_id_;
}

thread_pool::thread_pool()
	: init_thread_size_(0),
	thread_threshhold_(MSTL_THREAD_MAX_THRESHHOLD__),
	task_size_(0),
	idle_thread_size_(0),
	task_threshhold_(MSTL_TASK_MAX_THRESHHOLD__),
	pool_mode_(THREAD_POOL_MODE::MODE_FIXED),
	is_running_(false) {}

thread_pool::~thread_pool() {
	if(is_running_) stop();
}

bool thread_pool::set_mode(const THREAD_POOL_MODE mode) {
	if (is_running_) return false;
	pool_mode_ = mode;
    return true;
}
bool thread_pool::set_task_threshhold(const size_t threshhold) {
	if (is_running_) return false;
	task_threshhold_ = threshhold;
    return true;
}
bool thread_pool::set_thread_threshhold(const size_t threshhold) {
	if (is_running_ || pool_mode_ == THREAD_POOL_MODE::MODE_FIXED) return false;
	thread_threshhold_ = threshhold > MSTL_THREAD_MAX_THRESHHOLD__ ? MSTL_THREAD_MAX_THRESHHOLD__ : threshhold;
    return true;
}
size_t thread_pool::max_thread_size() noexcept {
	return MSTL_THREAD_MAX_THRESHHOLD__;
}

bool thread_pool::start(const size_t init_thread_size) {
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

void thread_pool::stop() {
    if (!is_running_) return;
    is_running_ = false;
    std::unique_lock<std::mutex> lock(task_queue_mtx_);
    not_empty_.notify_all();
    exit_cond_.wait(lock, [&]()->bool { return threads_map_.empty(); });
    __thread_pool_id_generator::reset_id();
}

void thread_pool::thread_function(const id_type thread_id) {
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

bool thread_pool::running() const {
	return is_running_;
}

THREAD_POOL_MODE thread_pool::mode() const {
    return pool_mode_;
}

thread_pool& get_instance_thread_pool() {
    static thread_pool instance;
    return instance;
}

MSTL_END_NAMESPACE__
