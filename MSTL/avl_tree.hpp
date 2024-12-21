#ifndef	MSTL_AVL_TREE_HPP__
#define MSTL_AVL_TREE_HPP__
#include "basiclib.h"
#include "pair.hpp"
#include "errorlib.h"
#include "mathlib.h"

MSTL_BEGIN_NAMESPACE__
template<class Key, class Value>
struct __avl_tree_node {
	typedef __avl_tree_node<Key, Value>*	link_type;
	typedef pair<Key, Value>				value_type;
	typedef int								balance_type;

	__avl_tree_node(const value_type& kv)
		: left_(nullptr)
		, right_(nullptr)
		, parent_(nullptr)
		, balance_(0)
		, data_(kv) {}
	~__avl_tree_node() {
		if (left_ != nullptr) delete left_;
		if (right_ != nullptr) delete right_;
		if (parent_ != nullptr) delete parent_;
		balance_ = 0;
	}

	link_type left_;
	link_type right_;
	link_type parent_;
	balance_type balance_;
	value_type data_;
};

template<class Key, class Value>
class avl_tree {
public:
	typedef __avl_tree_node<Key, Value>			node_type;
	typedef typename node_type::balance_type	balance_type;
	typedef node_type*			link_type;
	typedef pair<Key, Value>	value_type;
	typedef Key					key_type;
	typedef	Value				value_of_key;
	typedef size_t				size_type;

	avl_tree() : root_(nullptr), size_(0) {}
	~avl_tree() {
		if (root_ != nullptr) delete root_;
		size_ = 0;
	}

	void rotate_right(link_type parent) {
		link_type cur = parent->left_;
		link_type curR = cur->right_;
		link_type pparent = parent->parent_;
		parent->left_ = curR;
		if (curR)
			curR->parent_ = parent;
		cur->right_ = parent;
		parent->parent_ = cur;
		if (pparent == nullptr) {
			root_ = cur;
			cur->parent_ = nullptr;
		}
		else {
			cur->parent_ = pparent;
			if (parent == pparent->left_)
				pparent->left_ = cur;
			else
				pparent->right_ = cur;
		}
		parent->balance_ = 0;
		cur->balance_ = 0;
	}
	void rotate_left(link_type parent) {
		link_type cur = parent->right_;
		link_type curL = cur->left_;
		link_type pprent = parent->parent_;
		parent->right_ = curL;
		if (curL)
			curL->parent_ = parent;
		cur->left_ = parent;
		parent->parent_ = cur;
		if (pprent == nullptr) {
			root_ = cur;
			cur->parent_ = nullptr;
		}
		else {
			cur->parent_ = pprent;
			if (pprent->left_ == parent)
				pprent->left_ = cur;
			else
				pprent->right_ = cur;
		}
		parent->balance_ = 0;
		cur->balance_ = 0;
	}
	void rotate_left_right(link_type parent) {
		link_type cur = parent->left_;
		link_type curR = cur->right_;
		balance_type bf = curR->balance_;
		rotate_left(cur);
		rotate_right(parent);

		if (bf == 1) {
			curR->balance_ = 0;
			cur->balance_ = -1;
			parent->balance_ = 0;
		}
		else if (bf == -1) {
			curR->balance_ = 0;
			cur->balance_ = 0;
			parent->balance_ = 1;
		}
	}
	void rotate_right_left(link_type parent) {
		link_type cur = parent->right_;
		link_type curL = cur->left_;
		balance_type bf = curL->balance_;
		rotate_right(cur);
		rotate_left(parent);

		if (bf == 1) {
			curL->balance_ = 0;
			parent->balance_ = -1;
			cur->balance_ = 0;
		}
		else if (bf == -1) {
			curL->balance_ = 0;
			parent->balance_ = 0;
			cur->balance_ = 1;
		}
		else Exception(AssertError());
	}
	bool insert(const pair<Key, Value>& kv) {
		if (root_ == nullptr) {
			root_ = new node_type(kv);
			size_++;
			return true;
		}
		link_type parent = nullptr;
		link_type cur = root_;
		while (cur) {
			if (kv.first < cur->data_.first) {
				parent = cur;
				cur = cur->left_;
			}
			else if (kv.first > cur->data_.first) {
				parent = cur;
				cur = cur->right_;
			}
			else
				return false;
		}

		cur = new node_type(kv);
		if (parent->data_.first > kv.first) {
			parent->left_ = cur;
			cur->parent_ = parent;
		}
		else {
			parent->right_ = cur;
			cur->parent_ = parent;
		}
		while (parent) {
			if (parent->left_ == cur)
				parent->balance_--;
			else
				parent->balance_++;

			if (parent->balance_ == 0) break;
			else if (parent->balance_ == 1 || parent->balance_ == -1) {
				cur = parent;
				parent = parent->parent_;
			}
			else if (parent->balance_ == 2 || parent->balance_ == -2) {
				if (parent->balance_ == -2) {
					if (cur->balance_ == -1)
						rotate_right(parent);
					else
						rotate_left_right(parent);
				}
				else {
					if (cur->balance_ == 1)
						rotate_left(parent);
					else
						rotate_right_left(parent);
				}
				break;
			}
			else Exception(AssertError("AVL Insert Failed"));
		}
		size_++;
		return true;
	}
	bool erase(const Key& key) {
		link_type parent = nullptr;
		link_type cur = root_;
		link_type delParentPos = nullptr;
		link_type delPos = nullptr;

		while (cur) {
			if (key < cur->data_.first) {
				parent = cur;
				cur = cur->left_;
			}
			if (key > cur->data_.first) {
				parent = cur;
				cur = cur->right_;
			}
			else {
				if (cur->left_ == nullptr) {
					if (cur == root_) {
						root_ = root_->right_;
						if (root_) root_->parent_ = nullptr;
						delete cur;
						size_--;
						return true;
					}
					else {
						delParentPos = parent;
						delPos = cur;
					}
					break;
				}
				else if (cur->right_ == nullptr) {
					if (cur == root_) {
						root_ = root_->left_;
						if (root_) root_->parent_ = nullptr;
						delete cur;
						size_--;
						return true;
					}
					else {
						delParentPos = parent;
						delPos = cur;
					}
					break;
				}
				else {
					link_type minParent = cur;
					link_type minRight = cur->right_;
					while (minRight->left_) {
						minParent = minRight;
						minRight = minRight->left_;
					}
					cur->data_.first = minRight->data_.first;
					cur->data_.second = minRight->data_.second;
					delParentPos = minParent;
					delPos = minRight;
					break;
				}
			}
		}
		if (delParentPos == nullptr)
			return false;
		
		link_type del = delPos;
		link_type delP = delParentPos;
		while (delPos != root_) {
			if (delPos == delParentPos->left_)
				delParentPos->balance_++;
			else if (delPos == delParentPos->right_)
				delParentPos->balance_--;

			if (delParentPos->balance_ == -1 || delParentPos->balance_ == 1)
				break;
			else if (delParentPos->balance_ == 0) {
				delPos = delParentPos;
				delParentPos = delParentPos->parent_;
			}
			else if (delParentPos->balance_ == -2 || delParentPos->balance_ == 2) {
				if (delParentPos->balance_ == -2) {
					if (delParentPos->left_->balance_ == -1) {
						link_type tmp = delParentPos->left_;
						rotate_right(delParentPos);
						delParentPos = tmp;
					}
					else if (delParentPos->left_->balance_ == 1) {
						link_type tmp = delParentPos->left_->right_;
						rotate_left_right(delParentPos);
						delParentPos = tmp;
					}
					else {
						link_type tmp = delParentPos->left_;
						rotate_right(delParentPos);
						delParentPos = tmp;
						delParentPos->balance_ = 1;
						delParentPos->right_->balance_ = -1;
						break;
					}
				}
				else {
					if (delParentPos->right_->balance_ == -1) {
						link_type tmp = delParentPos->right_->left_;
						rotate_right_left(delParentPos);
						delParentPos = tmp;
					}
					else if (delParentPos->right_->balance_ == 1) {
						link_type tmp = delParentPos->right_;
						rotate_left(delParentPos);
						delParentPos = tmp;
					}
					else {
						link_type tmp = delParentPos->right_;
						rotate_left(delParentPos);
						delParentPos = tmp;
						delParentPos->balance_ = -1;
						delParentPos->left_->balance_ = 1;
						break;
					}
				}
				delPos = delParentPos;
				delParentPos = delParentPos->parent_;
			}
			else Exception(AssertError("AVL Delete Failed"));
		}
		if (del->left_ == nullptr) {
			if (del == delP->left_) {
				delP->left_ = del->right_;
				if (del->right_)
					del->right_->parent_ = parent;
			}
			else {
				delP->right_ = del->right_;
				if (del->right_)
					del->right_->parent_ = parent;
			}
		}
		else {
			if (del == delP->left_) {
				delP->left_ = del->left_;
				if (del->left_)
					del->left_->parent_ = parent;
			}
			else {
				delP->right_ = del->left_;
				if (del->left_)
					del->left_->parent_ = parent;
			}
		}
		delete del;
		size_--;
		return true;
	}

	link_type find(const Key& k) {
		link_type cur = root_;
		while (cur) {
			if (cur->data_.first > k)
				cur = cur->left_;
			else if (cur->data_.first < k)
				cur = cur->right_;
			else
				return cur;
		}
		return nullptr;
	}
	size_type depth(link_type root) {
		if (root == nullptr) 
			return 0;
		size_type leftDepth = depth(root->left_);
		size_type rightDepth = depth(root->right_);
		return leftDepth > rightDepth ? leftDepth + 1 : rightDepth + 1;
	}
	bool is_balanced() {
		return __balanced(root_);
	}
private:
	link_type root_;
	size_type size_;

	bool __balanced(link_type root) {
		if (root == nullptr)
			return true;
		mathl_t leftHight = (mathl_t)depth(root->left_);
		mathl_t rightHight = (mathl_t)depth(root->right_);
		return absolute(leftHight - rightHight) < 2 && __balanced(root->left_) && __balanced(root->right_);
	}
};

MSTL_END_NAMESPACE__
#endif // MSTL_AVL_TREE_HPP__
