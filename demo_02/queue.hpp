#ifndef QUEUE_H
#define QUEUE_H
#include "deque.hpp"
namespace MSTL {
    template<typename T, typename Sequence = deque<T>>
    class queue {
    public:
        typedef typename Sequence::value_type       value_type;
        typedef typename Sequence::difference_type  difference_type;
        typedef typename Sequence::size_type        size_type;
        typedef typename Sequence::reference        reference;
        typedef typename Sequence::reference_const  reference_const;

        static const char* const __type__;
    private:
        Sequence seq;
    public:
        void __det__(std::ostream& _out = std::cout) const {
            split_line(_out);
            _out << "type: " << __type__ << std::endl;
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
}
template <typename T, typename Sequence>
bool operator==(const MSTL::queue<T, Sequence>& x, const MSTL::queue<T, Sequence>& y) {
    return x.seq == y.seq;
}
template <typename T, typename Sequence>
bool operator<(const MSTL::queue<T, Sequence>& x, const MSTL::queue<T, Sequence>& y) {
    return x.seq < y.seq;
}

#endif // QUEUE_H
