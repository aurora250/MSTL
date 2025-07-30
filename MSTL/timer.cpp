#include "timer.h"
#include <chrono>
MSTL_BEGIN_NAMESPACE__

__timer_node_base::__timer_node_base(const std::time_t expire, const size_t id)
    : expire_(expire), id_(id) {}

MSTL_NODISCARD std::time_t __timer_node_base::expire() const { return expire_; }
MSTL_NODISCARD size_t __timer_node_base::id() const { return id_; }

__timer_node::__timer_node(const std::time_t expire, const size_t id, call_back func)
: __timer_node_base(expire, id), func_(_MSTL move(func)) {}

MSTL_NODISCARD const __timer_node::call_back& __timer_node::hold() const { return func_; }


size_t timer::timer_id_ = 0;

std::time_t timer::get_tick() {
    const auto sc
        = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());
    const auto sub
        = std::chrono::duration_cast<std::chrono::microseconds>(sc.time_since_epoch());
    return sub.count();
}

size_t timer::get_id() {
    return ++timer_id_;
}

timer::node_type timer::add(const std::time_t ms, const func_type &func) {
    node_type tnode(get_tick() + ms, get_id(), func);
    timer_set_.insert(tnode);
    return tnode;
}

bool timer::erase(const node_type& node) {
    const auto iter = timer_set_.find(node);
    if (iter != timer_set_.end()) {
        timer_set_.erase(iter);
        return true;
    }
    return false;
}

bool timer::check() {
    auto iter = timer_set_.begin();
    if (iter != timer_set_.end() && iter->expire() <= get_tick()) {
        iter->hold()(*iter);
        timer_set_.erase(iter);
        return true;
    }
    return false;
}

std::time_t timer::sleep() {
    const auto iter = timer_set_.begin();
    if (iter == timer_set_.end()) return -1;
    const std::time_t dis = iter->expire() - get_tick();
    return dis > 0 ? dis : 0;
}

MSTL_END_NAMESPACE__