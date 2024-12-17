#include "thread_pool.h"
MSTL_BEGIN_NAMESPACE__
const size_t TASK_MAX_THRESHHOLD = INT32_MAX;
const size_t THREAD_MAX_THRESHHOLD = std::thread::hardware_concurrency();
const size_t THREAD_MAX_IDLE_SECONDS = 60;

int Thread::generateId_ = 0;

Thread::Thread(ThreadFunc func)
	: func_(func),
	threadId_(generateId_++) {
}
Thread::~Thread() {}

void Thread::start() {
	std::thread t(func_, threadId_);
	t.detach();
}
int Thread::getId() const {
	return threadId_;
}

ThreadPool::ThreadPool()
	: initThreadSize_(0),
	threadSizeThreshHold_(THREAD_MAX_THRESHHOLD),
	taskSize_(0),
	idleThreadSize_(0),
	taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD),
	poolMode_(PoolMode::MODE_FIXED),
	isRunning_(false) {
}

ThreadPool::~ThreadPool() {
	isRunning_ = false;
	std::unique_lock<std::mutex> lock(taskQueMtx_);
	notEmpty_.notify_all();   // unlock and call threadFunc to exit
	exitCond_.wait(lock, [&]()->bool { return threads_.empty(); });
}

void ThreadPool::setInitThreadSize(size_t size) {
	if (checkRunningState()) return;
	initThreadSize_ = size;
}
void ThreadPool::setMode(PoolMode mode) {
	if (checkRunningState()) return;
	poolMode_ = mode;
}
void ThreadPool::setTaskQueMaxThreshHold(size_t threshHold) {
	if (checkRunningState()) return;
	taskQueMaxThreshHold_ = threshHold;
}
void ThreadPool::setThreadSizeThreshHold(size_t threshHold) {
	if (checkRunningState() || poolMode_ == PoolMode::MODE_FIXED) return;
	threadSizeThreshHold_ = threshHold > THREAD_MAX_THRESHHOLD ? THREAD_MAX_THRESHHOLD : threshHold;
}
size_t ThreadPool::maxThreadSize() {
	return THREAD_MAX_THRESHHOLD;
}

void ThreadPool::start(unsigned int initThreadSize) {
	isRunning_ = true;
	initThreadSize_ = initThreadSize;
	for (size_t i = 0; i < initThreadSize_; i++) {
		auto ptr = std::make_shared<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
		threads_.emplace(ptr->getId(), std::move(ptr));
	}
	for (int i = 0; i < initThreadSize_; i++) {
		threads_[i]->start();
		idleThreadSize_++;
	}
}

void ThreadPool::threadFunc(int threadid) {
	auto last = std::chrono::high_resolution_clock().now();

	for (;;) {
		Task task;
		{
			std::unique_lock<std::mutex> lock(taskQueMtx_);
			std::cout << "thread id (" << std::this_thread::get_id() << ") try to get Task..." << std::endl;
			while (taskQueue_.empty()) {
				if (not isRunning_) {  // unlock deadlock after finish every mission 
					threads_.erase(threadid);
					std::cout << "thread (" << std::this_thread::get_id() << ") exit" << std::endl;
					exitCond_.notify_all();
					return;
				}
				if (poolMode_ == PoolMode::MODE_CACHED) {
					if (std::cv_status::timeout == notEmpty_.wait_for(lock, std::chrono::seconds(1))) {
						auto now = std::chrono::high_resolution_clock().now();
						auto sub = std::chrono::duration_cast<std::chrono::seconds>(now - last);
						if (sub.count() >= THREAD_MAX_IDLE_SECONDS && threads_.size() > initThreadSize_) {
							threads_.erase(threadid);
							idleThreadSize_--;
							std::cout << "thread (" << std::this_thread::get_id() << ") exit" << std::endl;
							return;
						}
					}
				}
				else {
					notEmpty_.wait(lock);
				}
			} // free lock

			idleThreadSize_--;
			std::cout << "thread id (" << std::this_thread::get_id() << ") get Task!" << std::endl;
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
