#include "rb_tree.h"

MSTL_BEGIN_NAMESPACE__

__rb_tree_node_base::base_ptr __rb_tree_node_base::minimum(base_ptr x) {
    while (x->left != 0) x = x->left;
    return x;
}
__rb_tree_node_base::base_ptr __rb_tree_node_base::maximum(base_ptr x) {
    while (x->right != 0) x = x->right;
    return x;
}

void __rb_tree_base_iterator::increment() {
    if (node->right != 0) {
        node = node->right;
        while (node->left != 0) node = node->left;
    }
    else {
        base_ptr y = node->parent;
        while (node == y->right) {
            node = y;
            y = y->parent;
        }
        if (node->right != y) node = y;
    }
}
void __rb_tree_base_iterator::decrement() {
    if (node->color == __RB_TREE_RED && node->parent->parent == node) {
        node = node->right;
    }
    else if (node->left != 0) {
        base_ptr y = node->left;
        while (y->right != 0) y = y->right;
        node = y;
    }
    else {
        base_ptr y = node->parent;
        while (node == y->left) {
            node = y;
            y = y->parent;
        }
        node = y;
    }
}

void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
    __rb_tree_node_base* y = x->right;
    x->right = y->left;
    if (y->left != 0) y->left->parent = x;
    y->parent = x->parent;
    if (x == root) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}
void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
    __rb_tree_node_base* y = x->left;
    x->left = y->right;
    if (y->right != 0) y->right->parent = x;
    y->parent = x->parent;
    if (x == root) root = y;
    else if (x == x->parent->right) x->parent->right = y;
    else x->parent->left = y;
    y->right = x;
    x->parent = y;
}
void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
    x->color = __RB_TREE_RED;
    while (x != root && x->parent->color == __RB_TREE_RED) {
        if (x->parent == x->parent->parent->left) {
            __rb_tree_node_base* y = x->parent->parent->right;
            if (y && y->color == __RB_TREE_RED) {
                x->parent->color = __RB_TREE_BLACK;
                y->color = __RB_TREE_BLACK;
                x->parent->parent->color = __RB_TREE_RED;
                x = x->parent->parent;
            }
            else {
                if (x == x->parent->right) {
                    x = x->parent;
                    __rb_tree_rotate_left(x, root);
                }
                x->parent->color = __RB_TREE_BLACK;
                x->parent->parent->color = __RB_TREE_RED;
                __rb_tree_rotate_right(x->parent->parent, root);
            }
        }
        else {
            __rb_tree_node_base* y = x->parent->parent->left;
            if (y && y->color == __RB_TREE_RED) {
                x->parent->color = __RB_TREE_BLACK;
                y->color = __RB_TREE_BLACK;
                x->parent->parent->color = __RB_TREE_RED;
                x = x->parent->parent;
            }
            else {
                if (x == x->parent->left) {
                    x = x->parent;
                    __rb_tree_rotate_right(x, root);
                }
                x->parent->color = __RB_TREE_BLACK;
                x->parent->parent->color = __RB_TREE_RED;
                __rb_tree_rotate_left(x->parent->parent, root);
            }
        }
    }
    root->color = __RB_TREE_BLACK;
}
__rb_tree_node_base* __rb_tree_rebalance_for_erase(
    __rb_tree_node_base* z, __rb_tree_node_base*& root,
    __rb_tree_node_base*& leftmost, __rb_tree_node_base*& rightmost) {
    __rb_tree_node_base* y = z;
    __rb_tree_node_base* x = 0;
    __rb_tree_node_base* x_parent = 0;
    if (y->left == 0) x = y->right;
    else {
        if (y->right == 0) x = y->left;
        else {
            y = y->right;
            while (y->left != 0) y = y->left;
            x = y->right;
        }
    }
    if (y != z) {
        z->left->parent = y;
        y->left = z->left;
        if (y != z->right) {
            x_parent = y->parent;
            if (x) x->parent = y->parent;
            y->parent->left = x;
            y->right = z->right;
            z->right->parent = y;
        }
        else
            x_parent = y;
        if (root == z)
            root = y;
        else if (z->parent->left == z)
            z->parent->left = y;
        else
            z->parent->right = y;
        y->parent = z->parent;
        std::swap(y->color, z->color);
        y = z;
    }
    else {
        x_parent = y->parent;
        if (x) x->parent = y->parent;
        if (root == z) root = x;
        else {
            if (z->parent->left == z) z->parent->left = x;
            else z->parent->right = x;
        }
        if (leftmost == z) {
            if (z->right == 0) leftmost = z->parent;
            else leftmost = __rb_tree_node_base::minimum(x);
        }
        if (rightmost == z) {
            if (z->left == 0) rightmost = z->parent;
            else rightmost = __rb_tree_node_base::maximum(x);
        }
    }
    if (y->color != __RB_TREE_RED) {
        while (x != root && (x == 0 || x->color == __RB_TREE_BLACK)) {
            if (x == x_parent->left) {
                __rb_tree_node_base* w = x_parent->right;
                if (w->color == __RB_TREE_RED) {
                    w->color = __RB_TREE_BLACK;
                    x_parent->color = __RB_TREE_RED;
                    __rb_tree_rotate_left(x_parent, root);
                    w = x_parent->right;
                }
                if ((w->left == 0 || w->left->color == __RB_TREE_BLACK) &&
                    (w->right == 0 || w->right->color == __RB_TREE_BLACK)) {
                    w->color = __RB_TREE_RED;
                    x = x_parent;
                    x_parent = x_parent->parent;
                }
                else {
                    if (w->right == 0 || w->right->color == __RB_TREE_BLACK) {
                        if (w->left) w->left->color = __RB_TREE_BLACK;
                        w->color = __RB_TREE_RED;
                        __rb_tree_rotate_right(w, root);
                        w = x_parent->right;
                    }
                    w->color = x_parent->color;
                    x_parent->color = __RB_TREE_BLACK;
                    if (w->right) w->right->color = __RB_TREE_BLACK;
                    __rb_tree_rotate_left(x_parent, root);
                    break;
                }
            }
            else {
                __rb_tree_node_base* w = x_parent->left;
                if (w->color == __RB_TREE_RED) {
                    w->color = __RB_TREE_BLACK;
                    x_parent->color = __RB_TREE_RED;
                    __rb_tree_rotate_right(x_parent, root);
                    w = x_parent->left;
                }
                if ((w->right == 0 || w->right->color == __RB_TREE_BLACK) &&
                    (w->left == 0 || w->left->color == __RB_TREE_BLACK)) {
                    w->color = __RB_TREE_RED;
                    x = x_parent;
                    x_parent = x_parent->parent;
                }
                else {
                    if (w->left == 0 || w->left->color == __RB_TREE_BLACK) {
                        if (w->right) w->right->color = __RB_TREE_BLACK;
                        w->color = __RB_TREE_RED;
                        __rb_tree_rotate_left(w, root);
                        w = x_parent->left;
                    }
                    w->color = x_parent->color;
                    x_parent->color = __RB_TREE_BLACK;
                    if (w->left) w->left->color = __RB_TREE_BLACK;
                    __rb_tree_rotate_right(x_parent, root);
                    break;
                }
            }
        }
        if (x) x->color = __RB_TREE_BLACK;
    }
    return y;
}

int __black_count(__rb_tree_node_base* node, __rb_tree_node_base* root) {
    if (node == 0) return 0;
    else {
        int bc = node->color == __RB_TREE_BLACK ? 1 : 0;
        if (node == root) return bc;
        else  return bc + __black_count(node->parent, root);
    }
}

MSTL_END_NAMESPACE__
