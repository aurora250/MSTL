#ifndef MSTL_LIST_HPP__
#define MSTL_LIST_HPP__
#include "memory.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T>
struct __list_node {
    typedef T                       value_type;
    typedef const T&                reference;
    typedef __list_node<value_type> self;

    __list_node(reference _val = value_type(), self* _prev = nullptr, self* _next = nullptr)
        : data_(_val), prev_(_prev), next_(_next) {}

    value_type data_;
    self* prev_;
    self* next_;
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

    link_type node_;

    list_iterator(link_type x = nullptr) : node_(x) {}
    list_iterator(const iterator& x) : node_(x.node_) {}
    self& operator =(const iterator& rh) { 
    	if(*this == rh) return *this;
		this->node_ = rh.node_; 
		return *this;
	}
    ~list_iterator() = default;

    reference operator *() const { return this->node_->data_; }
    pointer operator->() const { return &(operator*()); }
    self& operator ++() {
        this->node_ = this->node_->next_;
        return *this;
    }
    self operator ++(int) {
        self _old = *this;
        ++(*this);
        return _old;
    }
    self& operator --() {
        this->node_ = this->node_->prev_;
        return *this;
    }
    self operator --(int) {
        self _old = *this;
        --(*this);
        return _old;
    }
    bool operator ==(const self& _iter) const { return this->node_ == _iter.node_; }
    bool operator !=(const self& _iter) const { return this->node_ != _iter.node_; }
};

template <typename T, typename Alloc = default_standard_alloc<__list_node<T>>>
class list {
public:
    typedef T                       value_type;
    typedef T&                      reference;
    typedef T*                      pointer;
    typedef const T&                const_reference;
    typedef list<T, Alloc>          self;
    typedef __list_node<T>          node_type;
    typedef node_type*              link_type;
    typedef list_iterator<T, T&, T*>              iterator;
    typedef list_iterator<T, const T&, const T*>  const_iterator;
    typedef size_t size_type;
private:
    typedef Alloc list_node_allocator;

    link_type node_;
    list_node_allocator alloc_;
    size_type size_;

    inline void range_check(int _pos) const {
        Exception(this->in_boundary(_pos), RangeError());
    }
    inline bool in_boundary(int _pos) const {
        if (_pos < 0) return false;
        else return _pos < this->size_ ? true : false;
    }
    link_type get_node() {
        return alloc_.allocate();
    }
    void put_node(link_type p) {
        alloc_.deallocate(p);
    }
    link_type create_node(const value_type& x) {
        link_type p = get_node();
        construct(&(p->data_), x);
        return p;
    }
    void destroy_node(link_type p) {
        destroy(p);
        put_node(p);
    }
    void transfer(iterator _pos, iterator _first, iterator _last) {
        if (_pos == _last) return;
        _last.node_->prev_->next_ = _pos.node_;
        _first.node_->prev_->next_ = _last.node_;
        _pos.node_->prev_->next_ = _first.node_;
        link_type tmp = _pos.node_->prev_;
        _pos.node_->prev_ = _last.node_->prev_;
        _last.node_->prev_ = _first.node_->prev_;
        _first.node_->prev_ = tmp;
    }
public:
    void empty_initialize() {
        node_ = new node_type();
        node_->prev_ = node_->next_ = node_;
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
        link_type p = x.node_->next_;
        while (p != x.node_) {
            link_type q = new node_type(p->data);
            q->prev_ = node_->prev_;
            q->next_ = node_;
            node_->prev_->next_ = q;
            node_->prev_ = q;
            p = p->next_;
        }
        this->size_ = x.size_;
        return *this;
    }
    const self& operator =(self&& x) {
        if (*this == x) return *this;
        clear();
        this->node_ = x.node_;
        x.node_ = new node_type();
        x.node_->prev_ = x.node_->next_ = x.node_;
        this->size_ = x.size_;
        return *this;
    }
    ~list() {
        link_type _ptr = this->node_->next_;
        while (_ptr != this->node_) {
            link_type q = _ptr;
            _ptr = _ptr->next_;
            destroy_node(q);
        }
        destroy_node(this->node_);
    }

    iterator begin() { return this->node_->next_; }
    size_type size() const { return size_; }
    iterator end() { return this->node_; }
    const_iterator const_begin() const { return this->node_->next_; }
    const_iterator const_end() const { return this->node_; }
    bool empty() const {
        return this->node_->next_ == this->node_ ||
            link_type(this->node_->next_)->next_ == this->node_;
    }
    const_reference front() const { return this->node_->next_->data_; }
    const_reference back() const { return this->node_->prev_->data_; }
    void pop_front() { this->erase(begin()); }
    void pop_back() { this->erase(this->node_->prev_); }
    void push_front(const_reference x) { insert(begin(), x); }
    void push_back(const_reference x) { insert(end(), x); }
    iterator insert(iterator position, const_reference x) {
        link_type temp = create_node(x);
        temp->next_ = position.node_;
        temp->prev_ = position.node_->prev_;
        position.node_->prev_->next_ = temp;
        position.node_->prev_ = temp;
        ++size_;
        return temp;
    }
    void insert(iterator position, size_type n, const_reference x) {
        link_type cur = position.node_;
        link_type temp;
        while (n--) {
            temp = create_node(x);
            temp->next_ = cur;
            temp->prev_ = cur->prev_;
            cur->prev_->next_ = temp;
            cur->prev_ = temp;
            cur = temp;
            ++size_;
        }
    }
    template<typename InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
        for (--last; first != last; --last)
            position = insert(position, *last);
        insert(position, *last);
    }

    iterator erase(iterator position) {
        if (empty()) return this->node_;
        link_type ret = position.node_->next_;
        position.node_->prev_->next_ = position.node_->next_;
        position.node_->next_->prev_ = position.node_->prev_;
        destroy_node(position.node_);
        --size_;
        return ret;
    }
    iterator erase(iterator first, iterator last) {
        while (first != last) first = erase(first);
        return first;
    }
    void clear() {
        link_type cur = node_->next_;
        while (cur != node_) {
            link_type temp = cur;
            cur = cur->next_;
            destroy_node(temp);
            --size_;
        }
        node_->prev_ = node_;
        node_->next_ = node_;
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
        std::swap(_lis.node_, this->node_);
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
        link_type p = node_->next_->next_;
        while (p != node_) {
            auto temp = p->data_;
            link_type prev = p->prev_;
            while (prev != node_ && prev->data_ > temp) {
                prev->next_->data_ = prev->data_;
                prev = prev->prev_;
            }
            prev->next_->data_ = temp;
            p = p->next_;
        }
    }

    const_reference at(int _pos) const {
        this->range_check(_pos);
        const_iterator iter = const_begin();
        while (_pos--) iter++;
        return iter.node_->data_;
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

MSTL_END_NAMESPACE__

#endif // MSTL_LIST_HPP__
