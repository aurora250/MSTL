#ifndef MSTL_LIST_HPP__
#define MSTL_LIST_HPP__
#include "memory.hpp"
#include "functor.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T>
struct __list_node {
    using value_type    = T;
    using reference     = const T&;
    using self          = __list_node<value_type>;

    __list_node(reference _val = value_type(), self* _prev = nullptr, self* _next = nullptr) noexcept
        : data_(_val), prev_(_prev), next_(_next) {}

    value_type data_;
    self* prev_;
    self* next_;
};

template <typename T, typename Ref = T&, typename Ptr = T*>
struct list_iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = Ptr;
    using reference         = Ref;
    using self              = list_iterator<T, Ref, Ptr>;
    using iterator          = list_iterator<T, T&, T*>;
    using link_type         = __list_node<T>*;

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
};
template <typename T, typename Ref, typename Ptr>
MSTL_NODISCARD bool operator ==(const list_iterator<T, Ref, Ptr>& x,
    const list_iterator<T, Ref, Ptr>& y) noexcept {
    return x.node_ == y.node_;
}
template <typename T, typename Ref, typename Ptr>
MSTL_NODISCARD bool operator !=(const list_iterator<T, Ref, Ptr>& x,
    const list_iterator<T, Ref, Ptr>& y) noexcept {
    return x.node_ != y.node_;
}


template <typename T, typename Alloc = standard_allocator<__list_node<T>>>
class list {
#ifdef MSTL_VERSION_20__	
    static_assert(is_allocator_v<Alloc>, "Alloc type is not a standard allocator type.");
#endif
    static_assert(is_same_v<__list_node<T>, typename Alloc::value_type>, "allocator type mismatch.");
    static_assert(is_object_v<T>, "list only contains object types.");

public:
    using value_type        = T;
    using reference         = T&;
    using pointer           = T*;
    using const_reference   = const T&;
    using node_type         = __list_node<T>;
    using link_type         = node_type*;
    using iterator          = list_iterator<T, T&, T*>;
    using const_iterator    = list_iterator<T, const T&, const T*>;
    using reverse_iterator  = MSTL::reverse_iterator<iterator>;
    using const_reverse_iterator = MSTL::reverse_iterator<const_iterator>;
    using size_type         = size_t;
    using allocator_type    = Alloc;
    using self              = list<T, Alloc>;

private:
    link_type node_;
    size_type size_;
    allocator_type alloc_;

    inline void range_check(size_type position) const noexcept {
        MSTL_DEBUG_VERIFY__(position < size_, "list index out of ranges.");
    }
    inline void range_check(iterator position) const noexcept {
        MSTL_DEBUG_VERIFY__(MSTL::distance(static_cast<const_iterator>(position), cend()) >= 0, 
            "list iterator out of ranges."
        );
    }

    template <typename... Args>
    link_type create_node(Args&&... args) {
        link_type p = alloc_.allocate();
        MSTL::construct(&(p->data_), MSTL::forward<Args>(args)...);
        return p;
    }
    void destroy_node(link_type p) noexcept {
        MSTL::destroy(p);
        alloc_.deallocate(p);
    }

    void empty_init() {
        node_ = create_node();
        node_->prev_ = node_->next_ = node_;
    }

public:
    list() {
        empty_init();
    }
    explicit list(size_type n) {
        empty_init();
        while (n--) push_back(value_type());
    };

    list(size_type n, const T& x) {
        empty_init();
        while (n--) push_back(x);
    };

    template <typename Iterator> 
        requires(input_iterator<Iterator>)
    list(Iterator first, Iterator last) {
        empty_init();
        while (first != last) {
            push_back(*first);
            ++first;
        }
    };

    list(std::initializer_list<T> l) : list(l.begin(), l.end()) {}

    self& operator =(std::initializer_list<T> l) {
        clear();
        insert(begin(), l.begin(), l.end());
        return *this;
    }

    list(const self& x) : list(x.cbegin(), x.cend()) {}

    self& operator =(const self& x) {
        if (MSTL::addressof(x) == this) return *this;
        clear();
        link_type p = x.node_->next_;
        while (p != x.node_) {
            link_type q = create_node(p->data);
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
        empty_init();
        swap(x);
    }
    self& operator =(self&& x) noexcept {
        if (MSTL::addressof(x) == this) return *this;
        clear();
        swap(x);
        return *this;
    }

    ~list() {
        link_type p = this->node_->next_;
        while (p != this->node_) {
            link_type q = p;
            p = p->next_;
            destroy_node(q);
        }
        destroy_node(this->node_);
    }

    MSTL_NODISCARD iterator begin() noexcept { return this->node_->next_; }
    MSTL_NODISCARD iterator end() noexcept { return this->node_; }
    MSTL_NODISCARD const_iterator cbegin() const noexcept { return this->node_->next_; }
    MSTL_NODISCARD const_iterator cend() const noexcept { return this->node_; }
    MSTL_NODISCARD reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    MSTL_NODISCARD reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    MSTL_NODISCARD const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
    MSTL_NODISCARD const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    MSTL_NODISCARD size_type size() const noexcept { return size_; }
    MSTL_NODISCARD size_type max_size() const noexcept { return static_cast<size_type>(-1); }
    MSTL_NODISCARD bool empty() const noexcept {
        return this->node_->next_ == this->node_ || link_type(this->node_->next_)->next_ == this->node_;
    }

    MSTL_NODISCARD allocator_type get_allocator() { return allocator_type(); }

    MSTL_NODISCARD reference front() noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "front called on empty list");
        return this->node_->next_->data_;
    }
    MSTL_NODISCARD const_reference front() const noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "front called on empty list");
        return this->node_->next_->data_;
    }
    MSTL_NODISCARD reference back() noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "back called on empty list");
        return this->node_->prev_->data_;
    }
    MSTL_NODISCARD const_reference back() const noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "back called on empty list");
        return this->node_->prev_->data_;
    }

    template <typename... U>
    iterator emplace(iterator position, U&&... args) {
        range_check(position);
        link_type temp = (create_node)(MSTL::forward<U>(args)...);
        temp->next_ = position.node_;
        temp->prev_ = position.node_->prev_;
        position.node_->prev_->next_ = temp;
        position.node_->prev_ = temp;
        ++size_;
        return temp;
    }

    template <typename... Args>
    iterator emplace_back(Args&&... args) {
        return (emplace)(end(), MSTL::forward<Args>(args)...);
    }
    template <typename... Args>
    iterator emplace_front(Args&&... args) {
        return (emplace)(begin(), MSTL::forward<Args>(args)...);
    }

    void push_front(const T& x) { insert(begin(), x); }
    void push_front(T&& x) { insert(begin(), MSTL::forward<T>(x)); }
    void push_back(const T& x) { insert(end(), x); }
    void push_back(T&& x) { insert(end(), MSTL::forward<T>(x)); }

    void pop_front() noexcept { erase(begin()); }
    void pop_back() noexcept { erase(node_->prev_); }

    void assign(size_type count, const T& value) {
        clear();
        insert(begin(), count, value);
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    void assign(Iterator first, Iterator last) {
        clear();
        insert(begin(), first, last);
    }

    void assign(std::initializer_list<T> l) {
        assign(l.begin(), l.end());
    }

    iterator insert(iterator position, const T& x) {
        return (emplace)(position, x);
    }
    iterator insert(iterator position, T&& x) {
        return (emplace)(position, MSTL::forward<T>(x));
    }

    template <typename InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
        range_check(position);
        Exception(MSTL::distance(first, last) >= 0, StopIterator("deque insert out of ranges."));
        for (--last; first != last; --last)
            position = insert(position, *last);
        insert(position, *last);
    }
    void insert(iterator position, std::initializer_list<T> l) {
        insert(position, l.begin(), l.end());
    }

    void insert(iterator position, size_type n, const T& x) {
        range_check(position);
        link_type cur = position.node_;
        link_type temp;
        while (n--) {
            temp = (create_node)(x);
            temp->next_ = cur;
            temp->prev_ = cur->prev_;
            cur->prev_->next_ = temp;
            cur->prev_ = temp;
            cur = temp;
            ++size_;
        }
    }

    iterator erase(iterator position) noexcept {
        range_check(position);
        if (empty()) return this->node_;
        link_type ret = position.node_->next_;
        position.node_->prev_->next_ = position.node_->next_;
        position.node_->next_->prev_ = position.node_->prev_;
        destroy_node(position.node_);
        --size_;
        return ret;
    }
    iterator erase(iterator first, iterator last) noexcept {
        Exception(MSTL::distance(first, last) >= 0, StopIterator("list erase out of ranges."));
        while (first != last) 
            first = erase(first);
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

    void swap(self& x) noexcept {
        MSTL::swap(x.node_, this->node_);
        MSTL::swap(x.size_, this->size_);
    }

    void transfer(iterator position, iterator first, iterator last) {
        range_check(position);
        range_check(first);
        range_check(last);
        Exception(MSTL::distance(first, last) >= 0, StopIterator("list transfer index out of ranges."));
        if (position == last) return;
        last.node_->prev_->next_ = position.node_;
        first.node_->prev_->next_ = last.node_;
        position.node_->prev_->next_ = first.node_;
        link_type tmp = position.node_->prev_;
        position.node_->prev_ = last.node_->prev_;
        last.node_->prev_ = first.node_->prev_;
        first.node_->prev_ = tmp;
    }

    template <typename Pred>
    void remove_if(Pred pred) {
        iterator iter = begin(), last = end();
        while (iter != last) {
            if (pred(*iter)) iter = erase(iter);
            else ++iter;
        }
    }
    void remove(const T& x) {
        return this->remove_if([&](const T& oth) -> bool { return oth == x; });
    }

    void splice(iterator position, self& x) {
        if (!x.empty()) 
            transfer(position, x.begin(), x.end());
    }
    void splice(iterator position, self&, iterator i) {
        iterator j = i;
        ++j;
        if (i == position || j == position) return;
        transfer(position, i, j);
    }
    void splice(iterator position, self&, iterator first, iterator last) {
        if (first != last) transfer(position, first, last);
    }

    template <typename Pred>
    void merge(self& x, Pred pred) {
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
    void merge(self& x) {
        this->merge(x, MSTL::less<T>());
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
        x.clear();
    }
    void merge(self&& x) {
        this->merge(MSTL::forward<self>(x), MSTL::less<T>());
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
    void unique() noexcept {
        unique(MSTL::equal_to<T>());
    }

    template <typename Pred>
    void sort(Pred pred) {
        if (empty()) return;
        link_type p = node_->next_->next_;
        while (p != node_) {
            T temp = p->data_;
            link_type prev = p->prev_;
            while (prev != node_ && pred(temp, prev->data_)) {
                prev->next_->data_ = prev->data_;
                prev = prev->prev_;
            }
            prev->next_->data_ = temp;
            p = p->next_;
        }
    }
    void sort() {
        sort(MSTL::less<T>());
    }

    MSTL_NODISCARD const_reference at(size_type position) const {
        range_check(position);
        const_iterator iter = cbegin();
        while (position--) iter++;
        return iter.node_->data_;
    }
    MSTL_NODISCARD reference at(size_type position) {
        return const_cast<reference>(
            static_cast<const self*>(this)->at(position)
            );
    }
    MSTL_NODISCARD const_reference operator [](size_type position) const {
        return this->at(position);
    }
    MSTL_NODISCARD reference operator [](size_type position) {
        return this->at(position);
    }
};
#if MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename Iterator, typename Alloc>
list(Iterator, Iterator, Alloc = Alloc()) -> list<iter_val_t<Iterator>, Alloc>;
#endif

template <typename T, typename Alloc>
MSTL_NODISCARD bool operator ==(const list<T, Alloc>& lh, const list<T, Alloc>& rh) noexcept {
    return lh.size() == rh.size() && MSTL::equal(lh.cbegin(), lh.cend(), rh.cbegin());
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator !=(const list<T, Alloc>& lh, const list<T, Alloc>& rh) noexcept {
    return !(lh == rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator <(const list<T, Alloc>& lh, const list<T, Alloc>& rh) noexcept {
    return MSTL::lexicographical_compare(lh.cbegin(), lh.cend(), rh.cbegin(), rh.cend());
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator >(const list<T, Alloc>& lh, const list<T, Alloc>& rh) noexcept {
    return rh < lh;
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator <=(const list<T, Alloc>& lh, const list<T, Alloc>& rh) noexcept {
    return !(lh > rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator >=(const list<T, Alloc>& lh, const list<T, Alloc>& rh) noexcept {
    return !(lh < rh);
}
template <typename T, typename Alloc>
void swap(list<T, Alloc>& lh, list<T, Alloc>& rh) noexcept {
    lh.swap(rh);
}

MSTL_END_NAMESPACE__

#endif // MSTL_LIST_HPP__
