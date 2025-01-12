#ifndef MSTL_MAP_HPP__
#define MSTL_MAP_HPP__
#include "rb_tree.h"
#include "functor.hpp"

MSTL_BEGIN_NAMESPACE__

template <typename Key, typename T, typename Compare = less<Key>,
	typename Alloc = default_standard_alloc<__rb_tree_node<pair<const Key, T>>>>
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
		friend class map;
		Compare comp;
	public:
		bool operator ()(const value_type& x, const value_type& y) {
			return comp(x.first, y.first);
		}
	};
private:
	typedef rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc> rep_type;
	rep_type t;

	friend inline bool operator ==(const map& x, const map& y);
	friend inline bool operator <(const map& x, const map& y);
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
	map(std::initializer_list<value_type> l) : map(l.begin(), l.end()) {}
	self& operator =(std::initializer_list<value_type> l) {
		clear();
		insert(l.begin(), l.end());
		return *this;
	}
	template <typename Iterator>
		requires(InputIterator<Iterator>)
	map(Iterator first, Iterator last) : t(Compare()) {
		t.insert_unique(first, last);
	}
	template <typename Iterator>
		requires(InputIterator<Iterator>)
	map(Iterator first, Iterator last, const Compare& comp) : t(comp) {
		t.insert_unique(first, last);
	}
	map(const self& x) : t(x.t) {}
	self& operator =(const self& x) {
		if (*this == x) return *this;
		t = x.t;
		return *this;
	}
	map(self&& x) : t(std::move(x.t)) {}
	self& operator =(self&& x) {
		clear();
		swap(std::forward<self>(x));
		return *this;
	}
	~map() = default;

	MSTL_NODISCARD iterator begin() noexcept { return t.begin(); }
	MSTL_NODISCARD iterator end() noexcept { return t.end(); }
	MSTL_NODISCARD const_iterator const_begin() const noexcept { return t.const_begin(); }
	MSTL_NODISCARD const_iterator const_end() const noexcept { return t.const_end(); }
	MSTL_NODISCARD bool empty() const noexcept { return t.empty(); }
	MSTL_NODISCARD size_type size() const noexcept { return t.size(); }
	MSTL_NODISCARD key_compare key_comp() const noexcept { return t.key_comp(); }
	MSTL_NODISCARD value_compare value_comp() const noexcept { return value_compare(t.key_comp()); }

	pair<iterator, bool> insert(const value_type& x) {
		return t.insert_unique(x);
	}
	pair<iterator, bool> insert(value_type&& x) {
		return t.insert_unique(std::forward<value_type>(x));
	}
	iterator insert(iterator position, const value_type& x) {
		return t.insert_unique(position, x);
	}
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		t.insert_unique(first, last);
	}
	void erase(iterator position) noexcept { t.erase(position); }
	size_type erase(const key_type& x) noexcept { return t.erase(x); }
	void erase(iterator first, iterator last) noexcept { t.erase(first, last); }
	void clear() noexcept { t.clear(); }

	const_iterator find(const key_type& x) const { return t.find(x); }
	const_iterator find(key_type&& x) const { return t.find(std::forward<Key>(x)); }
	void swap(self& x) noexcept(noexcept(t.swap(x.t))) { t.swap(x.t); }
	MSTL_NODISCARD size_type count(const key_type& x) const { 
		return t.count(x); 
	}
	MSTL_NODISCARD iterator lower_bound(const key_type& x) {
		return t.lower_bound(x);
	}
	MSTL_NODISCARD const_iterator lower_bound(const key_type& x) const {
		return t.lower_bound(x);
	}
	MSTL_NODISCARD iterator upper_bound(const key_type& x) {
		return t.upper_bound(x);
	}
	MSTL_NODISCARD const_iterator upper_bound(const key_type& x) const {
		return t.upper_bound(x);
	}
	MSTL_NODISCARD pair<iterator, iterator> equal_range(const key_type& x) {
		return t.equal_range(x);
	}
	MSTL_NODISCARD pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
		return t.equal_range(x);
	}

	T& operator[](const key_type& k) {
		return (*((
			insert(value_type(k, T()))
			).first)).second;
	}
	T& operator[](key_type&& k) {
		return (*((
			insert(value_type(std::move(k), T()))
			).first)).second;
	}
	MSTL_NODISCARD T& at(const key_type& k) {
		auto iter = find(k);
		if (iter == end()) Exception(StopIterator("Key does not exist"));
		else return iter->second;
	}
	MSTL_NODISCARD T& at(key_type&& k) {
		auto iter = find(std::move(k));
		if (iter == end()) Exception(StopIterator("Key does not exist"));
		else return iter->second;
	}
};
template <class Key, class T, class Compare, class Alloc>
MSTL_NODISCARD inline bool operator ==(
	const map<Key, T, Compare, Alloc>& x, const map<Key, T, Compare, Alloc>& y) {
	return x.t == y.t;
}
template <class Key, class T, class Compare, class Alloc>
MSTL_NODISCARD inline bool operator !=(
	const map<Key, T, Compare, Alloc>& x, const map<Key, T, Compare, Alloc>& y) {
	return !(x == y);
}
template <class Key, class T, class Compare, class Alloc>
MSTL_NODISCARD inline bool operator <(
	const map<Key, T, Compare, Alloc>& x, const map<Key, T, Compare, Alloc>& y) {
	return x.t < y.t;
}
template <class Key, class T, class Compare, class Alloc>
MSTL_NODISCARD inline bool operator >(
	const map<Key, T, Compare, Alloc>& x, const map<Key, T, Compare, Alloc>& y) {
	return y < x;
}
template <class Key, class T, class Compare, class Alloc>
MSTL_NODISCARD inline bool operator <=(
	const map<Key, T, Compare, Alloc>& x, const map<Key, T, Compare, Alloc>& y) {
	return !(x > y);
}
template <class Key, class T, class Compare, class Alloc>
MSTL_NODISCARD inline bool operator >=(
	const map<Key, T, Compare, Alloc>& x, const map<Key, T, Compare, Alloc>& y) {
	return !(x < y);
}
template <class Key, class T, class Compare, class Alloc>
void swap(map<Key, T, Compare, Alloc>& x, map<Key, T, Compare, Alloc>& y) 
	noexcept(noexcept(x.swap(y))) {
	x.swap(y);
}

template <class Key, class T, class Compare = less<Key>,
	class Alloc = default_standard_alloc<__rb_tree_node<pair<const Key, T>>>>
class multimap {
public:
	typedef Key key_type;
	typedef T data_type;
	typedef T mapped_type;
	typedef pair<const Key, T> value_type;
	typedef Compare key_compare;
	typedef multimap<Key, T, Compare, Alloc> self;

	class value_compare : public binary_function<value_type, value_type, bool> {
		friend class multimap<Key, T, Compare, Alloc>;
	protected:
		Compare comp;
		value_compare(Compare c) : comp(c) {}
	public:
		bool operator ()(const value_type& x, const value_type& y) const {
			return comp(x.first, y.first);
		}
	};
private:
	typedef rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc> rep_type;
	rep_type t;
public:
	typedef typename rep_type::pointer pointer;
	typedef typename rep_type::const_pointer const_pointer;
	typedef typename rep_type::reference reference;
	typedef typename rep_type::const_reference const_reference;
	typedef typename rep_type::iterator iterator;
	typedef typename rep_type::const_iterator const_iterator;
	typedef typename rep_type::reverse_iterator reverse_iterator;
	typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
	typedef typename rep_type::size_type size_type;
	typedef typename rep_type::difference_type difference_type;

	multimap() : t(Compare()) { }
	explicit multimap(const Compare& comp) : t(comp) { }
	template <class InputIterator>
	multimap(InputIterator first, InputIterator last) : t(Compare()) {
		t.insert_equal(first, last);
	}
	template <class InputIterator>
	multimap(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
		t.insert_equal(first, last);
	}
	multimap(const multimap<Key, T, Compare, Alloc>& x) : t(x.t) {}
	multimap<Key, T, Compare, Alloc>& operator =(const multimap<Key, T, Compare, Alloc>& x) {
		t = x.t;
		return *this;
	}

	key_compare key_comp() const { return t.key_comp(); }
	value_compare value_comp() const { return value_compare(t.key_comp()); }
	iterator begin() { return t.begin(); }
	const_iterator const_begin() const { return t.begin(); }
	iterator end() { return t.end(); }
	const_iterator const_end() const { return t.end(); }
	reverse_iterator rbegin() { return t.rbegin(); }
	const_reverse_iterator rbegin() const { return t.rbegin(); }
	reverse_iterator rend() { return t.rend(); }
	const_reverse_iterator rend() const { return t.rend(); }
	bool empty() const { return t.empty(); }
	size_type size() const { return t.size(); }
	size_type max_size() const { return t.max_size(); }
	void swap(multimap<Key, T, Compare, Alloc>& x) { t.swap(x.t); }
	iterator insert(const value_type& x) { return t.insert_equal(x); }
	iterator insert(iterator position, const value_type& x) { return t.insert_equal(position, x); }
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) { t.insert_equal(first, last); }
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
	pair<iterator, iterator> equal_range(const key_type& x) {  return t.equal_range(x); }
	pair<const_iterator, const_iterator> equal_range(const key_type& x) const { return t.equal_range(x); }
	friend bool operator ==(const multimap&, const multimap&);
	friend bool operator <(const multimap&, const multimap&);
};

template <class Key, class T, class Compare, class Alloc>
inline bool operator==(const multimap<Key, T, Compare, Alloc>& x, const multimap<Key, T, Compare, Alloc>& y) {
	return x.t == y.t;
}
template <class Key, class T, class Compare, class Alloc>
inline bool operator<(const multimap<Key, T, Compare, Alloc>& x, const multimap<Key, T, Compare, Alloc>& y) {
	return x.t < y.t;
}
template <class Key, class T, class Compare, class Alloc>
inline void swap(multimap<Key, T, Compare, Alloc>& x, multimap<Key, T, Compare, Alloc>& y) {
	x.swap(y);
}

MSTL_END_NAMESPACE__

#endif // MSTL_MAP_HPP__
