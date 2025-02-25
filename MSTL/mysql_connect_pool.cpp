#include "mysql_connect_pool.h"
MSTL_BEGIN_NAMESPACE__

const std::string DEFAULT_IP = "127.0.0.1";
const unsigned int DEFAULT_PORT = 3306;

DBConnect::DBConnect() : mysql(nullptr), alive_time_(0) {
	mysql = mysql_init(nullptr);
}
DBConnect::~DBConnect() {
	close();
}
bool DBConnect::connect_to(const std::string& user, const std::string& password,
	const std::string& dbname, const std::string& ip, unsigned int port) {
	MYSQL* p = mysql_real_connect(mysql, ip.c_str(), user.c_str(), 
		password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}
bool DBConnect::exec(std::string sql) {
	if (mysql_query(mysql, sql.c_str())) {
		//SIMPLE_LOG("Execute Failed : " + sql);
		return false;
	}
	return true;
}

MYSQL_RES* DBConnect::SELECT(std::string table, std::string selected, std::string cond) {
	ss << "SELECT " << selected << " FROM " << table << " WHERE " << cond;
	if (mysql_query(mysql, ss.str().c_str())) {
		//SIMPLE_LOG("SELECT Failed : " + sql);
		return nullptr;
	}
	return mysql_use_result(mysql);
}
bool DBConnect::close() {
	if (mysql != nullptr) {
		mysql_close(mysql);
		return true;
	}
	return false;
}
void DBConnect::refresh_alive() {
	alive_time_ = clock();
}
clock_t DBConnect::get_alive() {
	return clock() - alive_time_;
}

DBConnectPool::DBConnectPool(const std::string& user, const std::string& password,
	const std::string& dbname, const std::string& ip, unsigned int port)
	: ip_(ip), port_(port), username_(user), passwd_(password), dbname_(dbname), init_size_(10),
	max_size_(1024), max_idle_time_(60), connect_timeout_(100) {
	for (size_t i = 0; i < init_size_; i++) {
		DBConnect* p = new DBConnect;
		p->connect_to(username_, passwd_, dbname_, ip_, port_);
		p->refresh_alive();
		connect_queue_.push(p);
	}
	std::thread produce(std::bind(&DBConnectPool::produce_connect_task, this));
	produce.detach();
	std::thread scanner(std::bind(&DBConnectPool::scanner_connect_task, this));
	scanner.detach();
}
std::shared_ptr<DBConnect> DBConnectPool::get_connect() {
	std::unique_lock<std::mutex> lock(queue_mtx_);
	while (connect_queue_.empty()) {
		if (cv_.wait_for(lock, std::chrono::milliseconds(connect_timeout_)) == std::cv_status::timeout) {
			if (connect_queue_.empty()) {
				SIMPLE_LOG("Get Connection Timeout");
				return nullptr;
			}
		}
	}
	std::shared_ptr<DBConnect> ptr(connect_queue_.front(), [&](DBConnect* pcon) {
		std::unique_lock<std::mutex> lock(queue_mtx_);
		pcon->refresh_alive();
		connect_queue_.push(pcon); 
		});
	connect_queue_.pop();
	cv_.notify_all();
	return ptr;
}
void DBConnectPool::produce_connect_task() {
	while (true) {
		std::unique_lock<std::mutex> lock(queue_mtx_);
		while (not connect_queue_.empty()) {
			cv_.wait(lock);
		}
		if (connect_queue_.size() < max_size_) {
			DBConnect* p = new DBConnect;
			p->connect_to(username_, passwd_, dbname_, ip_, port_);
			p->refresh_alive();
			connect_queue_.push(p);
		}
		cv_.notify_all();
	}
}
void DBConnectPool::scanner_connect_task() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(max_idle_time_));
		std::unique_lock<std::mutex> lock(queue_mtx_);
		while (connect_queue_.size() > init_size_) {
			DBConnect* ptr = connect_queue_.front();
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
