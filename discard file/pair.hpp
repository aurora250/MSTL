#ifndef MSTL_PAIR_HPP__
#define MSTL_PAIR_HPP__
#include "tuple.hpp"
#include "container.h"

MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

template <typename T1, typename T2>
struct pair {
	typedef T1				first_type;
	typedef T2				second_type;

	T1 first;
	T2 second;

	template <typename U1 = T1, typename U2 = T2>
		requires(DefaultConstructible<U1>&& DefaultConstructible<U2>)
	MSTL_CONSTEXPR explicit(!(DefaultConstructible<U1> && DefaultConstructible<U2>))
		pair() noexcept(NothrowDefaultConstructible<U1> && NothrowDefaultConstructible<U2>)
		: first(), second() {}

	template <typename U1 = T1, typename U2 = T2>
		requires(CopyConstructible<U1> && CopyConstructible<U2>)
	MSTL_CONSTEXPR explicit(!(Convertible<const U1&, U1> && Convertible<const U2&, U2>))
		pair(const T1& a, const T2& b) 
		noexcept(NothrowCopyConstructible<U1> && NothrowCopyConstructible<U2>)
		: first(a), second(b) {}

	template <typename U1, typename U2>
		requires(ConstructibleFrom<T1, U1> && ConstructibleFrom<T2, U2>)
	MSTL_CONSTEXPR explicit(!(Convertible<T1, U1>&& Convertible<T2, U2>))
		pair(U1&& a, U2&& b) 
		noexcept(NothrowConstructibleFrom<T1, U1> && NothrowConstructibleFrom<T2, U2>)
		: first(std::forward<U1>(a)), second(std::forward<U2>(b)) {}

	pair(const pair& p) = default;
	pair(pair&& p) = default;
	
	template <typename U1, typename U2>
		requires(ConstructibleFrom<T1, const U1&> && ConstructibleFrom<T2, const U2&>)
	MSTL_CONSTEXPR explicit(!(Convertible<const U1&, T1> && Convertible<const U2&, T2>))
		pair(const pair<U1, U2>& p) 
		noexcept(NothrowConstructibleFrom<T1, const U1&>
			&& NothrowConstructibleFrom<T2, const U2&>)
		: first(p.first), second(p.second) {}

	template <typename U1, typename U2>
		requires(ConstructibleFrom<T1, U1> && ConstructibleFrom<T2, U2>)
	MSTL_CONSTEXPR explicit(!(Convertible<U1, T1> && Convertible<U2, T2>))
		pair(pair<U1, U2>&& p) 
		noexcept(NothrowConstructibleFrom<T1, U1> && NothrowConstructibleFrom<T2, U2>)
		: first(std::forward<U1>(p.first)), second(std::forward<U2>(p.second)) {}

	pair& operator =(const volatile pair&) = delete;

	template <typename self = pair>
		requires(CopyAssignable<typename self::first_type> 
	&& CopyAssignable<typename self::second_type>)
	MSTL_CONSTEXPR pair& operator =(const self& p)
		noexcept(NothrowCopyAssignable<T1> && NothrowCopyAssignable<T2>) {
		first = p.first;
		second = p.second;
		return *this;
	}
	template <typename self = pair>
		requires(MoveAssignable<typename self::first_type> && MoveAssignable<typename self::second_type>)
	MSTL_CONSTEXPR pair& operator =(self&& p)
		noexcept(NothrowMoveAssignable<T1> && NothrowMoveAssignable<T2>) {
		first = std::forward<T1>(p.first);
		second = std::forward<T2>(p.second);
		return *this;
	}
	template <typename U1, typename U2>
		requires((!SameTo<pair, pair<U1, U2>>) 
	&& AssignableFrom<T1&, const U1&> && AssignableFrom<T2&, const U2&>)
	MSTL_CONSTEXPR pair& operator =(const pair<U1, U2>& p)
		noexcept(NothrowAssignableFrom<T1&, const U1&> && NothrowAssignableFrom<T2&, const U2&>) {
		first = p.first;
		second = p.second;
		return *this;
	}
	template <typename U1, typename U2>
		requires((!SameTo<pair, pair<U1, U2>>) 
	&& AssignableFrom<T1&, U1>&& AssignableFrom<T2&, U2>)
	MSTL_CONSTEXPR pair& operator =(pair<U1, U2>&& p)
		noexcept(NothrowAssignableFrom<T1&, U1> && NothrowAssignableFrom<T2&, U2>) {
		first = std::forward<U1>(p.first);
		second = std::forward<U2>(p.second);
		return *this;
	}

	MSTL_CONSTEXPR void swap(pair& p)
		noexcept(NothrowSwappable<T1> && NothrowSwappable<T2>) {
		std::swap(first, p.first);
		std::swap(second, p.second);
	}
};
template <typename T1, typename T2, typename U1, typename U2>
MSTL_CONSTEXPR bool operator ==(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return x.first == y.first && x.second == y.second;
}
template <typename T1, typename T2, typename U1, typename U2>
MSTL_CONSTEXPR bool operator !=(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return !(x == y);
}
template <typename T1, typename T2, typename U1, typename U2>
MSTL_CONSTEXPR bool operator <(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
}
template <typename T1, typename T2, typename U1, typename U2>
MSTL_CONSTEXPR bool operator >(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return y < x;
}
template <typename T1, typename T2, typename U1, typename U2>
MSTL_CONSTEXPR bool operator <=(const pair<T1, T2>&x, const pair<U1, U2>&y) {
	return !(x > y);
}
template <typename T1, typename T2, typename U1, typename U2>
MSTL_CONSTEXPR bool operator >=(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return !(x < y);
}
template <typename T1, typename T2, typename U1, typename U2>
MSTL_CONSTEXPR pair<T1, T2> make_pair(U1&& x, U2&& y) {
	return pair<T1, T2>(std::forward<U1>(x), std::forward<U2>(y));
}
template <typename T1, typename T2>
	requires(Swappable<T1> && Swappable<T2>)
MSTL_CONSTEXPR void swap(pair<T1, T2>& lh, pair<T1, T2>& rh) noexcept(noexcept(lh.swap(rh))) {
	lh.swap(rh);
}
template <typename T1, typename T2>
	requires(Printable<T1> && Printable<T2>)
inline void show_data_only(const pair<T1, T2>& p, std::ostream& _out) {
	_out << "{ " << p.first << ", " << p.second << " }";
}
template <typename T1, typename T2>
	requires(Printable<T1>&& Printable<T2>)
std::ostream& operator <<(std::ostream& _out, const pair<T1, T2>& _p) {
	show_data_only(_p, _out);
	return _out;
}

MSTL_END_NAMESPACE__

#endif // MSTL_PAIR_HPP__
