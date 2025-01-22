#ifndef	MSTL_AVL_TREE_HPP__
#define MSTL_AVL_TREE_HPP__
#include "pair.hpp"
#include "errorlib.h"
#include "mathlib.h"
#include "memory.hpp"
#include "algobase.hpp"

MSTL_BEGIN_NAMESPACE__
struct __avl_tree_node_base {
	typedef __avl_tree_node_base*	base_ptr;
	typedef short					balance_type;

	__avl_tree_node_base();
	__avl_tree_node_base(const __avl_tree_node_base&) = default;
	__avl_tree_node_base(__avl_tree_node_base&&) = default;
	~__avl_tree_node_base() = default;

	base_ptr left_;
	base_ptr right_;
	base_ptr parent_;
	balance_type balance_;
};

template<class T>
struct __avl_tree_node : public __avl_tree_node_base {
	typedef T					value_type;
	typedef __avl_tree_node<T>	node_type;
	typedef node_type*			link_type;

	__avl_tree_node() : data_() {}
	__avl_tree_node(const value_type& v) : data_(v) {}
	__avl_tree_node(value_type&& v) : data_(v) {}
	~__avl_tree_node() = default;

	value_type data_;
};

struct avl_tree_base_iterator {
	typedef __avl_tree_node_base*			base_ptr;
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef ptrdiff_t                       difference_type;
	base_ptr node_;

	void increment();
	void decrement();
};

template<typename T, typename Ref = T&, typename Ptr = T*>
struct avl_tree_iterator : public avl_tree_base_iterator {
	typedef T                               value_type;
	typedef Ref                             reference;
	typedef Ptr                             pointer;
	typedef avl_tree_iterator<T, T&, T*>	iterator;
	typedef avl_tree_iterator<T, const T&, const T*> const_iterator;
	typedef const T&						const_reference;
	typedef avl_tree_iterator<T, Ref, Ptr>  self;
	typedef __avl_tree_node<T>*				link_type;

	avl_tree_iterator() {}
	avl_tree_iterator(link_type x) { node_ = x; }
	avl_tree_iterator(const iterator& it) { node_ = it.node_; }

	reference operator *() const { return link_type(node_)->data_; }
	pointer operator ->() const { return &(operator*()); }

	self& operator ++() {
		increment();
		return *this;
	}
	self operator ++(int) {
		self tmp = *this;
		increment();
		return tmp;
	}
	self& operator --() {
		decrement();
		return *this;
	}
	self operator --(int) {
		self tmp = *this;
		decrement();
		return tmp;
	}
};
MSTL_NODISCARD inline bool operator ==(
	const avl_tree_base_iterator& x, const avl_tree_base_iterator& y) {
	return x.node_ == y.node_;
}
MSTL_NODISCARD inline bool operator !=(
	const avl_tree_base_iterator& x, const avl_tree_base_iterator& y) {
	return x.node_ != y.node_;
}

void __rotate_right(__avl_tree_node_base* parent, __avl_tree_node_base*& root);
void __rotate_left(__avl_tree_node_base* parent, __avl_tree_node_base*& root);
void __rotate_left_right(__avl_tree_node_base* parent, __avl_tree_node_base*& root);
void __rotate_right_left(__avl_tree_node_base* parent, __avl_tree_node_base*& root);

template <typename Key, typename Value, typename KeyOfValue, typename Compare,
	typename Alloc = default_standard_alloc<__avl_tree_node<Value>>>
class avl_tree {
public:
	typedef Key                 key_type;
	typedef Value               value_type;
	typedef value_type*			pointer;
	typedef const value_type*	const_pointer;
	typedef value_type&			reference;
	typedef const value_type&	const_reference;
	typedef size_t				difference_type;
	typedef __avl_tree_node<value_type>			node_type;
	typedef typename node_type::balance_type	balance_type;
	typedef node_type*							link_type;
	typedef size_t								size_type;
	typedef typename __avl_tree_node_base::base_ptr					base_ptr;
	typedef avl_tree_iterator<Value, Value&, Value*>				iterator;
	typedef avl_tree_iterator<Value, const Value&, const Value*>	const_iterator;
	typedef avl_tree<Key, Value, KeyOfValue, Compare, Alloc>		self;

private:
	bool balanced_aux(link_type root) const {
		if (root == nullptr || root == header_) return true;
		return absolute(depth(root->left_) - depth(root->right_)) < 2
			&& balanced_aux(root->left_) && balanced_aux(root->right_);
	}
	void in_order_aux(base_ptr root) const {
		if (root == nullptr || root == header_) return;
		in_order_aux(root->left_);
		std::cout << link_type(root)->data_ << std::endl;
		in_order_aux(root->right_);
	}
	void destroy_tree_aux(base_ptr root) {
		if (root == nullptr) return;
		destroy_tree_aux(root->left_);
		destroy_tree_aux(root->right_);
		delete link_type(root);
	}
	Key& get_key(link_type x) const { return KeyOfValue()(x->data_); }

public:
	avl_tree() : header_(0), size_(0), key_compare_(Compare()) {
		header_ = new node_type();
		header_->parent_ = header_;
	}
	avl_tree(Compare c) : key_compare_(c) {
		header_ = new node_type();
		header_->parent_ = header_;
	}
	~avl_tree() {
		if (header_->parent_ = header_) 
			delete link_type(header_);
		else 
			destroy_tree_aux(header_->parent_);
		size_ = 0;
	}

	iterator begin() {
		base_ptr b = header_->parent_;
		while (b->left_ != nullptr) {
			b = b->left_;
		}
		return iterator(link_type(b));
	}
	iterator end() {
		return iterator(header_);
	}
	const_iterator const_begin() const {
		base_ptr b = header_->parent_;
		while (b->left_ != nullptr) {
			b = b->left_;
		}
		return const_iterator(link_type(b));
	}
	const_iterator const_end() const {
		return const_iterator(header_);
	}
	iterator rbegin() {
		base_ptr b = header_->parent_;
		while (b->right_ != nullptr) {
			b = b->right_;
		}
		return iterator(link_type(b));
	}
	iterator rend() {
		return iterator(header_);
	}

	bool insert(const value_type& v) {
		if (header_->parent_ == header_) {
			header_->parent_ = new node_type(v);
			header_->parent_->parent_ = header_;
			size_++;
			return true;
		}
		link_type parent = nullptr;
		link_type cur = link_type(header_->parent_);
		while (cur != nullptr) {
			if (key_compare_(KeyOfValue()(v), get_key(cur))) {
				parent = cur;
				cur = link_type(cur->left_);
			}
			else if (KeyOfValue()(v) == get_key(cur)) {
				return false;
			}
			else {
				parent = cur;
				cur = link_type(cur->right_);
			}
		}

		cur = new node_type(v);
		if (key_compare_(KeyOfValue()(v), get_key(parent))) 
			parent->left_ = cur;
		else 
			parent->right_ = cur;
		cur->parent_ = parent;

		while (parent != header_) {
			if (parent->left_ == cur)
				parent->balance_--;
			else
				parent->balance_++;

			if (parent->balance_ == 0) break;
			else if (parent->balance_ == 1 || parent->balance_ == -1) {
				cur = parent;
				parent = link_type(parent->parent_);
			}
			else if (parent->balance_ == 2 || parent->balance_ == -2) {
				if (parent->balance_ == -2) {
					if (cur->balance_ == -1)
						(__rotate_right)(parent, header_->parent_);
					else
						(__rotate_left_right)(parent, header_->parent_);
				}
				else {
					if (cur->balance_ == 1)
						(__rotate_left)(parent, header_->parent_);
					else
						(__rotate_right_left)(parent, header_->parent_);
				}
				break;
			}
			else Exception(AssertError("AVL Insert Failed"));
		}
		size_++;
		return true;
	}
	template <typename Iterator>
	void insert(Iterator start, Iterator end) {
		for (Iterator iter = start; start != end; ++iter)
			insert(*iter);
	}

	bool erase(const Key& key) {
		link_type parent = nullptr;
		link_type cur = header_->parent_;
		link_type delete_parent_pos = nullptr;
		link_type delete_pos = nullptr;

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
					if (cur == header_->parent_) {
						header_->parent_ = header_->parent_->right_;
						if (header_->parent_) header_->parent_->parent_ = nullptr;
						delete cur;
						size_--;
						return true;
					}
					else {
						delete_parent_pos = parent;
						delete_pos = cur;
					}
					break;
				}
				else if (cur->right_ == nullptr) {
					if (cur == header_->parent_) {
						header_->parent_ = header_->parent_->left_;
						if (header_->parent_) header_->parent_->parent_ = nullptr;
						delete cur;
						size_--;
						return true;
					}
					else {
						delete_parent_pos = parent;
						delete_pos = cur;
					}
					break;
				}
				else {
					link_type min_parent = cur;
					link_type min_right = cur->right_;
					while (min_right->left_) {
						min_parent = min_right;
						min_right = min_right->left_;
					}
					cur->data_.first = min_right->data_.first;
					cur->data_.second = min_right->data_.second;
					delete_parent_pos = min_parent;
					delete_pos = min_right;
					break;
				}
			}
		}
		if (delete_parent_pos == nullptr)
			return false;
		
		link_type del = delete_pos;
		link_type delete_parent = delete_parent_pos;
		while (delete_pos != header_->parent_) {
			if (delete_pos == delete_parent_pos->left_)
				delete_parent_pos->balance_++;
			else if (delete_pos == delete_parent_pos->right_)
				delete_parent_pos->balance_--;

			if (delete_parent_pos->balance_ == -1 || delete_parent_pos->balance_ == 1)
				break;
			else if (delete_parent_pos->balance_ == 0) {
				delete_pos = delete_parent_pos;
				delete_parent_pos = delete_parent_pos->parent_;
			}
			else if (delete_parent_pos->balance_ == -2 || delete_parent_pos->balance_ == 2) {
				if (delete_parent_pos->balance_ == -2) {
					if (delete_parent_pos->left_->balance_ == -1) {
						link_type tmp = delete_parent_pos->left_;
						(__rotate_right)(delete_parent_pos, header_->parent_);
						delete_parent_pos = tmp;
					}
					else if (delete_parent_pos->left_->balance_ == 1) {
						link_type tmp = delete_parent_pos->left_->right_;
						(__rotate_left_right)(delete_parent_pos, header_->parent_);
						delete_parent_pos = tmp;
					}
					else {
						link_type tmp = delete_parent_pos->left_;
						(__rotate_right)(delete_parent_pos, header_->parent_);
						delete_parent_pos = tmp;
						delete_parent_pos->balance_ = 1;
						delete_parent_pos->right_->balance_ = -1;
						break;
					}
				}
				else {
					if (delete_parent_pos->right_->balance_ == -1) {
						link_type tmp = delete_parent_pos->right_->left_;
						(__rotate_right_left)(delete_parent_pos, header_->parent_);
						delete_parent_pos = tmp;
					}
					else if (delete_parent_pos->right_->balance_ == 1) {
						link_type tmp = delete_parent_pos->right_;
						(__rotate_left)(delete_parent_pos, header_->parent_);
						delete_parent_pos = tmp;
					}
					else {
						link_type tmp = delete_parent_pos->right_;
						(__rotate_left)(delete_parent_pos, header_->parent_);
						delete_parent_pos = tmp;
						delete_parent_pos->balance_ = -1;
						delete_parent_pos->left_->balance_ = 1;
						break;
					}
				}
				delete_pos = delete_parent_pos;
				delete_parent_pos = delete_parent_pos->parent_;
			}
			else Exception(AssertError("AVL Delete Failed"));
		}
		if (del->left_ == nullptr) {
			if (del == delete_parent->left_) {
				delete_parent->left_ = del->right_;
				if (del->right_)
					del->right_->parent_ = parent;
			}
			else {
				delete_parent->right_ = del->right_;
				if (del->right_)
					del->right_->parent_ = parent;
			}
		}
		else {
			if (del == delete_parent->left_) {
				delete_parent->left_ = del->left_;
				if (del->left_)
					del->left_->parent_ = parent;
			}
			else {
				delete_parent->right_ = del->left_;
				if (del->left_)
					del->left_->parent_ = parent;
			}
		}
		delete del;
		size_--;
		return true;
	}
	void clear() {
		destroy_tree_aux(header_->parent_);
		header_->parent_ = header_;
	}
	link_type find(const Key& k) const {
		link_type cur = header_->parent_;
		while (cur) {
			if (key_compare_(k, get_key(cur)))
				cur = cur->left_;
			else if (k == get_key(cur))
				return cur;
			else
				cur = cur->right_;
		}
		return nullptr;
	}
	size_type depth(link_type root) const {
		if (root == nullptr) 
			return 0;
		size_type left_depth = depth(root->left_);
		size_type right_depth = depth(root->right_);
		return left_depth > right_depth ? left_depth + 1 : right_depth + 1;
	}
	size_type size() const { return size_; }
	bool empty() const { return header_->parent_ == base_ptr(header_); }
	bool is_balanced() { return balanced_aux(header_->parent_); }
	void in_order() const {
		in_order_aux(header_->parent_);
		std::cout << std::endl;
	}
	void swap(const self& v) {
		std::swap(this->header_, v.header_);
		std::swap(this->size_, v.size_);
	}
	Compare key_comp() const { return key_compare_; }
private:
	Compare key_compare_;
	link_type header_;
	size_type size_;
};

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator ==(const avl_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
	const avl_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
	return x.size() == y.size() && MSTL::equal(x.const_begin(), x.const_end(), y.const_begin());
}
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator <(const avl_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
	const avl_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
	return MSTL::lexicographical_compare(x.const_begin(), x.const_end(), y.const_begin(), y.const_end());
}

MSTL_END_NAMESPACE__
#endif // MSTL_AVL_TREE_HPP__
