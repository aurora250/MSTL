#include "avl_tree.h"
MSTL_BEGIN_NAMESPACE__

__avl_tree_node_base::__avl_tree_node_base()
	: left_(nullptr),
	right_(nullptr),
	parent_(nullptr),
	balance_(0) {}

void avl_tree_base_iterator::increment() {
	if (node_->left_ == nullptr && node_->right_ == nullptr) {
		base_ptr parent = node_->parent_;
		if (parent->left_ == node_) {
			node_ = parent;
		}
		else if (parent->right_ == node_) {
			while (node_->parent_->right_ == node_) {
				node_ = node_->parent_;
			}
			node_ = node_->parent_;
		}
		else Exception(AssertError());
	}
	else {
		base_ptr base_right = node_->right_;
		if (base_right != nullptr) {
			while (base_right->left_ != nullptr) {
				base_right = base_right->left_;
			}
			node_ = base_right;
		}
		else
			node_ = node_->parent_;
	}
}
void avl_tree_base_iterator::decrement() {
	if (node_->left_ == nullptr && node_->right_ == nullptr) {
		base_ptr parent = node_->parent_;
		if (parent->right_ == node_) {
			node_ = parent;
		}
		else if (parent->left_ == node_) {
			while (node_->parent_->left_ == node_) {
				node_ = node_->parent_;
			}
			node_ = node_->parent_;
		}
		else Exception(AssertError());
	}
	else {
		base_ptr base_left = node_->left_;
		if (base_left != nullptr) {
			while (base_left->right_ != nullptr) {
				base_left = base_left->right_;
			}
			node_ = base_left;
		}
		else
			node_ = node_->parent_;
	}
}

void __rotate_right(__avl_tree_node_base* parent, __avl_tree_node_base*& root) {
	//__avl_tree_node_base* header = nullptr;
	//if (parent->parent_ == root) {
	//	header = root->parent_;
	//	parent->parent_ = nullptr;
	//}
	__avl_tree_node_base* cur = parent->left_;
	__avl_tree_node_base* cur_right = cur->right_;
	parent->left_ = cur_right;
	if (cur_right != nullptr)
		cur_right->parent_ = parent;

	__avl_tree_node_base* grand_parent = parent->parent_;
	cur->right_ = parent;
	parent->parent_ = cur;
	if (parent == root) {
		cur->parent_ = root->parent_;
		root = cur;
	}
	else {
		if (parent == grand_parent->left_)
			grand_parent->left_ = cur;
		else
			grand_parent->right_ = cur;
		cur->parent_ = grand_parent;
	}
	parent->balance_ = 0;
	cur->balance_ = 0;
}
void __rotate_left(__avl_tree_node_base* parent, __avl_tree_node_base*& root) {
	__avl_tree_node_base* cur = parent->right_;
	__avl_tree_node_base* cur_left = cur->left_;
	parent->right_ = cur_left;
	if (cur_left != nullptr)
		cur_left->parent_ = parent;

	__avl_tree_node_base* grand_parent = parent->parent_;
	cur->left_ = parent;
	parent->parent_ = cur;
	if (parent == root) {
		cur->parent_ = root->parent_;
		root = cur;
	}
	else {
		if (grand_parent->left_ == parent)
			grand_parent->left_ = cur;
		else
			grand_parent->right_ = cur;
		cur->parent_ = grand_parent;
	}
	parent->balance_ = 0;
	cur->balance_ = 0;
}
void __rotate_left_right(__avl_tree_node_base* parent, __avl_tree_node_base*& root) {
	__avl_tree_node_base* cur = parent->left_;
	__avl_tree_node_base* cur_right = cur->right_;
	short bf = cur_right->balance_;
	__rotate_left(cur, root);
	__rotate_right(parent, root);

	if (bf == 1) {
		cur_right->balance_ = 0;
		cur->balance_ = -1;
		parent->balance_ = 0;
	}
	else if (bf == -1) {
		cur_right->balance_ = 0;
		cur->balance_ = 0;
		parent->balance_ = 1;
	}
	else if (bf == 0) {
		cur_right->balance_ = 0;
		cur->balance_ = 0;
		parent->balance_ = 0;
	}
	else Exception(AssertError());
}
void __rotate_right_left(__avl_tree_node_base* parent, __avl_tree_node_base*& root) {
	__avl_tree_node_base* cur = parent->right_;
	__avl_tree_node_base* cur_left = cur->left_;
	short bf = cur_left->balance_;
	__rotate_right(cur, root);
	__rotate_left(parent, root);

	if (bf == 1) {
		cur_left->balance_ = 0;
		parent->balance_ = -1;
		cur->balance_ = 0;
	}
	else if (bf == -1) {
		cur_left->balance_ = 0;
		parent->balance_ = 0;
		cur->balance_ = 1;
	}
	else if (bf == 0) {
		cur_left->balance_ = 0;
		parent->balance_ = 0;
		cur->balance_ = 0;
	}
	else Exception(AssertError());
}

MSTL_END_NAMESPACE__
