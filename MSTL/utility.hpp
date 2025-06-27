#ifndef MSTL_UTILITY_HPP__
#define MSTL_UTILITY_HPP__
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T, T... Values>
struct integer_sequence {
    static_assert(is_integral_v<T>, "integer sequence requires integral types.");

    using value_type = T;

    MSTL_NODISCARD static constexpr size_t size() noexcept {
        return sizeof...(Values);
    }
};
template <typename T, T Size>
using make_integer_sequence =
#ifdef MSTL_SUPPORT_MAKE_INTEGER_SEQ__
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


template <typename T, size_t Size, enable_if_t<is_swappable<T>::value, int>>
constexpr void swap(T(& lh)[Size], T(& rh)[Size]) noexcept(is_nothrow_swappable<T>::value) {
    if (&lh == &rh) return;
	T* first1 = lh;
	T* last1 = first1 + Size;
	T* first2 = rh;
	for (; first1 != last1; ++first1, ++first2) {
		_MSTL swap(*first1, *first2);
	}
}

template <typename T, enable_if_t<conjunction_v<is_move_constructible<T>, is_move_assignable<T>>, int>>
constexpr void swap(T& lh, T& rh)
noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>) {
    T tmp = _MSTL move(lh);
    lh = _MSTL move(rh);
    rh = _MSTL move(tmp);
}

template <typename T, typename U>
constexpr T exchange(T& val, U&& new_val) noexcept(conjunction_v<
	is_nothrow_move_constructible<T>, is_nothrow_assignable<T&, U>>) {
	T old_val = _MSTL move(val);
	val = _MSTL forward<U>(new_val);
	return old_val;
}


MSTL_BEGIN_TAG__
// use allocator as the argument of functions.
struct allocator_arg_tag {
	constexpr allocator_arg_tag() noexcept = default;
};

// construct without arguments.
struct default_construct_tag {
	constexpr default_construct_tag() noexcept = default;
};
// construct by arguments.
struct exact_arg_construct_tag {
	constexpr exact_arg_construct_tag() noexcept  = default;
};
// construct by arguments inplace.
struct inplace_construct_tag {
	constexpr inplace_construct_tag() noexcept  = default;
};
// construct by unpacking tuple or pair type.
struct unpack_utility_construct_tag {
	constexpr unpack_utility_construct_tag() noexcept = default;
};
MSTL_END_TAG__


// compressed_pair saves memory with EBCO(Empty Base Class Optimization).
// it`s an optimization strategy that reduces the align size of an object
// by using empty class as the base class.
template <typename T1, typename T2, bool = is_empty_v<T1> && !is_final_v<T1>>
class compressed_pair final : private T1 {
public:
	using base_type = T1;

	T2 value;

	constexpr compressed_pair(const compressed_pair& pir) : value(pir.value) {}
	constexpr compressed_pair(compressed_pair&& pir)  noexcept : value(_MSTL move(pir.value)) {}

	template <typename... Args>
	constexpr explicit compressed_pair(default_construct_tag, Args&&... args)
		noexcept(conjunction_v<is_nothrow_default_constructible<T1>, is_nothrow_constructible<T2, Args...>>)
		: T1(), value(_MSTL forward<Args>(args)...) {}

	template <typename U1, typename... U2>
	constexpr compressed_pair(exact_arg_construct_tag, U1&& first, U2&&... args)
		noexcept(conjunction_v<is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2...>>)
		: T1(_MSTL forward<U1>(first)), value(_MSTL forward<U2>(args)...) {}

	constexpr T1& get_base() noexcept {
		return *this;
	}
	constexpr const T1& get_base() const noexcept {
		return *this;
	}

	constexpr void swap(compressed_pair& rh)
	noexcept(is_nothrow_swappable_v<T2>) {
		_MSTL swap(value, rh.value);
	};
};
#if MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename T1, typename T2>
compressed_pair(T1, T2) -> compressed_pair<T1, T2>;
#endif

template <typename T1, typename T2, enable_if_t<is_swappable_v<T2>, int> = 0>
constexpr void swap(compressed_pair<T1, T2, true>& lh, compressed_pair<T1, T2, true>& rh)
noexcept(noexcept(lh.swap(rh))) {
	lh.swap(rh);
}

template <typename T1, typename T2>
class compressed_pair<T1, T2, false> final {
public:
	T1 no_compressed;
	T2 value;

	constexpr compressed_pair(const compressed_pair& pir)
		: no_compressed(pir.no_compressed), value(pir.value) {}
	constexpr compressed_pair(compressed_pair&& pir) noexcept
		: no_compressed(_MSTL move(pir.no_compressed)), value(_MSTL move(pir.value)) {}

	template <typename... Args>
	constexpr explicit compressed_pair(default_construct_tag, Args&&... args)
		noexcept(conjunction_v<is_nothrow_default_constructible<T1>, is_nothrow_constructible<T2, Args...>>)
		: no_compressed(), value(_MSTL forward<Args>(args)...) {
	}

	template <typename U1, typename... U2>
	constexpr compressed_pair(exact_arg_construct_tag, U1&& first, U2&&... args)
		noexcept(conjunction_v<is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2...>>)
		: no_compressed(_MSTL forward<U1>(first)), value(_MSTL forward<U2>(args)...) {
	}

	constexpr T1& get_base() noexcept {
		return no_compressed;
	}
	constexpr const T1& get_base() const noexcept {
		return no_compressed;
	}

	constexpr void swap(compressed_pair& rh)
	noexcept(conjunction_v<is_nothrow_swappable<T1>, is_nothrow_swappable<T2>>) {
		_MSTL swap(value, rh.value);
		_MSTL swap(no_compressed, rh.no_compressed);
	};
};

template <typename T1, typename T2, enable_if_t<
	is_swappable_v<T1> && is_swappable_v<T2>, int> = 0>
constexpr void swap(compressed_pair<T1, T2, false>& lh, compressed_pair<T1, T2, false>& rh)
noexcept(noexcept(lh.swap(rh))) {
	lh.swap(rh);
}


#ifdef MSTL_VERSION_17__
template <typename... Types>
struct visitor : Types... {
	using Types::operator()...;
};

template <typename... Types>
visitor(Types...) -> visitor<Types...>;
#endif


template <typename...>
struct tuple;

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
MSTL_NODISCARD constexpr tuple_element_t<Index, Types...>& get(tuple<Types...>& t) noexcept;
template <size_t Index, typename... Types>
MSTL_NODISCARD constexpr const tuple_element_t<Index, Types...>& get(const tuple<Types...>& t) noexcept;
template <size_t Index, typename... Types>
MSTL_NODISCARD constexpr tuple_element_t<Index, Types...>&& get(tuple<Types...>&& t) noexcept;
template <size_t Index, typename... Types>
MSTL_NODISCARD constexpr const tuple_element_t<Index, Types...>&& get(const tuple<Types...>&& t) noexcept;

template <size_t Index, typename... Types>
MSTL_NODISCARD constexpr tuple_element_t<Index, Types...>&& __pair_get_from_tuple(tuple<Types...>&& t) noexcept;


template <typename T1, typename T2>
struct pair {
	using first_type	= T1;
	using second_type	= T2;

	T1 first;
	T2 second;

#ifdef MSTL_VERSION_20__
	template <typename U1 = T1, typename U2 = T2, enable_if_t<
		conjunction_v<is_default_constructible<U1>, is_default_constructible<U2>>, int> = 0>
	constexpr explicit(!conjunction_v<
		is_implicitly_default_constructible<U1>, is_implicitly_default_constructible<U2>>)
		pair() noexcept(conjunction_v<
			is_nothrow_default_constructible<U1>, is_nothrow_default_constructible<U2>>)
		: first(), second() {}

	template <typename U1 = T1, typename U2 = T2, enable_if_t<
		conjunction_v<is_copy_constructible<U1>, is_copy_constructible<U2>>, int> = 0>
	constexpr explicit(!conjunction_v<is_convertible<const U1&, U1>, is_convertible<const U2&, U2>>)
		pair(const T1& a, const T2& b) noexcept(conjunction_v<
			is_nothrow_copy_constructible<U1>, is_nothrow_copy_constructible<U2>>)
		: first(a), second(b) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>>, int> = 0>
	constexpr explicit(!conjunction_v<is_convertible<U1, T1>, is_convertible<U2, T2>>)
		pair(U1&& a, U2&& b) noexcept(conjunction_v<
			is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(_MSTL forward<U1>(a)), second(_MSTL forward<U2>(b)) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, const U1&>, is_constructible<T2, const U2&>>, int> = 0>
	constexpr explicit(!conjunction_v<is_convertible<const U1&, T1>, is_convertible<const U2&, T2>>)
		pair(const pair<U1, U2>& p) noexcept(conjunction_v<
			is_nothrow_constructible<T1, const U1&>, is_nothrow_constructible<T2, const U2&>>)
		: first(p.first), second(p.second) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>>, int> = 0>
	constexpr explicit(!conjunction_v<
		is_convertible<U1, T1>, is_convertible<U2, T2>>)
		pair(pair<U1, U2>&& p) noexcept(conjunction_v<
			is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(_MSTL forward<U1>(p.first)), second(_MSTL forward<U2>(p.second)) {}
#else
	template <typename U1 = T1, typename U2 = T2, enable_if_t<
		conjunction_v<is_default_constructible<U1>, is_default_constructible<U2>> &&
		!conjunction_v<is_implicitly_default_constructible<U1>,
		is_implicitly_default_constructible<U2>>, int> = 0>
	explicit pair() noexcept(conjunction_v<
		is_nothrow_default_constructible<U1>, is_nothrow_default_constructible<U2>>)
		: first(), second() {}

	template <typename U1 = T1, typename U2 = T2, enable_if_t<
		conjunction_v<is_default_constructible<U1>, is_default_constructible<U2>>&&
		conjunction_v<is_implicitly_default_constructible<U1>,
		is_implicitly_default_constructible<U2>>, int> = 0>
	pair() noexcept(conjunction_v<
		is_nothrow_default_constructible<U1>, is_nothrow_default_constructible<U2>>)
		: first(), second() {}

	template <typename U1 = T1, typename U2 = T2, enable_if_t<
		conjunction_v<is_copy_constructible<U1>, is_copy_constructible<U2>> &&
		!conjunction_v<is_convertible<const U1&, U1>, is_convertible<const U2&, U2>>, int> = 0>
	explicit pair(const T1& a, const T2& b) noexcept(conjunction_v<
		is_nothrow_copy_constructible<U1>, is_nothrow_copy_constructible<U2>>)
		: first(a), second(b) {}

	template <typename U1 = T1, typename U2 = T2, enable_if_t<
		conjunction_v<is_copy_constructible<U1>, is_copy_constructible<U2>>&&
		conjunction_v<is_convertible<const U1&, U1>, is_convertible<const U2&, U2>>, int> = 0>
	pair(const T1& a, const T2& b) noexcept(conjunction_v<
		is_nothrow_copy_constructible<U1>, is_nothrow_copy_constructible<U2>>)
		: first(a), second(b) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>> &&
		!conjunction_v<is_convertible<U1, T1>, is_convertible<U2, T2>>, int> = 0>
	explicit pair(U1&& a, U2&& b) noexcept(conjunction_v<
		is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(_MSTL forward<U1>(a)), second(_MSTL forward<U2>(b)) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>> &&
		conjunction_v<is_convertible<U1, T1>, is_convertible<U2, T2>>, int> = 0>
	pair(U1&& a, U2&& b) noexcept(conjunction_v<
		is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(_MSTL forward<U1>(a)), second(_MSTL forward<U2>(b)) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, const U1&>, is_constructible<T2, const U2&>> &&
		!conjunction_v<is_convertible<const U1&, T1>, is_convertible<const U2&, T2>>, int> = 0>
	explicit pair(const pair<U1, U2>& p) noexcept(conjunction_v<
		is_nothrow_constructible<T1, const U1&>, is_nothrow_constructible<T2, const U2&>>)
		: first(p.first), second(p.second) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, const U1&>, is_constructible<T2, const U2&>>&&
		conjunction_v<is_convertible<const U1&, T1>, is_convertible<const U2&, T2>>, int> = 0>
	pair(const pair<U1, U2>& p) noexcept(conjunction_v<
		is_nothrow_constructible<T1, const U1&>, is_nothrow_constructible<T2, const U2&>>)
		: first(p.first), second(p.second) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>> &&
		!conjunction_v<is_convertible<U1, T1>, is_convertible<U2, T2>>, int> = 0>
	explicit pair(pair<U1, U2>&& p) noexcept(conjunction_v<
		is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(_MSTL forward<U1>(p.first)), second(_MSTL forward<U2>(p.second)) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>>&&
		conjunction_v<is_convertible<U1, T1>, is_convertible<U2, T2>>, int> = 0>
	pair(pair<U1, U2>&& p) noexcept(conjunction_v<
		is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(_MSTL forward<U1>(p.first)), second(_MSTL forward<U2>(p.second)) {}
#endif

	pair(const pair& p) = default;
	pair(pair&& p) = default;

	template <typename Tuple1, typename Tuple2, size_t... Index1, size_t... Index2>
	constexpr pair(Tuple1& t1, Tuple2& t2, index_sequence<Index1...>, index_sequence<Index2...>)
		: first(_MSTL __pair_get_from_tuple<Index1>(_MSTL move(t1))...),
		second(_MSTL __pair_get_from_tuple<Index2>(_MSTL move(t2))...) {}

	// construct from tuple
	template <typename... Types1, typename... Types2>
	constexpr pair(unpack_utility_construct_tag, tuple<Types1...> t1, tuple<Types2...> t2)
		: pair(t1, t2, index_sequence_for<Types1...>{}, index_sequence_for<Types2...>{}) {}


	// use identity_t to fasten type information
	template <typename self = pair, enable_if_t<conjunction_v<
		is_copy_assignable<typename self::first_type>, is_copy_assignable<typename self::second_type>>, int> = 0>
	constexpr pair& operator =(type_identity_t<const self&> p) noexcept(conjunction_v<
		is_nothrow_copy_assignable<T1>, is_nothrow_copy_assignable<T2>>) {
		first = p.first;
		second = p.second;
		return *this;
	}

	// use identity_t to fasten type information
	template <typename self = pair, enable_if_t<conjunction_v<
		is_move_assignable<typename self::first_type>, is_move_assignable<typename self::second_type>>, int> = 0>
	constexpr pair& operator =(type_identity_t<self&&> p) noexcept(conjunction_v<
		is_nothrow_move_assignable<T1>, is_nothrow_move_assignable<T2>>) {
		first = _MSTL forward<T1>(p.first);
		second = _MSTL forward<T2>(p.second);
		return *this;
	}

	template <typename U1, typename U2, enable_if_t<conjunction_v<negation<
		is_same<pair, pair<U1, U2>>>, is_assignable<T1&, const U1&>, is_assignable<T2&, const U2&>>, int> = 0>
	constexpr pair& operator =(const pair<U1, U2>& p) noexcept(conjunction_v<
		is_nothrow_assignable<T1&, const U1&>, is_nothrow_assignable<T2&, const U2&>>) {
		first = p.first;
		second = p.second;
		return *this;
	}

	template <typename U1, typename U2, enable_if_t<conjunction_v<negation<
		is_same<pair, pair<U1, U2>>>, is_assignable<T1&, U1>, is_assignable<T2&, U2>>, int> = 0>
	constexpr pair& operator =(pair<U1, U2>&& p) noexcept(conjunction_v<
		is_nothrow_assignable<T1&, U1>, is_nothrow_assignable<T2&, U2>>) {
		first = _MSTL forward<U1>(p.first);
		second = _MSTL forward<U2>(p.second);
		return *this;
	}

	pair& operator=(const volatile pair&) = delete;

	constexpr void swap(pair& p) noexcept(conjunction_v<
		is_nothrow_swappable<T1>, is_nothrow_swappable<T2>>) {
		_MSTL swap(first, p.first);
		_MSTL swap(second, p.second);
	}
};
#if MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename T1, typename T2>
pair(T1, T2) -> pair<T1, T2>;
#endif

template <typename T1, typename T2, typename U1, typename U2>
constexpr bool operator ==(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return x.first == y.first && x.second == y.second;
}
template <typename T1, typename T2, typename U1, typename U2>
constexpr bool operator !=(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return !(x == y);
}
template <typename T1, typename T2, typename U1, typename U2>
constexpr bool operator <(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
}
template <typename T1, typename T2, typename U1, typename U2>
constexpr bool operator >(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return y < x;
}
template <typename T1, typename T2, typename U1, typename U2>
constexpr bool operator <=(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return !(x > y);
}
template <typename T1, typename T2, typename U1, typename U2>
constexpr bool operator >=(const pair<T1, T2>& x, const pair<U1, U2>& y) {
	return !(x < y);
}
template <typename T1, typename T2, enable_if_t<
	conjunction_v<is_swappable<T1>, is_swappable<T2>>, int> = 0>
constexpr void swap(pair<T1, T2>& lh, pair<T1, T2>& rh) noexcept(noexcept(lh.swap(rh))) {
	lh.swap(rh);
}


template <typename T1, typename T2>
constexpr pair<unwrap_ref_decay_t<T1>, unwrap_ref_decay_t<T2>> make_pair(T1&& x, T2&& y)
noexcept(conjunction_v<is_nothrow_constructible<unwrap_ref_decay_t<T1>, T1>,
	is_nothrow_constructible<unwrap_ref_decay_t<T2>, T2>>) {
	using unwrap_pair = pair<unwrap_ref_decay_t<T1>, unwrap_ref_decay_t<T2>>;
	return unwrap_pair(_MSTL forward<T1>(x), _MSTL forward<T2>(y));
}


template <typename T1, typename T2>
struct hash<_MSTL pair<T1, T2>> {
	MSTL_NODISCARD size_t operator() (const _MSTL pair<T1, T2>& pair) const noexcept {
		return hash<remove_cvref_t<T1>>()(pair.first) ^ hash<remove_cvref_t<T2>>()(pair.second);
	}
};


template <typename... Types>
struct tuple_size<tuple<Types...>> : integral_constant<size_t, sizeof...(Types)> {};

template <size_t Index>
struct tuple_element<Index, tuple<>> {};

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


#ifndef MSTL_VERSION_17__
template <size_t Index, typename T1, typename T2>
struct __pair_get_helper;
template <typename T1, typename T2>
struct __pair_get_helper<0, T1, T2> {
	MSTL_NODISCARD constexpr static tuple_element_t<0, pair<T1, T2>>&
		get(pair<T1, T2>& pir) noexcept {
		return pir.first;
	}
	MSTL_NODISCARD constexpr static const tuple_element_t<0, pair<T1, T2>>&
		get(const pair<T1, T2>& pir) noexcept {
		return pir.first;
	}
	MSTL_NODISCARD constexpr static tuple_element_t<0, pair<T1, T2>>&&
		get(pair<T1, T2>&& pir) noexcept {
		return _MSTL forward<T1>(pir.first);
	}
	MSTL_NODISCARD constexpr static const tuple_element_t<0, pair<T1, T2>>&&
		get(const pair<T1, T2>&& pir) noexcept {
		return _MSTL forward<const T1>(pir.first);
	}
};
template <typename T1, typename T2>
struct __pair_get_helper<1, T1, T2> {
	MSTL_NODISCARD constexpr static tuple_element_t<1, pair<T1, T2>>&
		get(pair<T1, T2>& pir) noexcept {
		return pir.second;
	}
	MSTL_NODISCARD constexpr static const tuple_element_t<1, pair<T1, T2>>&
		get(const pair<T1, T2>& pir) noexcept {
		return pir.second;
	}
	MSTL_NODISCARD constexpr static tuple_element_t<1, pair<T1, T2>>&&
		get(pair<T1, T2>&& pir) noexcept {
		return _MSTL forward<T2>(pir.second);
	}
	MSTL_NODISCARD constexpr static const tuple_element_t<1, pair<T1, T2>>&&
		get(const pair<T1, T2>&& pir) noexcept {
		return _MSTL forward<const T2>(pir.second);
	}
};
#endif // !MSTL_VERSION_17__


#ifdef MSTL_VERSION_17__
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD constexpr tuple_element_t<Index, pair<T1, T2>>& get(pair<T1, T2>& pir) noexcept {
	if constexpr (Index == 0)
		return pir.first;
	else
		return pir.second;
}
#else
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD constexpr tuple_element_t<Index, pair<T1, T2>>&
get(pair<T1, T2>& pir) noexcept {
	return __pair_get_helper<Index, T1, T2>::get(pir);
}
#endif // MSTL_VERSION_17__
template <typename T1, typename T2>
MSTL_NODISCARD constexpr T1& get(pair<T1, T2>& pir) noexcept {
	return pir.first;
}
template <typename T2, typename T1>
MSTL_NODISCARD constexpr T2& get(pair<T1, T2>& pir) noexcept {
	return pir.second;
}


#ifdef MSTL_VERSION_17__
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD constexpr const tuple_element_t<Index, pair<T1, T2>>&
get(const pair<T1, T2>& pir) noexcept {
	if constexpr (Index == 0)
		return pir.first;
	else
		return pir.second;
}
#else
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD constexpr const tuple_element_t<Index, pair<T1, T2>>&
get(const pair<T1, T2>& pir) noexcept {
	return __pair_get_helper<Index, T1, T2>::get(pir);
}
#endif // MSTL_VERSION_17__
template <typename T1, typename T2>
MSTL_NODISCARD constexpr const T1& get(const pair<T1, T2>& pir) noexcept {
	return pir.first;
}
template <typename T2, typename T1>
MSTL_NODISCARD constexpr const T2& get(const pair<T1, T2>& pir) noexcept {
	return pir.second;
}


#ifdef MSTL_VERSION_17__
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD constexpr tuple_element_t<Index, pair<T1, T2>>&&
get(pair<T1, T2>&& pir) noexcept {
	if constexpr (Index == 0)
		return _MSTL forward<T1>(pir.first);
	else
		return _MSTL forward<T2>(pir.second);
}
#else
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD constexpr tuple_element_t<Index, pair<T1, T2>>&&
get(pair<T1, T2>&& pir) noexcept {
	return _MSTL forward<tuple_element_t<Index, pair<T1, T2>>>(
		__pair_get_helper<Index, T1, T2>::get(_MSTL forward<pair<T1, T2>>(pir)));
}
#endif // MSTL_VERSION_17__
template <typename T1, typename T2>
MSTL_NODISCARD constexpr T1&& get(pair<T1, T2>&& pir) noexcept {
	return _MSTL forward<T1>(pir.first);
}
template <typename T2, typename T1>
MSTL_NODISCARD constexpr T2&& get(pair<T1, T2>&& pir) noexcept {
	return _MSTL forward<T2>(pir.second);
}


#ifdef MSTL_VERSION_17__
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD constexpr const tuple_element_t<Index, pair<T1, T2>>&&
get(const pair<T1, T2>&& pir) noexcept {
	if constexpr (Index == 0)
		return _MSTL forward<const T1>(pir.first);
	else
		return _MSTL forward<const T2>(pir.second);
}
#else
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD constexpr const tuple_element_t<Index, pair<T1, T2>>&&
get(const pair<T1, T2>&& pir) noexcept {
	return _MSTL forward<const tuple_element_t<Index, pair<T1, T2>>>(
		__pair_get_helper<Index, T1, T2>::get(_MSTL forward<const pair<T1, T2>>(pir)));
}
#endif // MSTL_VERSION_17__
template <typename T1, typename T2>
MSTL_NODISCARD constexpr const T1&& get(const pair<T1, T2>&& pir) noexcept {
	return _MSTL forward<const T1>(pir.first);
}
template <typename T2, typename T1>
MSTL_NODISCARD constexpr const T2&& get(const pair<T1, T2>&& pir) noexcept {
	return _MSTL forward<const T2>(pir.second);
}


template <typename T, typename... Args>
MSTL_CONSTEXPR20 void* construct(T* ptr, Args&&... args)
noexcept(is_nothrow_constructible_v<T, Args...>) {
    return ::new (static_cast<void*>(ptr)) T(_MSTL forward<Args>(args)...);
}


template <typename T>
MSTL_CONSTEXPR20 void destroy(T* pointer) noexcept(is_nothrow_destructible_v<T>) {
	pointer->~T();
}

template <typename Iterator, enable_if_t<
	is_iter_v<Iterator> && !is_trivially_destructible_v<iter_val_t<Iterator>>, int> = 0>
MSTL_CONSTEXPR20 void destroy(Iterator first, Iterator last)
noexcept(is_nothrow_destructible_v<iter_val_t<Iterator>>) {
	for (; first < last; ++first) _MSTL destroy(&*first);
}

template <typename Iterator, enable_if_t<
	is_iter_v<Iterator> && is_trivially_destructible_v<iter_val_t<Iterator>>, int> = 0>
MSTL_CONSTEXPR20 void destroy(Iterator, Iterator) noexcept {}


template <typename Iterator>
using get_iter_key_t = remove_const_t<typename iterator_traits<Iterator>::value_type::first_type>;
template <typename Iterator>
using get_iter_val_t = typename iterator_traits<Iterator>::value_type::second_type;
template <typename Iterator>
using get_iter_pair_t = pair<add_const_t<typename iterator_traits<Iterator>::value_type::first_type>,
	typename iterator_traits<Iterator>::value_type::second_type>;


template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) begin(Container& cont) noexcept(noexcept(cont.begin())) {
	return cont.begin();
}
template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) begin(const Container& cont) noexcept(noexcept(cont.begin())) {
	return cont.begin();
}
template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) end(Container& cont) noexcept(noexcept(cont.end())) {
	return cont.end();
}
template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) end(const Container& cont) noexcept(noexcept(cont.end())) {
	return cont.end();
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR14
T* begin(T (&arr)[Size]) noexcept {
	return arr;
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR14
T* end(T (&arr)[Size]) noexcept {
	return arr + Size;
}

template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) cbegin(const Container& cont) noexcept(noexcept(cont.cbegin())) {
	return cont.cbegin();
}
template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) cend(const Container& cont) noexcept(noexcept(cont.cend())) {
	return cont.cend();
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR14
const T* cbegin(T (&arr)[Size]) noexcept {
    return arr;
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR14
const T* cend(T (&arr)[Size]) noexcept {
    return arr + Size;
}

template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) rbegin(Container& cont) noexcept(noexcept(cont.rbegin())) {
	return cont.rbegin();
}
template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) rbegin(const Container& cont) noexcept(noexcept(cont.rbegin())) {
	return cont.rbegin();
}
template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) rend(Container& cont) noexcept(noexcept(cont.rend())) {
	return cont.rend();
}
template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) rend(const Container& cont) noexcept(noexcept(cont.rend())) {
	return cont.rend();
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR14
reverse_iterator<T*> rbegin(T (&arr)[Size]) noexcept {
	return reverse_iterator<T*>(arr + Size);
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR14
reverse_iterator<T*> rend(T (&arr)[Size]) noexcept {
	return reverse_iterator<T*>(arr);
}
template <typename T>
MSTL_NODISCARD MSTL_CONSTEXPR14
reverse_iterator<const T*> rbegin(std::initializer_list<T> lls) noexcept {
	return reverse_iterator<const T*>(lls.end());
}
template <typename T>
MSTL_NODISCARD MSTL_CONSTEXPR14
reverse_iterator<const T*> rend(std::initializer_list<T> lls) noexcept {
	return reverse_iterator<const T*>(lls.begin());
}

template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) crbegin(const Container& cont) noexcept(noexcept(cont.crbegin())) {
	return cont.crbegin();
}
template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR17
decltype(auto) crend(const Container& cont) noexcept(noexcept(cont.crend())) {
	return cont.crend();
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR14
reverse_iterator<const T*> crbegin(T (&arr)[Size]) noexcept {
    return reverse_iterator<const T*>(arr + Size);
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE MSTL_CONSTEXPR14
reverse_iterator<const T*> crend(T (&arr)[Size]) noexcept {
    return reverse_iterator<const T*>(arr);
}

template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
decltype(auto) size(const Container& cont) noexcept(noexcept(cont.size())) {
	return cont.size();
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
decltype(auto) size(T (&)[Size]) noexcept {
	return Size;
}

template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
decltype(auto) empty(const Container& cont) noexcept(noexcept(cont.empty())) {
	return cont.empty();
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
bool empty(T (&)[Size]) noexcept {
	return false;
}
template <typename T>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
bool empty(T* ptr) noexcept {
	return ptr != nullptr;
}
template <typename T>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
bool empty(std::initializer_list<T> lls) noexcept {
	return lls.size() == 0;
}


template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
decltype(auto) data(Container& cont) noexcept(noexcept(cont.data())) {
	return cont.data();
}
template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
decltype(auto) data(const Container& cont) noexcept(noexcept(cont.data())) {
	return cont.data();
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
T* data(T (& arr)[Size]) noexcept {
	return arr;
}
template <typename T>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
T* data(T* ptr) noexcept {
	return ptr;
}
template <typename T>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
const T* data(std::initializer_list<T> lls) noexcept {
	return lls.begin();
}

template <typename Container>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
decltype(auto) ssize(const Container& cont) noexcept(noexcept(cont.size())) {
	using type = make_signed_t<decltype(cont.size())>;
	return static_cast<common_type_t<ptrdiff_t, type>>(cont.size());
}
template <typename T, size_t Size>
MSTL_NODISCARD MSTL_ALWAYS_INLINE constexpr
ptrdiff_t ssize(T (&)[Size]) noexcept {
	return Size;
}

MSTL_END_NAMESPACE__
#endif // MSTL_UTILITY_HPP__
