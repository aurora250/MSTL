#ifndef MSTL_DATABASE_H__
#define MSTL_DATABASE_H__
#include "basiclib.h"
#if MSTL_DLL_LINK__
#include "errorlib.h"
#include "threadsafe_print.h"
#include <mysql.h>
#include <string>
#include <mutex>
#include <thread>
#include <memory>
#include <functional>
#include <condition_variable>
#include "queue.hpp"
MSTL_BEGIN_NAMESPACE__

extern const std::string DEFAULT_IP;
extern const unsigned int DEFAULT_PORT;

class Connect { // based on MySql
public:
	Connect();
	~Connect();
	bool connect_to(const std::string& user, const std::string& password, 
		const std::string& dbname, const std::string& ip = DEFAULT_IP, unsigned int port = DEFAULT_PORT);
	bool update(std::string sql);
	MYSQL_RES* query(std::string sql);
	bool close();
	void refresh_alive();
	clock_t get_alive();
private:
	MYSQL* mysql;
	clock_t alive_time_;
};

class ConnectPool {
public:
	ConnectPool(const std::string& user, const std::string& password,
		const std::string& dbname, const std::string& ip = DEFAULT_IP, unsigned int port = DEFAULT_PORT);
	std::shared_ptr<Connect> get_connect();
	~ConnectPool() = default;
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

	queue<Connect*> connect_queue_;
	std::mutex queue_mtx_;
	std::condition_variable cv_;
};

MSTL_END_NAMESPACE__
#endif // MSTL_DLL_LINK__
#endif // MSTL_DATABASE_H__
