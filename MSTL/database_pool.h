#ifndef MSTL_DATABASE_POOL_H__
#define MSTL_DATABASE_POOL_H__
#include "list.hpp"
#ifdef MSTL_SUPPORT_MYSQL__
#include <mysql.h>
#include "undef_cmacro.hpp"
#include <mutex>
#include <thread>
#include <condition_variable>
#include "string.hpp"
#include "queue.hpp"
#include "unordered_map.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_FINAL_CLASS(DatabaseError, LinkError, "Database Operations Failed.")
MSTL_ERROR_BUILD_FINAL_CLASS(DatabaseTypeCastError, TypeCastError, "Database Type Cast Failed.")

struct database_settings final {
    static _MSTL string username; // default: root
    static _MSTL string password; // set it as you wish
    static _MSTL string dbname; // set it as you wish
    static _MSTL string ip; // default: 127.0.0.1
    static _MSTL uint16_t port; // default: 3306
    static _MSTL string character_set; // default: utf8mb4
};


struct database_result {
public:
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using self				= database_result;

private:
    MYSQL_RES* result = nullptr;
    size_type rows = 0;
    size_type columns = 0;
    MYSQL_ROW cursor = nullptr;
    list<string_view>* column_name_ = new list<string_view>;
    list<enum_field_types>* column_types_ = new list<enum_field_types>;

public:
    database_result() noexcept = default;

    database_result(MYSQL_RES* result) noexcept
        : result(result), rows(mysql_num_rows(result)), columns(mysql_num_fields(result)) {
        MYSQL_FIELD* field;
        while ((field = mysql_fetch_field(result))) {
            column_name_->push_back(field->name);
            column_types_->push_back(field->type);
        }
    }

    ~database_result() {
        mysql_free_result(result);
        delete column_name_;
        delete column_types_;
    }

    MSTL_NODISCARD bool empty() const noexcept { return result == nullptr; }

    MSTL_NODISCARD size_type row_count() const noexcept { return rows; }
    MSTL_NODISCARD size_type column_count() const noexcept { return columns; }

    MSTL_NODISCARD const list<string_view>& column_names() const noexcept { return *column_name_; }
    MSTL_NODISCARD const list<enum_field_types>& column_types() const noexcept { return *column_types_; }

    bool next() noexcept {
        if (!empty()) {
            cursor = mysql_fetch_row(result);
            return cursor != nullptr;
        }
        return false;
    }

    MSTL_NODISCARD string_view at(const size_type n) const noexcept {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        return cursor[n];
    }

    MSTL_NODISCARD int8_t at_int8(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_TINY)
            Exception(DatabaseTypeCastError("database type cast to int8 mismatch"));
        return to_int8(cursor[n]);
    }

    MSTL_NODISCARD int16_t at_int16(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY))
            Exception(DatabaseTypeCastError("database type cast to int16 mismatch"));
        return to_int16(cursor[n]);
    }

    MSTL_NODISCARD int32_t at_int32(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_LONG || type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY))
            Exception(DatabaseTypeCastError("database type cast to int32 mismatch"));
        return to_int32(cursor[n]);
    }

    MSTL_NODISCARD int64_t at_int64(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_LONGLONG || type == MYSQL_TYPE_LONG ||
            type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY))
            Exception(DatabaseTypeCastError("database type cast to int64 mismatch"));
        return to_int64(cursor[n]);
    }

    MSTL_NODISCARD float32_t at_float32(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_FLOAT || type == MYSQL_TYPE_LONG
            || type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY))
            Exception(DatabaseTypeCastError("database type cast to float32 mismatch"));
        return to_float32(cursor[n]);
    }

    MSTL_NODISCARD float64_t at_float64(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_DOUBLE || type == MYSQL_TYPE_FLOAT || type == MYSQL_TYPE_LONGLONG
            || type == MYSQL_TYPE_LONG || type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY))
            Exception(DatabaseTypeCastError("database type cast to float64 mismatch"));
        return to_float64(cursor[n]);
    }

    MSTL_NODISCARD decimal_t at_decimal(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_DECIMAL || type == MYSQL_TYPE_NEWDECIMAL || type == MYSQL_TYPE_DOUBLE ||
            type == MYSQL_TYPE_FLOAT || type == MYSQL_TYPE_LONGLONG || type == MYSQL_TYPE_LONG ||
            type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY))
            Exception(DatabaseTypeCastError("database type cast to decimal mismatch"));
        return to_decimal(cursor[n]);
    }

    MSTL_NODISCARD vector<char> at_blob(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_BLOB)
            Exception(DatabaseTypeCastError("database type cast to blob mismatch"));
        return {cursor[n], cursor[n] + mysql_fetch_lengths(result)[n]};
    }

    MSTL_NODISCARD string_view at_date(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_DATE)
            Exception(DatabaseTypeCastError("database type cast to date mismatch"));
        return at(n);
    }

    MSTL_NODISCARD string_view at_datetime(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_DATETIME)
            Exception(DatabaseTypeCastError("database type cast to datetime mismatch"));
        return at(n);
    }

    MSTL_NODISCARD string_view at_timestamp(const size_type n) const {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_TIMESTAMP)
            Exception(DatabaseTypeCastError("database type cast to timestamp mismatch"));
        return at(n);
    }

    MSTL_NODISCARD string_view at_string(const size_type n) const noexcept {
        return at(n);
    }

    MSTL_NODISCARD string_view at_enum(const size_type n) const {
        return at(n);
    }
};


// database connection is based on MySql connection.
class database_connect {
public:
    using clock_type = std::clock_t;

private:
    MYSQL* mysql = nullptr;
    clock_type alive_time_ = 0;

public:
    database_connect() noexcept {
        mysql = mysql_init(nullptr);
    }
    ~database_connect() noexcept {
        close();
    }

    MSTL_NODISCARD bool connect_to(
        const _MSTL string& user, const _MSTL string& password,
        const _MSTL string& dbname, const _MSTL string& ip,
        uint32_t port, const _MSTL string& character_set) const noexcept {
        const MYSQL* p = mysql_real_connect(mysql, ip.c_str(), user.c_str(),
            password.c_str(), dbname.c_str(), port, nullptr, 0);
        if (p == nullptr) return false;

        return this->set_character_set(character_set);
    }

    MSTL_NODISCARD bool connect_to() const noexcept {
        return connect_to(
            database_settings::username,
            database_settings::password,
            database_settings::dbname,
            database_settings::ip,
            database_settings::port,
            database_settings::character_set
        );
    }

    MSTL_NODISCARD bool set_character_set(
        const _MSTL string& encoding) const noexcept {
        if (connected()) {
            if (mysql_set_character_set(mysql, encoding.data())) {
                return false;
            }
            return true;
        }
        return false;
    }

    MSTL_NODISCARD const char* get_character_set() const noexcept {
        return mysql_character_set_name(mysql);
    }

    MSTL_NODISCARD bool set_options(
        const mysql_option option, const _MSTL string& str) const noexcept {
        if (connected()) {
            if (mysql_options(mysql, option, str.c_str())) {
                return false;
            }
            return true;
        }
        return false;
    }

    MSTL_NODISCARD const char* get_error() const noexcept {
        return mysql_error(mysql);
    }
    MSTL_NODISCARD uint32_t get_errno() const noexcept {
        return mysql_errno(mysql);
    }

    MSTL_NODISCARD bool update(const _MSTL string& sql) const noexcept {
        if (mysql_query(mysql, sql.c_str())) {
            return false;
        }
        return true;
    }

    MSTL_NODISCARD database_result query(const _MSTL string& sql) const noexcept {
        if (mysql_query(mysql, sql.c_str())) {
            return {};
        }
        return mysql_store_result(mysql);
    }

    MSTL_NODISCARD bool connected() const noexcept {
        return mysql != nullptr;
    }
    MSTL_NODISCARD bool is_valid() const noexcept {
        return mysql_ping(mysql) == 0;
    }

    void close() const noexcept {
        if (connected()) {
            mysql_close(mysql);
        }
    }

    void refresh_alive() noexcept {
        alive_time_ = std::clock();
    }
    MSTL_NODISCARD clock_type get_alive() const noexcept {
        return std::clock() - alive_time_;
    }

    MSTL_NODISCARD bool reset_connect() {
        if (connected()) {
            mysql_close(mysql);
            mysql = mysql_init(nullptr);
            return connect_to();
        }
        return false;
    }
};


class database_pool {
private:
    size_t init_size_;
    size_t max_size_;
    size_t max_idle_time_;  // s
    size_t connect_timeout_;  // ms

    _MSTL queue<database_connect*> connect_queue_;
    std::mutex queue_mtx_;
    std::condition_variable cv_;

    friend database_pool& get_instance_database_pool();

private:
    void produce_connect_task() {
        while (true) {
            std::unique_lock<std::mutex> lock(queue_mtx_);
            while (!connect_queue_.empty()) {
                cv_.wait(lock);
            }
            if (connect_queue_.size() < max_size_) {
                auto* p = new database_connect();
                if (p->connect_to()) {
                    p->refresh_alive();
                    connect_queue_.push(p);
                }
                else {
                    delete p;
                }
            }
            cv_.notify_all();
        }
    }

    void scanner_connect_task() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(max_idle_time_));
            std::unique_lock<std::mutex> lock(queue_mtx_);

            while (connect_queue_.size() > init_size_) {
                database_connect* ptr = connect_queue_.front();
                if (ptr->get_alive() >= max_idle_time_ * 1000) {
                    connect_queue_.pop();
                    delete ptr;
                }
                else
                    break;
            }
        }
    }

    database_pool()
        : init_size_(10), max_size_(1024), max_idle_time_(60), connect_timeout_(100) {
        for (size_t i = 0; i < init_size_; i++) {
            auto* p = new database_connect();
            if (p->connect_to()) {
                p->refresh_alive();
                connect_queue_.push(p);
            }
            else {
                delete p;
                --i;
            }
        }
        std::thread produce([this] { produce_connect_task(); });
        produce.detach();
        std::thread scanner([this] { scanner_connect_task(); });
        scanner.detach();
    }

    ~database_pool() {
        while (!connect_queue_.empty()) {
            delete connect_queue_.front();
            connect_queue_.pop();
        }
    }

public:
    database_pool(const database_pool&) = delete;
    database_pool& operator=(const database_pool&) = delete;
    database_pool(database_pool&&) = delete;
    database_pool& operator=(database_pool&&) = delete;

    _MSTL shared_ptr<database_connect> get_connect() {
        std::unique_lock<std::mutex> lock(queue_mtx_);

        while (connect_queue_.empty()) {
            if (cv_.wait_for(lock,
                std::chrono::milliseconds(connect_timeout_)) == std::cv_status::timeout) {
                if (connect_queue_.empty()) {
                    if (connect_queue_.size() < max_size_) {
                        auto* new_conn = new database_connect();
                        if (new_conn->connect_to()) {
                            new_conn->refresh_alive();
                            connect_queue_.push(new_conn);
                            continue; // 重试获取
                        }
                        delete new_conn;
                    }
                    return nullptr;
                }
            }
        }

        database_connect* raw_conn = connect_queue_.front();
        connect_queue_.pop();

        if (!raw_conn->is_valid()) {
            try {
                if (!raw_conn->reset_connect()) {
                    delete raw_conn;
                    raw_conn = new database_connect();
                    if (!raw_conn->connect_to()) {
                        delete raw_conn;
                        cv_.notify_all();
                        return nullptr;
                    }
                }
            }
            catch (...) {
                delete raw_conn;
                cv_.notify_all();
                return nullptr;
            }
        }

        auto conn_ptr = _MSTL shared_ptr<database_connect>(raw_conn,
            [this](database_connect* p) {
                std::unique_lock<std::mutex> lock(queue_mtx_);
                if (p->is_valid()) {
                    p->refresh_alive();
                    connect_queue_.push(p);
                }
                else {
                    delete p;
                }
                cv_.notify_all();
            }
        );

        cv_.notify_all();
        return conn_ptr;
    }
};

inline database_pool& get_instance_database_pool() {
    static database_pool database_pool;
    return database_pool;
}

MSTL_END_NAMESPACE__
#endif // MSTL_SUPPORT_MYSQL__
#endif // MSTL_DATABASE_POOL_H__
