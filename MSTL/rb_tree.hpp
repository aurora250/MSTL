#ifndef MSTL_RB_TREE_H__
#define MSTL_RB_TREE_H__
#include "memory.hpp"
MSTL_BEGIN_NAMESPACE__

static constexpr bool RB_TREE_RED__ = false;
static constexpr bool RB_TREE_BLACK__ = true;

struct __rb_tree_node_base;
struct __rb_tree_base_iterator;
template <bool IsConst, typename RB_TREE>
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

protected:
    using base_ptr = __rb_tree_node_base*;

    color_type color_ = RB_TREE_RED__;
    base_ptr parent_ = nullptr;
    base_ptr left_ = nullptr;
    base_ptr right_ = nullptr;

    friend __rb_tree_base_iterator;
    template <typename, typename, typename, typename, typename> friend class rb_tree;
    friend void rb_tree_rotate_left(__rb_tree_node_base*, __rb_tree_node_base*&) noexcept;
    friend void rb_tree_rotate_right(__rb_tree_node_base*, __rb_tree_node_base*&) noexcept;
    friend void rb_tree_rebalance(__rb_tree_node_base*, __rb_tree_node_base*&) noexcept;
    friend __rb_tree_node_base* rb_tree_rebalance_for_erase(
        __rb_tree_node_base*, __rb_tree_node_base*&,
        __rb_tree_node_base*&, __rb_tree_node_base*&) noexcept;

    static base_ptr minimum(base_ptr x) noexcept {
        while (x->left_ != nullptr) x = x->left_;
        return x;
    }
    static base_ptr maximum(base_ptr x) noexcept {
        while (x->right_ != nullptr) x = x->right_;
        return x;
    }
};


inline void rb_tree_rotate_left(
    __rb_tree_node_base* x, __rb_tree_node_base*& root) noexcept {
    __rb_tree_node_base* y = x->right_;
    x->right_ = y->left_;
    if (y->left_ != nullptr) y->left_->parent_ = x;
    y->parent_ = x->parent_;
    if (x == root) root = y;
    else if (x == x->parent_->left_) x->parent_->left_ = y;
    else x->parent_->right_ = y;
    y->left_ = x;
    x->parent_ = y;
}

inline void rb_tree_rotate_right(
    __rb_tree_node_base* x, __rb_tree_node_base*& root) noexcept {
    __rb_tree_node_base* y = x->left_;
    x->left_ = y->right_;
    if (y->right_ != nullptr) y->right_->parent_ = x;
    y->parent_ = x->parent_;
    if (x == root) root = y;
    else if (x == x->parent_->right_) x->parent_->right_ = y;
    else x->parent_->left_ = y;
    y->right_ = x;
    x->parent_ = y;
}

inline void rb_tree_rebalance(
    __rb_tree_node_base* x, __rb_tree_node_base*& root) noexcept {
    x->color_ = RB_TREE_RED__;
    while (x != root && x->parent_->color_ == RB_TREE_RED__) {
        if (x->parent_ == x->parent_->parent_->left_) {
            __rb_tree_node_base* y = x->parent_->parent_->right_;
            if (y != nullptr && y->color_ == RB_TREE_RED__) {
                x->parent_->color_ = RB_TREE_BLACK__;
                y->color_ = RB_TREE_BLACK__;
                x->parent_->parent_->color_ = RB_TREE_RED__;
                x = x->parent_->parent_;
            }
            else {
                if (x == x->parent_->right_) {
                    x = x->parent_;
                    rb_tree_rotate_left(x, root);
                }
                x->parent_->color_ = RB_TREE_BLACK__;
                x->parent_->parent_->color_ = RB_TREE_RED__;
                rb_tree_rotate_right(x->parent_->parent_, root);
            }
        }
        else {
            __rb_tree_node_base* y = x->parent_->parent_->left_;
            if (y != nullptr && y->color_ == RB_TREE_RED__) {
                x->parent_->color_ = RB_TREE_BLACK__;
                y->color_ = RB_TREE_BLACK__;
                x->parent_->parent_->color_ = RB_TREE_RED__;
                x = x->parent_->parent_;
            }
            else {
                if (x == x->parent_->left_) {
                    x = x->parent_;
                    rb_tree_rotate_right(x, root);
                }
                x->parent_->color_ = RB_TREE_BLACK__;
                x->parent_->parent_->color_ = RB_TREE_RED__;
                rb_tree_rotate_left(x->parent_->parent_, root);
            }
        }
    }
    root->color_ = RB_TREE_BLACK__;
}

inline __rb_tree_node_base* rb_tree_rebalance_for_erase(
    __rb_tree_node_base* z, __rb_tree_node_base*& root,
    __rb_tree_node_base*& leftmost, __rb_tree_node_base*& rightmost) noexcept {
    __rb_tree_node_base* y = z;
    __rb_tree_node_base* x;
    __rb_tree_node_base* x_parent;
    if (y->left_ == nullptr) x = y->right_;
    else {
        if (y->right_ == nullptr) x = y->left_;
        else {
            y = y->right_;
            while (y->left_ != nullptr) y = y->left_;
            x = y->right_;
        }
    }
    if (y != z) {
        z->left_->parent_ = y;
        y->left_ = z->left_;
        if (y != z->right_) {
            x_parent = y->parent_;
            if (x != nullptr) x->parent_ = y->parent_;
            y->parent_->left_ = x;
            y->right_ = z->right_;
            z->right_->parent_ = y;
        }
        else
            x_parent = y;
        if (root == z)
            root = y;
        else if (z->parent_->left_ == z)
            z->parent_->left_ = y;
        else
            z->parent_->right_ = y;
        y->parent_ = z->parent_;
        _MSTL swap(y->color_, z->color_);
        y = z;
    }
    else {
        x_parent = y->parent_;
        if (x != nullptr) x->parent_ = y->parent_;
        if (root == z) root = x;
        else {
            if (z->parent_->left_ == z) z->parent_->left_ = x;
            else z->parent_->right_ = x;
        }
        if (leftmost == z) {
            if (z->right_ == nullptr) leftmost = z->parent_;
            else leftmost = __rb_tree_node_base::minimum(x);
        }
        if (rightmost == z) {
            if (z->left_ == nullptr) rightmost = z->parent_;
            else rightmost = __rb_tree_node_base::maximum(x);
        }
    }
    if (y->color_ != RB_TREE_RED__) {
        while (x != root && (x == nullptr || x->color_ == RB_TREE_BLACK__)) {
            if (x == x_parent->left_) {
                __rb_tree_node_base* w = x_parent->right_;
                if (w->color_ == RB_TREE_RED__) {
                    w->color_ = RB_TREE_BLACK__;
                    x_parent->color_ = RB_TREE_RED__;
                    rb_tree_rotate_left(x_parent, root);
                    w = x_parent->right_;
                }
                if ((w->left_ == nullptr || w->left_->color_ == RB_TREE_BLACK__) &&
                    (w->right_ == nullptr || w->right_->color_ == RB_TREE_BLACK__)) {
                    w->color_ = RB_TREE_RED__;
                    x = x_parent;
                    x_parent = x_parent->parent_;
                }
                else {
                    if (w->right_ == nullptr || w->right_->color_ == RB_TREE_BLACK__) {
                        if (w->left_ != nullptr) w->left_->color_ = RB_TREE_BLACK__;
                        w->color_ = RB_TREE_RED__;
                        rb_tree_rotate_right(w, root);
                        w = x_parent->right_;
                    }
                    w->color_ = x_parent->color_;
                    x_parent->color_ = RB_TREE_BLACK__;
                    if (w->right_ != nullptr) w->right_->color_ = RB_TREE_BLACK__;
                    rb_tree_rotate_left(x_parent, root);
                    break;
                }
            }
            else {
                __rb_tree_node_base* w = x_parent->left_;
                if (w->color_ == RB_TREE_RED__) {
                    w->color_ = RB_TREE_BLACK__;
                    x_parent->color_ = RB_TREE_RED__;
                    rb_tree_rotate_right(x_parent, root);
                    w = x_parent->left_;
                }
                if ((w->right_ == nullptr || w->right_->color_ == RB_TREE_BLACK__) &&
                    (w->left_ == nullptr || w->left_->color_ == RB_TREE_BLACK__)) {
                    w->color_ = RB_TREE_RED__;
                    x = x_parent;
                    x_parent = x_parent->parent_;
                }
                else {
                    if (w->left_ == nullptr || w->left_->color_ == RB_TREE_BLACK__) {
                        if (w->right_ != nullptr) w->right_->color_ = RB_TREE_BLACK__;
                        w->color_ = RB_TREE_RED__;
                        rb_tree_rotate_left(w, root);
                        w = x_parent->left_;
                    }
                    w->color_ = x_parent->color_;
                    x_parent->color_ = RB_TREE_BLACK__;
                    if (w->left_ != nullptr) w->left_->color_ = RB_TREE_BLACK__;
                    rb_tree_rotate_right(x_parent, root);
                    break;
                }
            }
        }
        if (x != nullptr) x->color_ = RB_TREE_BLACK__;
    }
    return y;
}


template <typename T>
struct __rb_tree_node : __rb_tree_node_base {
private:
    T data_{};

    template <typename, typename, typename, typename, typename> friend class rb_tree;
    template <bool, typename> friend struct rb_tree_iterator;

public:
    __rb_tree_node() = default;
    ~__rb_tree_node() = default;
};

struct __rb_tree_base_iterator {
public:
    using iterator_category = bidirectional_iterator_tag;

protected:
    using base_ptr = __rb_tree_node_base::base_ptr;
    base_ptr node_ = nullptr;

    void increment() noexcept {
        if (node_->right_ != nullptr) {
            node_ = node_->right_;
            while (node_->left_ != nullptr) node_ = node_->left_;
        }
        else {
            base_ptr y = node_->parent_;
            while (node_ == y->right_) {
                node_ = y;
                y = y->parent_;
            }
            if (node_->right_ != y) node_ = y;
        }
    }

    void decrement() noexcept {
        if (node_->color_ == RB_TREE_RED__ && node_->parent_->parent_ == node_) {
            node_ = node_->right_;
        }
        else if (node_->left_ != nullptr) {
            base_ptr y = node_->left_;
            while (y->right_ != nullptr) y = y->right_;
            node_ = y;
        }
        else {
            base_ptr y = node_->parent_;
            while (node_ == y->left_) {
                node_ = y;
                y = y->parent_;
            }
            node_ = y;
        }
    }

public:
    MSTL_NODISCARD bool operator ==(const __rb_tree_base_iterator& rh) const noexcept {
        return node_ == rh.node_;
    }
    MSTL_NODISCARD bool operator !=(const __rb_tree_base_iterator& rh) const noexcept {
        return node_ != rh.node_;
    }
};

template <bool IsConst, typename RB_TREE>
struct rb_tree_iterator : __rb_tree_base_iterator {
private:
    using container_type	= RB_TREE;
    using iterator			= rb_tree_iterator<false, container_type>;
    using const_iterator	= rb_tree_iterator<true, container_type>;

public:
    using value_type		= typename container_type::value_type;
    using reference			= conditional_t<IsConst, typename container_type::const_reference, typename container_type::reference>;
    using pointer			= conditional_t<IsConst, typename container_type::const_pointer, typename container_type::pointer>;
    using difference_type	= typename container_type::difference_type;
    using size_type			= typename container_type::size_type;

    using self              = rb_tree_iterator<IsConst, container_type>;

private:
    using link_type         = __rb_tree_node<value_type>*;

    const container_type* tree_ = nullptr;

    template <typename, typename, typename, typename, typename> friend class rb_tree;
    template <bool, typename> friend struct rb_tree_iterator;

public:
    rb_tree_iterator() = default;
    rb_tree_iterator(link_type x, const container_type* cont)
    : tree_(cont) {
        node_ = x;
    }

    rb_tree_iterator(const iterator& it) {
        node_ = it.node_;
        tree_ = it.tree_;
    }
    self& operator =(const iterator& it) {
        if(_MSTL addressof(it) == this) return *this;
        node_ = it.node_;
        tree_ = it.tree_;
        return *this;
    }

    rb_tree_iterator(iterator&& it) noexcept {
        node_ = it.node_;
        tree_ = it.tree_;
        it.node_ = nullptr;
        it.tree_ = nullptr;
    }
    self& operator =(iterator&& it) {
        if(_MSTL addressof(it) == this) return *this;
        node_ = it.node_;
        tree_ = it.tree_;
        it.node_ = nullptr;
        it.tree_ = nullptr;
        return *this;
    }

    rb_tree_iterator(const const_iterator& it) {
        node_ = it.node_;
        tree_ = it.tree_;
    }
    self& operator =(const const_iterator& it) {
        if(_MSTL addressof(it) == this) return *this;
        node_ = it.node_;
        tree_ = it.tree_;
        return *this;
    }

    rb_tree_iterator(const_iterator&& it) {
        node_ = it.node_;
        tree_ = it.tree_;
        it.node_ = nullptr;
        it.tree_ = nullptr;
    }
    self& operator =(const_iterator&& it) {
        if(_MSTL addressof(it) == this) return *this;
        node_ = it.node_;
        tree_ = it.tree_;
        it.node_ = nullptr;
        it.tree_ = nullptr;
        return *this;
    }

    ~rb_tree_iterator() = default;

    MSTL_NODISCARD reference operator *() const noexcept {
        MSTL_DEBUG_VERIFY(node_ && tree_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(rb_tree_iterator, __MSTL_DEBUG_TAG_DEREFERENCE));
        link_type link = link_type(node_);
        MSTL_DEBUG_VERIFY(link != tree_->header_, __MSTL_DEBUG_MESG_OUT_OF_RANGE(rb_tree_iterator, __MSTL_DEBUG_TAG_DEREFERENCE));
        return link->data_;
    }
    MSTL_NODISCARD pointer operator ->() const noexcept {
        return &operator*();
    }

    self& operator ++() noexcept {
        MSTL_DEBUG_VERIFY(node_ && tree_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(rb_tree_iterator, __MSTL_DEBUG_TAG_INCREMENT));
        MSTL_DEBUG_VERIFY(link_type(node_) != tree_->header_,
            __MSTL_DEBUG_MESG_OUT_OF_RANGE(rb_tree_iterator, __MSTL_DEBUG_TAG_INCREMENT));
        increment();
        return *this;
    }
    self operator ++(int) noexcept {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator --() noexcept {
        MSTL_DEBUG_VERIFY(node_ && tree_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(rb_tree_iterator, __MSTL_DEBUG_TAG_DECREMENT));
        MSTL_DEBUG_VERIFY(node_ != tree_->header_,
            __MSTL_DEBUG_MESG_OUT_OF_RANGE(rb_tree_iterator, __MSTL_DEBUG_TAG_DECREMENT));
        decrement();
        return *this;
    }
    self operator --(int) noexcept {
        self tmp = *this;
        --*this;
        return tmp;
    }

    MSTL_NODISCARD bool operator ==(const rb_tree_iterator& rh) const noexcept {
        MSTL_DEBUG_VERIFY(node_ && tree_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(rb_tree_iterator, __MSTL_DEBUG_TAG_DEREFERENCE));
		MSTL_DEBUG_VERIFY(tree_ == rh.tree_, __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(rb_tree_iterator));
        return __rb_tree_base_iterator::operator ==(rh);
    }
    MSTL_NODISCARD bool operator !=(const rb_tree_iterator& rh) const noexcept {
        MSTL_DEBUG_VERIFY(node_ && tree_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(rb_tree_iterator, __MSTL_DEBUG_TAG_DEREFERENCE));
		MSTL_DEBUG_VERIFY(tree_ == rh.tree_, __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(rb_tree_iterator));
        return __rb_tree_base_iterator::operator !=(rh);
    }
};


template <typename Key, typename Value, typename KeyOfValue, typename Compare, 
    typename Alloc = allocator<__rb_tree_node<Value>>>
class rb_tree {
#ifdef MSTL_VERSION_20__
    static_assert(is_allocator_v<Alloc>, "Alloc type is not a standard allocator type.");
#endif
    static_assert(is_same_v<__rb_tree_node<Value>, typename Alloc::value_type>, "allocator type mismatch.");
    static_assert(is_object_v<Value>, "list only contains object types.");

public:
    MSTL_BUILD_TYPE_ALIAS(Value)
    using key_type                  = Key;
    using color_type                = bool;
    using allocator_type            = Alloc;
    using self                      = rb_tree<Key, Value, KeyOfValue, Compare, Alloc>;

    using iterator                  = rb_tree_iterator<false, self>;
    using const_iterator            = rb_tree_iterator<true, self>;
    using reverse_iterator          = _MSTL reverse_iterator<iterator>;
    using const_reverse_iterator    = _MSTL reverse_iterator<const_iterator>;

private:
    using base_ptr  = __rb_tree_node_base*;
    using link_type = __rb_tree_node<Value>*;

    link_type header_ = nullptr;
    Compare key_compare_{};
    KeyOfValue extracter_{};
    // allocator_, size_
    compressed_pair<allocator_type, size_t> size_pair_{ default_construct_tag{}, 0 };

    template <bool, typename> friend struct rb_tree_iterator;

private:
    template <typename... Args>
    link_type create_node(Args... args) {
        link_type tmp = size_pair_.get_base().allocate();
        try {
            _MSTL construct(&tmp->data_, _MSTL forward<Args>(args)...);
        }
        catch (...) {
            destroy_node(tmp);
            Exception(MemoryError("rb tree construct node failed."));
        }
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

    link_type& root() const noexcept { return (link_type &)header_->parent_; }
    link_type& leftmost() const noexcept { return (link_type &)header_->left_; }
    link_type& rightmost() const noexcept { return (link_type &)header_->right_; }

    static link_type& left(link_type x) noexcept { return (link_type &)x->left_; }
    static link_type& right(link_type x) noexcept { return (link_type &)x->right_; }
    static link_type& parent(link_type x) noexcept { return (link_type &)x->parent_; }
    static const Key& key(link_type x) noexcept { return KeyOfValue()(x->data_); }
    static const Key& key(base_ptr x) noexcept { return KeyOfValue()(link_type(x)->data_); }

    static link_type minimum(link_type x) noexcept {
        return static_cast<link_type>(__rb_tree_node_base::minimum(x));
    }
    static link_type maximum(link_type x) noexcept {
        return static_cast<link_type>(__rb_tree_node_base::maximum(x));
    }

    iterator insert_node_into(base_ptr bx, base_ptr by, link_type p) {
        auto x = static_cast<link_type>(bx);
        auto y = static_cast<link_type>(by);
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
        return iterator(p, this);
    }

    link_type copy_under_node(link_type x, link_type parent) {
        link_type top = copy_node(x);
        top->parent_ = parent;
        try{
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
        catch (MemoryError&) {
            erase_under_node(top);
            throw;
        }
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
            try {
              root() = copy_under_node(x.root(), header_);
            }
            catch (MemoryError&) {
                size_pair_.get_base().deallocate(header_);
                throw;
            }
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
        iterator j(y, this);
        if (comp) {
            if (j == begin())
                return pair<iterator, bool>(insert_node_into(x, y, p), true);
            --j;
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
    rb_tree() {
        header_init();
    }

    explicit rb_tree(const Compare& comp) : key_compare_(comp) {
        header_init();
    }

    rb_tree(const self& x) 
        : key_compare_(x.key_compare_), extracter_(x.extracter_), size_pair_(x.size_pair_) {
        header_init();
        copy_from(x);
    }
    self& operator =(const self& x) {
        if (_MSTL addressof(x) == this) return *this;
        clear();
        copy_from(x);
        return *this;
    }

    rb_tree(self&& x) 
        noexcept(is_nothrow_move_constructible_v<Compare> && is_nothrow_move_constructible_v<KeyOfValue>)
        : header_(_MSTL move(x.header_)), key_compare_(_MSTL move(x.key_compare_)), extracter_(_MSTL move(x.extracter_)),
        size_pair_(_MSTL move(x.size_pair_)) {}

    self& operator =(self&& x) noexcept(noexcept(swap(x))) {
        if (_MSTL addressof(x) == this) return *this;
        clear();
        swap(x);
        return *this;
    }

    ~rb_tree() {
        clear();
        size_pair_.get_base().deallocate(header_);
    }

    MSTL_NODISCARD iterator begin() noexcept { return {leftmost(), this}; }
    MSTL_NODISCARD iterator end() noexcept { return {header_, this}; }
    MSTL_NODISCARD const_iterator cbegin() const noexcept { return {leftmost(), this}; }
    MSTL_NODISCARD const_iterator cend() const noexcept { return {header_, this}; }
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
        link_type tmp = (create_node)(_MSTL forward<Args>(args)...);
        return (insert_unique_node)(tmp);
    }
    pair<iterator, bool> insert_unique(const value_type& v) {
        return (emplace_unique)(v);
    }
    pair<iterator, bool> insert_unique(value_type&& v) {
        return (emplace_unique)(_MSTL move(v));
    }
    template <typename... Args>
    iterator emplace_unique_hint(iterator position, Args&&... args) {
        link_type tmp = (create_node)(_MSTL forward<Args>(args)...);
        if (position.node_ == header_->left_) {
            if (size() > 0 && key_compare_(key(tmp), key(position.node_)))
                return insert_node_into(position.node_, position.node_, tmp);
            return insert_unique_node(tmp).first;
        }
        if (position.node_ == header_) {
            if (key_compare_(key(rightmost()), key(tmp)))
                return insert_node_into(nullptr, rightmost(), tmp);
            return insert_unique_node(tmp).first;
        }
        iterator before = position;
        --before;
        if (key_compare_(key(before.node_), key(tmp)) &&
            key_compare_(key(tmp), key(position.node_))) {
            if (right(link_type(before.node_)) == nullptr)
                return insert_node_into(nullptr, before.node_, tmp);
            return insert_node_into(position.node_, position.node_, tmp);
        }
        return insert_unique_node(tmp).first;
    }
    iterator insert_unique(iterator position, const value_type& v) {
        return (emplace_unique_hint)(position, v);
    }
    iterator insert_unique(iterator position, value_type&& v) {
        return (emplace_unique_hint)(position, _MSTL move(v));
    }
    template <typename Iterator, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
    void insert_unique(Iterator first, Iterator last) {
        for (; first != last; ++first) insert_unique(*first);
    }

    template <typename... Args>
    iterator emplace_equal(Args&&... args) {
        link_type tmp = (create_node)(_MSTL forward<Args>(args)...);
        return (insert_equal_node)(tmp);
    }
    iterator insert_equal(const value_type& v) {
        return (emplace_equal)(v);
    }
    iterator insert_equal(value_type&& v) {
        return (emplace_equal)(_MSTL move(v));
    }
    template <typename... Args>
    iterator emplace_equal_hint(iterator position, Args&&... args) {
        link_type tmp = (create_node)(_MSTL forward<Args>(args)...);
        if (position.node_ == header_->left_) {
            if (size() > 0 && key_compare_(key(tmp), key(position.node_)))
                return insert_node_into(position.node_, position.node_, tmp);
            return insert_equal_node(tmp);
        }
        if (position.node_ == header_) {
            if (!key_compare_(key(tmp), key(rightmost())))
                return insert_node_into(nullptr, rightmost(), tmp);
            return insert_equal_node(tmp);
        }
        iterator before = position;
        --before;
        if (!key_compare_(key(tmp), key(before.node_)) &&
            !key_compare_(key(position.node_), key(tmp))) {
            if (right(link_type(before.node_)) == nullptr)
                return insert_node_into(nullptr, before.node_, tmp);
            return insert_node_into(position.node_, position.node_, tmp);
        }
        return insert_equal_node(tmp);
    }
    iterator insert_equal(iterator position, const value_type& v) {
        return (emplace_equal_hint)(position, v);
    }
    iterator insert_equal(iterator position, value_type&& v) {
        return (emplace_equal_hint)(position, _MSTL move(v));
    }
    template <typename Iterator, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
    void insert_equal(Iterator first, Iterator last) {
        for (; first != last; ++first) insert_equal(*first);
    }

    size_type erase(const key_type& k) noexcept {
        pair<iterator, iterator> p = equal_range(k);
        const size_type n = _MSTL distance(p.first, p.second);
        erase(p.first, p.second);
        return n;
    }
    void erase(iterator position) noexcept {
        auto y = (link_type)rb_tree_rebalance_for_erase(
            position.node_, header_->parent_, header_->left_, header_->right_);
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

    void swap(self& x) noexcept(is_nothrow_swappable_v<Compare>
        && is_nothrow_swappable_v<KeyOfValue>
        && noexcept(size_pair_.swap(x.size_pair_))) {
        _MSTL swap(header_, x.header_);
        size_pair_.swap(x.size_pair_);
        _MSTL swap(key_compare_, x.key_compare_);
        _MSTL swap(extracter_, x.extracter_);
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
        iterator j(y, this);
        return j == end() || key_compare_(k, key(y)) ? end() : j;
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
        const_iterator j(y, this);
        return j == cend() || key_compare_(k, key(y)) ? cend() : j;
    }

    MSTL_NODISCARD size_type count(const key_type& k) const {
        pair<const_iterator, const_iterator> p = equal_range(k);
        const size_type n = _MSTL distance(p.first, p.second);
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
        return iterator(y, this);
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
        return const_iterator(y, this);
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
        return iterator(y, this);
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
        return const_iterator(y, this);
    }

    MSTL_NODISCARD pair<iterator, iterator> equal_range(const key_type& k) {
        return pair<iterator, iterator>(this->lower_bound(k), this->upper_bound(k));
    }
    MSTL_NODISCARD pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
        return pair<const_iterator, const_iterator>(this->lower_bound(k), this->upper_bound(k));
    }
};
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD bool operator ==(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return lh.size() == rh.size() && _MSTL equal(lh.cbegin(), lh.cend(), rh.cbegin());
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD bool operator !=(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return !(lh == rh);
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD bool operator <(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return _MSTL lexicographical_compare(lh.cbegin(), lh.cend(), rh.cbegin(), rh.cend());
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD bool operator >(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return rh < lh;
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD bool operator <=(
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lh,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rh) noexcept {
    return !(lh > rh);
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
MSTL_NODISCARD bool operator >=(
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
