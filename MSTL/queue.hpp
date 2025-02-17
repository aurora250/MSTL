#ifndef MSTL_QUEUE_HPP__
#define MSTL_QUEUE_HPP__
#include "deque.hpp"
#include "functor.hpp"
#include "heap.hpp"
MSTL_BEGIN_NAMESPACE__

template<typename T, typename Sequence = deque<T>>
class queue {
public:
    using value_type        = typename Sequence::value_type;
    using difference_type   = typename Sequence::difference_type;
    using size_type         = typename Sequence::size_type;
    using reference         = typename Sequence::reference;
    using const_reference   = typename Sequence::const_reference;
    using self              = queue<T, Sequence>;

    static_assert(is_object_v<T>, "queue only contains object types.");
    static_assert(is_same_v<T, value_type>, "queue require consistent types.");

private:
    Sequence seq_;

    template <typename T1, typename Sequence1>
    friend void detailof(const queue<T1, Sequence1>&, std::ostream&);

public:

    queue() = default;
    explicit queue(const Sequence& x) : seq_(x) {}
    explicit queue(Sequence&& x) noexcept(is_nothrow_move_constructible_v<Sequence>)
        : seq_(MSTL::move(x)) {}
    ~queue() = default;

    MSTL_NODISCARD size_type size() const noexcept(noexcept(seq_.size())) { return seq_.size(); }
    MSTL_NODISCARD bool empty() const noexcept(noexcept(seq_.empty())) { return seq_.empty(); }

    MSTL_NODISCARD reference front() noexcept(noexcept(seq_.front())) { return seq_.front(); }
    MSTL_NODISCARD const_reference front() const noexcept(noexcept(seq_.front())) { return seq_.front(); }
    MSTL_NODISCARD reference back() noexcept(noexcept(seq_.back())) { return seq_.back(); }
    MSTL_NODISCARD const_reference back() const noexcept(noexcept(seq_.back())) { return seq_.back(); }

    void push(const T& x) { seq_.push_back(x); }
    void push(T&& x) { seq_.push_back(MSTL::move(x)); }

    void pop() noexcept(noexcept(seq_.pop_front())) { seq_.pop_front(); }

    template <typename... Args>
    decltype(auto) emplace(Args&&... args) { return seq_.emplace_back(MSTL::forward<Args>(args)...); }

    void swap(self& x) noexcept(is_nothrow_swappable_v<Sequence>) {
        MSTL::swap(seq_, x.seq_);
    }

    MSTL_NODISCARD const Sequence& get_container() const noexcept { return seq_; }
};
#if MSTL_SUPPORT_DEDUCTION_GUIDES__
template <class Sequence>
queue(Sequence) -> queue<typename Sequence::value_type, Sequence>;
#endif

template <typename T, typename Sequence>
MSTL_NODISCARD bool operator ==(const queue<T, Sequence>& x, const queue<T, Sequence>& y)
noexcept(noexcept(x.get_container() == y.get_container())) {
    return x.get_container() == y.get_container();
}
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator !=(const queue<T, Sequence>& x, const queue<T, Sequence>& y)
noexcept(noexcept(x.get_container() != y.get_container())) {
    return x.get_container() != y.get_container();
}
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator <(const queue<T, Sequence>& x, const queue<T, Sequence>& y)
    noexcept(noexcept(x.get_container() < y.get_container())) {
    return x.get_container() < y.get_container();
}
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator >(const queue<T, Sequence>& x, const queue<T, Sequence>& y)
noexcept(noexcept(x.get_container() > y.get_container())) {
    return x.get_container() > y.get_container();
}
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator <=(const queue<T, Sequence>& x, const queue<T, Sequence>& y)
noexcept(noexcept(x.get_container() <= y.get_container())) {
    return x.get_container() <= y.get_container();
}
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator >=(const queue<T, Sequence>& x, const queue<T, Sequence>& y)
noexcept(noexcept(x.get_container() >= y.get_container())) {
    return x.get_container() >= y.get_container();
} 
template <typename T, typename Sequence>
    requires(is_swappable_v<Sequence>)
void swap(queue<T, Sequence>& lh, queue<T, Sequence>& rh) noexcept(noexcept(lh.swap(rh))) {
    lh.swap(rh);
}


template<typename T, typename Sequence = vector<T>,
    typename Compare = less<typename Sequence::value_type>>
class priority_queue {
public:
    using value_type        = typename Sequence::value_type;
    using size_type         = typename Sequence::size_type;
    using reference         = typename Sequence::reference;
    using const_reference   = typename Sequence::const_reference;
    using self              = priority_queue<T, Sequence>;

    static_assert(is_object_v<T>, "priority queue only contains object types.");
    static_assert(is_same_v<T, value_type>, "priority queue require consistent types.");

private:
    Sequence seq_;
    Compare comp_;

    template <typename T1, typename Sequence1, typename Compare1>
    friend void detailof(const priority_queue<T1, Sequence1, Compare1>&, std::ostream&);

    void make_heap_inside() {
        MSTL::make_heap(seq_.begin(), seq_.end(), comp_);
    }

public:
    priority_queue() = default;

    explicit priority_queue(const Compare& comp) 
        noexcept(is_nothrow_default_constructible_v<Sequence> && is_nothrow_copy_constructible_v<Compare>) 
        : seq_(), comp_(comp) {}

    priority_queue(const Compare& comp, const Sequence& seq) : seq_(seq), comp_(comp) {
        make_heap_inside();
    }

    priority_queue(const Compare& comp, Sequence&& seq)
        noexcept(is_nothrow_move_constructible_v<Sequence> && is_nothrow_copy_constructible_v<Compare>)
        : seq_(MSTL::move(seq)), comp_(comp) {
        make_heap_inside();
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    priority_queue(Iterator first, Iterator last, const Sequence& seq) : seq_(seq) {
        seq_.insert(seq_.end(), first, last);
        make_heap_inside();
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    priority_queue(Iterator first, Iterator last) : seq_(first, last) {
        make_heap_inside();
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    priority_queue(Iterator first, Iterator last, const Compare& comp) :
        seq_(first, last), comp_(comp) {
        make_heap_inside();
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    priority_queue(Iterator first, Iterator last, const Compare& comp, const Sequence& seq) :
        seq_(seq), comp_(comp) {
        seq_.insert(seq_.end(), first, last);
        make_heap_inside();
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    priority_queue(Iterator first, Iterator last, const Compare& comp, Sequence&& seq) :
        seq_(MSTL::move(seq)), comp_(comp) {
        seq_.insert(seq_.end(), first, last);
        make_heap_inside();
    }

    MSTL_NODISCARD bool empty() const noexcept(noexcept(seq_.empty())) { return seq_.empty(); }
    MSTL_NODISCARD size_type size() const noexcept(noexcept(seq_.size())) { return seq_.size(); }

    MSTL_NODISCARD const_reference top() const noexcept(seq_.front()) { return seq_.front(); }

    void push(const value_type& x) {
        seq_.push_back(x);
        MSTL::push_heap(seq_.begin(), seq_.end(), comp_);
    }
    void push(value_type&& x) {
        seq_.push_back(MSTL::move(x));
        MSTL::push_heap(seq_.begin(), seq_.end(), comp_);
    }

    void pop() {
        MSTL::pop_heap(seq_.begin(), seq_.end(), comp_);
        seq_.pop_back();
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        seq_.emplace_back(MSTL::forward<Args>(args)...);
        MSTL::push_heap(seq_.begin(), seq_.end(), comp_);
    }

    void swap(self& x) noexcept(is_nothrow_swappable_v<Sequence> && is_nothrow_swappable_v<Compare>) {
        MSTL::swap(seq_, x.seq_);
        MSTL::swap(comp_, x.comp_);
    }
};
#ifdef MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename Compare, typename Sequence>
priority_queue(Compare, Sequence) -> priority_queue<typename Sequence::value_type, Sequence, Compare>;

template <typename Iterator, typename Compare = less<iter_val_t<Iterator>>,
    typename Sequence = vector<iter_val_t<Iterator>>>
priority_queue(Iterator, Iterator, Compare = Compare(), Sequence = Sequence())
-> priority_queue<iter_val_t<Iterator>, Sequence, Compare>;
#endif

template <typename T, typename Sequence, typename Compare>
    requires(is_swappable_v<Sequence> && is_swappable_v<Compare>)
void swap(priority_queue<T, Sequence, Compare>& lh, priority_queue<T, Sequence, Compare>& rh)
    noexcept(noexcept(lh.swap(rh))) {
    lh.swap(rh);
}

MSTL_END_NAMESPACE__
#endif // MSTL_QUEUE_HPP__
