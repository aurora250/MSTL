#ifndef RB_TREE_H
#define RB_TREE_H
#include "basiclib.h"
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
        typedef Ptr                         pointer;
        typedef rb_tree_iterator<T, T&, T*> iterator;
        typedef rb_tree_iterator<T, const T&, const T*> const_iterator;
        typedef rb_tree_iterator<T, Ref, Ptr>           self;
        typedef rb_tree_node<T>*            link_type;

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
    };
}

#endif // RB_TREE_H
