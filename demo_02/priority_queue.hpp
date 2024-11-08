#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H
#include "functor.hpp"
#include "vector.hpp"
#include "algo.hpp"
#include <algorithm>

namespace MSTL {
    template<typename T, typename Sequence = vector<T>, 
        typename Compare = greater<typename Sequence::value_type>>
    class priority_queue {
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

        priority_queue() : seq(Sequence()) {}
        explicit priority_queue(const Compare& x) :seq(), comp(x) {}
        template<typename InputIterator>
        priority_queue(InputIterator first, InputIterator last) : seq(first, last) {
            std::make_heap(seq.begin(), seq.end(), comp);
        }
        template<typename InputIterator>
        priority_queue(InputIterator first, InputIterator last, const Compare& x) :
            seq(first, last), comp(x) {
            std::make_heap(seq.begin(), seq.end(), comp);
        }
        bool empty() const { return seq.empty(); }
        size_type size() const { return seq.size(); }
        const_reference top() const { return seq.front(); }
        void push(const value_type& x) {
            seq.push_back(x);
            std::push_heap(seq.begin(), seq.end(), comp);
        }
        void pop() {
            std::pop_heap(seq.begin(), seq.end(), comp);
            seq.pop_back();
        }
    };
    template <typename T, typename Sequence, typename Compare>
    const char* const priority_queue<T, Sequence, Compare>::__type__ = "priority_queue";
}

#endif
