#ifndef MSTL_UTILITY_HPP__
#define MSTL_UTILITY_HPP__
#include "type_traits.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T, T... Values>
struct integer_sequence {
    static_assert(is_integral_v<T>, "integer sequence requires integral types.");

    using value_type = T;

    MSTL_NODISCARD static MSTL_CONSTEXPR size_t size() noexcept {
        return sizeof...(Values);
    }
};
template <typename T, T Size>
using make_integer_sequence =
#ifdef MSTL_SUPPORT_MAKE_INTEGER_SEQ
__make_integer_seq<integer_sequence, T, Size>;
#else
integer_sequence<T, __integer_pack(Size)...>;
#endif
template <size_t... Values>
using index_sequence = integer_sequence<size_t, Values...>;
template <size_t Size>
using make_index_sequence = make_integer_sequence<size_t, Size>;
template <typename... Types>
using index_sequence_for = make_index_sequence<sizeof...(Types)>;


template <typename T, size_t Size> requires(is_swappable_v<T>)
MSTL_CONSTEXPR void swap(T(& lh)[Size], T(& rh)[Size]) noexcept(is_nothrow_swappable_v<T>) {
    if (&lh != &rh) {
        T* first1 = lh;
        T* last1 = first1 + Size;
        T* first2 = rh;
        for (; first1 != last1; ++first1, ++first2) {
            MSTL::swap(*first1, *first2);
        }
    }
}

template <typename T>
    requires(conjunction_v<is_move_constructible<T>, is_move_assignable<T>>)
MSTL_CONSTEXPR void swap(T& lh, T& rh)
noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>) {
    T tmp = std::move(lh);
    lh = std::move(rh);
    rh = std::move(tmp);
}

template <typename T, typename U>
MSTL_CONSTEXPR T exchange(T& val, U&& new_val) noexcept(conjunction_v<
	is_nothrow_move_constructible<T>, is_nothrow_assignable<T&, U>>) {
	T old_val = static_cast<T&&>(val);
	val = static_cast<U&&>(new_val);
	return old_val;
}

struct pair_piecewise_construct_tag {
	constexpr explicit pair_piecewise_construct_tag() = default;
	~pair_piecewise_construct_tag() = default;
};
inline constexpr pair_piecewise_construct_tag pair_piecewise_construct_t;

template <typename...>
class tuple;

template <typename>
struct tuple_size;
template <typename T>
constexpr size_t tuple_size_v = tuple_size<T>::value;

template <size_t, typename...>
struct tuple_element;
template <size_t Index, typename... Types>
using tuple_element_t = typename tuple_element<Index, Types...>::type;
template <size_t Index, typename... Types>
using tuple_extract_base_t = typename tuple_element<Index, Types...>::tuple_type;

template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, Types...>& get(tuple<Types...>& t) noexcept;
template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, Types...>& get(const tuple<Types...>& t) noexcept;
template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, Types...>&& get(tuple<Types...>&& t) noexcept;
template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, Types...>&& get(const tuple<Types...>&& t) noexcept;

template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, Types...>&& pair_get_from_tuple(tuple<Types...>&& t) noexcept;

template <typename T1, typename T2>
struct pair {
	typedef T1 first_type;
	typedef T2 second_type;

	T1 first;
	T2 second;

	template <typename U1 = T1, typename U2 = T2>
		requires(conjunction_v<is_default_constructible<U1>, is_default_constructible<U2>>)
	MSTL_CONSTEXPR explicit(!conjunction_v<
		is_implicitly_default_constructible<U1>, is_implicitly_default_constructible<U2>>)
		pair() noexcept(conjunction_v<
			is_nothrow_default_constructible<U1>, is_nothrow_default_constructible<U2>>)
		: first(), second() {}

	template <typename U1 = T1, typename U2 = T2>
		requires(conjunction_v<is_copy_constructible<U1>, is_copy_constructible<U2>>)
	MSTL_CONSTEXPR explicit(!conjunction_v<
		is_convertible<const U1&, U1>, is_convertible<const U2&, U2>>)
		pair(const T1& a, const T2& b) noexcept(conjunction_v<
			is_nothrow_copy_constructible<U1>, is_nothrow_copy_constructible<U2>>)
		: first(a), second(b) {}

	template <typename U1, typename U2>
		requires(conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>>)
	MSTL_CONSTEXPR explicit(!conjunction_v<
		is_convertible<U1, T1>, is_convertible<U2, T2>>)
		pair(U1&& a, U2&& b) noexcept(conjunction_v<
			is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(MSTL::forward<U1>(a)), second(MSTL::forward<U2>(b)) {}

	pair(const pair& p) = default;
	pair(pair&& p) = default;

	template <typename U1, typename U2>
		requires(conjunction_v<is_constructible<T1, const U1&>, is_constructible<T2, const U2&>>)
	MSTL_CONSTEXPR explicit(!conjunction_v<
		is_convertible<const U1&, T1>, is_convertible<const U2&, T2>>)
		pair(const pair<U1, U2>& p) noexcept(conjunction_v<
			is_nothrow_constructible<T1, const U1&>, is_nothrow_constructible<T2, const U2&>>)
		: first(p.first), second(p.second) {}

	template <typename U1, typename U2>
		requires(conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>>)
	MSTL_CONSTEXPR explicit(!conjunction_v<
		is_convertible<U1, T1>, is_convertible<U2, T2>>)
		pair(pair<U1, U2>&& p) noexcept(conjunction_v<
			is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(MSTL::forward<U1>(p.first)), second(MSTL::forward<U2>(p.second)) {}


	template <typename Tuple1, typename Tuple2, size_t... Index1, size_t... Index2>
	MSTL_CONSTEXPR pair(Tuple1& t1, Tuple2& t2, index_sequence<Index1...>, index_sequence<Index2...>)
		: first(MSTL::pair_get_from_tuple<Index1>(MSTL::move(t1))...),
		second(MSTL::pair_get_from_tuple<Index2>(MSTL::move(t2))...) {}

	// use pair_piecewise_construct_t to construct from tuple
	template <typename... Types1, typename... Types2>
	MSTL_CONSTEXPR pair(pair_piecewise_construct_tag, tuple<Types1...> t1, tuple<Types2...> t2)
		: pair(t1, t2, index_sequence_for<Types1...>{}, index_sequence_for<Types2...>{}) {}


	// use identity_t to fasten type informations
	template <typename self = pair>
		requires(conjunction_v<
			is_copy_assignable<typename self::first_type>, is_copy_assignable<typename self::second_type>>)
	MSTL_CONSTEXPR pair& operator =(type_identity_t<const self&> p) noexcept(conjunction_v<
		is_nothrow_copy_assignable<T1>, is_nothrow_copy_assignable<T2>>) {
		first = p.first;
		second = p.second;
		return *this;
	}

	// use identity_t to fasten type informations
	template <typename self = pair>
		requires(conjunction_v<
			is_move_assignable<typename self::first_type>, is_move_assignable<typename self::second_type>>)
	MSTL_CONSTEXPR pair& operator =(type_identity_t<self&&> p) noexcept(conjunction_v<
		is_nothrow_move_assignable<T1>, is_nothrow_move_assignable<T2>>) {
		first = MSTL::forward<T1>(p.first);
		second = MSTL::forward<T2>(p.second);
		return *this;
	}

	template <typename U1, typename U2>
		requires(conjunction_v<negation<
			is_same<pair, pair<U1, U2>>>, is_assignable<T1&, const U1&>, is_assignable<T2&, const U2&>>)
	MSTL_CONSTEXPR pair& operator =(const pair<U1, U2>& p) noexcept(conjunction_v<
		is_nothrow_assignable<T1&, const U1&>, is_nothrow_assignable<T2&, const U2&>>) {
		first = p.first;
		second = p.second;
		return *this;
	}

	template <typename U1, typename U2>
		requires(conjunction_v<negation<
			is_same<pair, pair<U1, U2>>>, is_assignable<T1&, U1>, is_assignable<T2&, U2>>)
	MSTL_CONSTEXPR pair& operator =(pair<U1, U2>&& p) noexcept(conjunction_v<
		is_nothrow_assignable<T1&, U1>, is_nothrow_assignable<T2&, U2>>) {
		first = MSTL::forward<U1>(p.first);
		second = MSTL::forward<U2>(p.second);
		return *this;
	}

	pair& operator=(const volatile pair&) = delete;

	MSTL_CONSTEXPR void swap(pair& p) noexcept(conjunction_v<
		is_nothrow_swappable<T1>, is_nothrow_swappable<T2>>) {
		MSTL::swap(first, p.first);
		MSTL::swap(second, p.second);
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
MSTL_CONSTEXPR bool operator <=(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return !(x > y);
}
template <typename T1, typename T2, typename U1, typename U2>
MSTL_CONSTEXPR bool operator >=(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return !(x < y);
}
template <typename T1, typename T2>
	requires(conjunction_v<is_swappable<T1>, is_swappable<T2>>)
MSTL_CONSTEXPR void swap(pair<T1, T2>& lh, pair<T1, T2>& rh) noexcept(noexcept(lh.swap(rh))) {
	lh.swap(rh);
}

template <typename T1, typename T2>
MSTL_CONSTEXPR pair<unwrap_ref_decay_t<T1>, unwrap_ref_decay_t<T2>> make_pair(T1&& x, T2&& y)
noexcept(conjunction_v<is_nothrow_constructible<unwrap_ref_decay_t<T1>, T1>,
	is_nothrow_constructible<unwrap_ref_decay_t<T2>, T2>>) {
	using unwrap_pair = pair<unwrap_ref_decay_t<T1>, unwrap_ref_decay_t<T2>>;
	return unwrap_pair(MSTL::forward<T1>(x), MSTL::forward<T2>(y));
}


template <typename... Types>
struct tuple_size<tuple<Types...>> : integral_constant<size_t, sizeof...(Types)> {};


template <size_t Index>
struct tuple_element<Index, tuple<>> {
	static_assert(false, "tuple element index out of range.");
};
template <typename This, typename... Rest>
struct tuple_element<0, tuple<This, Rest...>> {
	using type = This;
	using tuple_type = tuple<This, Rest...>;
};
template <size_t Index, typename This, typename... Rest>
struct tuple_element<Index, tuple<This, Rest...>>
	: tuple_element<Index - 1, tuple<Rest...>> {};

template <size_t Index, typename... Types>
struct tuple_element : tuple_element<Index, tuple<Types...>> {};


template <typename T1, typename T2>
struct tuple_size<pair<T1, T2>> : integral_constant<size_t, 2> {};


template <size_t Index, typename T1, typename T2>
struct tuple_element<Index, pair<T1, T2>> {
	static_assert(Index < 2, "pair element index out of range.");
	using type = conditional_t<Index == 0, T1, T2>;
	using tuple_type = tuple<T1, T2>;
};


template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, pair<T1, T2>>& get(pair<T1, T2>& pir) noexcept {
	if constexpr (Index == 0) 
		return pir.first;
	else 
		return pir.second;
}
template <typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR T1& get(pair<T1, T2>& pir) noexcept {
	return pir.first;
}
template <typename T2, typename T1>
MSTL_NODISCARD MSTL_CONSTEXPR T2& get(pair<T1, T2>& pir) noexcept {
	return pir.second;
}


template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, pair<T1, T2>>&
get(const pair<T1, T2>& pir) noexcept {
	if constexpr (Index == 0) 
		return pir.first;
	else 
		return pir.second;
}
template <typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR const T1& get(const pair<T1, T2>& pir) noexcept {
	return pir.first;
}
template <typename T2, typename T1>
MSTL_NODISCARD MSTL_CONSTEXPR const T2& get(const pair<T1, T2>& pir) noexcept {
	return pir.second;
}


template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, pair<T1, T2>>&&
get(pair<T1, T2>&& pir) noexcept {
	if constexpr (Index == 0) 
		return MSTL::forward<T1>(pir.first);
	else 
		return MSTL::forward<T2>(pir.second);
}
template <typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR T1&& get(pair<T1, T2>&& pir) noexcept {
	return MSTL::forward<T1>(pir.first);
}
template <typename T2, typename T1>
MSTL_NODISCARD MSTL_CONSTEXPR T2&& get(pair<T1, T2>&& pir) noexcept {
	return MSTL::forward<T2>(pir.second);
}


template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, pair<T1, T2>>&&
get(const pair<T1, T2>&& pir) noexcept {
	if constexpr (Index == 0) 
		return MSTL::forward<const T1>(pir.first);
	else 
		return MSTL::forward<const T2>(pir.second);
}
template <typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR const T1&& get(const pair<T1, T2>&& pir) noexcept {
	return MSTL::forward<const T1>(pir.first);
}
template <typename T2, typename T1>
MSTL_NODISCARD MSTL_CONSTEXPR const T2&& get(const pair<T1, T2>&& pir) noexcept {
	return MSTL::forward<const T2>(pir.second);
}


MSTL_END_NAMESPACE__
#endif // MSTL_UTILITY_HPP__
