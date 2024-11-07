#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP
#include "iterable.h"
#include <memory>

namespace MSTL {
    template <typename T>
    class doubly_node {
    public:
        typedef T                       value_type;
        typedef const T&                reference;
        typedef doubly_node<value_type> self;

        doubly_node(reference _val = value_type(), self* _prev = nullptr, self* _next = nullptr)
            : _data(_val), _prev(_prev), _next(_next) {}

        value_type _data;
        self* _prev;
        self* _next;
    };

    template <typename T, typename Ref = T&, typename Ptr = T*>
    struct doublylist_iterator {
        // iterator_traits: 
        typedef std::bidirectional_iterator_tag  iterator_category;
        typedef ptrdiff_t                        difference_type;
        typedef T                                value_type;
        typedef Ptr                              pointer;
        typedef Ref                              reference;

        typedef doublylist_iterator<T, Ref, Ptr> self;
        typedef doublylist_iterator<T, T&, T*>   iterator;
        typedef doubly_node<T>*                  link_type;

        link_type _node;

        doublylist_iterator(link_type x = nullptr) : _node(x) {}
        doublylist_iterator(const iterator& x) : _node(x._node) {}
        ~doublylist_iterator() = default;

        reference operator *() const { return this->_node->_data; }
        pointer operator ->() const { return &(this->_node->_data); }
        self& operator ++() {
            this->_node = this->_node->_next;
            return *this;
        }
        self operator ++(int) {
            self _old = *this;
            ++(*this);
            return _old;
        }
        self& operator --() {
            this->_node = this->_node->_prev;
            return *this;
        }
        self operator --(int) {
            self _old = *this;
            --(*this);
            return _old;
        }
        bool operator ==(const self& _iter) const { return this->_node == _iter._node; }
        bool operator !=(const self& _iter) const { return this->_node != _iter._node; }
    };

    template <typename T>
    doubly_node<T>* make_dnode(const T& value) {
        doubly_node<T> new_node(value);
        return &new_node;
    }

    template <typename T, typename Alloc = std::allocator<T>>
    class doublylist : public siterable {
    public:
        typedef T                       value_type;
        typedef T&                      reference;
        typedef const T&                reference_const;
        typedef doublylist<T>           self;
        typedef doubly_node<T>          node_type;
        typedef node_type*              link_type;
        typedef doublylist_iterator<T, T&, T*>              iterator;
        typedef doublylist_iterator<T, const T&, const T*>  const_iterator;

        static const char* const __type__;

    private:
        inline void __range_check(int _pos) const {
            Exception(this->__in_boundary(_pos), error_map[__error::RangeErr]);
        }
        link_type _node;
    public:
        void __det__(std::ostream& _out = std::cout) const {
            split_line(_out);
            _out << "type: " << __type__ << std::endl;
            this->__show_size_only(_out);
            _out << "data: " << std::flush;
            this->__show_data_only(_out);
            _out << std::endl;
            split_line(_out);
        }
        void __show_data_only(std::ostream& _out) const {
            _out << '[' << std::flush;
            const_iterator band = const_iterator(this->_node->_prev);
            for (const_iterator iter = const_begin(); iter != const_end(); ++iter) {
                _out << *iter << std::flush;
                if (iter != band) _out << " ," << std::flush;
            }
            _out << ']' << std::flush;
        }

        doublylist() : siterable(0), _node(new node_type()) {
            this->_node->_next = this->_node->_prev = this->_node;
        }
        explicit doublylist(const self& _list) : siterable(_list._size), _node(new node_type()) {
            link_type p = this->_node;
            for (link_type iter = _list._node->_next; iter != _list._node; iter = iter->_next) {
                link_type q = new node_type(iter->_data);
                q->_prev = p;
                q->_next = this->_node;
                p->_next = q;
                p = q;
            }
            p->_next = this->_node;
            this->_node->_prev = p;
        }
        explicit doublylist(self&& x) : siterable(x._size), _node(x._node) {
            x._node->_prev = x._node->_next = x._node;
        }
        const self& operator =(const self& x) {
            if (*this == x) return *this;
            this->clear();
            link_type p = x._node->_next;
            while (p != x._node) {
                link_type q = new node_type(p->data);
                q->_prev = _node->_prev;
                q->_next = _node;
                _node->_prev->_next = q;
                _node->_prev = q;
                p = p->_next;
            }
            this->_size = x._size;
            return *this;
        }
        const self& operator =(self&& x) {
            if (*this == x) return *this;
            clear();
            this->_node = x._node;
            x._node = new node_type();
            x._node->_prev = x._node->_next = x._node;
            this->_size = x._size;
            return *this;
        }
        ~doublylist() {
            link_type _ptr = this->_node->_next;
            while (_ptr != this->_node) {
                link_type q = _ptr;
                _ptr = _ptr->_next;
                delete q;
            }
            delete this->_node;
        }

        iterator begin() {
            return this->_node->_next;
        }
        iterator end() {
            return this->_node;
        }
        const_iterator const_begin() const {
            return this->_node->_next;
        }
        const_iterator const_end() const {
            return this->_node;
        }
        bool empty() const {
            return this->_node->_next == this->_node ||
                link_type(this->_node->_next)->_next == this->_node;
        }
        reference_const front() const {
            return this->_node->_next->_data;
        }
        reference_const back() const {
            return this->_node->_prev->_data;
        }
        void insert(reference_const _val, iterator _pos) {
            link_type tmp = new node_type(_val);
            tmp->_prev = _pos._node->_prev;
            tmp->_next = _pos._node;
            _pos._node->_prev->_next = tmp;
            _pos._node->_prev = tmp;
            this->_size++;
        }
        void push_front(reference_const _val) {
            this->insert(_val, begin());
        }
        void push_back(reference_const _val) {
            this->insert(_val, end());
        }
        void pop_front() {
            this->erase(begin());
        }
        void pop_back() {
            this->erase(this->_node->_prev);
        }
        void clear() {
            link_type p = this->_node->_next;
            while (p != this->_node) {
                link_type q = p;
                p = p->_next;
                delete p;
            }
            this->_node->_next = this->_node->_prev = this->_node;
            this->_size = 0;
        }
        const_iterator erase(iterator _pos) {
            if (empty()) return this->_node;
            _pos._node->_prev->_next = _pos._node->_next;
            _pos._node->_next->_prev = _pos._node->_prev;
            link_type temp = _pos._node->_next;
            delete _pos._node;
            this->_size--;
            return temp;
        }
        void swap(self& _lis) noexcept {
            std::swap(_lis._node, this->_node);
        }
        void transfer(iterator _pos, iterator _first, iterator _last) {
            if (_pos == _last) return;
            _last._node->_prev->_next = _pos._node;
            _first._node->_prev->_next = _last._node;
            _pos._node->_prev->_next = _first._node;
            link_type tmp = _pos._node->_prev;
            _pos._node->_prev = _last._node->_prev;
            _last._node->_prev = _first._node->_prev;
            _first._node->_prev = tmp;
        }
        void splice(iterator _pos, self& _tar) {
            if (_tar.empty()) return;
            transfer(_pos, _tar.begin(), _tar.end());
        }
        void splice(iterator _pos, self&, iterator _iter) {
            if (_pos == _iter || _pos == iterator(_iter._node->_next)) return;
            transfer(_pos, _iter, iterator(_iter._node->_next));
        }
        void splice(iterator _pos, self&, iterator _start, iterator _end) {
            if (_start == _end) return;
            transfer(_pos, _start, _end);
        }
        void merge(self& x) {
            iterator first1 = this->begin();
            iterator last1 = this->end();
            iterator first2 = x.begin();
            iterator last2 = x.end();
            while (first1 != last1 && first2 != last2) {
                if (*first2 < *first1) {
                    iterator next = first2;
                    transfer(first1, first2, ++next);
                    first2 = next;
                }
                else ++first1;
            }
            if (first2 != last2) transfer(end(), first2, last2);
        }
        void reverse() {
            if (empty()) return;
            for (iterator first = ++begin(); first != end(); ) {
                iterator old = first++;
                this->transfer(begin(), old, first);
            }
        }
        void sort() {
            if(empty()) return;
            self carry;
            self counter[64];
            int fill = 0;
            while (not empty()) {
                carry.splice(carry.begin(), *this, this->begin());
                int i = 0;
                while (i < fill && !counter[i].empty()) {
                    counter[i].merge(carry);
                    carry.swap(counter[i++]);
                }
                carry.swap(counter[i]);
                if (i == fill) ++fill;
            }
            for (int i = 1; i < fill; ++i) counter[i].merge(counter[i - 1]);
            swap(counter[fill - 1]);
        }
        reference_const at(int _pos) const {
            this->__range_check(_pos);
            const_iterator iter = const_begin();
            while (_pos--) iter++;
            return iter._node->_data;
        }
        reference at(int _pos) {
            return const_cast<reference>(
                static_cast<const self*>(this)->at(_pos)
                );
        }
        reference_const operator [](int _pos) const {
            return this->at(_pos);
        }
        reference operator [](int _pos) {
            return this->at(_pos);
        }
    };
    template <typename T, typename Alloc>
    const char* const doublylist<T, Alloc>::__type__ = "linkedlist";

    template<typename T, typename Alloc>
    std::ostream& operator <<(std::ostream& _out, const doublylist<T, Alloc>& _list) {
        _list.__show_data_only(_out);
        return _out;
    }
}
#endif
