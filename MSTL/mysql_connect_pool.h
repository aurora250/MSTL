#ifndef MSTL_DATABASE_H__
#define MSTL_DATABASE_H__
#include "basiclib.h"
#if MSTL_DLL_LINK__
#include "threadsafe_print.h"
#include "tuple.hpp"
#include <winsock.h>
#include <mysql.h>
#include <string>
#include <mutex>
#include <thread>
#include <memory>
#include <functional>
#include <condition_variable>
#include <sstream>
MSTL_BEGIN_NAMESPACE__

extern const std::string DEFAULT_IP;
extern const unsigned int DEFAULT_PORT;

class DBConnect { // based on MySql
private:
	template <typename T>
	void __create_sql(const T& val) {
		ss << val << ", ";
	}
	void __create_sql(const std::string& str) {
		ss << "'" << str << "', ";
	}
	void __create_sql(const char* str) {
		ss << "'" << str << "', ";
	}
	void __create_sql(char str) {
		ss << "'" << str << "', ";
	}
	template <typename T>
	void __create_sql_last(const T& val) {
		ss << val;
	}
	void __create_sql_last(const std::string& str) {
		ss << "'" << str << "'";
	}
	void __create_sql_last(const char* str) {
		ss << "'" << str << "'";
	}
	void __create_sql_last(char str) {
		ss << "'" << str << "'";
	}
	template <typename Tuple, size_t... Index>
	void create_sql(const Tuple& t, MSTL::index_sequence<Index...>) {
		((Index == sizeof...(Index) - 1 ?
			__create_sql_last(MSTL::get<Index>(t)) :
			__create_sql(MSTL::get<Index>(t))), ...);
	}
public:
	DBConnect();
	~DBConnect();
	bool connect_to(const std::string& user, const std::string& password,
		const std::string& dbname, const std::string& ip = DEFAULT_IP, unsigned int port = DEFAULT_PORT);
	bool exec(std::string sql);
	MYSQL_RES* SELECT(std::string table, std::string selected, std::string cond);
	template <typename... Args>
	bool INSERT_INTO(std::string table, Args... args) {
		ss << "INSERT INTO " << table << " VALUES (";
		auto tuple = MSTL::make_tuple(MSTL::forward<Args>(args)...);
		(create_sql)(tuple, MSTL::make_index_sequence<sizeof...(Args)>{});
		ss << ")";
		std::string sql = ss.str(); ss.str("");
		return exec(sql);
	}
	bool close();
	void refresh_alive();
	clock_t get_alive();
private:
	MYSQL* mysql;
	clock_t alive_time_;
	std::stringstream ss;
};

class DBConnectPool {
public:
	DBConnectPool(const std::string& user, const std::string& password,
		const std::string& dbname, const std::string& ip = DEFAULT_IP, unsigned int port = DEFAULT_PORT);
	std::shared_ptr<DBConnect> get_connect();
	~DBConnectPool() = default;
	DBConnectPool(const DBConnectPool&) = delete;
	DBConnectPool(DBConnectPool&&) = delete;
	DBConnectPool& operator =(const DBConnectPool&) = delete;
	DBConnectPool& operator =(DBConnectPool&&) = delete;
private:
	void produce_connect_task();
	void scanner_connect_task();

	std::string ip_;
	unsigned int port_;
	std::string username_;
	std::string passwd_;
	std::string dbname_;
	size_t init_size_;
	size_t max_size_;
	size_t max_idle_time_;  // s
	size_t connect_timeout_;  // ms

	queue<DBConnect*> connect_queue_;
	std::mutex queue_mtx_;
	std::condition_variable cv_;
};

MSTL_END_NAMESPACE__
#endif // MSTL_DLL_LINK__
#endif // MSTL_DATABASE_H__