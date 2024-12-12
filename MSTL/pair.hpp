#ifndef MSTL_PAIR_HPP__
#define MSTL_PAIR_HPP__
#include "basiclib.h"
#include "object.h"

MSTL_BEGIN_NAMESPACE__

template <typename T1, typename T2>
struct pair {
	typedef T1				first_type;
	typedef T2				second_type;
	typedef pair<T1, T2>	self;

	T1 first;
	T2 second;

	pair() : first(T1()), second(T2()) {}
	pair(const T1& a, const T2& b) : first(a), second(b) {}
	pair(const self& p) : first(p.first), second(p.second) {}
	void __show_data_only(std::ostream& _out) const {
		_out << "{ " << first << ", " << second << " }";
	}

#if defined(MSTL_COMPILE_GNUC__)
	self& operator =(const self& _p) {
		if (*this == _p) return *this;
		this->first = _p.first;
		this->second = _p.second;
		return *this;
	}
#endif
};

template <typename T1, typename T2>
inline bool operator ==(const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return x.first == y.first && x.second == y.second;
}
template <typename T1, typename T2>
inline bool operator <(const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
}
template <typename T1, typename T2>
inline bool operator >(const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return x.first > y.first || (!(y.first > x.first) && x.second > y.second);
}
template <typename T1, typename T2>
inline bool operator <=(const pair<T1, T2>&x, const pair<T1, T2>&y) {
	return !(x > y);
}
template <typename T1, typename T2>
inline bool operator >=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return !(x < y);
}
template <typename T1, typename T2>
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
