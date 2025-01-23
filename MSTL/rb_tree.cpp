#include "rb_tree.h"

MSTL_BEGIN_NAMESPACE__

__rb_tree_node_base::base_ptr __rb_tree_node_base::minimum(base_ptr x) noexcept {
    while (x->left_ != 0) x = x->left_;
    return x;
}
__rb_tree_node_base::base_ptr __rb_tree_node_base::maximum(base_ptr x) noexcept {
    while (x->right_ != 0) x = x->right_;
    return x;
}

void __rb_tree_base_iterator::increment() noexcept {
    if (node_->right_ != 0) {
        node_ = node_->right_;
        while (node_->left_ != 0) node_ = node_->left_;
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
void __rb_tree_base_iterator::decrement() noexcept {
    if (node_->color_ == RB_TREE_RED__ && node_->parent_->parent_ == node_) {
        node_ = node_->right_;
    }
    else if (node_->left_ != 0) {
        base_ptr y = node_->left_;
        while (y->right_ != 0) y = y->right_;
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

void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root) noexcept {
    __rb_tree_node_base* y = x->right_;
    x->right_ = y->left_;
    if (y->left_ != 0) y->left_->parent_ = x;
    y->parent_ = x->parent_;
    if (x == root) root = y;
    else if (x == x->parent_->left_) x->parent_->left_ = y;
    else x->parent_->right_ = y;
    y->left_ = x;
    x->parent_ = y;
}
void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root) noexcept {
    __rb_tree_node_base* y = x->left_;
    x->left_ = y->right_;
    if (y->right_ != 0) y->right_->parent_ = x;
    y->parent_ = x->parent_;
    if (x == root) root = y;
    else if (x == x->parent_->right_) x->parent_->right_ = y;
    else x->parent_->left_ = y;
    y->right_ = x;
    x->parent_ = y;
}
void rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) noexcept {
    x->color_ = RB_TREE_RED__;
    while (x != root && x->parent_->color_ == RB_TREE_RED__) {
        if (x->parent_ == x->parent_->parent_->left_) {
            __rb_tree_node_base* y = x->parent_->parent_->right_;
            if (y && y->color_ == RB_TREE_RED__) {
                x->parent_->color_ = RB_TREE_BLACK__;
                y->color_ = RB_TREE_BLACK__;
                x->parent_->parent_->color_ = RB_TREE_RED__;
                x = x->parent_->parent_;
            }
            else {
                if (x == x->parent_->right_) {
                    x = x->parent_;
                    __rb_tree_rotate_left(x, root);
                }
                x->parent_->color_ = RB_TREE_BLACK__;
                x->parent_->parent_->color_ = RB_TREE_RED__;
                __rb_tree_rotate_right(x->parent_->parent_, root);
            }
        }
        else {
            __rb_tree_node_base* y = x->parent_->parent_->left_;
            if (y && y->color_ == RB_TREE_RED__) {
                x->parent_->color_ = RB_TREE_BLACK__;
                y->color_ = RB_TREE_BLACK__;
                x->parent_->parent_->color_ = RB_TREE_RED__;
                x = x->parent_->parent_;
            }
            else {
                if (x == x->parent_->left_) {
                    x = x->parent_;
                    __rb_tree_rotate_right(x, root);
                }
                x->parent_->color_ = RB_TREE_BLACK__;
                x->parent_->parent_->color_ = RB_TREE_RED__;
                __rb_tree_rotate_left(x->parent_->parent_, root);
            }
        }
    }
    root->color_ = RB_TREE_BLACK__;
}
__rb_tree_node_base* __rb_tree_rebalance_for_erase(
    __rb_tree_node_base* z, __rb_tree_node_base*& root,
    __rb_tree_node_base*& leftmost, __rb_tree_node_base*& rightmost) noexcept {
    __rb_tree_node_base* y = z;
    __rb_tree_node_base* x = 0;
    __rb_tree_node_base* x_parent = 0;
    if (y->left_ == 0) x = y->right_;
    else {
        if (y->right_ == 0) x = y->left_;
        else {
            y = y->right_;
            while (y->left_ != 0) y = y->left_;
            x = y->right_;
        }
    }
    if (y != z) {
        z->left_->parent_ = y;
        y->left_ = z->left_;
        if (y != z->right_) {
            x_parent = y->parent_;
            if (x) x->parent_ = y->parent_;
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
        std::swap(y->color_, z->color_);
        y = z;
    }
    else {
        x_parent = y->parent_;
        if (x) x->parent_ = y->parent_;
        if (root == z) root = x;
        else {
            if (z->parent_->left_ == z) z->parent_->left_ = x;
            else z->parent_->right_ = x;
        }
        if (leftmost == z) {
            if (z->right_ == 0) leftmost = z->parent_;
            else leftmost = __rb_tree_node_base::minimum(x);
        }
        if (rightmost == z) {
            if (z->left_ == 0) rightmost = z->parent_;
            else rightmost = __rb_tree_node_base::maximum(x);
        }
    }
    if (y->color_ != RB_TREE_RED__) {
        while (x != root && (x == 0 || x->color_ == RB_TREE_BLACK__)) {
            if (x == x_parent->left_) {
                __rb_tree_node_base* w = x_parent->right_;
                if (w->color_ == RB_TREE_RED__) {
                    w->color_ = RB_TREE_BLACK__;
                    x_parent->color_ = RB_TREE_RED__;
                    __rb_tree_rotate_left(x_parent, root);
                    w = x_parent->right_;
                }
                if ((w->left_ == 0 || w->left_->color_ == RB_TREE_BLACK__) &&
                    (w->right_ == 0 || w->right_->color_ == RB_TREE_BLACK__)) {
                    w->color_ = RB_TREE_RED__;
                    x = x_parent;
                    x_parent = x_parent->parent_;
                }
                else {
                    if (w->right_ == 0 || w->right_->color_ == RB_TREE_BLACK__) {
                        if (w->left_) w->left_->color_ = RB_TREE_BLACK__;
                        w->color_ = RB_TREE_RED__;
                        __rb_tree_rotate_right(w, root);
                        w = x_parent->right_;
                    }
                    w->color_ = x_parent->color_;
                    x_parent->color_ = RB_TREE_BLACK__;
                    if (w->right_) w->right_->color_ = RB_TREE_BLACK__;
                    __rb_tree_rotate_left(x_parent, root);
                    break;
                }
            }
            else {
                __rb_tree_node_base* w = x_parent->left_;
                if (w->color_ == RB_TREE_RED__) {
                    w->color_ = RB_TREE_BLACK__;
                    x_parent->color_ = RB_TREE_RED__;
                    __rb_tree_rotate_right(x_parent, root);
                    w = x_parent->left_;
                }
                if ((w->right_ == 0 || w->right_->color_ == RB_TREE_BLACK__) &&
                    (w->left_ == 0 || w->left_->color_ == RB_TREE_BLACK__)) {
                    w->color_ = RB_TREE_RED__;
                    x = x_parent;
                    x_parent = x_parent->parent_;
                }
                else {
                    if (w->left_ == 0 || w->left_->color_ == RB_TREE_BLACK__) {
                        if (w->right_) w->right_->color_ = RB_TREE_BLACK__;
                        w->color_ = RB_TREE_RED__;
                        __rb_tree_rotate_left(w, root);
                        w = x_parent->left_;
                    }
                    w->color_ = x_parent->color_;
                    x_parent->color_ = RB_TREE_BLACK__;
                    if (w->left_) w->left_->color_ = RB_TREE_BLACK__;
                    __rb_tree_rotate_right(x_parent, root);
                    break;
                }
            }
        }
        if (x) x->color_ = RB_TREE_BLACK__;
    }
    return y;
}

int __black_count(__rb_tree_node_base* node, __rb_tree_node_base* root) noexcept {
    if (node == 0) return 0;
    else {
        int bc = node->color_ == RB_TREE_BLACK__ ? 1 : 0;
        if (node == root) return bc;
        else  return bc + __black_count(node->parent_, root);
    }
}

MSTL_END_NAMESPACE__
