#ifndef MSTL_RB_TREE_H__
#define MSTL_RB_TREE_H__
#include "pair.hpp"
#include "memory.hpp"
#include "iterator.hpp"
MSTL_BEGIN_NAMESPACE__

typedef bool RB_TREE_COLORS;
const RB_TREE_COLORS RB_TREE_RED__ = false;
const RB_TREE_COLORS RB_TREE_BLACK__ = true;

struct __rb_tree_node_base {
    typedef RB_TREE_COLORS          color_type;
    typedef __rb_tree_node_base*    base_ptr;

    color_type color_;
    base_ptr parent_;
    base_ptr left_;
    base_ptr right_;

    static base_ptr minimum(base_ptr x) noexcept;
    static base_ptr maximum(base_ptr x) noexcept;
};
template <typename T>
struct __rb_tree_node : public __rb_tree_node_base {
    typedef __rb_tree_node<T>*  link_type;
    typedef T                   value_type;
    
    __rb_tree_node() = default;
    __rb_tree_node(const __rb_tree_node&) = delete;
    __rb_tree_node& operator =(const __rb_tree_node&) = delete;
    ~__rb_tree_node() = default;

    T data_;
};

struct __rb_tree_base_iterator {
    typedef __rb_tree_node_base::base_ptr    base_ptr;
    typedef std::bidirectional_iterator_tag  iterator_category;
    typedef ptrdiff_t                        difference_type;
    base_ptr node_;

    void increment() noexcept;
    void decrement() noexcept;
};

template<typename T, typename Ref = T&, typename Ptr = T*>
struct rb_tree_iterator : public __rb_tree_base_iterator {
    typedef T                                       value_type;
    typedef Ref                                     reference;
    typedef Ptr                                     pointer;
    typedef rb_tree_iterator<T, T&, T*>             iterator;
    typedef rb_tree_iterator<T, const T&, const T*> const_iterator;
    typedef const T&                                const_reference;
    typedef rb_tree_iterator<T, Ref, Ptr>           self;
    typedef __rb_tree_node<T>*                      link_type;

    rb_tree_iterator() = default;
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
        increment();
        return tmp;
    }
    self& operator --() noexcept {
        decrement();
        return *this;
    }
    self operator --(int) noexcept {
        self tmp = *this;
        decrement();
        return tmp;
    }
};
inline MSTL_NODISCARD bool operator ==(const __rb_tree_base_iterator& x, const __rb_tree_base_iterator& y) {
    return x.node_ == y.node_;
}
inline MSTL_NODISCARD bool operator !=(const __rb_tree_base_iterator& x, const __rb_tree_base_iterator& y) {
    return x.node_ != y.node_;
}

void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root);
void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root);
void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root);
__rb_tree_node_base* __rb_tree_rebalance_for_erase(
    __rb_tree_node_base* z, __rb_tree_node_base*& root,
    __rb_tree_node_base*& leftmost, __rb_tree_node_base*& rightmost);
int __black_count(__rb_tree_node_base* node, __rb_tree_node_base* root);

template <typename Key, typename Value, typename KeyOfValue, typename Compare, 
    typename Alloc = default_standard_alloc<__rb_tree_node<Value>> >
class rb_tree {
private:
    typedef __rb_tree_node_base*    base_ptr;
    typedef __rb_tree_node<Value>   rb_tree_node;
    typedef Alloc                   rb_tree_node_allocator;
    typedef RB_TREE_COLORS          color_type;
public:
    typedef Key                     key_type;
    typedef Value                   value_type;
    typedef value_type*             pointer;
    typedef const value_type*       const_pointer;
    typedef value_type&             reference;
    typedef const value_type&       const_reference;
    typedef rb_tree_node*           link_type;
    typedef size_t                  size_type;
    typedef ptrdiff_t               difference_type;
    typedef rb_tree<Key, Value, KeyOfValue, Compare, Alloc>                 self;
    typedef rb_tree_iterator<value_type, reference, pointer>                iterator;
    typedef rb_tree_iterator<value_type, const_reference, const_pointer>    const_iterator;
private:
    size_type node_num_;
    link_type header_;
    Compare key_compare_;
    rb_tree_node_allocator alloc_;

    link_type get_node() {
        return alloc_.allocate();
    }
    void put_node(link_type p) {
        alloc_.deallocate(p);
    }
    link_type create_node(const_reference x) {
        link_type tmp = get_node();
        MSTL_TRY__{
            construct(&tmp->data_, x);
        }
        MSTL_CATCH_UNWIND_THROW_M__(put_node(tmp));
        return tmp;
    }
    link_type clone_node(link_type x) {
        link_type tmp = create_node(x->data_);
        tmp->color_ = x->color_;
        tmp->left_ = 0;
        tmp->right_ = 0;
        return tmp;
    }
    void destroy_node(link_type p) {
        destroy(&p->data_);
        put_node(p);
    }

    link_type& root() const { return (link_type&)header_->parent_; }
    link_type& leftmost() const { return (link_type&)header_->left_; }
    link_type& rightmost() const { return (link_type&)header_->right_; }

    static link_type& left(link_type x) { return (link_type&)(x->left_); }
    static link_type& right(link_type x) { return (link_type&)(x->right_); }
    static link_type& parent(link_type x) { return (link_type&)(x->parent_); }

    static reference value(link_type x) { return x->data_; }
    static const Key& key(link_type x) { return KeyOfValue()(value(x)); }
    static color_type& color(link_type x) { return (color_type&)(x->color_); }

    static link_type minimum(link_type x) { return (link_type)__rb_tree_node_base::minimum(x); }
    static link_type maximum(link_type x) { return (link_type)__rb_tree_node_base::maximum(x); }

    iterator __insert(base_ptr x_, base_ptr y_, const_reference v) {
        link_type x = (link_type)x_;
        link_type y = (link_type)y_;
        link_type z;
        if (y == header_ || x != 0 || key_compare_(KeyOfValue()(v), key(y))) {
            z = create_node(v);
            left(y) = z;
            if (y == header_) {
                root() = z;
                rightmost() = z;
            }
            else if (y == leftmost()) leftmost() = z;
        }
        else {
            z = create_node(v);
            right(y) = z;
            if (y == rightmost()) rightmost() = z;
        }
        parent(z) = y;
        left(z) = 0;
        right(z) = 0;
        (__rb_tree_rebalance)(z, header_->parent_);
        ++node_num_;
        return iterator(z);
    }
    link_type __copy(link_type x, link_type p) {
        link_type top = clone_node(x);
        top->parent_ = p;
        MSTL_TRY__{
            if (x->right_) top->right_ = __copy(right(x), top);
            p = top;
            x = left(x);
            while (x != 0) {
                link_type y = clone_node(x);
                p->left_ = y;
                y->parent_ = p;
                if (x->right_) y->right_ = __copy(right(x), y);
                p = y;
                x = left(x);
            }
        }
        MSTL_CATCH_UNWIND_THROW_M__(__erase(top));
        return top;
    }
    void __erase(link_type x) {
        while (x != 0) {
            __erase(right(x));
            link_type y = left(x);
            destroy_node(x);
            x = y;
        }
    }

    void init() {
        header_ = get_node();
        color(header_) = RB_TREE_RED__;
        root() = 0;
        leftmost() = header_;
        rightmost() = header_;
    }
public:
    rb_tree(const Compare& comp = Compare()) : node_num_(0), key_compare_(comp) {
        init();
    }
    rb_tree(const self& x)
        : node_num_(0), key_compare_(x.key_compare_) {
        header_ = get_node();
        color(header_) = RB_TREE_RED__;
        if (x.root() == 0) {
            root() = 0;
            leftmost() = header_;
            rightmost() = header_;
        }
        else {
            MSTL_TRY__{
              root() = __copy(x.root(), header_);
            }
            MSTL_CATCH_UNWIND_THROW_M__(put_node(header_));
            leftmost() = minimum(root());
            rightmost() = maximum(root());
        }
        node_num_ = x.node_num_;
    }
    self& operator =(const self& x) {
        if (*this == x) return *this;
        clear();
        node_num_ = 0;
        key_compare_ = x.key_compare_;
        if (x.root() == 0) {
            root() = 0;
            leftmost() = header_;
            rightmost() = header_;
        }
        else {
            root() = __copy(x.root(), header_);
            leftmost() = minimum(root());
            rightmost() = maximum(root());
            node_num_ = x.node_num_;
        }
        return *this;
    }
    ~rb_tree() {
        clear();
        put_node(header_);
    }

    iterator begin() { return leftmost(); }
    iterator end() { return header_; }
    const_iterator const_begin() const { return leftmost(); }
    const_iterator const_end() const { return header_; }
    void swap(self& rep) {
        std::swap(header_, rep.header_);
        std::swap(node_num_, rep.node_num_);
        std::swap(key_compare_, rep.key_compare_);
    }
    bool empty() const { return node_num_ == 0; }
    Compare key_comp() const { return key_compare_; }
    size_type size() const { return node_num_; }
    // size_type max_height() const { return 2 * log2(node_count + 1); }

    pair<iterator, bool> insert_unique(const Value& v) {
        link_type y = header_;
        link_type x = root();
        bool comp = true;
        while (x != 0) {
            y = x;
            comp = key_compare_(KeyOfValue()(v), key(x));
            x = comp ? left(x) : right(x);
        }
        iterator j = iterator(y);
        if (comp) {
            if (j == begin()) 
                return pair<iterator, bool>(__insert(x, y, v), true);
            else --j;
        }
        if (key_compare_(KeyOfValue()(value(link_type(j.node_))), KeyOfValue()(v)))
            return pair<iterator, bool>(__insert(x, y, v), true);
        return pair<iterator, bool>(iterator(j), false);
    }
    iterator insert_unique(iterator position, const_reference v) {
        if (position.node_ == header_->left_) {
            if (size() > 0 && key_compare_(KeyOfValue()(v), key(position.node_)))
                return __insert(position.node_, position.node_, v);
            else  return insert_unique(v).first;
        }
        else if (position.node_ == header_) {
            if (key_compare_(key(rightmost()), KeyOfValue()(v)))
                return __insert(0, rightmost(), v);
            else  return insert_unique(v).first;
        }
        else {
            iterator before = position;
            --before;
            if (key_compare_(key(before.node_), KeyOfValue()(v))
                && key_compare_(KeyOfValue()(v), key(position.node_)))
                if (right(before.node_) == 0)
                    return __insert(0, before.node_, v);
                else  return __insert(position.node_, position.node_, v);
            else  return insert_unique(v).first;
        }
    }
    template <typename Iterator>
    void insert_unique(Iterator first, Iterator last) {
        for (; first != last; ++first) insert_unique(*first);
    }
    iterator insert_equal(const_reference v) {
        link_type y = header_;
        link_type x = root();
        while (x != 0) {
            y = x;
            x = key_compare_(KeyOfValue()(v), key(x)) ? left(x) : right(x);
        }
        return __insert(x, y, v);
    }
    iterator insert_equal(iterator position, const_reference v) {
        if (position.node_ == header_->left_) {
            if (size() > 0 && key_compare_(KeyOfValue()(v), key(position.node_)))
                return __insert(position.node_, position.node_, v);
            else  return insert_equal(v);

        }
        else if (position.node_ == header_) {
            if (!key_compare_(KeyOfValue()(v), key(rightmost())))
                return __insert(0, rightmost(), v);
            else  return insert_equal(v);
        }
        else {
            iterator before = position;
            --before;
            if (!key_compare_(KeyOfValue()(v), key(before.node_))
                && !key_compare_(key(position.node_), KeyOfValue()(v)))
                if (right(before.node_) == 0) return __insert(0, before.node_, v);
                else  return __insert(position.node_, position.node_, v);
            else  return insert_equal(v);
        }
    }
    template <typename Iterator>
    void insert_equal(Iterator first, Iterator last) {
        for (; first != last; ++first) insert_equal(*first);
    }
    size_type erase(const Key& x) {
        pair<iterator, iterator> p = equal_range(x);
        size_type n = 0;
        distance(p.first, p.second, n);
        erase(p.first, p.second);
        return n;
    }
    void erase(iterator position) {
        link_type y = (link_type)
            __rb_tree_rebalance_for_erase(position.node_, header_->parent_, header_->left_, header_->right_);
        destroy_node(y);
        --node_num_;
    }
    void erase(iterator first, iterator last) {
        if (first == begin() && last == end()) clear();
        else
            while (first != last) erase(first++);
    }
    void erase(const Key* first, const Key* last) {
        while (first != last) erase(*first++);
    }
    void clear() {
        if (node_num_ == 0) return;
        __erase(root());
        leftmost() = header_;
        root() = 0;
        rightmost() = header_;
        node_num_ = 0;
    }

    iterator find(const Key& k) {
        link_type y = header_;
        link_type x = root();
        while (x != 0) {
            if (!key_compare_(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        iterator j = iterator(y);
        return (j == end() || key_compare_(k, KeyOfValue()(y->data_))) ? end() : j;
    }
    const_iterator find(const Key& k) const {
        link_type y = header_;
        link_type x = root();
        while (x != 0) {
            if (!key_compare_(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        const_iterator j = const_iterator(y);
        return (j == end() || key_compare_(k, KeyOfValue()(y->data_))) ? end() : j;
    }
    size_type count(const Key& k) const {
        pair<const_iterator, const_iterator> p = equal_range(k);
        size_type n = 0;
        MSTL::distance(p.first, p.second, n);
        return n;
    }
    iterator lower_bound(const Key& k) {
        link_type y = header_;
        link_type x = root();
        while (x != 0) {
            if (!key_compare_(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return iterator(y);
    }
    const_iterator lower_bound(const Key& k) const {
        link_type y = header_;
        link_type x = root();
        while (x != 0) {
            if (!key_compare_(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return const_iterator(y);
    }
    iterator upper_bound(const Key& k) {
        link_type y = header_;
        link_type x = root();
        while (x != 0) {
            if (key_compare_(k, key(x))) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return iterator(y);
    }
    const_iterator upper_bound(const Key& k) const {
        link_type y = header_;
        link_type x = root();
        while (x != 0) {
            if (key_compare_(k, key(x))) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return const_iterator(y);
    }
    inline pair<iterator, iterator> equal_range(const Key& k) {
        return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
    }
    inline pair<const_iterator, const_iterator> equal_range(const Key& k) const {
        return pair<const_iterator, const_iterator>(lower_bound(k), upper_bound(k));
    }
    bool __rb_verify() const {
        if (node_num_ == 0 || begin() == end())
            return node_num_ == 0 && begin() == end() &&
            header_->left_ == header_ && header_->right_ == header_;
        int len = __black_count(leftmost(), root());
        for (const_iterator it = begin(); it != end(); ++it) {
            link_type x = (link_type)it.node_;
            link_type L = left(x);
            link_type R = right(x);
            if (x->color_ == RB_TREE_RED__)
                if ((L && L->color_ == RB_TREE_RED__) || (R && R->color_ == RB_TREE_RED__))
                    return false;
            if (L && key_compare_(key(x), key(L))) return false;
            if (R && key_compare_(key(R), key(x))) return false;
            if (!L && !R && __black_count(x, root()) != len) return false;
        }
        if (leftmost() != __rb_tree_node_base::minimum(root())) return false;
        if (rightmost() != __rb_tree_node_base::maximum(root())) return false;
        return true;
    }
};
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator ==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return x.size() == y.size() && equal(x.const_begin(), x.const_end(), y.const_begin());
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator <(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return lexicographical_compare(x.const_begin(), x.const_end(), y.const_begin(), y.const_end());
}
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    x.swap(y);
}


MSTL_END_NAMESPACE__

#endif // MSTL_RB_TREE_H__
