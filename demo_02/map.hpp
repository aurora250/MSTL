#ifndef MSTL_MAP_HPP__
#define MSTL_MAP_HPP__
#include "rb_tree.h"
#include "basiclib.h"

MSTL_BEGIN_NAMESPACE__

template <typename Key, typename T, typename Compare = less<T>,
	typename Alloc = simple_alloc<T, std::allocator<T>>>
class map {
public:
	typedef Key							key_type;
	typedef T							data_type;
	typedef T							mapped_type;
	typedef pair<const Key, T>			value_type;
	typedef Compare						key_compare;
	typedef map<Key, T, Compare, Alloc> self;

	class value_compare : public binary_function<value_type, value_type, bool> {
	private:
		friend class self;
		Compare comp;
	public:
		bool operator ()(const value_type& x, const value_type& y) {
			return comp(x.first, y.first);
		}
	};
private:
	typedef rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc> rep_type;
	rep_type t;
public:
	typedef typename rep_type::pointer			pointer;
	typedef typename rep_type::const_pointer	const_pointer;
	typedef typename rep_type::reference		reference;
	typedef typename rep_type::const_reference	const_reference;
	typedef typename rep_type::iterator			iterator;
	typedef typename rep_type::const_iterator	const_iterator;
	typedef typename rep_type::size_type		size_type;
	typedef typename rep_type::difference_type	difference_type;

	map() : t(Compare()) {}
	explicit map(const Compare& comp) : t(comp) {}
	template <typename InputIterator>
	map(InputIterator first, InputIterator last)
		: t(Compare()) {
		t.insert_unique(first, last);
	}
	template <typename InputIterator>
	map(InputIterator first, InputIterator last, const Compare& comp)
		: t(comp) {
		t.insert_unique(first, last);
	}
	map(const self& x) : t(x.t) {}
	self& operator =(const self& x) {
		t = x.t;
		return *this;
	}

	iterator begin() { return t.begin(); }
	iterator end() { return t.end(); }
	const_iterator const_begin() const { return t.const_begin(); }
	const_iterator const_end() const { return t.const_end(); }

	bool empty() const { return t.empty(); }
	size_type size() const { return t.size(); }
	key_compare key_comp() const { return t.key_comp(); }
	value_compare value_comp() const { return value_compare(t.key_comp()); }

	pair<iterator, bool> insert(const value_type& x) { 
		return t.insert_unique(x); 
	}
	iterator insert(iterator position, const value_type& x) {
		return t.insert_unique(position, x);
	}
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		t.insert_unique(first, last);
	}
	void erase(iterator position) { t.erase(position); }
	size_type erase(const key_type& x) { return t.erase(x); }
	void erase(iterator first, iterator last) { t.erase(first, last); }
	void clear() { t.clear(); }

	iterator find(const key_type& x) { return t.find(x); }
	const_iterator find(const key_type& x) const { return t.find(x); }
	size_type count(const key_type& x) const { return t.count(x); }
	iterator lower_bound(const key_type& x) { return t.lower_bound(x); }
	const_iterator lower_bound(const key_type& x) const { return t.lower_bound(x); }
	iterator upper_bound(const key_type& x) { return t.upper_bound(x); }
	const_iterator upper_bound(const key_type& x) const { return t.upper_bound(x); }

	pair<iterator, iterator> equal_range(const key_type& x) {
		return t.equal_range(x);
	}
	pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
		return t.equal_range(x);
	}

	T& operator[](const key_type& k) {
		return (*(( 
			insert(value_type(k, T()))
			).first)).second;
	}
	friend bool operator ==(const map&, const map&);
	friend bool operator <(const map&, const map&);
};
template <class Key, class T, class Compare, class Alloc>
inline bool operator ==(const map<Key, T, Compare, Alloc>& x, const map<Key, T, Compare, Alloc>& y) {
	return x.t == y.t;
}

template <class Key, class T, class Compare, class Alloc>
inline bool operator <(const map<Key, T, Compare, Alloc>& x, const map<Key, T, Compare, Alloc>& y) {
	return x.t < y.t;
}

MSTL_END_NAMESPACE__

#endif // MSTL_MAP_HPP__
