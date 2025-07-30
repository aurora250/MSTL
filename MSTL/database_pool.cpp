#include "database_pool.h"
#ifdef MSTL_SUPPORT_MYSQL__
#include <thread>
MSTL_BEGIN_NAMESPACE__

_MSTL string database_settings::username = "root";
_MSTL string database_settings::password = "147258hu";
_MSTL string database_settings::dbname = "students";
_MSTL string database_settings::ip = "127.0.0.1";
_MSTL uint16_t database_settings::port = 3306;


database_result_row::database_result_row(MYSQL_ROW cur, const size_t column_length) noexcept
: cur(cur), column_length(column_length) {}

MSTL_NODISCARD string_view database_result_row::operator [](const size_t n) const noexcept {
    return at(n);
}
MSTL_NODISCARD string_view database_result_row::at(const size_type n) const noexcept {
    MSTL_DEBUG_VERIFY(cur, "database_result_row_value can`t dereference nullptr.")
    MSTL_DEBUG_VERIFY(column_length > n, "database_result_row_value out of ranges.")
    return cur[n];
}

MSTL_NODISCARD database_result_row::size_type database_result_row::size() const noexcept {
    return column_length;
}
MSTL_NODISCARD bool database_result_row::empty() const noexcept {
    return cur == nullptr;
}


database_result_iterator::database_result_iterator(MYSQL_RES* result, MYSQL_ROW cur, const size_t column) noexcept
    : result(result), cur(cur), column_length(column) {}
MSTL_NODISCARD database_result_iterator::reference database_result_iterator::operator *() const noexcept {
    MSTL_DEBUG_VERIFY(cur, "database_result_iterator can`t dereference nullptr.")
    return {cur, column_length};
}

database_result_iterator::self& database_result_iterator::operator ++() noexcept {
    if (cur != nullptr) {
        cur = mysql_fetch_row(result);
    }
    return *this;
}
database_result_iterator::self database_result_iterator::operator ++(int) noexcept {
    self cur = *this;
    ++*this;
    return cur;
}

MSTL_NODISCARD bool database_result_iterator::operator ==(const self& rh) const noexcept {
    MSTL_DEBUG_VERIFY(result == rh.result || rh.result == nullptr || result == nullptr,
        __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(database_result_iterator));
    return cur == rh.cur;
}
MSTL_NODISCARD bool database_result_iterator::operator !=(const self& rh) const noexcept {
    return !(*this == rh);
}


database_result::database_result(MYSQL_RES* result) noexcept
    : result(result), rows(mysql_num_rows(result)), columns(mysql_num_fields(result)) {}

database_result::~database_result() {
    mysql_free_result(result);
}

MSTL_NODISCARD database_result::iterator database_result::begin() noexcept {
    return {result, mysql_fetch_row(result), column()};
}
MSTL_NODISCARD database_result::iterator database_result::end() noexcept {
    return {};
}
MSTL_NODISCARD database_result::const_iterator database_result::cbegin() const noexcept {
    return {result, mysql_fetch_row(result), column()};
}
MSTL_NODISCARD database_result::const_iterator database_result::cend() const noexcept {
    return {};
}

MSTL_NODISCARD bool database_result::empty() const noexcept { return result == nullptr; }
MSTL_NODISCARD database_result::size_type database_result::row() const noexcept { return rows; }
MSTL_NODISCARD database_result::size_type database_result::column() const noexcept { return columns; }
MSTL_NODISCARD MYSQL_RES* database_result::data() const noexcept { return result; }


database_single_connect::database_single_connect() noexcept : mysql(nullptr), alive_time_(0) {
    mysql = mysql_init(nullptr);
}
database_single_connect::~database_single_connect() noexcept {
    close();
}
bool database_single_connect::connect_to(const _MSTL string& user, const _MSTL string& password,
    const _MSTL string& dbname, const _MSTL string& ip, const unsigned int port) noexcept {
 	MYSQL* p = mysql_real_connect(mysql, ip.c_str(), user.c_str(),
 		password.c_str(), dbname.c_str(), port, nullptr, 0);
 	return p != nullptr;
}
bool database_single_connect::update(const _MSTL string& sql) const noexcept {
 	if (mysql_query(mysql, sql.c_str())) {
 		return false;
 	}
 	return true;
}
database_result database_single_connect::query(const _MSTL string& sql) const noexcept {
    if (mysql_query(mysql, sql.c_str())) {
 		return {};
 	}
 	return mysql_store_result(mysql);
}
void database_single_connect::close() const noexcept {
 	if (mysql != nullptr) {
 	    mysql_close(mysql);
 	}
}
void database_single_connect::refresh_alive() noexcept {
 	alive_time_ = std::clock();
}
database_single_connect::clock_type database_single_connect::get_alive() const noexcept {
 	return std::clock() - alive_time_;
}

 
database_pool::database_pool(_MSTL string user, _MSTL string password,
 	_MSTL string dbname, _MSTL string ip, const uint32_t port)
 	: ip_(_MSTL move(ip)), port_(port), username_(_MSTL move(user)),
    passwd_(_MSTL move(password)), dbname_(_MSTL move(dbname)), init_size_(10),
 	max_size_(1024), max_idle_time_(60), connect_timeout_(100) {
 	for (size_t i = 0; i < init_size_; i++) {
 		auto* p = new database_single_connect();
 		p->connect_to(username_, passwd_, dbname_, ip_, port_);
 		p->refresh_alive();
 		connect_queue_.push(p);
 	}
 	std::thread produce([this] { produce_connect_task(); });
 	produce.detach();
 	std::thread scanner([this] { scanner_connect_task(); });
 	scanner.detach();
}

database_pool::~database_pool(){
    while (!connect_queue_.empty()) {
        delete connect_queue_.front();
        connect_queue_.pop();
    }
}

_MSTL shared_ptr<database_single_connect> database_pool::get_connect() {
 	std::unique_lock<std::mutex> lock(queue_mtx_);
 	while (connect_queue_.empty()) {
 		if (cv_.wait_for(lock, std::chrono::milliseconds(connect_timeout_)) == std::cv_status::timeout) {
 			if (connect_queue_.empty()) {
 				return nullptr;
 			}
 		}
 	}
 	_MSTL shared_ptr<database_single_connect> ptr(connect_queue_.front(),
 		[this](database_single_connect* pcon) {
 			std::unique_lock<std::mutex> lock1(queue_mtx_);
 			pcon->refresh_alive();
 			connect_queue_.push(pcon);
 		}
 	);
 	connect_queue_.pop();
 	cv_.notify_all();
 	return ptr;
}

void database_pool::produce_connect_task() {
 	while (true) {
 		std::unique_lock<std::mutex> lock(queue_mtx_);
 		while (!connect_queue_.empty()) {
 			cv_.wait(lock);
 		}
 		if (connect_queue_.size() < max_size_) {
 			auto* p = new database_single_connect();
 			p->connect_to(username_, passwd_, dbname_, ip_, port_);
 			p->refresh_alive();
 			connect_queue_.push(p);
 		}
 		cv_.notify_all();
 	}
}

void database_pool::scanner_connect_task() {
 	while (true) {
 		std::this_thread::sleep_for(std::chrono::seconds(max_idle_time_));
 		std::unique_lock<std::mutex> lock(queue_mtx_);
 		while (connect_queue_.size() > init_size_) {
 			database_single_connect* ptr = connect_queue_.front();
 			if (ptr->get_alive() >= max_idle_time_ * 1000) {
 				connect_queue_.pop();
 				delete ptr;
 			}
 			else
 				break;
 		}
 	}
}


database_pool& get_instance_database_pool() {
    static database_pool database_pool(
        database_settings::username,
        database_settings::password,
        database_settings::dbname,
        database_settings::ip,
        database_settings::port
        );
    return database_pool;
}

MSTL_END_NAMESPACE__
#endif // MSTL_SUPPORT_MYSQL__
