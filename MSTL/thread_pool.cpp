#include "thread_pool.h"
MSTL_BEGIN_NAMESPACE__
const size_t TASK_MAX_THRESHHOLD = INT32_MAX;
const size_t THREAD_MAX_THRESHHOLD = std::thread::hardware_concurrency();
const size_t THREAD_MAX_IDLE_SECONDS = 60;

int __thread::generateId_ = 0;

__thread::__thread(ThreadFunc func)
	: func_(func),
	threadId_(generateId_++) {
}
__thread::~__thread() {}

void __thread::start() {
	std::thread t(func_, threadId_);
	t.detach();
}
int __thread::getId() const {
	return threadId_;
}

ThreadPool::ThreadPool()
	: initThreadSize_(0),
	threadSizeThreshHold_(THREAD_MAX_THRESHHOLD),
	taskSize_(0),
	idleThreadSize_(0),
	taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD),
	poolMode_(POOL_MODE::MODE_FIXED),
	isRunning_(false) {
}

ThreadPool::~ThreadPool() {
	isRunning_ = false;
	std::unique_lock<std::mutex> lock(taskQueMtx_);
	notEmpty_.notify_all();   // unlock and call threadFunc to exit
	exitCond_.wait(lock, [&]()->bool { return threads_.empty(); });
}

void ThreadPool::set_mode(POOL_MODE mode) {
	if (checkRunningState()) return;
	poolMode_ = mode;
}
void ThreadPool::set_taskque_max_thresh_hold(size_t threshHold) {
	if (checkRunningState()) return;
	taskQueMaxThreshHold_ = threshHold;
}
void ThreadPool::set_thread_size_thresh_hold(size_t threshHold) {
	if (checkRunningState() || poolMode_ == POOL_MODE::MODE_FIXED) return;
	threadSizeThreshHold_ = threshHold > THREAD_MAX_THRESHHOLD ? THREAD_MAX_THRESHHOLD : threshHold;
}
size_t ThreadPool::max_thread_size() {
	return THREAD_MAX_THRESHHOLD;
}

void ThreadPool::start(unsigned int initThreadSize) {
	isRunning_ = true;
	initThreadSize_ = initThreadSize;
	for (size_t i = 0; i < initThreadSize_; i++) {
		auto ptr = std::make_shared<__thread>(std::bind(&ThreadPool::thread_function, this, std::placeholders::_1));
		threads_.emplace(ptr->getId(), std::move(ptr));
	}
	for (int i = 0; i < initThreadSize_; i++) {
		threads_[i]->start();
		idleThreadSize_++;
	}
}

void ThreadPool::thread_function(int threadid) {
	auto last = std::chrono::high_resolution_clock().now();

	for (;;) {
		Task task;
		{
			std::unique_lock<std::mutex> lock(taskQueMtx_);
			aux_output_ << "thread id (" << std::this_thread::get_id() << ") try to get Task..." << endl;
			sout << aux_output_;
			while (taskQueue_.empty()) {
				if (not isRunning_) {  // unlock deadlock after finish every mission 
					threads_.erase(threadid);
					aux_output_ << "thread (" << std::this_thread::get_id() << ") exit" << endl;
					sout << aux_output_;
					exitCond_.notify_all();
					return;
				}
				if (poolMode_ == POOL_MODE::MODE_CACHED) {
					if (std::cv_status::timeout == notEmpty_.wait_for(lock, std::chrono::seconds(1))) {
						auto now = std::chrono::high_resolution_clock().now();
						auto sub = std::chrono::duration_cast<std::chrono::seconds>(now - last);
						if (sub.count() >= THREAD_MAX_IDLE_SECONDS && threads_.size() > initThreadSize_) {
							threads_.erase(threadid);
							idleThreadSize_--;
							aux_output_ << "thread (" << std::this_thread::get_id() << ") exit" << endl;
							sout << aux_output_;
							return;
						}
					}
				}
				else {
					notEmpty_.wait(lock);
				}
			} // free lock

			idleThreadSize_--;
			aux_output_ << "thread id (" << std::this_thread::get_id() << ") get Task!" << endl;
			sout << aux_output_;
			task = taskQueue_.front();
			taskQueue_.pop();
			taskSize_--;
			if (not taskQueue_.empty()) notEmpty_.notify_all();
			notFull_.notify_all();
		}
		if (task != nullptr) {
			task();
		}
		idleThreadSize_++;
		last = std::chrono::high_resolution_clock().now();
	}
}

bool ThreadPool::checkRunningState() const {
	return isRunning_;
}
MSTL_END_NAMESPACE__
