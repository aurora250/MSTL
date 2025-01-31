#ifndef MSTL_QUEUE_HPP__
#define MSTL_QUEUE_HPP__
#include "deque.hpp"
#include "functor.hpp"
#include "vector.hpp"
#include "check_type.h"
#include "heap.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__

template<typename T, typename Sequence = deque<T>>
class queue {
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::difference_type  difference_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;
    typedef queue<T, Sequence>                  self;
private:
    Sequence seq_;

    template <typename T, typename Sequence>
    friend void detailof(const queue<T, Sequence>& pq, std::ostream& _out);
public:
    queue() = default;
    explicit queue(const Sequence& x) : seq_(x) {}
    explicit queue(Sequence&& x) 
        noexcept(NothrowMoveConstructible<Sequence>) : seq_(std::move(x)) {}
    ~queue() = default;
    template <typename U = value_type>
    void push(U&& x) { seq_.push_back(std::forward<U>(x)); }
    template <typename... U>
    void emplace(U&&... x) { seq_.emplace_back(std::forward<U>(x)...); }
    void pop() noexcept { seq_.pop_front(); }

    MSTL_NODISCARD reference front() { return seq_.front(); }
    MSTL_NODISCARD const_reference front() const noexcept { return seq_.front(); }
    MSTL_NODISCARD reference back() noexcept { return seq_.back(); }
    MSTL_NODISCARD const_reference back() const noexcept { return seq_.back(); }
    MSTL_NODISCARD const Sequence& get_container() const noexcept { return seq_; }

    MSTL_NODISCARD size_type size() const noexcept { return seq_.size(); }
    MSTL_NODISCARD bool empty() const noexcept { return seq_.empty(); }

    void swap(self&& x) {
        seq_.swap(std::forward<self>(x.seq_));
    }
};
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator ==(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.get_container() == y.get_container();
}
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator !=(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.get_container() != y.get_container();
}
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator <(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.get_container() < y.get_container();
}
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator >(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.get_container() > y.get_container();
}
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator <=(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.get_container() <= y.get_container();
}
template <typename T, typename Sequence>
MSTL_NODISCARD bool operator >=(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.get_container() >= y.get_container();
}
template <class T, class Sequence> requires(Swappable<Sequence>)
void swap(queue<T, Sequence>& lh, queue<T, Sequence>& rh) noexcept(noexcept(lh.swap(rh))) {
    lh.swap(rh);
}


template<typename T, typename Sequence = vector<T>,
    typename Compare = less<typename Sequence::value_type>>
class priority_queue {
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;
    typedef priority_queue<T, Sequence>         self;

private:
    Sequence seq_;
    Compare comp_;

    template <typename T, typename Sequence, typename Compare>
    friend void detailof(const priority_queue<T, Sequence, Compare>& pq, std::ostream& _out);
public:
    priority_queue() = default;
    explicit priority_queue(const Compare& x) 
        noexcept(NothrowDefaultConstructible<Sequence> && NothrowCopyConstructible<Compare>) 
        : seq_(), comp_(x) {}
    priority_queue(const Sequence& s, const Compare& c)
        noexcept(NothrowCopyConstructible<Sequence> && NothrowCopyConstructible<Compare>)
        : seq_(s), comp_(c) {}
    priority_queue(Sequence&& s, const Compare& c)
        noexcept(NothrowMoveConstructible<Sequence> && NothrowCopyConstructible<Compare>)
        : seq_(std::move(s)), comp_(c) {}

    template <typename Iterator>
        requires(InputIterator<Iterator>)
    priority_queue(Iterator first, Iterator last) : seq_(first, last) {
        MSTL::make_heap(seq_.begin(), seq_.end(), comp_);
    }
    template <typename Iterator>
        requires(InputIterator<Iterator>)
    priority_queue(Iterator first, Iterator last, const Compare& x) :
        seq_(first, last), comp_(x) {
        MSTL::make_heap(seq_.begin(), seq_.end(), comp_);
    }
    template <typename Iterator>
        requires(InputIterator<Iterator>)
    priority_queue(Iterator first, Iterator last, const Compare& c, const Sequence& s) :
        seq_(s), comp_(c) {
        seq_.insert(seq_.end(), first, last);
        MSTL::make_heap(seq_.begin(), seq_.end(), comp_);
    }
    template <typename Iterator>
        requires(InputIterator<Iterator>)
    priority_queue(Iterator first, Iterator last, const Compare& c, Sequence&& s) :
        seq_(std::move(s)), comp_(c) {
        seq_.insert(seq_.end(), first, last);
        MSTL::make_heap(seq_.begin(), seq_.end(), comp_);
    }

    MSTL_NODISCARD bool empty() const noexcept(noexcept(seq_.empty())) { return seq_.empty(); }
    MSTL_NODISCARD size_type size() const noexcept(noexcept(seq_.size())) { return seq_.size(); }
    MSTL_NODISCARD const_reference top() const noexcept(seq_.front()) { return seq_.front(); }

    void push(T&& x) {
        MSTL_TRY__{
            seq_.push_back(std::forward<T>(x));
        MSTL::push_heap(seq_.begin(), seq_.end(), comp_);
        }
        MSTL_CATCH_UNWIND_THROW_M__(seq_.clear());
    }
    void pop() {
        MSTL_TRY__{
            MSTL::pop_heap(seq_.begin(), seq_.end(), comp_);
            seq_.pop_back();
        }
        MSTL_CATCH_UNWIND_THROW_M__(seq_.clear());
    }

    void swap(self&& x) noexcept(NothrowSwappable<Sequence> && NothrowSwappable<Compare>) {
        std::swap(seq_, x.seq_);
        std::swap(comp_, x.comp_);
    }
};
template <class T, class Sequence, class Compare> requires(Swappable<Sequence> && Swappable<Compare>)
    void swap(priority_queue<T, Sequence, Compare>& lh, priority_queue<T, Sequence, Compare>& rh)
    noexcept(noexcept(lh.swap(rh))) {
    lh.swap(rh);
}

MSTL_END_NAMESPACE__
#endif // MSTL_QUEUE_HPP__
