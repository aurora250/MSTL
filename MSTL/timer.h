#ifndef MSTL_TIMER_HPP__
#define MSTL_TIMER_HPP__
#include "set.hpp"
#include "functional.hpp"
#include <chrono>
MSTL_BEGIN_NAMESPACE__

struct __timer_node_base {
protected:
	std::time_t expire_ = 0;
	size_t id_ = 0;

public:
    __timer_node_base() = default;
    __timer_node_base(std::time_t expire, size_t id)
        : expire_(expire), id_(id) {}

    __timer_node_base(const __timer_node_base &) = default;
    __timer_node_base &operator=(const __timer_node_base &) = default;
    __timer_node_base(__timer_node_base &&) = default;
    __timer_node_base &operator=(__timer_node_base &&) = default;
    ~__timer_node_base() = default;

    MSTL_NODISCARD std::time_t expire() const { return expire_; }
    MSTL_NODISCARD size_t id() const { return id_; }
};

struct __timer_node : __timer_node_base {
public:
    using call_back = _MSTL function<void(const __timer_node& node)>;

private:
	call_back func_;

public:
    __timer_node() = default;
    __timer_node(std::time_t expire, size_t id, call_back func)
        : __timer_node_base(expire, id), func_(_MSTL move(func)) {}

    __timer_node(const __timer_node& node) = default;
    __timer_node& operator=(const __timer_node& node) = default;
    __timer_node(__timer_node&& node) = default;
    __timer_node& operator=(__timer_node&& node) = default;
    ~__timer_node() = default;

    MSTL_NODISCARD const call_back& hold() const { return func_; }
};

inline bool operator <(const __timer_node_base& lh, const __timer_node_base& rh) {
	if (lh.expire() < rh.expire()) return true;
	if (lh.expire() > rh.expire()) return false;
	return lh.id() < rh.id();
}


class timer {
public:
    using node_type = __timer_node;
    using func_type = __timer_node::call_back;

private:
    static size_t timer_id_;
    _MSTL set<node_type> timer_set_;

public:
    timer() = default;
    ~timer() = default;

    static std::time_t get_tick() {
        const auto sc
            = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());
        const auto sub
            = std::chrono::duration_cast<std::chrono::microseconds>(sc.time_since_epoch());
        return sub.count();
    }

    static size_t generate_id() {
        return ++timer_id_;
    }

    node_type add(std::time_t ms, const func_type &func) {
        node_type tnode(get_tick() + ms, generate_id(), func);
        timer_set_.insert(tnode);
        return tnode;
    }

    bool erase(const node_type& node)  {
        const auto iter = timer_set_.find(node);
        if (iter != timer_set_.end()) {
            timer_set_.erase(iter);
            return true;
        }
        return false;
    }

    bool check() {
        auto iter = timer_set_.begin();
        if (iter != timer_set_.end() && iter->expire() <= get_tick()) {
            iter->hold()(*iter);
            timer_set_.erase(iter);
            return true;
        }
        return false;
    }

    std::time_t sleep() {
        const auto iter = timer_set_.begin();
        if (iter == timer_set_.end()) return -1;
        const std::time_t dis = iter->expire() - get_tick();
        return dis > 0 ? dis : 0;
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_TIMER_HPP__
