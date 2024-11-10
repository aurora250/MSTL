#ifndef MSTL_PAIR_HPP__
#define MSTL_PAIR_HPP__
#include "object.h"
#include <iostream>

MSTL_BEGIN_NAMESPACE__

template <typename T1, typename T2>
struct pair : public object {
	typedef T1				first_type;
	typedef T2				second_type;
	typedef pair<T1, T2>	self;

	T1 first;
	T2 second;
	static const char* const __type__;

	inline void __show_data_only(std::ostream& _out) const {
		_out << "first: " << this->first << std::endl;
		_out << "second: " << this->second;
	}
	void __det__(std::ostream& _out = std::cout) const {
		split_line(_out);
		_out << "type: " << __type__ << std::endl;
		_out << "check type: " << check_type<self>() << std::endl;
		this->__show_data_only(_out);
		_out << std::endl;
		split_line(_out);
	}

	pair() : first(T1()), second(T2()) {}
	pair(const T1& a, const T2& b) : first(a), second(b) {}
	template <class U1, class U2>
	pair(const pair<U1, U2>& p) : first(p.first), second(p.second) {}
	self& operator =(const self& _p) {
		if (*this == _p) return *this;
		this->first = _p.first;
		this->second = _p.second;
		return *this;
	}
	~pair() = default;
};
template <typename T1, typename T2>
const char* const pair<T1, T2>::__type__ = "pair";

template <class T1, class T2>
inline bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return x.first == y.first && x.second == y.second;
}
template <class T1, class T2>
inline bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
}
template <class T1, class T2>
inline pair<T1, T2> make_pair(const T1& x, const T2& y) {
	return pair<T1, T2>(x, y);
}

template <typename T1, typename T2>
std::ostream& operator <<(std::ostream& _out, const pair<T1, T2>& _p) {
	_p.__show_data_only(_out);
	return _out;
}

MSTL_END_NAMESPACE__

#endif // MSTL_PAIR_HPP__
