#ifndef MSTL_SET_HPP__
#define MSTL_SET_HPP__
#include "rb_tree.h"
#include "memory.hpp"
#include "functor.hpp"

MSTL_BEGIN_NAMESPACE__

template <typename Key, typename Compare = less<Key>, typename Alloc = default_standard_alloc<Key>>
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
	rep_type t;
	Compare comp;
public:
	set() : t(Compare()) {}
	explicit set(const Compare& comp) : t(comp) {}
	template <typename InputIterator>
	set(InputIterator first, InputIterator last) : t(Compare()) {
		t.insert_unique(first, last);
	}
	template <typename InputIterator>
	set(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
		t.insert_unique(first, last);
	}
	explicit set(const self& x) : t(x.t) {}
	self& operator =(const self& x) {
		t = x.t;
		return *this;
	}

	key_compare key_comp() const { return t.key_comp(); }
	value_compare value_comp() const { return t.key_comp(); }

	iterator begin() { return t.begin(); }
	iterator end() { return t.end(); }
	const_iterator const_begin() const { return t.const_begin(); }
	const_iterator const_end() const { return t.const_end(); }
	bool empty() const { return t.empty(); }
	size_type size() const { return t.size(); }
	bool same_to(const self& rh) { return this->t == rh.t; }
	bool less_to(const self& rh) { return this->t < rh.t; }

	pair<iterator, bool> insert(const value_type& x) {
		pair<rep_iterator, bool> p = t.insert_unique(x);
		return pair<iterator, bool>(p.first, p.second);
	}
	iterator insert(iterator position, const value_type& x) {
		return t.insert_unique((rep_iterator&)position, x);
	}
	template <typename InputIterator>
	void insert(InputIterator first, InputIterator last) { t.insert_unique(first, last); }
	void erase(iterator position) { t.erase((rep_iterator&)position); }
	size_type erase(const key_type& x) { return t.erase(x); }
	void erase(iterator first, iterator last) { t.erase((rep_iterator&)first, (rep_iterator&)last); }
	void clear() { t.clear(); }
	iterator find(const key_type& x) const { return t.find(x); }
	size_type count(const key_type& x) const { return t.count(x); }
	iterator lower_bound(const key_type& x) const { return t.lower_bound(x); }
	iterator upper_bound(const key_type& x) const { return t.upper_bound(x); }
	pair<iterator, iterator> equal_range(const key_type& x) const { return t.equal_range(x); }
};
template <typename Key, typename Compare, typename Alloc>
inline bool operator ==(const set<Key, Compare, Alloc>& x, const set<Key, Compare, Alloc>& y) {
	return x.same_to(y);
}
template <typename Key, typename Compare, typename Alloc>
inline bool operator <(const set<Key, Compare, Alloc>& x, const set<Key, Compare, Alloc>& y) {
	return x.less_to(y);
}



template <class Key, class Compare = less<Key>, class Alloc = default_standard_alloc<Key>>
class multiset {
public:
	typedef Key key_type;
	typedef Key value_type;
	typedef Compare key_compare;
	typedef Compare value_compare;
private:
	typedef rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> rep_type;
	rep_type t;
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

	multiset() : t(Compare()) {}
	explicit multiset(const Compare& comp) : t(comp) {}
	template <class InputIterator>
	multiset(InputIterator first, InputIterator last) : t(Compare()) {
		t.insert_equal(first, last);
	}
	template <class InputIterator>
	multiset(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
		t.insert_equal(first, last);
	}
	explicit multiset(const multiset<Key, Compare, Alloc>& x) : t(x.t) {}
	multiset<Key, Compare, Alloc>& operator =(const multiset<Key, Compare, Alloc>& x) {
		t = x.t;
		return *this;
	}

	key_compare key_comp() const { return t.key_comp(); }
	value_compare value_comp() const { return t.key_comp(); }
	iterator begin() const { return t.begin(); }
	iterator end() const { return t.end(); }
	reverse_iterator rbegin() const { return t.rbegin(); }
	reverse_iterator rend() const { return t.rend(); }
	bool empty() const { return t.empty(); }
	size_type size() const { return t.size(); }
	size_type max_size() const { return t.max_size(); }
	void swap(multiset<Key, Compare, Alloc>& x) { t.swap(x.t); }
	iterator insert(const value_type& x) { return t.insert_equal(x); }
	iterator insert(iterator position, const value_type& x) {
		typedef typename rep_type::iterator rep_iterator;
		return t.insert_equal((rep_iterator&)position, x);
	}
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) { t.insert_equal(first, last); }
	void erase(iterator position) {
		typedef typename rep_type::iterator rep_iterator;
		t.erase((rep_iterator&)position);
	}
	size_type erase(const key_type& x) { return t.erase(x); }
	void erase(iterator first, iterator last) {
		typedef typename rep_type::iterator rep_iterator;
		t.erase((rep_iterator&)first, (rep_iterator&)last);
	}
	void clear() { t.clear(); }

	iterator find(const key_type& x) const { return t.find(x); }
	size_type count(const key_type& x) const { return t.count(x); }
	iterator lower_bound(const key_type& x) const { return t.lower_bound(x); }
	iterator upper_bound(const key_type& x) const { return t.upper_bound(x); }
	pair<iterator, iterator> equal_range(const key_type& x) const { return t.equal_range(x); }
	friend bool operator ==(const multiset&, const multiset&);
	friend bool operator <(const multiset&, const multiset&);
};
template <class Key, class Compare, class Alloc>
inline bool operator ==(const multiset<Key, Compare, Alloc>& x, const multiset<Key, Compare, Alloc>& y) {
	return x.t == y.t;
}
template <class Key, class Compare, class Alloc>
inline bool operator <(const multiset<Key, Compare, Alloc>& x, const multiset<Key, Compare, Alloc>& y) {
	return x.t < y.t;
}
template <class Key, class Compare, class Alloc>
inline void swap(multiset<Key, Compare, Alloc>& x, multiset<Key, Compare, Alloc>& y) {
	x.swap(y);
}

MSTL_END_NAMESPACE__

#endif // MSTL_SET_HPP__
