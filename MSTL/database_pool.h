#ifndef MSTL_DATABASE_POOL_H__
#define MSTL_DATABASE_POOL_H__
#ifdef MSTL_SUPPORT_MYSQL__
#include <mysql.h>
#include "undef_cmacro.hpp"
#include <ctime>
#include <mutex>
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
};


struct database_result_row {
public:
    using size_type = size_t;

private:
    MYSQL_ROW cur = nullptr;
    size_t column_length = 0;

public:
    database_result_row() noexcept = default;
    database_result_row(MYSQL_ROW cur, size_t column_length) noexcept;
    ~database_result_row() = default;

    MSTL_NODISCARD string_view operator [](size_t n) const noexcept;
    MSTL_NODISCARD string_view at(size_type n) const noexcept;
    MSTL_NODISCARD size_type size() const noexcept;
    MSTL_NODISCARD bool empty() const noexcept;
};

struct database_result_iterator {
public:
    using iterator_category = forward_iterator_tag;
    using value_type        = database_result_row;
    using difference_type   = ptrdiff_t;
    using pointer           = const database_result_row*;
    using reference         = const database_result_row;

    using self = database_result_iterator;

private:
    MYSQL_RES* result = nullptr;
    MYSQL_ROW cur = nullptr;
    size_t column_length = 0;

public:
    database_result_iterator() noexcept = default;
    database_result_iterator(MYSQL_RES* result, MYSQL_ROW cur, size_t column) noexcept;
    database_result_iterator(const database_result_iterator&) noexcept = default;
    database_result_iterator& operator =(const database_result_iterator&) noexcept = default;
    database_result_iterator(database_result_iterator&&) noexcept = default;
    database_result_iterator& operator =(database_result_iterator&&) noexcept = default;
    ~database_result_iterator() = default;

    MSTL_NODISCARD reference operator *() const noexcept;
    self& operator ++() noexcept;
    self operator ++(int) noexcept;
    MSTL_NODISCARD bool operator ==(const self& rh) const noexcept;
    MSTL_NODISCARD bool operator !=(const self& rh) const noexcept;
};

struct database_result {
public:
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using self				= database_result;

    using iterator			= database_result_iterator;
    using const_iterator	= database_result_iterator;

private:
    MYSQL_RES* result = nullptr;
    size_type rows = 0;
    size_type columns = 0;
    MYSQL_ROW cur = nullptr;

public:
    database_result() noexcept = default;
    database_result(MYSQL_RES* result) noexcept;
    ~database_result();

    MSTL_NODISCARD iterator begin() noexcept;
    MSTL_NODISCARD iterator end() noexcept;
    MSTL_NODISCARD const_iterator cbegin() const noexcept;
    MSTL_NODISCARD const_iterator cend() const noexcept;

    MSTL_NODISCARD bool empty() const noexcept;
    MSTL_NODISCARD size_type row() const noexcept;
    MSTL_NODISCARD size_type column() const noexcept;
    MSTL_NODISCARD MYSQL_RES* data() const noexcept;
};

// database connection is based on MySql connection.
class database_single_connect {
public:
    using clock_type = std::clock_t;

private:
    MYSQL* mysql;
    clock_type alive_time_;

public:
    database_single_connect() noexcept;
    ~database_single_connect() noexcept;
    bool connect_to(const _MSTL string& user, const _MSTL string& password,
        const _MSTL string& dbname, const _MSTL string& ip, uint32_t port) noexcept;
    bool update(const _MSTL string& sql) const noexcept;
    database_result query(const _MSTL string& sql) const noexcept;
    void close() const noexcept;
    void refresh_alive() noexcept;
    MSTL_NODISCARD clock_type get_alive() const noexcept;
};


class database_pool {
private:
    _MSTL string ip_;
    uint32_t port_;
    _MSTL string username_;
    _MSTL string passwd_;
    _MSTL string dbname_;
    size_t init_size_;
    size_t max_size_;
    size_t max_idle_time_;  // s
    size_t connect_timeout_;  // ms

    std::queue<database_single_connect*> connect_queue_;
    std::mutex queue_mtx_;
    std::condition_variable cv_;

    friend database_pool& get_instance_database_pool();

private:
    void produce_connect_task();
    void scanner_connect_task();

    database_pool(_MSTL string user, _MSTL string password,
        _MSTL string dbname, _MSTL string ip, unsigned int port);
    ~database_pool();

public:
    database_pool(const database_pool&) = delete;
    database_pool& operator=(const database_pool&) = delete;
    database_pool(database_pool&&) = delete;
    database_pool& operator=(database_pool&&) = delete;

    _MSTL shared_ptr<database_single_connect> get_connect();
};

database_pool& get_instance_database_pool();

MSTL_END_NAMESPACE__
#endif // MSTL_SUPPORT_MYSQL__
#endif // MSTL_DATABASE_POOL_H__
