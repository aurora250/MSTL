#include "mysql_connect_pool.h"
MSTL_BEGIN_NAMESPACE__

const std::string DEFAULT_IP = "127.0.0.1";
const unsigned int DEFAULT_PORT = 3306;

Connect::Connect() : mysql(nullptr), alive_time_(0) {
	mysql = mysql_init(nullptr);
}
Connect::~Connect() {
	close();
}
bool Connect::connect_to(const std::string& user, const std::string& password,
	const std::string& dbname, const std::string& ip, unsigned int port) {
	MYSQL* p = mysql_real_connect(mysql, ip.c_str(), user.c_str(), 
		password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}
bool Connect::update(std::string sql) {
	if (mysql_query(mysql, sql.c_str())) {
		//SIMPLE_LOG("Update Failed : " + sql);
		return false;
	}
	return true;
}
MYSQL_RES* Connect::query(std::string sql) {
	if (mysql_query(mysql, sql.c_str())) {
		SIMPLE_LOG("Query Failed : " + sql);
		return nullptr;
	}
	return mysql_use_result(mysql);
}
bool Connect::close() {
	if (mysql != nullptr) {
		mysql_close(mysql);
		return true;
	}
	return false;
}
void Connect::refresh_alive() {
	alive_time_ = clock();
}
clock_t Connect::get_alive() {
	return clock() - alive_time_;
}

ConnectPool::ConnectPool(const std::string& user, const std::string& password,
	const std::string& dbname, const std::string& ip, unsigned int port)
	: ip_(ip), port_(port), username_(user), passwd_(password), dbname_(dbname), init_size_(10),
	max_size_(1024), max_idle_time_(60), connect_timeout_(100) {
	for (size_t i = 0; i < init_size_; i++) {
		Connect* p = new Connect;
		p->connect_to(username_, passwd_, dbname_, ip_, port_);
		p->refresh_alive();
		connect_queue_.push(p);
	}
	std::thread produce(std::bind(&ConnectPool::produce_connect_task, this));
	produce.detach();
	std::thread scanner(std::bind(&ConnectPool::scanner_connect_task, this));
	scanner.detach();
}
std::shared_ptr<Connect> ConnectPool::get_connect() {
	std::unique_lock<std::mutex> lock(queue_mtx_);
	while (connect_queue_.empty()) {
		if (cv_.wait_for(lock, std::chrono::milliseconds(connect_timeout_)) == std::cv_status::timeout) {
			if (connect_queue_.empty()) {
				SIMPLE_LOG("Get Connection Timeout");
				return nullptr;
			}
		}
	}
	std::shared_ptr<Connect> ptr(connect_queue_.front(), [&](Connect* pcon) {
		std::unique_lock<std::mutex> lock(queue_mtx_);
		pcon->refresh_alive();
		connect_queue_.push(pcon); 
		});
	connect_queue_.pop();
	cv_.notify_all();
	return ptr;
}
void ConnectPool::produce_connect_task() {
	while (true) {
		std::unique_lock<std::mutex> lock(queue_mtx_);
		while (not connect_queue_.empty()) {
			cv_.wait(lock);
		}
		if (connect_queue_.size() < max_size_) {
			Connect* p = new Connect;
			p->connect_to(username_, passwd_, dbname_, ip_, port_);
			p->refresh_alive();
			connect_queue_.push(p);
		}
		cv_.notify_all();
	}
}
void ConnectPool::scanner_connect_task() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(max_idle_time_));
		std::unique_lock<std::mutex> lock(queue_mtx_);
		while (connect_queue_.size() > init_size_) {
			Connect* ptr = connect_queue_.front();
			if (ptr->get_alive() >= (max_idle_time_ * 1000)) {
				connect_queue_.pop();
				delete ptr;
			}
			else 
				break;
		}
	}
}

MSTL_END_NAMESPACE__
