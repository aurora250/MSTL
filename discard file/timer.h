#ifndef MSTL_TIMER_H__
#define MSTL_TIMER_H__
#include "basiclib.h"
#include "set.hpp"
#include <chrono>
#include <functional>
#ifdef MSTL_PLATFORM_LINUX__
#include <sys/epoll>
#endif
MSTL_BEGIN_NAMESPACE__

struct __timer_node_base {
	time_t expire_;
	size_t id_;
};

struct __timer_node : public __timer_node_base {
	using call_back = std::function<void(const __timer_node& node)>;

	call_back func_;
};

inline bool operator <(const __timer_node_base& lh, const __timer_node_base& rh) {
	if (lh.expire_ < rh.expire_) return true;
	else if (lh.expire_ > rh.expire_) return false;
	else return lh.id_ < rh.id_;
}

class timer {
public:
	typedef __timer_node node_type;
	typedef __timer_node::call_back func_type;

	static time_t get_tick() {
		auto sc = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());
		auto sub = std::chrono::duration_cast<std::chrono::microseconds>(sc.time_since_epoch());
		return sub.count();
	}
	static size_t get_id() {
		return ++timer_id_;
	}
	node_type add(time_t ms, func_type func) {
		node_type tnode;
		tnode.expire_ = get_tick() + ms;
		tnode.func_ = func;
		tnode.id_ = get_id();
		timer_set_.insert(tnode);
		return tnode;
	}
	bool erase(node_type& node) {
		auto iter = timer_set_.find(node);
		if (iter != timer_set_.end()) {
			timer_set_.erase(iter);
			return true;
		}
		return false;
	}
	bool check() {
		auto iter = timer_set_.begin();
		if (iter != timer_set_.end() && iter->expire_ <= get_tick()) {
			iter->func_(*iter);
			timer_set_.erase(iter);
			return true;
		}
		return false;
	}
	time_t sleep() {
		auto iter = timer_set_.begin();
		if (iter == timer_set_.end()) return -1;
		time_t dis = iter->expire_ - get_tick();
		return dis > 0 ? dis : 0;
	}
private:
	static size_t timer_id_;
	set<node_type> timer_set_;
};
size_t timer::timer_id_ = 0;

MSTL_END_NAMESPACE__
#endif // MSTL_TIMER_H__
