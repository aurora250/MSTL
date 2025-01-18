#ifndef MSTL_AVL_SET_HPP__
#define MSTL_AVL_SET_HPP__
#include "avl_tree.h"
#include "memory.hpp"
#include "functor.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Key, typename Compare = less<Key>, typename Alloc = default_standard_alloc<Key>>
class avl_set {
public:
	typedef Key			key_type;
	typedef Key			value_type;
	typedef Compare		key_compare;
	typedef Compare		value_compare;
private:
	typedef avl_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> rep_type;
	typedef typename rep_type::iterator			rep_iterator;
public:
	typedef typename rep_type::const_pointer	pointer;
	typedef typename rep_type::const_pointer	const_pointer;
	typedef typename rep_type::const_reference	reference;
	typedef typename rep_type::const_reference	const_reference;
	typedef typename rep_type::const_iterator	iterator;
	typedef typename rep_type::const_iterator	const_iterator;
	typedef typename rep_type::size_type		size_type;
	typedef typename rep_type::difference_type	difference_type;
	typedef avl_set<Key, Compare, Alloc>		self;
private:
	rep_type rep_;
public:
	avl_set() : rep_(Compare()) {}
	explicit avl_set(const Compare& comp) : rep_(comp) {}
	template <typename InputIterator>
	avl_set(InputIterator first, InputIterator last) : rep_(Compare()) {
		rep_.insert(first, last);
	}
	template <typename InputIterator>
	avl_set(InputIterator first, InputIterator last, const Compare& comp) : rep_(comp) {
		rep_.insert(first, last);
	}
	explicit avl_set(const self& x) : rep_(x.rep_) {}
	self& operator =(const self& x) {
		rep_ = x.rep_;
		return *this;
	}

	key_compare key_comp() const { return rep_.key_comp(); }
	value_compare value_comp() const { return rep_.key_comp(); }

	iterator begin() { return rep_.begin(); }
	iterator end() { return rep_.end(); }
	const_iterator const_begin() const { return rep_.const_begin(); }
	const_iterator const_end() const { return rep_.const_end(); }
	iterator rbegin() { return rep_.rbegin(); }
	iterator rend() { return rep_.rend(); }
	bool empty() const { return rep_.empty(); }
	size_type size() const { return rep_.size(); }
	bool same_to(const self& rh) { return this->rep_ == rh.rep_; }
	bool less_to(const self& rh) { return this->rep_ < rh.rep_; }

	bool insert(const value_type& x) { return rep_.insert(x); }
	template <typename InputIterator>
	void insert(InputIterator first, InputIterator last) { rep_.insert(first, last); }
	void erase(iterator position) { rep_.erase((rep_iterator&)position); }
	size_type erase(const key_type& x) { return rep_.erase(x); }
	void clear() { rep_.clear(); }
	iterator find(const key_type& x) const { return rep_.find(x); }
	void swap(self& x) { rep_.swap(x.rep_); }
};
template <typename Key, typename Compare, typename Alloc>
inline bool operator ==(const avl_set<Key, Compare, Alloc>& x, const avl_set<Key, Compare, Alloc>& y) {
	return x.same_to(y);
}
template <typename Key, typename Compare, typename Alloc>
inline bool operator <(const avl_set<Key, Compare, Alloc>& x, const avl_set<Key, Compare, Alloc>& y) {
	return x.less_to(y);
}
template <class Key, class Compare, class Alloc>
inline void swap(avl_set<Key, Compare, Alloc>& x, avl_set<Key, Compare, Alloc>& y) {
	x.swap(y);
}

MSTL_END_NAMESPACE__
#endif // MSTL_AVL_SET_HPP__
