#ifndef MSTL_STACK_HPP__
#define MSTL_STACK_HPP__
#include "deque.hpp"

MSTL_BEGIN_NAMESPACE__

template <typename T, typename Sequence = deque<T>>
class stack : public object {
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::difference_type  difference_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::reference_const  reference_const;
    typedef stack<T, Sequence>                  self;

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

    stack() : seq(Sequence()) {}
    ~stack() = default;

    void push(const value_type& x) { seq.push_back(x); }
    void pop() { seq.pop_back(); }
    reference top() { return seq.back(); }
    reference_const top() const { return seq.back(); }
    size_type size() { return seq.size(); }
    bool empty() { return seq.empty(); }
};
template<typename T, typename Sequence>
const char* const stack<T, Sequence>::__type__ = "stack";


template<typename T, typename Sequence>
bool operator==(const stack<T, Sequence>& x, const stack<T, Sequence>& y) {
    return x.seq == y.seq;
}
template<typename T, typename Sequence>
bool operator<(const stack<T, Sequence>& x, const stack<T, Sequence>& y) {
    return x.seq < y.seq;
}

MSTL_END_NAMESPACE__

#endif // MSTL_STACK_HPP__
