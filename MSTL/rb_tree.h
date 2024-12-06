#ifndef MSTL_RB_TREE_H__
#define MSTL_RB_TREE_H__
#include "pair.hpp"
#include "memory.hpp"
#include <iterator>

MSTL_BEGIN_NAMESPACE__

typedef bool _rb_tree_color_type;
const _rb_tree_color_type __rb_tree_red = false;
const _rb_tree_color_type __rb_tree_black = true;

struct _rb_tree_node_base {
    typedef _rb_tree_color_type     color_type;
    typedef _rb_tree_node_base*     base_ptr;

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x);
    static base_ptr maximum(base_ptr x);
};
template <typename T>
struct _rb_tree_node : public _rb_tree_node_base {
    typedef _rb_tree_node<T>*   link_type;
    typedef T                   value_type;
    T value_field;
};

struct _rb_tree_base_iterator {
    typedef _rb_tree_node_base::base_ptr    base_ptr;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t                       differrnce_type;
    base_ptr node;

    void increment();
    void decrement();
};

template<typename T, typename Ref = T&, typename Ptr = T*>
struct rb_tree_iterator : public _rb_tree_base_iterator {
    typedef T                                       value_type;
    typedef Ref                                     reference;
    typedef Ptr                                     pointer;
    typedef rb_tree_iterator<T, T&, T*>             iterator;
    typedef rb_tree_iterator<T, const T&, const T>  const_iterator;
    typedef const T&                                const_reference;
    typedef rb_tree_iterator<T, Ref, Ptr>           self;
    typedef _rb_tree_node<T>*                       link_type;

    rb_tree_iterator() {}
    rb_tree_iterator(link_type x) { node = x; }
    rb_tree_iterator(const iterator& it) { node = it.node; }

    reference operator*() const { return link_type(node)->value_field; }
    pointer operator->() const { return &(operator*()); }

    self& operator++() {
        increment();
        return *this;
    }
    self& operator++(int) {
        self tmp = *this;
        increment();
        return tmp;
    }
    self& operator--() {
        decrement();
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }
};
inline bool operator ==(const _rb_tree_base_iterator& x,
    const _rb_tree_base_iterator& y) {
    return x.node == y.node;
}
inline bool operator !=(const _rb_tree_base_iterator& x,
    const _rb_tree_base_iterator& y) {
    return x.node != y.node;
}

void __rb_tree_rotate_left(_rb_tree_node_base* x, _rb_tree_node_base*& root);
void __rb_tree_rotate_right(_rb_tree_node_base* x, _rb_tree_node_base*& root);
void __rb_tree_rebalance(_rb_tree_node_base* x, _rb_tree_node_base*& root);
_rb_tree_node_base* __rb_tree_rebalance_for_erase(
    _rb_tree_node_base* z, _rb_tree_node_base*& root,
    _rb_tree_node_base*& leftmost, _rb_tree_node_base*& rightmost);

int __black_count(_rb_tree_node_base* node, _rb_tree_node_base* root);



template <typename Key, typename Value, typename KeyOfValue, typename Compare, 
    typename Alloc = default_standard_alloc<_rb_tree_node<Value>> >
class rb_tree {
private:
    typedef void*                   void_pointer;
    typedef _rb_tree_node_base*     base_ptr;
    typedef _rb_tree_node<Value>    rb_tree_node;
    typedef Alloc                   rb_tree_node_allocator;
    typedef _rb_tree_color_type     color_type;
public:
    typedef Key                 key_type;
    typedef Value               value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef rb_tree_node*       link_type;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;
    typedef rb_tree<Key, Value, KeyOfValue, Compare, Alloc>                 self;
    typedef rb_tree_iterator<value_type, reference, pointer>                iterator;
    typedef rb_tree_iterator<value_type, const_reference, const_pointer>    const_iterator;

    static const char* const __type__;
private:
    size_type node_count;
    link_type header;
    Compare key_compare;
    rb_tree_node_allocator alloc;

    link_type get_node() {
        return alloc.allocate();
    }
    void put_node(link_type p) {
        alloc.deallocate(p);
    }
    link_type create_node(const_reference x) {
        link_type tmp = get_node();
        MSTL_TRY__{
            construct(&tmp->value_field, x);
        }
        MSTL_CATCH_UNWIND_THROW_M__(put_node(tmp));
        return tmp;
    }
    link_type clone_node(link_type x) {
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = 0;
        tmp->right = 0;
        return tmp;
    }
    void destroy_node(link_type p) {
        destroy(&p->value_field);
        put_node(p);
    }

    link_type& root() const { return (link_type&)header->parent; }
    link_type& leftmost() const { return (link_type&)header->left; }
    link_type& rightmost() const { return (link_type&)header->right; }

    static link_type& left(link_type x) { return (link_type&)(x->left); }
    static link_type& right(link_type x) { return (link_type&)(x->right); }
    static link_type& parent(link_type x) { return (link_type&)(x->parent); }

    static reference value(link_type x) { return x->value_field; }
    static const Key& key(link_type x) { return KeyOfValue()(value(x)); }
    static color_type& color(link_type x) { return (color_type&)(x->color); }

    static link_type minimum(link_type x) { return (link_type)_rb_tree_node_base::minimum(x); }
    static link_type maximum(link_type x) { return (link_type)_rb_tree_node_base::maximum(x); }

    iterator __insert(base_ptr x_, base_ptr y_, const_reference v) {
        link_type x = (link_type)x_;
        link_type y = (link_type)y_;
        link_type z;
        if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
            z = create_node(v);
            left(y) = z;
            if (y == header) {
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
        __rb_tree_rebalance(z, header->parent);
        ++node_count;
        return iterator(z);
    }
    link_type __copy(link_type x, link_type p) {
        link_type top = clone_node(x);
        top->parent = p;
        MSTL_TRY__{
            if (x->right) top->right = __copy(right(x), top);
            p = top;
            x = left(x);
            while (x != 0) {
                link_type y = clone_node(x);
                p->left = y;
                y->parent = p;
                if (x->right) y->right = __copy(right(x), y);
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
        header = get_node();
        color(header) = __rb_tree_red;
        root() = 0;
        leftmost() = header;
        rightmost() = header;
    }
public:
    rb_tree(const Compare& comp = Compare()) : node_count(0), key_compare(comp) {
        init();
    }
    rb_tree(const self& x)
        : node_count(0), key_compare(x.key_compare) {
        header = get_node();
        color(header) = __rb_tree_red;
        if (x.root() == 0) {
            root() = 0;
            leftmost() = header;
            rightmost() = header;
        }
        else {
            MSTL_TRY__{
              root() = __copy(x.root(), header);
            }
            MSTL_CATCH_UNWIND_THROW_M__(put_node(header));
            leftmost() = minimum(root());
            rightmost() = maximum(root());
        }
        node_count = x.node_count;
    }
    self& operator =(const self& x) {
        if (*this == x) return *this;
        clear();
        node_count = 0;
        key_compare = x.key_compare;
        if (x.root() == 0) {
            root() = 0;
            leftmost() = header;
            rightmost() = header;
        }
        else {
            root() = __copy(x.root(), header);
            leftmost() = minimum(root());
            rightmost() = maximum(root());
            node_count = x.node_count;
        }
        return *this;
    }
    ~rb_tree() {
        clear();
        put_node(header);
    }

    iterator begin() { return leftmost(); }
    iterator end() { return header; }
    const_iterator const_begin() const { return leftmost(); }
    const_iterator const_end() const { return header; }

    bool empty() const { return node_count == 0; }
    Compare key_comp() const { return key_compare; }
    size_type size() const { return node_count; }
    // size_type max_height() const { return 2 * log2(node_count + 1); }

    pair<iterator, bool> insert_unique(const_reference v) {
        link_type y = header;
        link_type x = root();
        bool comp = true;
        while (x != 0) {
            y = x;
            comp = key_compare(KeyOfValue()(v), key(x));
            x = comp ? left(x) : right(x);
        }
        iterator j = iterator(y);
        if (comp) {
            if (j == begin()) 
                return pair<iterator, bool>(__insert(x, y, v), true);
            else --j;
        }
        if (key_compare(key(link_type(j.node)), KeyOfValue()(v)))
            return pair<iterator, bool>(__insert(x, y, v), true);
        return pair<iterator, bool>(j, false);
    }
    iterator insert_unique(iterator position, const_reference v) {
        if (position.node == header->left) {
            if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
                return __insert(position.node, position.node, v);
            else  return insert_unique(v).first;
        }
        else if (position.node == header) {
            if (key_compare(key(rightmost()), KeyOfValue()(v)))
                return __insert(0, rightmost(), v);
            else  return insert_unique(v).first;
        }
        else {
            iterator before = position;
            --before;
            if (key_compare(key(before.node), KeyOfValue()(v))
                && key_compare(KeyOfValue()(v), key(position.node)))
                if (right(before.node) == 0)
                    return __insert(0, before.node, v);
                else  return __insert(position.node, position.node, v);
            else  return insert_unique(v).first;
        }
    }
    template <typename Iterator>
    void insert_unique(Iterator first, Iterator last) {
        for (; first != last; ++first) insert_unique(*first);
    }
    iterator insert_equal(const_reference v) {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            y = x;
            x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
        }
        return __insert(x, y, v);
    }
    iterator insert_equal(iterator position, const_reference v) {
        if (position.node == header->left) {
            if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
                return __insert(position.node, position.node, v);
            else  return insert_equal(v);

        }
        else if (position.node == header) {
            if (!key_compare(KeyOfValue()(v), key(rightmost())))
                return __insert(0, rightmost(), v);
            else  return insert_equal(v);
        }
        else {
            iterator before = position;
            --before;
            if (!key_compare(KeyOfValue()(v), key(before.node))
                && !key_compare(key(position.node), KeyOfValue()(v)))
                if (right(before.node) == 0) return __insert(0, before.node, v);
                else  return __insert(position.node, position.node, v);
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
        link_type y = (link_type)__rb_tree_rebalance_for_erase(position.node, header->parent,
            header->left, header->right);
        destroy_node(y);
        --node_count;
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
        if (node_count == 0) return;
        __erase(root());
        leftmost() = header;
        root() = 0;
        rightmost() = header;
        node_count = 0;
    }

    iterator find(const Key& k) {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            if (!key_compare(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        iterator j = iterator(y);
        return (j == end() || key_compare(k, key(j.node))) ? end() : j;
    }
    const_iterator find(const Key& k) const {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            if (!key_compare(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        const_iterator j = const_iterator(y);
        return (j == end() || key_compare(k, key(j.node))) ? end() : j;
    }
    size_type count(const Key& k) const {
        pair<const_iterator, const_iterator> p = equal_range(k);
        size_type n = 0;
        distance(p.first, p.second, n);
        return n;
    }
    iterator lower_bound(const Key& k) {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            if (!key_compare(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return iterator(y);
    }
    const_iterator lower_bound(const Key& k) const {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            if (!key_compare(key(x), k)) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return const_iterator(y);
    }
    iterator upper_bound(const Key& k) {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            if (key_compare(k, key(x))) {
                y = x;
                x = left(x);
            }
            else x = right(x);
        }
        return iterator(y);
    }
    const_iterator upper_bound(const Key& k) const {
        link_type y = header;
        link_type x = root();
        while (x != 0) {
            if (key_compare(k, key(x))) {
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
        if (node_count == 0 || begin() == end())
            return node_count == 0 && begin() == end() &&
            header->left == header && header->right == header;
        int len = __black_count(leftmost(), root());
        for (const_iterator it = begin(); it != end(); ++it) {
            link_type x = (link_type)it.node;
            link_type L = left(x);
            link_type R = right(x);
            if (x->color == __rb_tree_red)
                if ((L && L->color == __rb_tree_red) || (R && R->color == __rb_tree_red))
                    return false;
            if (L && key_compare(key(x), key(L))) return false;
            if (R && key_compare(key(R), key(x))) return false;
            if (!L && !R && __black_count(x, root()) != len) return false;
        }
        if (leftmost() != _rb_tree_node_base::minimum(root())) return false;
        if (rightmost() != _rb_tree_node_base::maximum(root())) return false;
        return true;
    }
};
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
const char* const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__type__ = "rb_tree";

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

MSTL_END_NAMESPACE__

#endif // MSTL_RB_TREE_H__
