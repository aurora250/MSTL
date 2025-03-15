#include "thread_pool.h"

#include <utility>
MSTL_BEGIN_NAMESPACE__

int __thread_aux::generateId_ = 0;

__thread_aux::__thread_aux(ThreadFunc func)
	: func_(MSTL::move(func)),
	threadId_(generateId_++) {
}

void __thread_aux::start() {
	std::thread t(func_, threadId_);
	t.detach();
}
int __thread_aux::get_id() const {
	return threadId_;
}

ThreadPool::ThreadPool()
	: init_thread_size_(0),
	thread_size_thresh_hold_(MSTL_THREAD_MAX_THRESHHOLD__),
	task_size_(0),
	idle_thread_size_(0),
	task_queue_max_thresh_hold_(MSTL_TASK_MAX_THRESHHOLD__),
	pool_mode_(POOL_MODE::MODE_FIXED),
	is_running_(false) {
}

ThreadPool::~ThreadPool() {
	is_running_ = false;
	std::unique_lock<std::mutex> lock(task_queue_mtx_);
	not_empty_.notify_all();   // unlock and call threadFunc to exit
	exit_cond_.wait(lock, [&]()->bool { return threads_.empty(); });
}

void ThreadPool::set_mode(const POOL_MODE mode) {
	if (running()) return;
	pool_mode_ = mode;
}
void ThreadPool::set_taskque_max_thresh_hold(const size_t threshHold) {
	if (running()) return;
	task_queue_max_thresh_hold_ = threshHold;
}
void ThreadPool::set_thread_size_thresh_hold(const size_t threshHold) {
	if (running() || pool_mode_ == POOL_MODE::MODE_FIXED) return;
	thread_size_thresh_hold_ = threshHold > MSTL_THREAD_MAX_THRESHHOLD__ ? MSTL_THREAD_MAX_THRESHHOLD__ : threshHold;
}
size_t ThreadPool::max_thread_size() {
	return MSTL_THREAD_MAX_THRESHHOLD__;
}

void ThreadPool::start(unsigned int initThreadSize) {
	is_running_ = true;
	init_thread_size_ = initThreadSize;
	for (int i = 0; i < init_thread_size_; i++) {
		auto ptr = MSTL::make_unique<__thread_aux>(
			std::bind(&ThreadPool::thread_function, this, std::placeholders::_1));
		threads_.emplace(ptr->get_id(), MSTL::move(ptr));
	}
	for (int i = 0; i < init_thread_size_; i++) {
		threads_[i]->start();
		++idle_thread_size_;
	}
}

void ThreadPool::thread_function(const int thread_id) {
	auto last = std::chrono::high_resolution_clock::now();

	for (;;) {
		Task task;
		{
			std::unique_lock<std::mutex> lock(task_queue_mtx_);
			while (task_queue_.empty()) {
				if (!is_running_) {
					threads_.erase(thread_id);
					exit_cond_.notify_all();
					return;
				}
				if (pool_mode_ == POOL_MODE::MODE_CACHED) {
					if (std::cv_status::timeout == not_empty_.wait_for(lock, std::chrono::seconds(1))) {
						auto now = std::chrono::high_resolution_clock::now();
						auto sub = std::chrono::duration_cast<std::chrono::seconds>(now - last);
						if (sub.count() >= MSTL_THREAD_MAX_IDLE_SECONDS__ && threads_.size() > init_thread_size_) {
							threads_.erase(thread_id);
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
			finished_queue_.push(task);
			task_queue_.pop();
			--task_size_;
			if (!task_queue_.empty()) not_empty_.notify_all();
			not_full_.notify_all();
		}
		if (task != nullptr) {
			task();
		}
		++idle_thread_size_;
		last = std::chrono::high_resolution_clock::now();
	}
}

bool ThreadPool::running() const {
	return is_running_;
}

MSTL_END_NAMESPACE__
