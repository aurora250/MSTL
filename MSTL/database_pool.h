#ifndef MSTL_DATABASE_POOL_H__
#define MSTL_DATABASE_POOL_H__
#ifdef MSTL_SUPPORT_MYSQL__
#include <mysql.h>
#include "undef_cmacro.hpp"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include "string.hpp"
#include "queue.hpp"
#include "unordered_map.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_FINAL_CLASS(DatabaseError, LinkError, "Database Operations Failed.")

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

public:
    database_result() noexcept = default;

    database_result(MYSQL_RES* result) noexcept
        : result(result), rows(mysql_num_rows(result)), columns(mysql_num_fields(result)) {}

    ~database_result() {
        mysql_free_result(result);
    }

    MSTL_NODISCARD bool empty() const noexcept { return result == nullptr; }
    MSTL_NODISCARD size_type row() const noexcept { return rows; }
    MSTL_NODISCARD size_type column() const noexcept { return columns; }
    MSTL_NODISCARD MYSQL_RES* data() const noexcept { return result; }

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

    bool connect_to(
        const _MSTL string& user, const _MSTL string& password,
        const _MSTL string& dbname, const _MSTL string& ip,
        uint32_t port, const _MSTL string& character_set) const noexcept {
        const MYSQL* p = mysql_real_connect(mysql, ip.c_str(), user.c_str(),
            password.c_str(), dbname.c_str(), port, nullptr, 0);
        if (p == nullptr) return false;

        return this->set_character_set(character_set);
    }

    MSTL_NODISCARD bool set_character_set(const _MSTL string& encoding) const noexcept {
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

    MSTL_NODISCARD bool set_options(const mysql_option option, const _MSTL string& str) const noexcept {
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

    bool update(const _MSTL string& sql) const noexcept {
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
};


class database_pool {
private:
    size_t init_size_;
    size_t max_size_;
    size_t max_idle_time_;  // s
    size_t connect_timeout_;  // ms

    std::queue<database_connect*> connect_queue_;
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
                if (p->connect_to(
                    database_settings::username,
                    database_settings::password,
                    database_settings::dbname,
                    database_settings::ip,
                    database_settings::port,
                    database_settings::character_set
                    )) {
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
            if (p->connect_to(
                database_settings::username,
                database_settings::password,
                database_settings::dbname,
                database_settings::ip,
                database_settings::port,
                database_settings::character_set)) {
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
            if (cv_.wait_for(lock, std::chrono::milliseconds(connect_timeout_)) == std::cv_status::timeout) {
                if (connect_queue_.empty()) {
                    return nullptr;
                }
            }
        }
        _MSTL shared_ptr<database_connect> ptr(connect_queue_.front(),
            [this](database_connect* pcon) {
                std::unique_lock<std::mutex> lock1(queue_mtx_);
                pcon->refresh_alive();
                connect_queue_.push(pcon);
            }
        );
        connect_queue_.pop();
        cv_.notify_all();
        return ptr;
    }
};

inline database_pool& get_instance_database_pool() {
    static database_pool database_pool;
    return database_pool;
}

MSTL_END_NAMESPACE__
#endif // MSTL_SUPPORT_MYSQL__
#endif // MSTL_DATABASE_POOL_H__
