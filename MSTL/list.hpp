#ifndef MSTL_LIST_HPP__
#define MSTL_LIST_HPP__
#include "container.h"
#include "memory.hpp"
#include <memory>

MSTL_BEGIN_NAMESPACE__

template <typename T>
struct __list_node {
    typedef T                     value_type;
    typedef const T& reference;
    typedef __list_node<value_type> self;

    __list_node(reference _val = value_type(), self* _prev = nullptr, self* _next = nullptr)
        : _data(_val), _prev(_prev), _next(_next) {}

    value_type _data;
    self* _prev;
    self* _next;
};

template <typename T, typename Ref = T&, typename Ptr = T*>
struct list_iterator {
    // iterator_traits: 
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t                       difference_type;
    typedef T                               value_type;
    typedef Ptr                             pointer;
    typedef Ref                             reference;

    typedef list_iterator<T, Ref, Ptr>      self;
    typedef list_iterator<T, T&, T*>        iterator;
    typedef __list_node<T>*                 link_type;

    link_type _node;

    list_iterator(link_type x = nullptr) : _node(x) {}
    list_iterator(const iterator& x) : _node(x._node) {}
    self& operator =(const iterator& rh) { 
    	if(*this == rh) return *this;
		this->_node = rh._node; 
		return *this;
	}
    ~list_iterator() = default;

    reference operator *() const { return this->_node->_data; }
    pointer operator->() const { return &(operator*()); }
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

template <typename T, typename Alloc = default_standard_alloc<__list_node<T>>>
class list : public container {
public:
    typedef T                       value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef const T& const_reference;
    typedef list<T, Alloc>          self;
    typedef __list_node<T>            node_type;
    typedef node_type* link_type;
    typedef list_iterator<T, T&, T*>              iterator;
    typedef list_iterator<T, const T&, const T*>  const_iterator;

    static const char* const __type__;

private:
    typedef Alloc                   list_node_allocator;

    link_type _node;
    list_node_allocator alloc;
    size_type _size;

    inline void __range_check(int _pos) const {
        Exception(this->__in_boundary(_pos), new RangeError());
    }
    bool __in_boundary(int _pos) const {
        if (_pos < 0) return false;
        else return _pos < this->_size ? true : false;
    }
    link_type get_node() {
        return alloc.allocate();
    }
    void put_node(link_type p) {
        alloc.deallocate(p);
    }
    link_type create_node(const value_type& x) {
        link_type p = get_node();
        construct(&(p->_data), x);
        return p;
    }
    void destroy_node(link_type p) {
        destroy(p);
        put_node(p);
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
public:
    void __det__(std::ostream& _out = std::cout) const {
        split_line(_out);
        _out << "type: " << __type__ << std::endl;
        _out << "check type: " << check_type<self>() << std::endl;
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
    void __show_size_only(std::ostream& _out) const {
        _out << "size: " << _size << std::endl;
    }

    void empty_initialize() {
        _node = new node_type();
        _node->_prev = _node->_next = _node;
    }
    list() {
        empty_initialize();
    }
    list(size_type n) {
        empty_initialize();
        while (n--) push_back(value_type());
    };
    list(size_type n, const_reference x) {
        empty_initialize();
        while (n--) push_back(x);
    };
    template<typename InputIterator>
    list(InputIterator first, InputIterator last) {
        empty_initialize();
        while (first != last) {
            push_back(*first);
            ++first;
        }
    };
    list(std::initializer_list<T> l) {
        empty_initialize();
        for (auto iter = l.begin(); iter != l.end(); ++iter) {
            push_back(*iter);
        }
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
    ~list() {
        link_type _ptr = this->_node->_next;
        while (_ptr != this->_node) {
            link_type q = _ptr;
            _ptr = _ptr->_next;
            destroy_node(q);
        }
        destroy_node(this->_node);
    }

    iterator begin() { return this->_node->_next; }
    size_type size() const { return _size; }
    iterator end() { return this->_node; }
    const_iterator const_begin() const { return this->_node->_next; }
    const_iterator const_end() const { return this->_node; }
    bool empty() const {
        return this->_node->_next == this->_node ||
            link_type(this->_node->_next)->_next == this->_node;
    }
    const_reference front() const { return this->_node->_next->_data; }
    const_reference back() const { return this->_node->_prev->_data; }
    void pop_front() { this->erase(begin()); }
    void pop_back() { this->erase(this->_node->_prev); }
    void push_front(const_reference x) { insert(begin(), x); }
    void push_back(const_reference x) { insert(end(), x); }
    iterator insert(iterator position, const_reference x) {
        link_type temp = create_node(x);
        temp->_next = position._node;
        temp->_prev = position._node->_prev;
        position._node->_prev->_next = temp;
        position._node->_prev = temp;
        ++_size;
        return temp;
    }
    void insert(iterator position, size_type n, const_reference x) {
        link_type cur = position._node;
        link_type temp;
        while (n--) {
            temp = create_node(x);
            temp->_next = cur;
            temp->_prev = cur->_prev;
            cur->_prev->_next = temp;
            cur->_prev = temp;
            cur = temp;
            ++_size;
        }
    }
    template<typename InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
        for (--last; first != last; --last)
            position = insert(position, *last);
        insert(position, *last);
    }

    iterator erase(iterator position) {
        if (empty()) return this->_node;
        link_type ret = position._node->_next;
        position._node->_prev->_next = position._node->_next;
        position._node->_next->_prev = position._node->_prev;
        destroy_node(position._node);
        --_size;
        return ret;
    }
    iterator erase(iterator first, iterator last) {
        while (first != last) first = erase(first);
        return first;
    }
    void clear() {
        link_type cur = _node->_next;
        while (cur != _node) {
            link_type temp = cur;
            cur = cur->_next;
            destroy_node(temp);
            --_size;
        }
        _node->_prev = _node;
        _node->_next = _node;
    }
    void remove(const value_type& x) {
        auto iter = begin(), last = end();
        while (iter != last) {
            if (*iter == x) iter = erase(iter);
            else ++iter;
        }
    }
    void splice(iterator position, self x) {
        if (!x.empty()) transfer(position, x.begin(), x.end());
    }
    void splice(iterator position, self, iterator i) {
        iterator j = i;
        ++j;
        if (i == position || j == position) return;
        transfer(position, i, j);
    }
    void splice(iterator position, self, iterator first, iterator last) {
        if (first != last) transfer(position, first, last);
    }
    void merge(self x) {
        iterator first1 = begin(), first2 = x.begin();
        iterator last1 = end(), last2 = x.end();
        while (first1 != last1 && first2 != last2) {
            if (*first1 <= *first2) {
                ++first1;
            }
            else {
                iterator temp = first2;
                ++temp;
                transfer(first1, first2, temp);
                first2 = temp;
            }
        }
        if (first2 != last2) {
            transfer(last1, first2, last2);
        }
    }
    void reverse() {
        if (empty()) return;
        iterator position = begin();
        iterator first = position, last = end();
        for (++first; first != last;) {
            iterator temp = first;
            transfer(position, first, ++temp);
            position = first;
            first = temp;
        }
    }

    void swap(self& _lis) noexcept {
        std::swap(_lis._node, this->_node);
    }
    void unique() {
        iterator first = begin();
        iterator last = end();
        if (first == last) return;
        while (first != last) {
            iterator next = first;
            ++next;
            if (next != last && *first == *next) erase(first);
            first = next;
        }
    }
    void sort() {
        if (empty()) return;
        link_type p = _node->_next->_next;
        while (p != _node) {
            auto temp = p->_data;
            link_type prev = p->_prev;
            while (prev != _node && prev->_data > temp) {
                prev->_next->_data = prev->_data;
                prev = prev->_prev;
            }
            prev->_next->_data = temp;
            p = p->_next;
        }
    }

    const_reference at(int _pos) const {
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
    const_reference operator [](int _pos) const {
        return this->at(_pos);
    }
    reference operator [](int _pos) {
        return this->at(_pos);
    }
};
template <typename T, typename Alloc>
const char* const list<T, Alloc>::__type__ = "list";

template<typename T, typename Alloc>
std::ostream& operator <<(std::ostream& _out, const list<T, Alloc>& _list) {
    _list.__show_data_only(_out);
    return _out;
}

MSTL_END_NAMESPACE__

#endif // MSTL_LIST_HPP__
