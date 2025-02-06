#ifndef MSTL_STACK_HPP__
#define MSTL_STACK_HPP__
#include "deque.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T, typename Sequence = deque<T>>
class stack {
public:
    using value_type        = typename Sequence::value_type;
    using difference_type   = typename Sequence::difference_type;
    using size_type         = typename Sequence::size_type;
    using reference         = typename Sequence::reference;
    using const_reference   = typename Sequence::const_reference;
    using self              = stack<T, Sequence>;

    static_assert(is_object_v<T>, "stack only contains object types.");
    static_assert(is_same_v<T, value_type>, "stack require consistent types.");

private:
    Sequence seq_;

    template <typename T1, typename Sequence1>
    friend void detailof(const stack<T1, Sequence1>&, std::ostream&);

public:
    stack() = default;
    explicit stack(const Sequence& seq) : seq_(seq) {}
    explicit stack(Sequence&& seq) noexcept(is_nothrow_move_constructible_v<Sequence>) 
        : seq_(MSTL::move(seq)) {}
    ~stack() = default;

    MSTL_NODISCARD size_type size() const noexcept(noexcept(seq_.size())) { return seq_.size(); }
    MSTL_NODISCARD bool empty() const noexcept(noexcept(seq_.empty())) { return seq_.empty(); }

    MSTL_NODISCARD reference top() noexcept(noexcept(seq_.back())) { return seq_.back(); }
    MSTL_NODISCARD const_reference top() const noexcept(noexcept(seq_.back())) { return seq_.back(); }

    template <typename... Args>
    decltype(auto) emplace(Args&&... args) { return seq_.emplace(MSTL::forward<Args>(args)...); }

    void push(const value_type& x) { seq_.push_back(x); }
    void push(value_type&& x) { seq_.push_back(MSTL::move(x)); }

    void pop() noexcept(noexcept(seq_.pop_back())) { seq_.pop_back(); }

    void swap(self& x) noexcept(is_nothrow_swappable_v<Sequence>) {
        MSTL::swap(seq_, x.seq_);
    }

    MSTL_NODISCARD const Sequence& get_container() const noexcept { return seq_; }
};
#ifdef MSTL_SUPPORT_DEDUCTION_GUIDES__
template <class Sequence>
stack(Sequence) -> stack<typename Sequence::value_type, Sequence>;
#endif

template<typename T, typename Sequence>
MSTL_NODISCARD bool operator ==(const stack<T, Sequence>& x, const stack<T, Sequence>& y)
noexcept(noexcept(x.get_container() == y.get_container())) {
    return x.get_container() == y.get_container();
}
template<typename T, typename Sequence>
MSTL_NODISCARD bool operator !=(const stack<T, Sequence>& x, const stack<T, Sequence>& y)
noexcept(noexcept(x.get_container() != y.get_container())) {
    return x.get_container() != y.get_container();
}
template<typename T, typename Sequence>
MSTL_NODISCARD bool operator <(const stack<T, Sequence>& x, const stack<T, Sequence>& y)
noexcept(noexcept(x.get_container() < y.get_container())) {
    return x.get_container() < y.get_container();
}
template<typename T, typename Sequence>
MSTL_NODISCARD bool operator >(const stack<T, Sequence>& x, const stack<T, Sequence>& y)
    noexcept(noexcept(x.get_container() > y.get_container())) {
    return x.get_container() > y.get_container();
}
template<typename T, typename Sequence>
MSTL_NODISCARD bool operator <=(const stack<T, Sequence>& x, const stack<T, Sequence>& y)
    noexcept(noexcept(x.get_container() <= y.get_container())) {
    return x.get_container() <= y.get_container();
}
template<typename T, typename Sequence>
MSTL_NODISCARD bool operator >=(const stack<T, Sequence>& x, const stack<T, Sequence>& y)
noexcept(noexcept(x.get_container() >= y.get_container())) {
    return x.get_container() >= y.get_container();
}
template <typename T, typename Sequence>
void swap(stack<T, Sequence>& lh, stack<T, Sequence>& rh) noexcept(noexcept(lh.swap(rh))) {
    lh.swap(rh);
}

MSTL_END_NAMESPACE__
#endif // MSTL_STACK_HPP__
