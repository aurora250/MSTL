#ifndef MSTL_DATABASE_POOL_HPP__
#define MSTL_DATABASE_POOL_HPP__
#ifdef MSTL_SUPPORT_DB__
#ifdef MSTL_SUPPORT_MYSQL__
#include <mysql.h>
#endif
#ifdef MSTL_SUPPORT_SQLITE3__
#include <sqlite3.h>
#endif
#ifdef MSTL_SUPPORT_REDIS__
#ifdef MSTL_PLATFORM_WINDOWS__
#include <hiredis.h>
#elif defined(MSTL_PLATFORM_LINUX__)
#include <hiredis/hiredis.h>
#endif
#endif
#include "MSTL/core/undef_cmacro.hpp"
#include <mutex>
#include <thread>
#include <condition_variable>
#include "MSTL/core/queue.hpp"
#include "MSTL/core/list.hpp"
#include "MSTL/core/datetime.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_FINAL_CLASS(DatabaseError, LinkError, "Database Operations Failed.")
MSTL_ERROR_BUILD_FINAL_CLASS(DatabaseTypeCastError, TypeCastError, "Database Type Cast Failed.")

enum class DB_TYPE {
    MYSQL = 1, SQLITE3, REDIS
};

struct db_connect_config {
    string username{};
    string password{};
    string database{};
    string host = "127.0.0.1";
    uint16_t port = 0;
    string charset{};

#ifdef MSTL_SUPPORT_MYSQL__
    static db_connect_config for_mysql(const string& db) {
        db_connect_config config;
        config.port = 3306;
        config.database = db;
        config.charset = "utf8mb4";
        config.username = "root";
        return config;
    }
#endif

#ifdef MSTL_SUPPORT_SQLITE3__
    static db_connect_config for_sqlite(const string& file) {
        db_connect_config config;
        config.database = file;
        return config;
    }
#endif

#ifdef MSTL_SUPPORT_REDIS__
    static db_connect_config for_redis(const string& db) {
        db_connect_config config;
        config.port = 6379;
        config.database = db;
        return config;
    }
#endif
};


struct idb_result {
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;

    virtual ~idb_result() = default;
    virtual bool empty() const = 0;
    virtual size_type row_count() const = 0;
    virtual size_type column_count() const = 0;

    virtual const list<string_view>& column_names() const = 0;

    virtual bool next() = 0;

    virtual string_view at(size_type) const = 0;
    virtual bool at_bool(size_type) const = 0;
    virtual int8_t at_int8(size_type) const = 0;
    virtual int16_t at_int16(size_type) const = 0;
    virtual int32_t at_int32(size_type) const = 0;
    virtual int64_t at_int64(size_type) const = 0;
    virtual float32_t at_float32(size_type) const = 0;
    virtual float64_t at_float64(size_type) const = 0;
    virtual decimal_t at_decimal(size_type) const = 0;
    virtual vector<char> at_blob(size_type) const = 0;
    virtual string at_set(size_type) const = 0;
    virtual uint64_t at_bit(size_type) const = 0;
    virtual date at_date(size_type) const = 0;
    virtual time at_time(size_type) const = 0;
    virtual datetime at_datetime(size_type) const = 0;
    virtual timestamp at_timestamp(size_type) const = 0;
    virtual string at_string(size_type) const = 0;
    virtual string_view at_enum(size_type) const = 0;
};

struct idb_connect {
    using clock_type = std::clock_t;

    virtual ~idb_connect() = default;

    virtual bool connect_to(const _MSTL string& user, const _MSTL string& password,
        const _MSTL string& dbname, const _MSTL string& ip,
        uint32_t port, const _MSTL string& character_set) = 0;
    virtual bool connect_to(const db_connect_config& config) = 0;

    virtual bool set_character_set(const _MSTL string& encoding) const = 0;
    virtual string_view get_character_set() const = 0;
    virtual string_view get_error() const = 0;
    virtual uint32_t get_errno() const = 0;

    virtual bool update(const _MSTL string& sql) const = 0;
    virtual unique_ptr<idb_result> query(const string& sql) const = 0;
    virtual bool connected() const = 0;
    virtual bool is_valid() const = 0;
    virtual void close() = 0;
    virtual void refresh_alive() = 0;
    virtual clock_type get_alive() const = 0;
    virtual bool reset_connect(const db_connect_config& config) = 0;
};

class idb_factory {
protected:
    db_connect_config config_;

public:
    explicit idb_factory(const db_connect_config& config)
       : config_(config) {}

    virtual ~idb_factory() = default;
    virtual idb_connect* create_connect() = 0;
    virtual idb_result* create_result(void* native_result) = 0;
};


struct db_mysql_result final : idb_result {
private:
    MYSQL_RES* result = nullptr;
    size_type rows = 0;
    size_type columns = 0;
    MYSQL_ROW cursor = nullptr;
    list<string_view>* column_name_ = new list<string_view>;
    list<enum_field_types>* column_types_ = new list<enum_field_types>;

public:
    db_mysql_result() noexcept = default;

    explicit db_mysql_result(MYSQL_RES* result) noexcept
        : result(result), rows(mysql_num_rows(result)), columns(mysql_num_fields(result)) {
        MYSQL_FIELD* field;
        while ((field = mysql_fetch_field(result))) {
            column_name_->push_back(field->name);
            column_types_->push_back(field->type);
        }
    }

    ~db_mysql_result() override {
        mysql_free_result(result);
        delete column_name_;
        delete column_types_;
    }

    MSTL_NODISCARD bool empty() const noexcept override { return result == nullptr; }

    MSTL_NODISCARD size_type row_count() const noexcept override { return rows; }
    MSTL_NODISCARD size_type column_count() const noexcept override { return columns; }

    MSTL_NODISCARD const list<string_view>& column_names() const noexcept override {
        return *column_name_;
    }
    MSTL_NODISCARD decltype(auto) column_types() const noexcept {
        return const_cast<const list<enum_field_types>&>(*column_types_);
    }

    MSTL_NODISCARD bool next() noexcept override {
        if (!empty()) {
            cursor = mysql_fetch_row(result);
            return cursor != nullptr;
        }
        return false;
    }

    MSTL_NODISCARD _MSTL string_view at(const size_type n) const noexcept override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        return cursor[n];
    }

    MSTL_NODISCARD bool at_bool(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_BOOL)
            Exception(DatabaseTypeCastError("database type cast to bool mismatch"));
        return static_cast<bool>(to_int8(cursor[n]));
    }

    MSTL_NODISCARD int8_t at_int8(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_TINY || type == MYSQL_TYPE_BOOL))
            Exception(DatabaseTypeCastError("database type cast to int8 mismatch"));
        return to_int8(cursor[n]);
    }

    MSTL_NODISCARD int16_t at_int16(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY || type == MYSQL_TYPE_BOOL))
            Exception(DatabaseTypeCastError("database type cast to int16 mismatch"));
        return to_int16(cursor[n]);
    }

    MSTL_NODISCARD int32_t at_int32(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_LONG || type == MYSQL_TYPE_INT24 || type == MYSQL_TYPE_SHORT ||
            type == MYSQL_TYPE_TINY || type == MYSQL_TYPE_BOOL))
            Exception(DatabaseTypeCastError("database type cast to int32 mismatch"));
        return to_int32(cursor[n]);
    }

    MSTL_NODISCARD int64_t at_int64(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_LONGLONG || type == MYSQL_TYPE_LONG || type == MYSQL_TYPE_INT24 ||
            type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY || type == MYSQL_TYPE_BOOL))
            Exception(DatabaseTypeCastError("database type cast to int64 mismatch"));
        return to_int64(cursor[n]);
    }

    MSTL_NODISCARD float32_t at_float32(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_FLOAT || type == MYSQL_TYPE_LONG
            || type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY))
            Exception(DatabaseTypeCastError("database type cast to float32 mismatch"));
        return to_float32(cursor[n]);
    }

    MSTL_NODISCARD float64_t at_float64(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_DOUBLE || type == MYSQL_TYPE_FLOAT || type == MYSQL_TYPE_LONGLONG
            || type == MYSQL_TYPE_LONG || type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY))
            Exception(DatabaseTypeCastError("database type cast to float64 mismatch"));
        return to_float64(cursor[n]);
    }

    MSTL_NODISCARD decimal_t at_decimal(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_DECIMAL || type == MYSQL_TYPE_NEWDECIMAL || type == MYSQL_TYPE_DOUBLE ||
            type == MYSQL_TYPE_FLOAT || type == MYSQL_TYPE_LONGLONG || type == MYSQL_TYPE_LONG ||
            type == MYSQL_TYPE_SHORT || type == MYSQL_TYPE_TINY))
            Exception(DatabaseTypeCastError("database type cast to decimal mismatch"));
        return to_decimal(cursor[n]);
    }

    MSTL_NODISCARD _MSTL vector<char> at_blob(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        const auto type = column_types_->at(n);
        if (!(type == MYSQL_TYPE_BLOB || type == MYSQL_TYPE_TINY_BLOB ||
            type == MYSQL_TYPE_MEDIUM_BLOB || type == MYSQL_TYPE_LONG_BLOB))
            Exception(DatabaseTypeCastError("database type cast to blob mismatch"));
        return {cursor[n], cursor[n] + mysql_fetch_lengths(result)[n]};
    }

    MSTL_NODISCARD _MSTL string at_set(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_SET) {
            Exception(DatabaseTypeCastError("database type cast to SET mismatch"));
        }
        return cursor[n];
    }

    MSTL_NODISCARD uint64_t at_bit(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_BIT) {
            Exception(DatabaseTypeCastError("database type cast to BIT mismatch"));
        }
        const unsigned long length = mysql_fetch_lengths(result)[n];
        const char* data = cursor[n];

        uint64_t value = 0;
        for (unsigned long i = 0; i < length; ++i) {
            value = (value << 8) | static_cast<byte_t>(data[i]);
        }
        return value;
    }

    MSTL_NODISCARD _MSTL date at_date(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_DATE)
            Exception(DatabaseTypeCastError("database type cast to date mismatch"));
        return _MSTL date::from_string(cursor[n]);
    }

    MSTL_NODISCARD _MSTL time at_time(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_DATE)
            Exception(DatabaseTypeCastError("database type cast to time mismatch"));
        return _MSTL time::from_string(cursor[n]);
    }

    MSTL_NODISCARD _MSTL datetime at_datetime(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_DATETIME)
            Exception(DatabaseTypeCastError("database type cast to datetime mismatch"));
        return _MSTL datetime::from_string(cursor[n]);
    }

    MSTL_NODISCARD _MSTL timestamp at_timestamp(const size_type n) const override {
        MSTL_DEBUG_VERIFY(cursor, "database_result_row_value can`t dereference nullptr.")
        MSTL_DEBUG_VERIFY(columns > n, "database_result_row_value out of ranges.")
        if (column_types_->at(n) != MYSQL_TYPE_TIMESTAMP)
            Exception(DatabaseTypeCastError("database type cast to timestamp mismatch"));
        return _MSTL timestamp(_MSTL datetime::from_string(cursor[n]));
    }

    MSTL_NODISCARD string at_string(const size_type n) const noexcept override {
        return string{at(n)};
    }

    MSTL_NODISCARD string_view at_enum(const size_type n) const noexcept override {
        return at(n);
    }
};


#ifdef MSTL_SUPPORT_MYSQL__
// database connection is based on MySql connection.
struct db_mysql_connect final : idb_connect {
private:
    MYSQL* mysql = nullptr;
    clock_type alive_time_ = 0;

public:
    db_mysql_connect() noexcept {
        mysql = mysql_init(nullptr);
    }
    ~db_mysql_connect() noexcept override {
        this->close();
    }

    MSTL_NODISCARD bool connect_to(
        const _MSTL string& user, const _MSTL string& password,
        const _MSTL string& dbname, const _MSTL string& ip,
        const uint32_t port, const _MSTL string& character_set) noexcept override {
        const MYSQL* p = mysql_real_connect(mysql, ip.c_str(), user.c_str(),
            password.c_str(), dbname.c_str(), port, nullptr, 0);
        if (p == nullptr) return false;

        return this->set_character_set(character_set);
    }

    MSTL_NODISCARD bool connect_to(const db_connect_config& config) noexcept override {
        return connect_to(
            config.username,
            config.password,
            config.database,
            config.host,
            config.port,
            config.charset
        );
    }

    MSTL_NODISCARD bool set_character_set(
        const _MSTL string& encoding) const noexcept override {
        if (connected()) {
            if (mysql_set_character_set(mysql, encoding.data())) {
                return false;
            }
            return true;
        }
        return false;
    }

    MSTL_NODISCARD string_view get_character_set() const noexcept override {
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

    MSTL_NODISCARD string_view get_error() const noexcept override {
        return mysql_error(mysql);
    }
    MSTL_NODISCARD uint32_t get_errno() const noexcept override {
        return mysql_errno(mysql);
    }

    MSTL_NODISCARD bool update(const _MSTL string& sql) const noexcept override {
        if (mysql_query(mysql, sql.c_str())) {
            return false;
        }
        return true;
    }

    MSTL_NODISCARD unique_ptr<idb_result> query(const _MSTL string& sql) const noexcept override {
        if (mysql_query(mysql, sql.c_str())) {
            return {};
        }
        return make_unique<db_mysql_result>(mysql_store_result(mysql));
    }

    MSTL_NODISCARD bool connected() const noexcept override {
        return mysql != nullptr;
    }
    MSTL_NODISCARD bool is_valid() const noexcept override {
        return mysql_ping(mysql) == 0;
    }

    void close() noexcept override {
        if (connected()) {
            mysql_close(mysql);
        }
    }

    void refresh_alive() noexcept override {
        alive_time_ = std::clock();
    }
    MSTL_NODISCARD clock_type get_alive() const noexcept override {
        return std::clock() - alive_time_;
    }

    MSTL_NODISCARD bool reset_connect(const db_connect_config& config) override {
        if (connected()) {
            mysql_close(mysql);
            mysql = mysql_init(nullptr);
            return connect_to(config);
        }
        return false;
    }
};

class db_mysql_factory final : public idb_factory {
public:
    explicit db_mysql_factory(const db_connect_config& config)
        : idb_factory(config) {}

    idb_connect* create_connect() override {
        auto conn = new db_mysql_connect();
        if (!conn->connect_to(config_)) {
            delete conn;
            return nullptr;
        }
        return conn;
    }
    idb_result* create_result(void* native_result) override {
        return new db_mysql_result(static_cast<MYSQL_RES*>(native_result));
    }
};
#endif

#ifdef MSTL_SUPPORT_SQLITE3__
struct db_sqlite_result final : idb_result {
private:
    sqlite3_stmt* stmt = nullptr;
    size_type cursor_ = 0;
    size_type columns = 0;
    list<string_view>* column_names_ = new list<string_view>;
    list<int>* column_types_ = new list<int>;

public:
    db_sqlite_result() noexcept = default;

    explicit db_sqlite_result(sqlite3_stmt* statement) noexcept : stmt(statement) {
        if (stmt) {
            columns = sqlite3_column_count(stmt);
            for (int i = 0; i < columns; ++i) {
                column_names_->push_back(sqlite3_column_name(stmt, i));
                column_types_->push_back(sqlite3_column_type(stmt, i));
            }
        }
    }

    ~db_sqlite_result() override {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
        delete column_names_;
        delete column_types_;
    }

    // useless function
    MSTL_NODISCARD MSTL_DEPRECATE_FOR("use COUNT * instead of using this function")
    size_type row_count() const noexcept override {
        return 0;
    }

    MSTL_NODISCARD size_type column_count() const noexcept override {
        return columns;
    }

    MSTL_NODISCARD bool empty() const noexcept override {
        return stmt == nullptr;
    }

    MSTL_NODISCARD const list<string_view>& column_names() const override {
        return *column_names_;
    }

    MSTL_NODISCARD bool next() noexcept override {
        if (!empty()) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                ++cursor_;
                return true;
            }
        }
        return false;
    }

    MSTL_NODISCARD _MSTL string_view at(const size_type index) const noexcept override {
        const auto text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, index));
        return text ? string_view(text) : string_view{};
    }

    MSTL_NODISCARD bool at_bool(const size_type index) const override {
        return sqlite3_column_int(stmt, index) != 0;
    }

    MSTL_NODISCARD int8_t at_int8(const size_type index) const override {
        return static_cast<int8_t>(sqlite3_column_int(stmt, index));
    }

    MSTL_NODISCARD int16_t at_int16(const size_type index) const override {
        return static_cast<int16_t>(sqlite3_column_int(stmt, index));
    }

    MSTL_NODISCARD int32_t at_int32(const size_type index) const override {
        return sqlite3_column_int(stmt, index);
    }

    MSTL_NODISCARD int64_t at_int64(const size_type index) const override {
        return sqlite3_column_int64(stmt, index);
    }

    MSTL_NODISCARD float32_t at_float32(const size_type index) const override {
        return static_cast<float32_t>(sqlite3_column_double(stmt, index));
    }

    MSTL_NODISCARD float64_t at_float64(const size_type index) const override {
        return static_cast<float64_t>(sqlite3_column_double(stmt, index));
    }

    MSTL_NODISCARD decimal_t at_decimal(const size_type index) const override {
        return static_cast<decimal_t>(sqlite3_column_double(stmt, index));
    }

    MSTL_NODISCARD _MSTL vector<char> at_blob(const size_type index) const override {
        const string_view view = this->at(index);
        return vector<char>{view.data(), view.data() + view.size()};
    }

    MSTL_NODISCARD _MSTL string at_set(const size_type index) const override {
        return at_string(index);
    }

    MSTL_NODISCARD uint64_t at_bit(const size_type index) const noexcept override {
        const auto data = at(index);
        uint64_t value = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            value = (value << 8) | static_cast<byte_t>(data.at(i));
        }
        return value;
    }

    MSTL_NODISCARD _MSTL date at_date(const size_type index) const noexcept override {
        return at_datetime(index).get_date();
    }

    MSTL_NODISCARD _MSTL time at_time(const size_type index) const noexcept override {
        return at_datetime(index).get_time();
    }

    MSTL_NODISCARD _MSTL datetime at_datetime(const size_type index) const override {
        const auto text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, index));
        if (text) {
            return _MSTL datetime::from_string(text);
        }
        return _MSTL datetime{};
    }

    MSTL_NODISCARD timestamp at_timestamp(const size_type index) const override {
        return timestamp{sqlite3_column_int64(stmt, index)};
    }

    MSTL_NODISCARD string at_string(const size_type index) const override {
        return string{at(index)};
    }

    MSTL_NODISCARD string_view at_enum(const size_type index) const override {
        return at(index);
    }
};

struct db_sqlite_connect final : idb_connect {
private:
    mutable sqlite3* db = nullptr;
    clock_type alive_time_ = 0;
    mutable string_view last_error_;

public:
    db_sqlite_connect() noexcept {
        sqlite3_open(nullptr, &db);
    }
    ~db_sqlite_connect() noexcept override {
        this->close();
    }

    bool connect_to(const _MSTL string&, const _MSTL string&,
            const _MSTL string& dbname, const _MSTL string&,
            uint32_t, const _MSTL string&) override {
        sqlite3_open(dbname.c_str(), &db);
        return connect_to_file(dbname);
    }

    bool connect_to(const db_connect_config& config) override {
        sqlite3_open(config.database.c_str(), &db);
        return connect_to_file(config.database);
    }

    MSTL_NODISCARD bool set_character_set(const _MSTL string& encoding) const override {
        const string sql = "PRAGMA encoding = '" + encoding + "';";
        return update(sql);
    }

    MSTL_NODISCARD string_view get_character_set() const override {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, "PRAGMA encoding;", -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }
        string_view encoding;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            encoding = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
        return encoding;
    }

    MSTL_NODISCARD string_view get_error() const override {
        if (db) {
            last_error_ = sqlite3_errmsg(db);
        }
        return last_error_;
    }

    MSTL_NODISCARD uint32_t get_errno() const override {
        return db ? sqlite3_errcode(db) : 0;
    }

    MSTL_NODISCARD bool update(const string& sql) const override {
        if (!connected()) return false;

        char* error_msg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error_msg) != SQLITE_OK) {
            if (error_msg) {
                last_error_ = error_msg;
                sqlite3_free(error_msg);
            }
            return false;
        }
        return true;
    }

    MSTL_NODISCARD unique_ptr<idb_result> query(const string& sql) const override {
        if (!connected()) return {};

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }
        return make_unique<db_sqlite_result>(stmt);
    }

    MSTL_NODISCARD bool connected() const override {
        return db != nullptr;
    }

    MSTL_NODISCARD bool is_valid() const override {
        if (!connected()) return false;
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, "SELECT 1;", -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_finalize(stmt);
            return true;
        }
        return false;
    }

    void close() noexcept override {
        if (db) {
            sqlite3_close(db);
        }
    }

    void refresh_alive() noexcept override {
        alive_time_ = std::clock();
    }

    MSTL_NODISCARD clock_type get_alive() const noexcept override {
        return std::clock() - alive_time_;
    }

    MSTL_NODISCARD bool reset_connect(const db_connect_config& config) override {
        if (connected()) {
            sqlite3_close(db);
            return connect_to(config);
        }
        return false;
    }

private:
    bool connect_to_file(const string& file_path) {
        if (connected()) {
            close();
        }

        if (sqlite3_open(file_path.c_str(), &db) != SQLITE_OK) {
            last_error_ = sqlite3_errmsg(db);
            close();
            return false;
        }
        return true;
    }
};

class db_sqlite_factory final : public idb_factory {
public:
    explicit db_sqlite_factory(const db_connect_config& config)
        : idb_factory(config) {}

    idb_connect* create_connect() override {
        auto conn = new db_sqlite_connect();
        if (!conn->connect_to(config_)) {
            return nullptr;
        }
        return conn;
    }
    idb_result* create_result(void* native_result) override {
        return new db_sqlite_result(static_cast<sqlite3_stmt*>(native_result));
    }
};
#endif

#ifdef MSTL_SUPPORT_REDIS__
struct db_redis_result final : idb_result {
private:
    redisReply* reply_ = nullptr;
    size_type cursor_ = 0;
    size_type rows_ = 0;
    list<string_view> column_names_;
    bool is_array_ = false;

private:
    static string format_redis_reply_element(redisReply* element) {
        switch (element->type) {
            case REDIS_REPLY_STRING:
            case REDIS_REPLY_STATUS:
            case REDIS_REPLY_ERROR:
                return {element->str, element->len};
            case REDIS_REPLY_INTEGER:
                return to_string(element->integer);
            case REDIS_REPLY_NIL:
                return {};
            case REDIS_REPLY_ARRAY: {
                string result;
                for (size_t i = 0; i < element->elements; ++i) {
                    if (i > 0) result += " ";
                    result += format_redis_reply_element(element->element[i]);
                }
                return result;
            }
            default:
                return "unsupported-type";
        }
    }

public:
    db_redis_result() noexcept = default;

    explicit db_redis_result(redisReply* reply) noexcept
    : reply_(reply) {
        if (reply_) {
            if (reply_->type == REDIS_REPLY_ARRAY) {
                is_array_ = true;
                rows_ = reply_->elements;
                column_names_.push_back("value");
            } else {
                rows_ = 1;
                column_names_.push_back("result");
            }
        }
    }

    ~db_redis_result() override {
        if (reply_) {
            freeReplyObject(reply_);
        }
    }

    MSTL_NODISCARD bool empty() const noexcept override {
        return !reply_ || rows_ == 0;
    }
    MSTL_NODISCARD size_type row_count() const noexcept override {
        return rows_;
    }
    MSTL_NODISCARD size_type column_count() const noexcept override {
        return 1;
    }
    MSTL_NODISCARD const list<string_view>& column_names() const noexcept override {
        return column_names_;
    }

    MSTL_NODISCARD bool next() noexcept override {
        if (empty() || cursor_ >= rows_) return false;
        ++cursor_;
        return cursor_ <= rows_;
    }

    MSTL_NODISCARD string_view at(size_type) const noexcept override {
        return {at_string(0).data(), at_string(0).length()};
    }

    MSTL_NODISCARD bool at_bool(size_type) const override {
        const string s = at_string(0);
        return s == "1" || s == "true" || s == "TRUE" || s == "yes";
    }

    MSTL_NODISCARD int8_t at_int8(size_type) const override {
        return to_int8(at_string(0).c_str());
    }
    MSTL_NODISCARD int16_t at_int16(size_type) const override {
        return to_int16(at_string(0).c_str());
    }
    MSTL_NODISCARD int32_t at_int32(size_type) const override {
        return to_int32(at_string(0).c_str());
    }
    MSTL_NODISCARD int64_t at_int64(size_type) const override {
        return to_int64(at_string(0).c_str());
    }

    MSTL_NODISCARD float32_t at_float32(size_type) const override {
        return to_float32(at_string(0).c_str());
    }

    MSTL_NODISCARD float64_t at_float64(size_type) const override {
        return to_float64(at_string(0).c_str());
    }

    MSTL_NODISCARD decimal_t at_decimal(size_type) const override {
        return to_decimal(at_string(0).c_str());
    }

    MSTL_NODISCARD vector<char> at_blob(size_type) const override {
        const string s = at_string(0);
        return {s.begin(), s.end()};
    }

    MSTL_NODISCARD string at_set(size_type) const override {
        return at_string(0);
    }

    MSTL_NODISCARD uint64_t at_bit(size_type) const override {
        const string data = at_string(0);
        uint64_t value = 0;
        for (unsigned long i = 0; i < data.size(); ++i) {
            value = (value << 8) | static_cast<byte_t>(data[i]);
        }
        return value;
    }

    MSTL_NODISCARD date at_date(size_type) const override {
        return date::from_string(at_string(0));
    }

    MSTL_NODISCARD time at_time(size_type) const override {
        return time::from_string(at_string(0));
    }

    MSTL_NODISCARD datetime at_datetime(size_type) const override {
        return datetime::from_string(at_string(0));
    }

    MSTL_NODISCARD timestamp at_timestamp(size_type) const override {
        return timestamp(to_int64(at_string(0).c_str()));
    }

    MSTL_NODISCARD string at_string(size_type) const override {
        if (empty() || cursor_ == 0) return {};

        if (is_array_) {
            redisReply* element = reply_->element[cursor_ - 1];
            return format_redis_reply_element(element);
        }
        return format_redis_reply_element(reply_);
    }

    MSTL_NODISCARD string_view at_enum(size_type) const noexcept override {
        return {at_string(0).c_str(), at_string(0).length()};
    }
};

struct db_redis_connect final : idb_connect {
private:
    redisContext* context_ = nullptr;
    clock_type alive_time_ = 0;
    mutable string last_error_;

private:
    bool authenticate(const string& password) const {
        if (password.empty()) return true;
        const auto reply = static_cast<redisReply*>(redisCommand(context_, "AUTH %s", password.c_str()));
        if (!reply || reply->type == REDIS_REPLY_ERROR) {
            if (reply) {
                last_error_ = reply->str ? reply->str : "Authentication failed";
                freeReplyObject(reply);
            }
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    bool select_database(const string& db_index) const {
        if (db_index.empty()) return true;
        try {
            const int db = to_int32(db_index.c_str());
            const auto reply = static_cast<redisReply*>(redisCommand(context_, "SELECT %d", db));
            if (!reply || reply->type == REDIS_REPLY_ERROR) {
                if (reply) {
                    last_error_ = reply->str ? reply->str : "SELECT failed";
                    freeReplyObject(reply);
                }
                return false;
            }
            freeReplyObject(reply);
            return true;
        } catch (...) {
            last_error_ = "Invalid database index";
            return false;
        }
    }

    bool connect_to_host(const string& host, uint16_t port,
        const string& password, const string& dbname) {
        context_ = redisConnect(host.c_str(), port);
        if (!context_ || context_->err) {
            if (context_) {
                last_error_ = context_->errstr;
                redisFree(context_);
                context_ = nullptr;
            } else {
                last_error_ = "Connection failed";
            }
            return false;
        }
        if (!authenticate(password)) {
            close();
            return false;
        }
        if (!select_database(dbname)) {
            close();
            return false;
        }
        return true;
    }

public:
    db_redis_connect() = default;
    ~db_redis_connect() override {
        close();
    }

    bool connect_to(const string&, const string& password,
        const string& dbname, const string& host,
        uint32_t port, const string&) override {
        return connect_to_host(host, port, password, dbname);
    }

    bool connect_to(const db_connect_config& config) override {
        return connect_to_host(config.host, config.port, config.password, config.database);
    }

    MSTL_DEPRECATE_FOR("Redis not support setting character sets")
    bool set_character_set(const string&) const override {
        return true;
    }
    MSTL_DEPRECATE_FOR("Redis not support setting character sets")
    string_view get_character_set() const override {
        return {};
    }

    string_view get_error() const override {
        if (context_ && context_->errstr[0] != '\0') {
            last_error_ = context_->errstr;
        }
        return {last_error_.data(), last_error_.size()};
    }

    uint32_t get_errno() const override {
        return context_ ? context_->err : 0;
    }

    bool update(const string& sql) const override {
        const auto reply = static_cast<redisReply*>(redisCommand(context_, sql.c_str()));
        if (!reply || reply->type == REDIS_REPLY_ERROR) {
            if (reply) {
                last_error_ = reply->str ? reply->str : "Command failed";
                freeReplyObject(reply);
            }
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    unique_ptr<idb_result> query(const string& sql) const override {
        const auto reply = static_cast<redisReply*>(redisCommand(context_, sql.c_str()));
        if (!reply || reply->type == REDIS_REPLY_ERROR) {
            if (reply) {
                last_error_ = reply->str ? reply->str : "Query failed";
                freeReplyObject(reply);
            }
            return nullptr;
        }
        return make_unique<db_redis_result>(reply);
    }

    bool connected() const override {
        return context_ != nullptr && !context_->err;
    }

    bool is_valid() const override {
        if (!connected()) return false;
        const auto reply = static_cast<redisReply*>(redisCommand(context_, "PING"));
        if (!reply || reply->type != REDIS_REPLY_STATUS ||
            string_compare(reply->str, "PONG") != 0) {
            if (reply) freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    void close() noexcept override {
        if (context_) {
            redisFree(context_);
            context_ = nullptr;
        }
    }

    void refresh_alive() noexcept override {
        alive_time_ = std::clock();
    }

    clock_type get_alive() const noexcept override {
        return std::clock() - alive_time_;
    }

    bool reset_connect(const db_connect_config& config) override {
        close();
        return connect_to(config);
    }
};

class db_redis_factory final : public idb_factory {
public:
    db_redis_factory(const db_connect_config& config) : idb_factory(config) {}

    idb_connect* create_connect() override {
        auto conn = new db_redis_connect();
        if (!conn->connect_to(config_)) {
            delete conn;
            return nullptr;
        }
        return conn;
    }

    idb_result* create_result(void* native_result) override {
        return new db_redis_result(static_cast<redisReply*>(native_result));
    }
};
#endif


class database_pool {
private:
    db_connect_config config_;
    size_t init_size_;
    size_t max_size_;
    size_t max_idle_time_;  // s
    size_t connect_timeout_;  // ms

    unique_ptr<idb_factory> factory_ = nullptr;
    _MSTL queue<idb_connect*> connect_queue_;
    std::mutex queue_mtx_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};

    std::thread produce_;
    std::thread scanner_;

    friend database_pool& get_instance_database_pool();

private:
    void produce_connect_task() {
        while (true) {
            if (!running_) break;
            std::unique_lock<std::mutex> lock(queue_mtx_);
            while (!connect_queue_.empty()) {
                cv_.wait(lock);
                if (!running_) break;
            }
            if (!running_) break;
            if (connect_queue_.size() < max_size_) {
                auto* p = factory_->create_connect();
                if (p != nullptr) {
                    p->refresh_alive();
                    connect_queue_.push(p);
                }
            }
            cv_.notify_all();
        }
    }

    void scanner_connect_task() {
        while (true) {
            if (!running_) break;
            std::this_thread::sleep_for(std::chrono::seconds(max_idle_time_));
            if (!running_) break;
            std::unique_lock<std::mutex> lock(queue_mtx_);

            while (connect_queue_.size() > init_size_) {
                idb_connect* ptr = connect_queue_.front();
                if (ptr->get_alive() >= max_idle_time_ * 1000) {
                    connect_queue_.pop();
                    delete ptr;
                }
                else
                    break;
            }
        }
    }

public:
    database_pool(
        const DB_TYPE type, const db_connect_config& config,
        const size_t init_size = 50, const size_t max_size = 1024,
        const size_t max_idle_time = 30, const size_t connect_timeout = 100) :
    config_(config), init_size_(init_size), max_size_(max_size), max_idle_time_(max_idle_time),
    connect_timeout_(connect_timeout), running_(true) {
        switch(type) {
#ifdef MSTL_SUPPORT_MYSQL__
            case DB_TYPE::MYSQL:
                factory_ = make_unique<db_mysql_factory>(config);
                break;
#endif
#ifdef MSTL_SUPPORT_SQLITE3__
            case DB_TYPE::SQLITE3:
                factory_ = make_unique<db_sqlite_factory>(config);
                break;
#endif
#ifdef MSTL_SUPPORT_REDIS__
            case DB_TYPE::REDIS:
                factory_ = make_unique<db_redis_factory>(config);
                break;
#endif
            default:
                // never run:
                // Exception(DatabaseError("Useless Database Type"));
                break;
        }

        for (size_t i = 0; i < init_size_; i++) {
            auto* p = factory_->create_connect();
            if (p != nullptr) {
                p->refresh_alive();
                connect_queue_.push(p);
            }
            else {
                --i;
            }
        }
        produce_ = std::thread([this] { produce_connect_task(); });
        scanner_ = std::thread([this] { scanner_connect_task(); });
    }

    ~database_pool() {
        running_ = false;
        cv_.notify_all();

        if (produce_.joinable()) {
            produce_.join();
        }
        if (scanner_.joinable()) {
            scanner_.join();
        }

        while (!connect_queue_.empty()) {
            delete connect_queue_.front();
            connect_queue_.pop();
        }
    }

    database_pool(const database_pool&) = delete;
    database_pool& operator=(const database_pool&) = delete;
    database_pool(database_pool&&) = delete;
    database_pool& operator=(database_pool&&) = delete;

    _MSTL shared_ptr<idb_connect> get_connect() {
        std::unique_lock<std::mutex> lock(queue_mtx_);

        while (connect_queue_.empty()) {
            if (cv_.wait_for(lock,
                std::chrono::milliseconds(connect_timeout_)) == std::cv_status::timeout) {
                if (connect_queue_.empty()) {
                    if (connect_queue_.size() < max_size_) {
                        auto* new_conn = factory_->create_connect();
                        if (new_conn != nullptr) {
                            new_conn->refresh_alive();
                            connect_queue_.push(new_conn);
                            continue;
                        }
                    }
                    return nullptr;
                }
            }
        }

        idb_connect* raw_conn = connect_queue_.front();
        connect_queue_.pop();

        if (!raw_conn->is_valid()) {
            try {
                if (!raw_conn->reset_connect(config_)) {
                    delete raw_conn;
                    raw_conn = factory_->create_connect();
                    if (raw_conn == nullptr) {
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

        auto conn_ptr = _MSTL shared_ptr<idb_connect>(raw_conn,
            [this](idb_connect* p) {
                std::unique_lock<std::mutex> lock1(queue_mtx_);
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

MSTL_END_NAMESPACE__
#endif
#endif // MSTL_DATABASE_POOL_HPP__
