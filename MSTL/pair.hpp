#ifndef MSTL_PAIR_HPP__
#define MSTL_PAIR_HPP__
#include "basiclib.h"
#include "object.h"
#include "tuple.hpp"
#include <type_traits>

MSTL_BEGIN_NAMESPACE__

template <typename T1, typename T2>
struct pair {
	typedef T1				first_type;
	typedef T2				second_type;
	typedef pair<T1, T2>	self;

	T1 first;
	T2 second;

	template <typename U1 = T1, typename U2 = T2, std::enable_if_t<
		std::conjunction_v<std::is_default_constructible<U1>,std::is_default_constructible<U2>>, int> = 0>
	MSTL_CONSTEXPR__ explicit(not std::conjunction_v<std::_Is_implicitly_default_constructible<U1>,
		std::_Is_implicitly_default_constructible<U2>>)
		pair() 
		noexcept(std::is_nothrow_default_constructible_v<U1>&& std::is_nothrow_default_constructible_v<U2>)
		: first(), second() {}

	template <typename U1 = T1, typename U2 = T2, std::enable_if_t<
		std::conjunction_v<std::is_copy_constructible<U1>, std::is_copy_constructible<U2>>, int> = 0>
	MSTL_CONSTEXPR__ explicit(not std::conjunction_v<std::is_convertible<const U1&, U1>,
		std::is_convertible<const U2&, U2>>)
		pair(const T1& a, const T2& b) 
		noexcept(std::is_nothrow_copy_constructible_v<U1>&& std::is_nothrow_copy_constructible_v<U2>)
		: first(a), second(b) {}

	template <class U1, class U2, std::enable_if_t<
		std::conjunction_v<std::is_constructible<T1, U1>, std::is_constructible<T2, U2>>, int> = 0>
	MSTL_CONSTEXPR__ explicit(not std::conjunction_v<std::is_convertible<U1, T1>,
		std::is_convertible<U2, T2>>)
		pair(U1&& _Val1, U2&& _Val2) 
		noexcept(std::is_nothrow_constructible_v<T1, U1>&& std::is_nothrow_constructible_v<T2, U2>)
		: first(std::forward<U1>(_Val1)), second(std::forward<U2>(_Val2)) {}

	template <class Tuple1, class Tuple2, size_t... Ids1, size_t... Ids2>
	MSTL_CONSTEXPR__ pair(Tuple1& val1, Tuple2& val2, std::index_sequence<Ids1...>, std::index_sequence<Ids2...>)
		: first(get<Ids1>(std::move(val1))...), second(get<Ids2>(std::move(val2))...) {}

	template <class... Types1, class... Types2>
	MSTL_CONSTEXPR__ pair(std::piecewise_construct_t, tuple<Types1...> val1, tuple<Types2...> val2)
		: pair(val1, val2, std::index_sequence_for<Types1...>{}, std::index_sequence_for<Types2...>{}) {}

	pair(const pair& p) = default;
	pair(pair&& p) = default;
	
	template <class U1, class U2, std::enable_if_t<
		std::conjunction_v<std::is_constructible<T1, const U1&>, std::is_constructible<T2, const U2&>>, int> = 0>
	MSTL_CONSTEXPR__ explicit(not std::conjunction_v<std::is_convertible<const U1&, T1>,
		std::is_convertible<const U2&, T1>>)
		pair(const pair<U1, U2>& p) 
		noexcept(std::is_nothrow_constructible_v<T1, const U1&> && std::is_nothrow_constructible_v<T2, const U2&>)
		: first(p.first), second(p.second) {}

	template <class U1, class U2, std::enable_if_t<
		std::conjunction_v<std::is_constructible<T1, U1>, std::is_constructible<T2, U2>>, int> = 0>
	MSTL_CONSTEXPR__ explicit(not std::conjunction_v<std::is_convertible<U1, T1>,
		std::is_convertible<U2, T2>>)
		pair(pair<U1, U2>&& _Right) 
		noexcept(std::is_nothrow_constructible_v<T1, U1>&& std::is_nothrow_constructible_v<T2, U2>)
		: first(std::forward<U1>(_Right.first)), second(std::forward<U2>(_Right.second)) {}

	pair& operator=(const volatile pair&) = delete;

	template <class self_ = pair, std::enable_if_t<
		std::conjunction_v<std::_Is_copy_assignable_no_precondition_check<typename self_::first_type>,
		std::_Is_copy_assignable_no_precondition_check<typename self_::second_type>>, int> = 0>
		MSTL_CONSTEXPR__ pair& operator =(std::_Identity_t<const self_&> p)
		noexcept(std::conjunction_v<std::is_nothrow_copy_assignable<T1>, std::is_nothrow_copy_assignable<T2>>) {
		first = p.first;
		second = p.second;
		return *this;
	}
	template <class self_ = pair, std::enable_if_t<
		std::conjunction_v<std::_Is_move_assignable_no_precondition_check<typename self_::first_type>,
		std::_Is_move_assignable_no_precondition_check<typename self_::second_type>>, int> = 0>
		MSTL_CONSTEXPR__ pair& operator =(std::_Identity_t<self_&&> p)
		noexcept(std::conjunction_v<std::is_nothrow_move_assignable<T1>, std::is_nothrow_move_assignable<T2>>) {
		first = std::forward<T1>(p.first);
		second = std::forward<T2>(p.second);
		return *this;
	}
	template <class U1, class U2, std::enable_if_t<
		std::conjunction_v<std::negation<std::is_same<pair, pair<U1, U2>>>,
		std::is_assignable<T1&, const U1&>, std::is_assignable<T2&, const U2&>>, int> = 0>
		MSTL_CONSTEXPR__ pair& operator =(const pair<U1, U2>& p)
		noexcept(std::is_nothrow_assignable_v<T1&, const U1&> && std::is_nothrow_assignable_v<T2&, const U2&>) {
		first = p.first;
		second = p.second;
		return *this;
	}
	template <class U1, class U2, std::enable_if_t<
		std::conjunction_v<std::negation<std::is_same<pair, pair<U1, U2>>>, 
		std::is_assignable<T1&, U1>, std::is_assignable<T2&, U2>>, int> = 0>
		MSTL_CONSTEXPR__ pair& operator =(pair<U1, U2>&& _Right)
		noexcept(std::is_nothrow_assignable_v<T1&, U1>&& std::is_nothrow_assignable_v<T2&, U2>) {
		first = std::forward<U1>(_Right.first);
		second = std::forward<U2>(_Right.second);
		return *this;
	}
	MSTL_CONSTEXPR__ void swap(pair& _Right)
		noexcept(std::_Is_nothrow_swappable<T1>::value && std::_Is_nothrow_swappable<T2>::value) {
		std::swap(first, _Right.first);
		std::swap(second, _Right.second);
	}

	void __show_data_only(std::ostream& _out) const {
		_out << "{ " << first << ", " << second << " }";
	}
};

template <class T1, class T2, class U1, class U2>
MSTL_CONSTEXPR__ bool operator==(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return x.first == y.first && x.second == y.second;
}
template <typename T1, typename T2, class U1, class U2>
MSTL_CONSTEXPR__ bool operator <(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
}
template <typename T1, typename T2, class U1, class U2>
MSTL_CONSTEXPR__ bool operator >(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return x.first > y.first || (!(y.first > x.first) && x.second > y.second);
}
template <typename T1, typename T2, class U1, class U2>
MSTL_CONSTEXPR__ bool operator <=(const pair<T1, T2>&x, const pair<U1, U2>&y) {
	return !(x > y);
}
template <typename T1, typename T2, class U1, class U2>
MSTL_CONSTEXPR__ bool operator >=(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return !(x < y);
}
template <typename T1, typename T2, class U1, class U2>
MSTL_CONSTEXPR__ pair<T1, T2> make_pair(const T1& x, const T2& y) {
	return pair<T1, T2>(x, y);
}
template <class T1, class T2, std::enable_if_t<
	std::_Is_swappable<T1>::value && std::_Is_swappable<T2>::value, int> = 0>
	MSTL_CONSTEXPR__ void swap(pair<T1, T2>& lh, pair<T1, T2>& rh) noexcept(noexcept(lh.swap(rh))) {
	lh.swap(rh);
}

template <typename T1, typename T2>
std::ostream& operator <<(std::ostream& _out, const pair<T1, T2>& _p) {
	_p.__show_data_only(_out);
	return _out;
}

MSTL_END_NAMESPACE__

#endif // MSTL_PAIR_HPP__
