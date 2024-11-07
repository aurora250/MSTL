#include "rb_tree.h"

namespace MSTL {
    rb_tree_node_base::base_ptr rb_tree_node_base::minimun(base_ptr x) {
        while (x->left) x = x->left;
        return x;
    }
    rb_tree_node_base::base_ptr rb_tree_node_base::maximun(base_ptr x) {
        while (x->right) x = x->right;
        return x;
    }

    void rb_tree_base_iterator::increment() {
        if (node->right != 0) {
            node = node->right;
            while (node->left != 0)
                node = node->left;
        }
        else {
            base_ptr y = node->parent;
            while (y->right == node) {
                node = y;
                y = y->parent;
            }
            if (node->right != y)
                node = y;

        }
    }
    void rb_tree_base_iterator::decrement() {
        if (node->color == rb_tree_red && node->parent->parent == node)
            node = node->right;
        else if (node->left != 0) {
            base_ptr y = node->left;
            while (y->right != 0)
                y = y->right;
            node = y;
        }
        else {
            base_ptr y = node->parent;
            while (y->left == node) {
                node = y;
                y = y->parent;
            }
            node = y;
        }
    }
}