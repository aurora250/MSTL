#ifndef MSTL_RB_TREE_H__
#define MSTL_RB_TREE_H__
#include "functor.hpp"
#include "algo.hpp"
MSTL_BEGIN_NAMESPACE__

static MSTL_CONSTEXPR bool RB_TREE_RED__ = false;
static MSTL_CONSTEXPR bool RB_TREE_BLACK__ = true;

struct __rb_tree_node_base;
struct __rb_tree_base_iterator;
template<typename T, typename Ref, typename Ptr>
struct rb_tree_iterator;
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
class rb_tree;

void rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root) noexcept;
void rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root) noexcept;
void rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) noexcept;
__rb_tree_node_base* rb_tree_rebalance_for_erase(
    __rb_tree_node_base* z, __rb_tree_node_base*& root,
    __rb_tree_node_base*& leftmost, __rb_tree_node_base*& rightmost) noexcept;

struct __rb_tree_node_base {
public:
    using color_type = bool;
    using base_ptr = __rb_tree_node_base*;

    friend __rb_tree_base_iterator;
    friend void rb_tree_rotate_left(__rb_tree_node_base*, __rb_tree_node_base*&) noexcept;
    friend void rb_tree_rotate_right(__rb_tree_node_base*, __rb_tree_node_base*&) noexcept;
    friend void rb_tree_rebalance(__rb_tree_node_base*, __rb_tree_node_base*&) noexcept;
    friend __rb_tree_node_base* rb_tree_rebalance_for_erase(
        __rb_tree_node_base*, __rb_tree_node_base*&,
        __rb_tree_node_base*&, __rb_tree_node_base*&) noexcept;

protected:
    color_type color_ = false;
    base_ptr parent_ = nullptr;
    base_ptr left_ = nullptr;
    base_ptr right_ = nullptr;

public:
    static base_ptr minimum(base_ptr x) noexcept;
    static base_ptr maximum(base_ptr x) noexcept;
};
template <typename T>
struct __rb_tree_node : public __rb_tree_node_base {
public:
    using link_type = __rb_tree_node<T>*;
    using value_type = T;

    friend rb_tree;
    friend rb_tree_iterator;

private:
    T data_{};

public:
    __rb_tree_node() = default;
    __rb_tree_node(const __rb_tree_node&) = delete;
    __rb_tree_node& operator =(const __rb_tree_node&) = delete;
    ~__rb_tree_node() = default;
};

struct __rb_tree_base_iterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using base_ptr          = __rb_tree_node_base::base_ptr;
    using difference_type   = ptrdiff_t;

protected:
    base_ptr node_;

public:
    void increment() noexcept;
    void decrement() noexcept;

    friend inline MSTL_NODISCARD bool operator ==(
        const __rb_tree_base_iterator& x, const __rb_tree_base_iterator& y) noexcept {
        return x.node_ == y.node_;
    }
    friend inline MSTL_NODISCARD bool operator !=(
        const __rb_tree_base_iterator& x, const __rb_tree_base_iterator& y) noexcept {
        return x.node_ != y.node_;
    }
};

template<typename T, typename Ref = T&, typename Ptr = T*>
struct rb_tree_iterator : public __rb_tree_base_iterator {
public:
    using value_type        = T;
    using reference         = Ref;
    using pointer           = Ptr;
    using iterator          = rb_tree_iterator<T, T&, T*>;
    using const_iterator    = rb_tree_iterator<T, const T&, const T*>;
    using const_reference   = const T&;
    using link_type         = __rb_tree_node<T>*;
    using self              = rb_tree_iterator<T, Ref, Ptr>;

    friend rb_tree;

    rb_tree_iterator() : node_(nullptr) {}
    rb_tree_iterator(link_type x) { node_ = x; }
    rb_tree_iterator(const iterator& it) { node_ = it.node_; }
    ~rb_tree_iterator() = default;

    MSTL_NODISCARD reference operator *() const noexcept { return link_type(node_)->data_; }
    MSTL_NODISCARD pointer operator ->() const noexcept { return &(operator*()); }

    self& operator ++() noexcept {
        increment();
        return *this;
    }
    self& operator ++(int) noexcept {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator --() noexcept {
        decrement();
        return *this;
    }
    self operator --(int) noexcept {
        self tmp = *this;
        --*this;
        return tmp;
    }
};


template <typename Key, typename Value, typename KeyOfValue, typename Compare, 
    typename Alloc = standard_allocator<__rb_tree_node<Value>>>
class rb_tree {
private:
    using base_ptr          = __rb_tree_node_base*;
    using node_type         = __rb_tree_node<Value>;
    using color_type        = bool;
public:
    using key_type          = Key;
    using value_type        = Value;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using link_type         = node_type*;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using allocator_type    = Alloc;
    using self              = rb_tree<Key, Value, KeyOfValue, Compare, Alloc>;
    using iterator          = rb_tree_iterator<value_type, reference, pointer>;
    using const_iterator    = rb_tree_iterator<value_type, const_reference, const_pointer>;
    using reverse_iterator  = MSTL::reverse_iterator<iterator>;
    using const_reverse_iterator = MSTL::reverse_iterator<const_iterator>;

private:
    link_type header_;
    Compare key_compare_;
    KeyOfValue extracter_;
    compressed_pair<allocator_type, size_t> size_pair_{ default_construct_tag{}, 0 };

    template <typename... Args>
    link_type create_node(Args... args) {
        link_type tmp = size_pair_.get_base().allocate();
        MSTL_TRY__{
            MSTL::construct(&tmp->data_, MSTL::forward<Args>(args)...);
        }
        MSTL_CATCH_UNWIND_THROW_M__(destroy_node(tmp));
        return tmp;
    }
    link_type copy_node(link_type x) {
        link_type tmp = (create_node)(x->data_);
        tmp->color_ = x->color_;
        tmp->left_ = nullptr;
        tmp->right_ = nullptr;
        return tmp;
    }
    void destroy_node(link_type p) noexcept {
        if (p == nullptr) return;
        destroy(&p->data_);
        size_pair_.get_base().deallocate(p);
    }

    link_type& root() const noexcept { return (link_type&)header_->parent_; }
    link_type& leftmost() const noexcept { return (link_type&)header_->left_; }
    link_type& rightmost() const noexcept { return (link_type&)header_->right_; }

    static link_type& left(link_type x) noexcept { return (link_type&)(x->left_); }
    static link_type& right(link_type x) noexcept { return (link_type&)(x->right_); }
    static link_type& parent(link_type x) noexcept { return (link_type&)(x->parent_); }
    static const Key& key(link_type x) noexcept { return KeyOfValue()(x->data_); }
    static const Key& key(base_ptr x) noexcept { return KeyOfValue()(link_type(x)->data_); }

    static link_type minimum(link_type x) noexcept {
        return (link_type)__rb_tree_node_base::minimum(x);
    }
    static link_type maximum(link_type x) noexcept {
        return (link_type)__rb_tree_node_base::maximum(x);
    }

    iterator insert_node_into(base_ptr bx, base_ptr by, link_type p) {
        link_type x = (link_type)bx;
        link_type y = (link_type)by;
        if (y == header_ || x != nullptr || key_compare_(key(p), key(y))) {
            left(y) = p;
            if (y == header_) {
                root() = p;
                rightmost() = p;
            }
            else if (y == leftmost()) leftmost() = p;
        }
        else {
            right(y) = p;
            if (y == rightmost()) rightmost() = p;
        }
        parent(p) = y;
        left(p) = nullptr;
        right(p) = nullptr;
        (rb_tree_rebalance)(p, header_->parent_);
        ++size_pair_.value;
        return iterator(p);
    }

    link_type copy_under_node(link_type x, link_type parent) {
        link_type top = copy_node(x);
        top->parent_ = parent;
        MSTL_TRY__{
            if (x->right_ != nullptr) 
                top->right_ = copy_under_node(right(x), top);
            parent = top;
            x = left(x);
            while (x != nullptr) {
                link_type y = copy_node(x);
                parent->left_ = y;
                y->parent_ = parent;
                if (x->right_ != nullptr) 
                    y->right_ = copy_under_node(right(x), y);
                parent = y;
                x = left(x);
            }
        }
        MSTL_CATCH_UNWIND_THROW_U__(erase_under_node(top));
        return top;
    }
    void erase_under_node(link_type x) noexcept {
        while (x == nullptr) return;
        erase_under_node(right(x));
        link_type y = left(x);
        destroy_node(x);
        x = y;
    }

    void header_init() {
        header_ = size_pair_.get_base().allocate();
        header_->color_ = RB_TREE_RED__;
        root() = nullptr;
        leftmost() = header_;
        rightmost() = header_;
    }

    void copy_from(const self& x) {
        if (x.root() == nullptr) {
            root() = nullptr;
            leftmost() = header_;
            rightmost() = header_;
        }
        else {
            MSTL_TRY__{
              root() = copy_under_node(x.root(), header_);
            }
            MSTL_CATCH_UNWIND_THROW_M__(size_pair_.get_base().deallocate(header_));
            leftmost() = minimum(root());
            rightmost() = maximum(root());
        }
        size_pair_.value = x.size_pair_.value;
    }

    pair<iterator, bool> insert_unique_node(link_type p) {
        link_type y = header_;
        link_type x = root();
        bool comp = true;
        while (x != nullptr) {
            y = x;
            comp = key_compare_(key(p), key(x));
            x = comp ? left(x) : right(x);
        }
        iterator j(y);
        if (comp) {
            if (j == begin())
                return pair<iterator, bool>(insert_node_into(x, y, p), true);
            else --j;
        }
        if (key_compare_(key(link_type(j.node_)), key(p)))
            return pair<iterator, bool>(insert_node_into(x, y, p), true);
        return pair<iterator, bool>(j, false);
    }
    iterator insert_equal_node(link_type p) {
        link_type y = header_;
        link_type x = root();
        while (x != nullptr) {
            y = x;
            x = key_compare_(key(p), key(x)) ? left(x) : right(x);
        }
        return insert_node_into(x, y, p);
    }

public:
    rb_tree() {}

    explicit rb_tree(const Compare& comp) : key_compare_(comp) {
        header_init();
    }

    rb_tree(const self& x) 
        : size_pair_(x.size_pair_), key_compare_(x.key_compare_), extracter_(x.extracter_) {
        header_init();
        copy_from(x);
    }
    self& operator =(const self& x) {
        if (MSTL::addressof(x) == this) return *this;
        clear();
        copy_from(x);
        return *this;
    }

    rb_tree(self&& x) 
        noexcept(is_nothrow_move_constructible_v<Compare> && is_nothrow_move_constructible_v<KeyOfValue>)
        : size_pair_(x.size_pair_), header_(MSTL::move(x.header_)), key_compare_(MSTL::move(x.key_compare_)),
        extracter_(MSTL::move(x.extracter_)) {}

    self& operator =(self&& x) noexcept(noexcept(swap(x))) {
        if (MSTL::addressof(x) == this) return *this;
        clear();
        swap(x);
        return *this;
    }

    ~rb_tree() {
        clear();
        size_pair_.get_base().deallocate(header_);
    }

    MSTL_NODISCARD iterator begin() noexcept { return leftmost(); }
    MSTL_NODISCARD iterator end() noexcept { return header_; }
    MSTL_NODISCARD const_iterator cbegin() const noexcept { return leftmost(); }
    MSTL_NODISCARD const_iterator cend() const noexcept { return header_; }
    MSTL_NODISCARD reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    MSTL_NODISCARD reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    MSTL_NODISCARD const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
    MSTL_NODISCARD const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    MSTL_NODISCARD size_type size() const noexcept { return size_pair_.value; }
    MSTL_NODISCARD size_type max_size() const noexcept { return static_cast<size_type>(-1); }
    MSTL_NODISCARD bool empty() const noexcept { return size_pair_.value == 0; }

    MSTL_NODISCARD allocator_type get_allocator() const noexcept { return allocator_type(); }

    MSTL_NODISCARD Compare key_comp() const noexcept(is_nothrow_copy_constructible_v<Compare>) {
        return key_compare_;
    }

    template <typename... Args>
    pair<iterator, bool> emplace_unique(Args&&... args) {
        link_type tmp = (create_node)(MSTL::forward<Args>(args)...);
        return (insert_unique_node)(tmp);
    }
    pair<iterator, bool> insert_unique(const value_type& v) {
        return (emplace_unique)(v);
    }
    pair<iterator, bool> insert_unique(value_type&& v) {
        return (emplace_unique)(MSTL::move(v));
    }
    template <typename... Args>
    iterator emplace_unique_hint(iterator position, Args&&... args) {
        link_type tmp = (create_node)(MSTL::forward<Args>(args)...);
        if (position.node_ == header_->left_) {
            if (size() > 0 && key_compare_(key(tmp), key(position.node_)))
                return insert_node_into(position.node_, position.node_, tmp);
            else
                return insert_unique_node(tmp).first;
        }
        else if (position.node_ == header_) {
            if (key_compare_(key(rightmost()), key(tmp)))
                return insert_node_into(nullptr, rightmost(), tmp);
            else
                return insert_unique_node(tmp).first;
        }
        else {
            iterator before = position;
            --before;
            if (key_compare_(key(before.node_), key(tmp)) &&
                key_compare_(key(tmp), key(position.node_)))
                if (right(link_type(before.node_)) == nullptr)
                    return insert_node_into(nullptr, before.node_, tmp);
                else
                    return insert_node_into(position.node_, position.node_, tmp);
            else
                return insert_unique_node(tmp).first;
        }
    }
    iterator insert_unique(iterator position, const value_type& v) {
        return (emplace_unique_hint)(position, v);
    }
    iterator insert_unique(iterator position, value_type&& v) {
        return (emplace_unique_hint)(position, MSTL::move(v));
    }
    template <typename Iterator, enable_if_t<
        is_input_iter_v<Iterator>, int> = 0>
    void insert_unique(Iterator first, Iterator last) {
        for (; first != last; ++first) insert_unique(*first);
    }

    template <typename... Args>
    iterator emplace_equal(Args&&... args) {
        link_type tmp = (create_node)(MSTL::forward<Args>(args)...);
        return (insert_equal_node)(tmp);
    }
    iterator insert_equal(const value_type& v) {
        return (emplace_equal)(v);
    }
    iterator insert_equal(value_type&& v) {
        return (emplace_equal)(MSTL::move(v));
    }
    template <typename... Args>
    iterator emplace_equal_hint(iterator position, Args&&... args) {
        link_type tmp = (create_node)(MSTL::forward<Args>(args)...);
        if (position.node_ == header_->left_) {
            if (size() > 0 && key_compare_(key(tmp), key(position.node_)))
                return insert_node_into(position.node_, position.node_, tmp);
            else
                return insert_equal_node(tmp);

        }
        else if (position.node_ == header_) {
            if (!key_compare_(key(tmp), key(rightmost())))
                return insert_node_into(nullptr, rightmost(), tmp);
            else
                return insert_equal_node(tmp);
        }
        else {
            iterator before = position;
            --before;
            if (!key_compare_(key(tmp), key(before.node_)) &&
                !key_compare_(key(position.node_), key(tmp)))
                if (right(link_type(before.node_)) == nullptr)
                    return insert_node_into(nullptr, before.node_, tmp);
                else
                    return insert_node_into(position.node_, position.node_, tmp);
            else
                return insert_equal_node(tmp);
        }
    }
    iterator insert_equal(iterator position, const value_type& v) {
        return (emplace_equal_hint)(position, v);
    }
    iterator insert_equal(iterator position, value_type&& v) {
        return (emplace_equal_hint)(position, MSTL::move(v));
    }
    template <typename Iterator, enable_if_t<
        is_input_iter_v<Iterator>, int> = 0>
    void insert_equal(Iterator first, Iterator last) {
        for (; first != last; ++first) insert_equal(*first);
    }

    size_type erase(const key_type& k) noexcept {
        pair<iterator, iterator> p = equal_range(k);
        size_type n = MSTL::distance(p.first, p.second);
        erase(p.first, p.second);
        return n;
    }
    void erase(iterator position) noexcept {
        link_type y = (link_type)
            rb_tree_rebalance_for_erase(position.node_, header_->parent_, header_->left_, header_->right_);
        destroy_node(y);
        --size_pair_.value;
    }
    void erase(iterator first, iterator last) noexcept {
        if (first == begin() && last == end()) 
            clear();
        else 
            while (first != last) erase(first++);
    }

    void clear() noexcept {
        if (size_pair_.value == 0) return;
        erase_under_node(root());
        leftmost() = header_;
        root() = nullptr;
        rightmost() = header_;
        size_pair_.value = 0;
    }

    void swap(self& x) noexcept(is_nothrow_swappable_v<Compare> && is_nothrow_swappable_v<KeyOfValue>) {
        MSTL::swap(header_, x.header_);
        MSTL::swap(size_pair_, x.size_pair_);
        MSTL::swap(key_compare_, x.key_compare_);
        MSTL::swap(extracter_, x.extracter_);
    }

    MSTL_NODISCARD iterator find(const key_type& k) {
        link_type y = header_;
        link_type x = root();
        while (x != nullptr) {
            if (!key_compare_(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        iterator j(y);
        return (j == end() || key_compare_(k, key(y))) ? end() : j;
    }
    MSTL_NODISCARD const_iterator find(const key_type& k) const {
        link_type y = header_;
        link_type x = root();
        while (x != nullptr) {
            if (!key_compare_(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        const_iterator j(y);
        return (j == cend() || key_compare_(k, key(y))) ? cend() : j;
    }

    MSTL_NODISCARD size_type count(const key_type& k) const {
        pair<const_iterator, const_iterator> p = equal_range(k);
        size_type n = MSTL::distance(p.first, p.second);
        return n;
    }

    MSTL_NODISCARD iterator lower_bound(const key_type& k) {
        link_type y = header_;
        link_type x = root();
        while (x != nullptr) {
            if (!key_compare_(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return iterator(y);
    }
    MSTL_NODISCARD const_iterator lower_bound(const key_type& k) const {
        link_type y = header_;
        link_type x = root();
        while (x != nullptr) {
            if (!key_compare_(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return const_iterator(y);
    }

    MSTL_NODISCARD iterator upper_bound(const key_type& k) {
        link_type y = header_;
        link_type x = root();
        while (x != nullptr) {
            if (key_compare_(k, key(x))) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return iterator(y);
    }
    MSTL_NODISCARD const_iterator upper_bound(const key_type& k) const {
        link_type y = header_;
        link_type x = root();
        while (x != nullptr) {
            if (key_compare_(k, key(x))) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return const_iterator(y);
    }

    MSTL_NODISCARD pair<iterator, iterator> equal_range(const key_type& k) {
        return pair<iterator, iterator>(this->lower_bound(k), this->upper_bound(k));
    }
    MSTL_NODISCARD pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
        return pair<const_iterator, const_iterator>(this->lower_bound(k), this->upper_bound(k));
    }
};
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return lh.size() == rh.size() && MSTL::equal(lh.cbegin(), lh.cend(), rh.cbegin());
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return !(lh == rh);
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return MSTL::lexicographical_compare(lh.cbegin(), lh.cend(), rh.cbegin(), rh.cend());
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return rh < lh;
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return !(lh > rh);
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return !(lh < rh);
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept(noexcept(lh.swap(rh))) {
    lh.swap(rh);
}

MSTL_END_NAMESPACE__
#endif // MSTL_RB_TREE_H__
