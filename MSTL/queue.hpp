#ifndef MSTL_QUEUE_HPP__
#define MSTL_QUEUE_HPP__
#include "deque.hpp"
#include "functor.hpp"
#include "vector.hpp"
#include "heap.hpp"

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
    void __det__(std::ostream& _out = std::cout) const {
        split_line(_out);
        _out << "type: " << __type__ << std::endl;
        _out << "check type: " << check_type<self>() << std::endl;
        seq.__show_size_only(_out);
        _out << "data: " << std::flush;
        seq.__show_data_only(_out);
        _out << std::endl;
        split_line(_out);
    }
    void __show_data_only(std::ostream& _out) const {
        seq.__show_data_only(_out);
    }

    queue() : seq(Sequence()) {}
    ~queue() = default;

    void push(const value_type& x) { seq.push_back(x); }
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

template <typename T, typename Sequence>
std::ostream& operator <<(std::ostream& _out, const queue<T, Sequence>& _tar) {
    _tar.__show_data_only(_out);
    return _out;
}


template<typename T, typename Sequence = vector<T>,
    typename Compare = greater<typename Sequence::value_type>>
class priority_queue : public container {
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;
    typedef priority_queue<T, Sequence>         self;

    static const char* const __type__;
private:
    Sequence seq;
    Compare comp;

public:
    void __det__(std::ostream& _out = std::cout) const {
        split_line(_out);
        _out << "type: " << __type__ << std::endl;
        _out << "check type: " << check_type<self>() << std::endl;
        seq.__show_size_only(_out);
        _out << "data: " << std::flush;
        seq.__show_data_only(_out);
        _out << std::endl;
        split_line(_out);
    }
    void __show_data_only(std::ostream& _out) const {
        seq.__show_data_only(_out);
    }
    void __show_size_only(std::ostream& _out) const {
        seq.__show_size_only(_out);
    }

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
const char* const priority_queue<T, Sequence, Compare>::__type__ = "priority_queue";

template <typename T, typename Sequence, typename Compare>
std::ostream& operator <<(std::ostream& _out, const priority_queue<T, Sequence, Compare>& _prq) {
    _prq.__show_data_only(_out);
    return _out;
}

MSTL_END_NAMESPACE__

#endif // MSTL_QUEUE_HPP__
