#ifndef MSTL_QUEUE_HPP__
#define MSTL_QUEUE_HPP__
#include "deque.hpp"
#include "functor.hpp"
#include "vector.hpp"
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

    static const char* const __type__;
private:
    Sequence seq;
public:
    queue() : seq(Sequence()) {}
    ~queue() = default;

    void push(const value_type& x) { seq.push_back(x); }
    void emplace(const value_type& x) { seq.push_back(x); }
    void pop() { seq.pop_front(); }
    reference front() { return seq.front(); }
    reference_const front()const { return seq.front(); }
    reference back() { return seq.back(); }
    reference_const back()const { return seq.back(); }
    size_type size() { return seq.size(); }
    bool empty() { return seq.empty(); }
};
template <typename T, typename Sequence>
const char* const queue<T, Sequence>::__type__ = "queue";

template <typename T, typename Sequence>
bool operator==(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.seq == y.seq;
}
template <typename T, typename Sequence>
bool operator<(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.seq < y.seq;
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
    Sequence seq;
    Compare comp;

    template <typename T, typename Sequence, typename Compare>
    friend void detailof(const priority_queue<T, Sequence, Compare>& pq, std::ostream& _out);
public:
    priority_queue() : seq(), comp() {}
    explicit priority_queue(const Compare& x) :seq(), comp(x) {}
    template <typename InputIterator>
    priority_queue(InputIterator first, InputIterator last) : seq(first, last) {
        make_heap(seq.begin(), seq.end(), comp);
    }
    template<typename InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x) :
        seq(first, last), comp(x) {
        make_heap(seq.begin(), seq.end(), comp);
    }
    bool empty() const { return seq.empty(); }
    size_type size() const { return seq.size(); }
    const_reference top() const { return seq.front(); }
    void push(const_reference x) {
        MSTL_TRY__{
            seq.push_back(x);
            push_heap(seq.begin(), seq.end(), comp);
        }
        MSTL_CATCH_UNWIND_THROW_M__(seq.clear());
    }
    void pop() {
        MSTL_TRY__{
            pop_heap(seq.begin(), seq.end(), comp);
            seq.pop_back();
        }
        MSTL_CATCH_UNWIND_THROW_M__(seq.clear());
    }
};

template <typename T, typename Sequence, typename Compare>
void detailof(const priority_queue<T, Sequence, Compare>& pq, std::ostream& _out = std::cout) {
    split_line(_out);
    _out << "type: " << check_type<priority_queue<T, Sequence, Compare>>() << std::endl;
    _out << "size: " << pq.size() << std::endl;
    _out << "data: " << std::flush;
    __show_data_only(pq.seq, _out);
    _out << std::endl;
    split_line(_out);
}

MSTL_END_NAMESPACE__

#endif // MSTL_QUEUE_HPP__
