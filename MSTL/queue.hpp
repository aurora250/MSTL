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
    typedef typename Sequence::reference_const  reference_const;
    typedef queue<T, Sequence>                  self;
private:
    Sequence seq_;

    template <typename T, typename Sequence>
    friend MSTL_CONSTEXPR void detailof(const queue<T, Sequence>& pq, std::ostream& _out);
public:
    queue() : seq_(Sequence()) {}
    ~queue() = default;

    void push(const value_type& x) { seq_.push_back(x); }
    void emplace(const value_type& x) { seq_.push_back(x); }
    void pop() { seq_.pop_front(); }
    reference front() { return seq_.front(); }
    reference_const front()const { return seq_.front(); }
    reference back() { return seq_.back(); }
    reference_const back()const { return seq_.back(); }
    size_type size() { return seq_.size(); }
    bool empty() { return seq_.empty(); }
};
template <typename T, typename Sequence>
MSTL_CONSTEXPR bool operator==(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.seq_ == y.seq_;
}
template <typename T, typename Sequence>
MSTL_CONSTEXPR bool operator<(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.seq_ < y.seq_;
}
template <typename T, typename Sequence>
MSTL_CONSTEXPR void detailof(const queue<T, Sequence>& pq, std::ostream& _out = std::cout) {
    split_line(_out);
    _out << "type: " << check_type<queue<T, Sequence>>() << std::endl;
    _out << "size: " << pq.size() << std::endl;
    _out << "data: " << std::flush;
    __show_data_only(pq.seq_, _out);
    _out << std::endl;
    split_line(_out);
}

template<typename T, typename Sequence = vector<T>,
    typename Compare = greater<typename Sequence::value_type>>
    requires(concepts::BinaryFunction<Compare>)
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
    friend MSTL_CONSTEXPR void detailof(const priority_queue<T, Sequence, Compare>& pq, std::ostream& _out);
public:
    priority_queue() : seq_(), comp_() {}
    explicit priority_queue(const Compare& x) :seq_(), comp_(x) {}
    template <typename InputIterator>
    priority_queue(InputIterator first, InputIterator last) : seq_(first, last) {
        make_heap(seq_.begin(), seq_.end(), comp_);
    }
    template<typename InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x) :
        seq_(first, last), comp_(x) {
        make_heap(seq_.begin(), seq_.end(), comp_);
    }
    bool empty() const { return seq_.empty(); }
    size_type size() const { return seq_.size(); }
    const_reference top() const { return seq_.front(); }
    void push(const_reference x) {
        MSTL_TRY__{
            seq_.push_back(x);
            push_heap(seq_.begin(), seq_.end(), comp_);
        }
        MSTL_CATCH_UNWIND_THROW_M__(seq_.clear());
    }
    void pop() {
        MSTL_TRY__{
            pop_heap(seq_.begin(), seq_.end(), comp_);
            seq_.pop_back();
        }
        MSTL_CATCH_UNWIND_THROW_M__(seq_.clear());
    }
};

template <typename T, typename Sequence, typename Compare>
MSTL_CONSTEXPR void detailof(const priority_queue<T, Sequence, Compare>& pq, std::ostream& _out = std::cout) {
    split_line(_out);
    _out << "type: " << check_type<priority_queue<T, Sequence, Compare>>() << std::endl;
    _out << "size: " << pq.size() << std::endl;
    _out << "data: " << std::flush;
    __show_data_only(pq.seq_, _out);
    _out << std::endl;
    split_line(_out);
}

MSTL_END_NAMESPACE__

#endif // MSTL_QUEUE_HPP__
