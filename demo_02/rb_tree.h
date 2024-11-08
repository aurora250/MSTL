#ifndef RB_TREE_H
#define RB_TREE_H
#include "basiclib.h"
#include "pair.hpp"
#include <iterator>

namespace MSTL {
    typedef bool rb_tree_color_type;
    const rb_tree_color_type rb_tree_red = false;
    const rb_tree_color_type rb_tree_black = true;

    struct rb_tree_node_base {
        typedef rb_tree_color_type  color_type;
        typedef rb_tree_node_base*  base_ptr;

        color_type color;
        base_ptr parent;
        base_ptr left;
        base_ptr right;
        static base_ptr minimun(base_ptr x);
        static base_ptr maximun(base_ptr x);
    };
    template <typename T>
    struct rb_tree_node : public rb_tree_node_base {
        typedef rb_tree_node<T>* link_type;
        T value_field;
    };

    struct rb_tree_base_iterator {
        typedef rb_tree_node_base::base_ptr     base_ptr;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t                       difference_type;

        base_ptr node;
        void increment();
        void decrement();
    };
    template <typename T, typename Ref = T&, typename Ptr = T*>
    struct rb_tree_iterator : public rb_tree_base_iterator {
        typedef T                           value_type;
        typedef Ref                         reference;
        typedef const T&                    const_reference;
        typedef Ptr                         pointer;
        typedef rb_tree_iterator<T, T&, T*> iterator;
        typedef rb_tree_iterator<T, const T&, const T*> const_iterator;
        typedef rb_tree_iterator<T, Ref, Ptr>           self;
        typedef rb_tree_node<T>*                        link_type;

        rb_tree_iterator() {}
        rb_tree_iterator(link_type x) { node = x; }
        rb_tree_iterator(const iterator& it) { node = it.node; }

        reference operator*() const {
            return link_type(node)->value_field;
        }
        pointer operator->() const {
            return &(operator*());
        }
        self& operator++() {
            increment();
            return *this;
        }
        self operator++(int) {
            self tmp = *this;
            increment();
            return tmp;
        }
        self& operator--() {
            decrement();
            return *this;
        }
        self operator--() {
            self tmp = *this;
            decrement();
            return tmp;
        }

        /*pair<iterator, bool> insert_unique(const_reference v) {
            link_type y = header;
            link_type x = root();
            bool comp = true;
            while (x != 0) {
                y = x;
                comp = key_compare(KeyOfValue()(v), key(x));
                x = comp ? left(x) : right(x);
            }
            iterator j = iterator(y);
            if (comp)
                if (j == begin())
                    return pair<iterator, bool>{_insert(x, y, v), true};
                else --j;
            if (key_compare(key(j.node), KeyOfValue()(v)))
                return pair<iterator, bool>{_insert(x, y, v), true};
            return pair<iterator, bool>{j, false};
        }
        iterator insert_equal(const_reference v) {
            link_type y = header;
            link_type x = root();
            while (x != 0) {
                y = x;
                x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
            }
            return _insert(x, y, v);
        }*/
    };
}

#endif // RB_TREE_H
