#ifndef MSTL_SET_HPP__
#define MSTL_SET_HPP__
#include "rb_tree.h"
#include "memory.hpp"
#include "functor.hpp"

MSTL_BEGIN_NAMESPACE__

template <typename Key, typename Compare = less<Key>, typename Alloc = standard_allocator<Key>>
class set {
public:
	typedef Key			key_type;
	typedef Key			value_type;
	typedef Compare		key_compare;
	typedef Compare		value_compare;
private:
	typedef rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> rep_type;
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
	typedef set<Key, Compare, Alloc>			self;
private:
	rep_type rep_;
	Compare comp_;
public:
	set() : rep_(Compare()) {}
	explicit set(const Compare& comp) : rep_(comp) {}
	template <typename InputIterator>
	set(InputIterator first, InputIterator last) : rep_(Compare()) {
		rep_.insert_unique(first, last);
	}
	template <typename InputIterator>
	set(InputIterator first, InputIterator last, const Compare& comp) : rep_(comp) {
		rep_.insert_unique(first, last);
	}
	explicit set(const self& x) : rep_(x.rep_) {}
	self& operator =(const self& x) {
		rep_ = x.rep_;
		return *this;
	}

	key_compare key_comp() const { return rep_.key_comp(); }
	value_compare value_comp() const { return rep_.key_comp(); }

	iterator begin() { return rep_.begin(); }
	iterator end() { return rep_.end(); }
	const_iterator cbegin() const { return rep_.cbegin(); }
	const_iterator cend() const { return rep_.cend(); }
	bool empty() const { return rep_.empty(); }
	size_type size() const { return rep_.size(); }
	bool same_to(const self& rh) { return this->rep_ == rh.rep_; }
	bool less_to(const self& rh) { return this->rep_ < rh.rep_; }

	pair<iterator, bool> insert(const value_type& x) {
		pair<rep_iterator, bool> p = rep_.insert_unique(x);
		return pair<iterator, bool>(p.first, p.second);
	}
	iterator insert(iterator position, const value_type& x) {
		return rep_.insert_unique((rep_iterator&)position, x);
	}
	template <typename InputIterator>
	void insert(InputIterator first, InputIterator last) { rep_.insert_unique(first, last); }
	void erase(iterator position) { rep_.erase((rep_iterator&)position); }
	size_type erase(const key_type& x) { return rep_.erase(x); }
	void erase(iterator first, iterator last) { rep_.erase((rep_iterator&)first, (rep_iterator&)last); }
	void clear() { rep_.clear(); }
	iterator find(const key_type& x) const { return rep_.find(x); }
	void swap(self& x) { rep_.swap(x.rep_); }
	size_type count(const key_type& x) const { return rep_.count(x); }
	iterator lower_bound(const key_type& x) const { return rep_.lower_bound(x); }
	iterator upper_bound(const key_type& x) const { return rep_.upper_bound(x); }
	pair<iterator, iterator> equal_range(const key_type& x) const { return rep_.equal_range(x); }
};
template <typename Key, typename Compare, typename Alloc>
inline bool operator ==(const set<Key, Compare, Alloc>& x, const set<Key, Compare, Alloc>& y) {
	return x.same_to(y);
}
template <typename Key, typename Compare, typename Alloc>
inline bool operator <(const set<Key, Compare, Alloc>& x, const set<Key, Compare, Alloc>& y) {
	return x.less_to(y);
}
template <class Key, class Compare, class Alloc>
inline void swap(set<Key, Compare, Alloc>& x, set<Key, Compare, Alloc>& y) {
	x.swap(y);
}


template <class Key, class Compare = less<Key>, class Alloc = standard_allocator<Key>>
class multiset {
public:
	typedef Key key_type;
	typedef Key value_type;
	typedef Compare key_compare;
	typedef Compare value_compare;
private:
	typedef rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> rep_type;
	rep_type rep_;
public:
	typedef typename rep_type::const_pointer pointer;
	typedef typename rep_type::const_pointer const_pointer;
	typedef typename rep_type::const_reference reference;
	typedef typename rep_type::const_reference const_reference;
	typedef typename rep_type::const_iterator iterator;
	typedef typename rep_type::const_iterator const_iterator;
	typedef typename rep_type::const_reverse_iterator reverse_iterator;
	typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
	typedef typename rep_type::size_type size_type;
	typedef typename rep_type::difference_type difference_type;

	multiset() : rep_(Compare()) {}
	explicit multiset(const Compare& comp) : rep_(comp) {}
	template <class InputIterator>
	multiset(InputIterator first, InputIterator last) : rep_(Compare()) {
		rep_.insert_equal(first, last);
	}
	template <class InputIterator>
	multiset(InputIterator first, InputIterator last, const Compare& comp) : rep_(comp) {
		rep_.insert_equal(first, last);
	}
	explicit multiset(const multiset<Key, Compare, Alloc>& x) : rep_(x.rep_) {}
	multiset<Key, Compare, Alloc>& operator =(const multiset<Key, Compare, Alloc>& x) {
		rep_ = x.rep_;
		return *this;
	}

	key_compare key_comp() const { return rep_.key_comp(); }
	value_compare value_comp() const { return rep_.key_comp(); }
	iterator begin() const { return rep_.begin(); }
	iterator end() const { return rep_.end(); }
	reverse_iterator rbegin() const { return rep_.rbegin(); }
	reverse_iterator rend() const { return rep_.rend(); }
	bool empty() const { return rep_.empty(); }
	size_type size() const { return rep_.size(); }
	size_type max_size() const { return rep_.max_size(); }
	void swap(multiset<Key, Compare, Alloc>& x) { rep_.swap(x.rep_); }
	iterator insert(const value_type& x) { return rep_.insert_equal(x); }
	iterator insert(iterator position, const value_type& x) {
		typedef typename rep_type::iterator rep_iterator;
		return rep_.insert_equal((rep_iterator&)position, x);
	}
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) { rep_.insert_equal(first, last); }
	void erase(iterator position) {
		typedef typename rep_type::iterator rep_iterator;
		rep_.erase((rep_iterator&)position);
	}
	size_type erase(const key_type& x) { return rep_.erase(x); }
	void erase(iterator first, iterator last) {
		typedef typename rep_type::iterator rep_iterator;
		rep_.erase((rep_iterator&)first, (rep_iterator&)last);
	}
	void clear() { rep_.clear(); }

	iterator find(const key_type& x) const { return rep_.find(x); }
	size_type count(const key_type& x) const { return rep_.count(x); }
	iterator lower_bound(const key_type& x) const { return rep_.lower_bound(x); }
	iterator upper_bound(const key_type& x) const { return rep_.upper_bound(x); }
	pair<iterator, iterator> equal_range(const key_type& x) const { return rep_.equal_range(x); }
	friend bool operator ==(const multiset&, const multiset&);
	friend bool operator <(const multiset&, const multiset&);
};
template <class Key, class Compare, class Alloc>
inline bool operator ==(const multiset<Key, Compare, Alloc>& x, const multiset<Key, Compare, Alloc>& y) {
	return x.rep_ == y.rep_;
}
template <class Key, class Compare, class Alloc>
inline bool operator <(const multiset<Key, Compare, Alloc>& x, const multiset<Key, Compare, Alloc>& y) {
	return x.rep_ < y.rep_;
}
template <class Key, class Compare, class Alloc>
inline void swap(multiset<Key, Compare, Alloc>& x, multiset<Key, Compare, Alloc>& y) {
	x.swap(y);
}

MSTL_END_NAMESPACE__

#endif // MSTL_SET_HPP__
