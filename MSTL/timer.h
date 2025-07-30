#ifndef MSTL_TIMER_HPP__
#define MSTL_TIMER_HPP__
#include "set.hpp"
#include "functional.hpp"
MSTL_BEGIN_NAMESPACE__

struct __timer_node_base {
protected:
	std::time_t expire_ = 0;
	size_t id_ = 0;

public:
    __timer_node_base() = default;
    __timer_node_base(std::time_t expire, size_t id);

    __timer_node_base(const __timer_node_base &) = default;
    __timer_node_base &operator=(const __timer_node_base &) = default;
    __timer_node_base(__timer_node_base &&) = default;
    __timer_node_base &operator=(__timer_node_base &&) = default;
    ~__timer_node_base() = default;

    MSTL_NODISCARD std::time_t expire() const;
    MSTL_NODISCARD size_t id() const;
};

struct __timer_node : __timer_node_base {
public:
    using call_back = _MSTL function<void(const __timer_node& node)>;

private:
	call_back func_;

public:
    __timer_node() = default;
    __timer_node(std::time_t expire, size_t id, call_back func);

    __timer_node(const __timer_node& node) = default;
    __timer_node& operator=(const __timer_node& node) = default;
    __timer_node(__timer_node&& node) = default;
    __timer_node& operator=(__timer_node&& node) = default;
    ~__timer_node() = default;

    MSTL_NODISCARD const call_back& hold() const;
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

	static std::time_t get_tick();
	static size_t get_id();

	node_type add(std::time_t ms, const func_type &func);
	bool erase(const node_type& node);

	bool check();
	std::time_t sleep();
};

MSTL_END_NAMESPACE__
#endif // MSTL_TIMER_HPP__
