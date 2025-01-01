#ifndef MSTL_LIST_HPP__
#define MSTL_LIST_HPP__
#include "memory.hpp"
#include "functor.hpp"
#include "algobase.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T>
struct __list_node {
    typedef T                       value_type;
    typedef const T&                reference;
    typedef __list_node<value_type> self;

    __list_node(reference _val = value_type(), self* _prev = nullptr, self* _next = nullptr) noexcept
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

    list_iterator(link_type x = nullptr) noexcept : node_(x) {}
    list_iterator(const iterator& x) noexcept : node_(x.node_) {}
    self& operator =(const iterator& rh) noexcept {
    	if(*this == rh) return *this;
		this->node_ = rh.node_; 
		return *this;
	}
    ~list_iterator() = default;

    MSTL_NODISCARD reference operator *() const noexcept { return this->node_->data_; }
    MSTL_NODISCARD pointer operator->() const noexcept { return &(operator*()); }
    self& operator ++() noexcept {
        this->node_ = this->node_->next_;
        return *this;
    }
    self operator ++(int) noexcept {
        self _old = *this;
        ++(*this);
        return _old;
    }
    self& operator --() noexcept {
        this->node_ = this->node_->prev_;
        return *this;
    }
    self operator --(int) noexcept {
        self _old = *this;
        --(*this);
        return _old;
    }
    MSTL_NODISCARD bool operator ==(const self& _iter) const noexcept { return this->node_ == _iter.node_; }
    MSTL_NODISCARD bool operator !=(const self& _iter) const noexcept { return this->node_ != _iter.node_; }
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

    inline void range_check(size_type _pos) const noexcept {
        Exception(this->in_boundary(_pos), RangeError());
    }
    inline bool in_boundary(size_type _pos) const noexcept {
        if (_pos < 0) return false;
        else return _pos < this->size_ ? true : false;
    }
    link_type get_node() {
        return alloc_.allocate();
    }
    void put_node(link_type p) noexcept {
        alloc_.deallocate(p);
    }
    link_type create_node(T&& x) {
        link_type p = get_node();
        MSTL::construct(&(p->data_), std::forward<T>(x));
        return p;
    }
    void destroy_node(link_type p) noexcept {
        MSTL::destroy(p);
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
    explicit list(size_type n) {
        empty_initialize();
        while (n--) push_back(value_type());
    };
    list(size_type n, const_reference x) {
        empty_initialize();
        while (n--) push_back(x);
    };
    template <typename InputIterator>
    list(InputIterator first, InputIterator last) {
        empty_initialize();
        while (first != last) {
            push_back(*first);
            ++first;
        }
    };
    list(const std::initializer_list<T>& l) {
        empty_initialize();
        for (auto iter = l.begin(); iter != l.end(); ++iter) {
            push_back((T)*iter);
        }
    }
    self& operator =(const std::initializer_list<T>& l) {
        this->clear();
        for (auto it = l.begin(); it != l.end(); ++it) {
            this->push_back(*it);
        }
        return *this;
    }
    list(const self& x) : list(x.const_begin(), x.const_end()) {}
    self& operator =(const self& x) {
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
    list(self&& x) noexcept {
        empty_initialize();
        swap(std::forward<self>(x));
    }
    self& operator =(self&& x) noexcept {
        if (*this == x) return *this;
        clear();
        swap(std::forward<self>(x));
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

    MSTL_NODISCARD iterator begin() noexcept { return this->node_->next_; }
    MSTL_NODISCARD const_iterator const_begin() const noexcept { return this->node_->next_; }
    MSTL_NODISCARD iterator end() noexcept { return this->node_; }
    MSTL_NODISCARD const_iterator const_end() const noexcept { return this->node_; }

    MSTL_NODISCARD size_type size() const noexcept { return size_; }
    MSTL_NODISCARD bool empty() const noexcept {
        return this->node_->next_ == this->node_ || 
            link_type(this->node_->next_)->next_ == this->node_;
    }

    MSTL_NODISCARD reference front() noexcept { return this->node_->next_->data_; }
    MSTL_NODISCARD const_reference front() const noexcept { return this->node_->next_->data_; }
    MSTL_NODISCARD reference back() noexcept { return this->node_->prev_->data_; }
    MSTL_NODISCARD const_reference back() const noexcept { return this->node_->prev_->data_; }

    void pop_front() noexcept { this->erase(begin()); }
    void pop_back() noexcept { this->erase(this->node_->prev_); }
    void push_front(T&& x) { insert(begin(), std::forward<T>(x)); }
    void push_back(T&& x) { insert(end(), std::forward<T>(x)); }

    iterator insert(iterator position, T&& x) {
        link_type temp = create_node(std::forward<T>(x));
        temp->next_ = position.node_;
        temp->prev_ = position.node_->prev_;
        position.node_->prev_->next_ = temp;
        position.node_->prev_ = temp;
        ++size_;
        return temp;
    }
    void insert(iterator position, size_type n, T&& x) {
        link_type cur = position.node_;
        link_type temp;
        while (n--) {
            temp = create_node(std::forward<T>(x));
            temp->next_ = cur;
            temp->prev_ = cur->prev_;
            cur->prev_->next_ = temp;
            cur->prev_ = temp;
            cur = temp;
            ++size_;
        }
    }
    template <typename InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
        for (--last; first != last; --last)
            position = insert(position, *last);
        insert(position, *last);
    }

    iterator erase(iterator position) noexcept {
        if (empty()) return this->node_;
        link_type ret = position.node_->next_;
        position.node_->prev_->next_ = position.node_->next_;
        position.node_->next_->prev_ = position.node_->prev_;
        destroy_node(position.node_);
        --size_;
        return ret;
    }
    iterator erase(iterator first, iterator last) noexcept {
        while (first != last) first = erase(first);
        return first;
    }
    void clear() noexcept {
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

    void remove(T&& x) {
        return this->remove_if([&](const T& oth) -> bool { return oth == x; });
    }
    template <typename Pred>
    void remove_if(Pred pred) {
        auto iter = begin(), last = end();
        while (iter != last) {
            if (pred(*iter)) iter = erase(iter);
            else ++iter;
        }
    }

    void splice(iterator position, self&& x) {
        if (!x.empty()) transfer(position, x.begin(), x.end());
    }
    void splice(iterator position, self&&, iterator i) {
        iterator j = i;
        ++j;
        if (i == position || j == position) return;
        transfer(position, i, j);
    }
    void splice(iterator position, self&&, iterator first, iterator last) {
        if (first != last) transfer(position, first, last);
    }

    void merge(self&& x) {
        this->merge(std::forward<self>(x), MSTL::less<T>());
    }
    template <typename Pred>
    void merge(self&& x, Pred pred) {
        iterator first1 = begin(), first2 = x.begin();
        iterator last1 = end(), last2 = x.end();
        while (first1 != last1 && first2 != last2) {
            if (!pred(*first2, *first1)) {
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

    void reverse() noexcept {
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

    void swap(self&& _lis) noexcept {
        std::swap(_lis.node_, this->node_);
        std::swap(_lis.size_, this->size_);
    }

    void unique() noexcept {
        unique(MSTL::equal_to<T>());
    }
    template <typename Pred>
    void unique(Pred pred) noexcept {
        iterator first = begin();
        iterator last = end();
        if (first == last) return;
        while (first != last) {
            iterator next = first;
            ++next;
            if (next != last && pred(*first, *next)) erase(first);
            first = next;
        }
    }
    void sort() {
        sort(MSTL::less<T>());
    }
    template <typename Pred>
    void sort(Pred pred) {
        if (empty()) return;
        link_type p = node_->next_->next_;
        while (p != node_) {
            auto temp = p->data_;
            link_type prev = p->prev_;
            while (prev != node_ && pred(temp, prev->data_)) {
                prev->next_->data_ = prev->data_;
                prev = prev->prev_;
            }
            prev->next_->data_ = temp;
            p = p->next_;
        }
    }

    const_reference at(size_type _pos) const noexcept {
        this->range_check(_pos);
        const_iterator iter = const_begin();
        while (_pos--) iter++;
        return iter.node_->data_;
    }
    reference at(size_type _pos) noexcept {
        return const_cast<reference>(
            static_cast<const self*>(this)->at(_pos)
            );
    }
    const_reference operator [](size_type _pos) const noexcept {
        return this->at(_pos);
    }
    reference operator [](size_type _pos) noexcept {
        return this->at(_pos);
    }
};
template <class T, class Alloc>
void swap(list<T, Alloc>& lh, list<T, Alloc>& rh) noexcept {
    lh.swap(rh);
}
template <class T, class Alloc>
MSTL_NODISCARD bool operator ==(const list<T, Alloc>& lh, const list<T, Alloc>& rh) noexcept {
    return lh.size() == rh.size() && MSTL::equal(lh.const_begin(), lh.const_end(), rh.const_begin());
}

MSTL_END_NAMESPACE__

#endif // MSTL_LIST_HPP__
