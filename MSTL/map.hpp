#ifndef MSTL_MAP_HPP__
#define MSTL_MAP_HPP__
#include "rb_tree.h"
#include "basiclib.h"
#include "container.h"
#include "functor.hpp"

MSTL_BEGIN_NAMESPACE__

template <typename Key, typename T, typename Compare = less<T>,
	typename Alloc = default_standard_alloc<__rb_tree_node<pair<const Key, T>>>>
class map : public container {
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
	static const char* const __type__;
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

	void __det__(std::ostream& _out = std::cout) const {
		split_line(_out);
		_out << "type: " << __type__ << std::endl;
		_out << "check type: " << check_type<self>() << std::endl;
		this->__show_size_only(_out);
		_out << "data: " << std::flush;
		this->__show_data_only(_out);
		_out << std::endl;
		split_line(_out);
	}
	void __show_data_only(std::ostream& _out) const {
		const_iterator _band = const_end();
		--_band;
		_out << '[' << std::flush;
		for (const_iterator iter = const_begin(); iter != const_end(); ++iter) {
			_out << *iter << std::flush;
			if (iter != _band) _out << ", " << std::flush;
		}
		_out << ']' << std::flush;
	}
	void __show_size_only(std::ostream& _out) const {
		_out << "size: " << size() << std::endl;
	}

	map() : t(Compare()) {}
	explicit map(const Compare& comp) : t(comp) {}
	template <typename InputIterator>
	map(InputIterator first, InputIterator last) : t(Compare()) {
		t.insert_unique(first, last);
	}
	template <typename InputIterator>
	map(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
		t.insert_unique(first, last);
	}
	explicit map(const self& x) : t(x.t) {}
	self& operator =(const self& x) {
		t = x.t;
		return *this;
	}

	iterator begin() { return t.begin(); }
	iterator end() { return t.end(); }
	const_iterator const_begin() const { return t.const_begin(); }
	const_iterator const_end() const { return t.const_end(); }
	bool same_to(const self& rh) { return this->t == rh.t; }
	bool less_to(const self& rh) { return this->t < rh.t; }
	bool empty() const { return t.empty(); }
	size_type size() const { return t.size(); }
	size_type max_height() const { return t.max_height(); }
	key_compare key_comp() const { return t.key_comp(); }
	value_compare value_comp() const { return value_compare(t.key_comp()); }

	pair<iterator, bool> insert(const value_type& x) {  return t.insert_unique(x); }
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
	pair<iterator, iterator> equal_range(const key_type& x) { return t.equal_range(x); }
	pair<const_iterator, const_iterator> equal_range(const key_type& x) const { return t.equal_range(x); }

	T& operator[](const key_type& k) {
		return (*(( 
			insert(value_type(k, T()))
			).first)).second;
	}
};
template <class Key, class T, class Compare, class Alloc>
const char* const map<Key, T, Compare, Alloc>::__type__ = "map";

template <class Key, class T, class Compare, class Alloc>
inline bool operator ==(const map<Key, T, Compare, Alloc>& x, const map<Key, T, Compare, Alloc>& y) {
	return x.same_to(y);
}
template <class Key, class T, class Compare, class Alloc>
inline bool operator <(const map<Key, T, Compare, Alloc>& x, const map<Key, T, Compare, Alloc>& y) {
	return x.less_to(y);
}
template <class Key, class T, class Compare, class Alloc>
std::ostream& operator <<(std::ostream& _out, const map<Key, T, Compare, Alloc>& _tar) {
	_tar.__show_data_only(_out);
	return _out;
}



template <class Key, class T, class Compare = less<Key>,
	class Alloc = default_standard_alloc<__rb_tree_node<pair<const Key, T>>>>
class multimap : public container {
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

	static const char* const __type__;

	void __det__(std::ostream& _out = std::cout) const {
		split_line(_out);
		_out << "type: " << __type__ << std::endl;
		_out << "check type: " << check_type<self>() << std::endl;
		this->__show_size_only(_out);
		_out << "data: " << std::flush;
		this->__show_data_only(_out);
		_out << std::endl;
		split_line(_out);
	}
	void __show_data_only(std::ostream& _out) const {
		const_iterator _band = const_end();
		--_band;
		_out << '[' << std::flush;
		for (const_iterator iter = const_begin(); iter != const_end(); ++iter) {
			_out << *iter << std::flush;
			if (iter != _band) _out << ", " << std::flush;
		}
		_out << ']' << std::flush;
	}
	void __show_size_only(std::ostream& _out) const {
		_out << "size: " << size() << std::endl;
	}

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
const char* const multimap<Key, T, Compare, Alloc>::__type__ = "multimap";

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
