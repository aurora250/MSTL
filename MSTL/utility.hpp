#ifndef MSTL_UTILITY_HPP__
#define MSTL_UTILITY_HPP__
#include "concepts.hpp"
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
MSTL_CONSTEXPR void swap(T(& lh)[Size], T(& rh)[Size]) noexcept(is_nothrow_swappable<T>::value) {
    if (&lh != &rh) {
        T* first1 = lh;
        T* last1 = first1 + Size;
        T* first2 = rh;
        for (; first1 != last1; ++first1, ++first2) {
            MSTL::swap(*first1, *first2);
        }
    }
}

template <typename T, enable_if_t<conjunction_v<is_move_constructible<T>, is_move_assignable<T>>, int>>
MSTL_CONSTEXPR void swap(T& lh, T& rh)
noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>) {
    T tmp = MSTL::move(lh);
    lh = MSTL::move(rh);
    rh = MSTL::move(tmp);
}

template <typename T, typename U>
MSTL_CONSTEXPR T exchange(T& val, U&& new_val) noexcept(conjunction_v<
	is_nothrow_move_constructible<T>, is_nothrow_assignable<T&, U>>) {
	T old_val = MSTL::move(val);
	val = MSTL::forward<U>(new_val);
	return old_val;
}

// have the compile perform NRVO(Named Return Value Optimization) instead of moving it.
// after C++ 11, compilers do NRVO when the following conditions are met:
//   the function returns a class-type object and the object is a local object of the function.
//   the function's return statement returns the local object directly.
template <typename T, enable_if_t<is_default_constructible_v<T>, int> = 0>
MSTL_CONSTEXPR T initialize() noexcept(is_nothrow_default_constructible_v<T>) {
	return T();
}
#define INITIALIZE_BASIC_FUNCTION__(OPT) \
	TEMNULL__ MSTL_CONSTEXPR OPT initialize<OPT>() noexcept { return static_cast<OPT>(0); }
MSTL_MACRO_RANGE_CHARS(INITIALIZE_BASIC_FUNCTION__)
MSTL_MACRO_RANGE_FLOAT(INITIALIZE_BASIC_FUNCTION__)
MSTL_MACRO_RANGE_INT(INITIALIZE_BASIC_FUNCTION__)


inline namespace tags {
	// construct without arguments
	struct default_construct_tag {
		constexpr explicit default_construct_tag() = default;
	};
	// construct by arguments
	struct exact_arg_construct_tag {
		constexpr explicit exact_arg_construct_tag() = default;
	};
	// construct by multiple tuples or pairs
	struct piecewise_construct_tag {
		constexpr explicit piecewise_construct_tag() = default;
	};
	// construct by unpacking a tuple or pair
	struct unpack_tuple_construct_tag {
		constexpr explicit unpack_tuple_construct_tag() = default;
	};
}


// compressed_pair saves memory with EBCO(Empty Base Class Optimization).
// it`s an optimization strategy that reduces the align size of an object
// by using empty class as the base class.
template <typename T1, typename T2, bool = is_empty_v<T1> && !is_final_v<T1>>
class compressed_pair final : private T1 {
public:
	using base_type = T1;

	T2 value;

	constexpr compressed_pair(const compressed_pair& pir) : value(pir.value) {}
	constexpr compressed_pair(compressed_pair&& pir) : value(MSTL::move(pir.value)) {}

	template <typename... Args>
	constexpr compressed_pair(default_construct_tag, Args&&... args)
		noexcept(conjunction_v<is_nothrow_default_constructible<T1>, is_nothrow_constructible<T2, Args...>>)
		: T1(), value(MSTL::forward<Args>(args)...) {
	}

	template <typename U1, typename... U2>
	constexpr compressed_pair(exact_arg_construct_tag, U1&& first, U2&&... args)
		noexcept(conjunction_v<is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2...>>)
		: T1(MSTL::forward<U1>(first)), value(MSTL::forward<U2>(args)...) {
	}

	constexpr T1& get_base() noexcept {
		return *this;
	}
	constexpr const T1& get_base() const noexcept {
		return *this;
	}

	constexpr void swap(compressed_pair& rh)
		noexcept(is_nothrow_swappable_v<T2>) {
		MSTL::swap(value, rh.value);
	};
};

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
		: no_compressed(pir.no_compressed), value(pir.value) {
	}
	constexpr compressed_pair(compressed_pair&& pir)
		: no_compressed(MSTL::move(pir.no_compressed)), value(MSTL::move(pir.value)) {
	}

	template <typename... Args>
	constexpr compressed_pair(default_construct_tag, Args&&... args)
		noexcept(conjunction_v<is_nothrow_default_constructible<T1>, is_nothrow_constructible<T2, Args>>)
		: no_compressed(), value(MSTL::forward<Args>(args)...) {
	}

	template <typename U1, typename... U2>
	constexpr compressed_pair(exact_arg_construct_tag, U1&& first, U2&&... args)
		noexcept(conjunction_v<is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2...>>)
		: no_compressed(MSTL::forward<U1>(first)), value(MSTL::forward<U2>(args)...) {
	}

	constexpr T1& get_base() noexcept {
		return no_compressed;
	}
	constexpr const T1& get_base() const noexcept {
		return no_compressed;
	}

	constexpr void swap(compressed_pair& rh)
		noexcept(conjunction_v<is_nothrow_swappable<T1>, is_nothrow_swappable<T2>>) {
		MSTL::swap(value, rh.value);
		MSTL::swap(no_compressed, rh.no_compressed);
	};
};

template <typename T1, typename T2, enable_if_t<
	is_swappable_v<T1> && is_swappable_v<T2>, int> = 0>
constexpr void swap(compressed_pair<T1, T2, false>& lh, compressed_pair<T1, T2, false>& rh)
noexcept(noexcept(lh.swap(rh))) {
	lh.swap(rh);
}


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
	using first_type	= T1;
	using second_type	= T2;

	T1 first;
	T2 second;

#ifdef MSTL_VERSION_20__
	template <typename U1 = T1, typename U2 = T2, enable_if_t<
		conjunction_v<is_default_constructible<U1>, is_default_constructible<U2>>, int> = 0>
	MSTL_CONSTEXPR explicit(!conjunction_v<
		is_implicitly_default_constructible<U1>, is_implicitly_default_constructible<U2>>)
		pair() noexcept(conjunction_v<
			is_nothrow_default_constructible<U1>, is_nothrow_default_constructible<U2>>)
		: first(), second() {}

	template <typename U1 = T1, typename U2 = T2, enable_if_t<
		conjunction_v<is_copy_constructible<U1>, is_copy_constructible<U2>>, int> = 0>
	MSTL_CONSTEXPR explicit(!conjunction_v<is_convertible<const U1&, U1>, is_convertible<const U2&, U2>>)
		pair(const T1& a, const T2& b) noexcept(conjunction_v<
			is_nothrow_copy_constructible<U1>, is_nothrow_copy_constructible<U2>>)
		: first(a), second(b) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>>, int> = 0>
	MSTL_CONSTEXPR explicit(!conjunction_v<is_convertible<U1, T1>, is_convertible<U2, T2>>)
		pair(U1&& a, U2&& b) noexcept(conjunction_v<
			is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(MSTL::forward<U1>(a)), second(MSTL::forward<U2>(b)) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, const U1&>, is_constructible<T2, const U2&>>, int> = 0>
	MSTL_CONSTEXPR explicit(!conjunction_v<is_convertible<const U1&, T1>, is_convertible<const U2&, T2>>)
		pair(const pair<U1, U2>& p) noexcept(conjunction_v<
			is_nothrow_constructible<T1, const U1&>, is_nothrow_constructible<T2, const U2&>>)
		: first(p.first), second(p.second) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>>, int> = 0>
	MSTL_CONSTEXPR explicit(!conjunction_v<
		is_convertible<U1, T1>, is_convertible<U2, T2>>)
		pair(pair<U1, U2>&& p) noexcept(conjunction_v<
			is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(MSTL::forward<U1>(p.first)), second(MSTL::forward<U2>(p.second)) {}
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
		: first(MSTL::forward<U1>(a)), second(MSTL::forward<U2>(b)) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>>&&
		conjunction_v<is_convertible<U1, T1>, is_convertible<U2, T2>>, int> = 0>
	pair(U1&& a, U2&& b) noexcept(conjunction_v<
		is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(MSTL::forward<U1>(a)), second(MSTL::forward<U2>(b)) {}

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
		: first(MSTL::forward<U1>(p.first)), second(MSTL::forward<U2>(p.second)) {}

	template <typename U1, typename U2, enable_if_t<
		conjunction_v<is_constructible<T1, U1>, is_constructible<T2, U2>>&&
		conjunction_v<is_convertible<U1, T1>, is_convertible<U2, T2>>, int> = 0>
	pair(pair<U1, U2>&& p) noexcept(conjunction_v<
		is_nothrow_constructible<T1, U1>, is_nothrow_constructible<T2, U2>>)
		: first(MSTL::forward<U1>(p.first)), second(MSTL::forward<U2>(p.second)) {}
#endif

	pair(const pair& p) = default;
	pair(pair&& p) = default;

	template <typename Tuple1, typename Tuple2, size_t... Index1, size_t... Index2>
	MSTL_CONSTEXPR pair(Tuple1& t1, Tuple2& t2, index_sequence<Index1...>, index_sequence<Index2...>)
		: first(MSTL::pair_get_from_tuple<Index1>(MSTL::move(t1))...),
		second(MSTL::pair_get_from_tuple<Index2>(MSTL::move(t2))...) {}

	// use pair_piecewise_construct_t to construct from tuple
	template <typename... Types1, typename... Types2>
	MSTL_CONSTEXPR pair(piecewise_construct_tag, tuple<Types1...> t1, tuple<Types2...> t2)
		: pair(t1, t2, index_sequence_for<Types1...>{}, index_sequence_for<Types2...>{}) {}


	// use identity_t to fasten type informations
	template <typename self = pair, enable_if_t<conjunction_v<
		is_copy_assignable<typename self::first_type>, is_copy_assignable<typename self::second_type>>, int> = 0>
	MSTL_CONSTEXPR pair& operator =(type_identity_t<const self&> p) noexcept(conjunction_v<
		is_nothrow_copy_assignable<T1>, is_nothrow_copy_assignable<T2>>) {
		first = p.first;
		second = p.second;
		return *this;
	}

	// use identity_t to fasten type informations
	template <typename self = pair, enable_if_t<conjunction_v<
		is_move_assignable<typename self::first_type>, is_move_assignable<typename self::second_type>>, int> = 0>
	MSTL_CONSTEXPR pair& operator =(type_identity_t<self&&> p) noexcept(conjunction_v<
		is_nothrow_move_assignable<T1>, is_nothrow_move_assignable<T2>>) {
		first = MSTL::forward<T1>(p.first);
		second = MSTL::forward<T2>(p.second);
		return *this;
	}

	template <typename U1, typename U2, enable_if_t<conjunction_v<negation<
		is_same<pair, pair<U1, U2>>>, is_assignable<T1&, const U1&>, is_assignable<T2&, const U2&>>, int> = 0>
	MSTL_CONSTEXPR pair& operator =(const pair<U1, U2>& p) noexcept(conjunction_v<
		is_nothrow_assignable<T1&, const U1&>, is_nothrow_assignable<T2&, const U2&>>) {
		first = p.first;
		second = p.second;
		return *this;
	}

	template <typename U1, typename U2, enable_if_t<conjunction_v<negation<
		is_same<pair, pair<U1, U2>>>, is_assignable<T1&, U1>, is_assignable<T2&, U2>>, int> = 0>
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
#if MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename T1, typename T2>
pair(T1, T2) -> pair<T1, T2>;
#endif

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
template <typename T1, typename T2, enable_if_t<
	conjunction_v<is_swappable<T1>, is_swappable<T2>>, int> = 0>
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


template <typename T1, typename T2>
struct hash<MSTL::pair<T1, T2>> {
	MSTL_NODISCARD size_t operator() (const MSTL::pair<T1, T2>& pair) const noexcept {
		return hash<remove_cvref_t<T1>>()(pair.first) ^ hash<remove_cvref_t<T2>>()(pair.second);
	}
};


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


#ifndef MSTL_VERSION_17__
template <size_t Index, typename T1, typename T2>
struct __pair_get_helper;
template <typename T1, typename T2>
struct __pair_get_helper<0, T1, T2> {
	MSTL_NODISCARD MSTL_CONSTEXPR static tuple_element_t<0, pair<T1, T2>>& 
		get(pair<T1, T2>& pir) noexcept {
		return pir.first;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR static const tuple_element_t<0, pair<T1, T2>>& 
		get(const pair<T1, T2>& pir) noexcept {
		return pir.first;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR static tuple_element_t<0, pair<T1, T2>>&&
		get(pair<T1, T2>&& pir) noexcept {
		return MSTL::forward<T1>(pir.first);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR static const tuple_element_t<0, pair<T1, T2>>&&
		get(const pair<T1, T2>&& pir) noexcept {
		return MSTL::forward<const T1>(pir.first);
	}
};
template <typename T1, typename T2>
struct __pair_get_helper<1, T1, T2> {
	MSTL_NODISCARD MSTL_CONSTEXPR static tuple_element_t<1, pair<T1, T2>>&
		get(pair<T1, T2>& pir) noexcept {
		return pir.second;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR static const tuple_element_t<1, pair<T1, T2>>&
		get(const pair<T1, T2>& pir) noexcept {
		return pir.second;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR static tuple_element_t<1, pair<T1, T2>>&&
		get(pair<T1, T2>&& pir) noexcept {
		return MSTL::forward<T2>(pir.second);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR static const tuple_element_t<1, pair<T1, T2>>&&
		get(const pair<T1, T2>&& pir) noexcept {
		return MSTL::forward<const T2>(pir.second);
	}
};
#endif // !MSTL_VERSION_17__


#ifdef MSTL_VERSION_17__
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, pair<T1, T2>>& get(pair<T1, T2>& pir) noexcept {
	if constexpr (Index == 0) 
		return pir.first;
	else 
		return pir.second;
}
#else
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, pair<T1, T2>>&
get(pair<T1, T2>& pir) noexcept {
	return __pair_get_helper<Index, T1, T2>::get(pir);
}
#endif // MSTL_VERSION_17__
template <typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR T1& get(pair<T1, T2>& pir) noexcept {
	return pir.first;
}
template <typename T2, typename T1>
MSTL_NODISCARD MSTL_CONSTEXPR T2& get(pair<T1, T2>& pir) noexcept {
	return pir.second;
}


#ifdef MSTL_VERSION_17__
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, pair<T1, T2>>&
get(const pair<T1, T2>& pir) noexcept {
	if constexpr (Index == 0) 
		return pir.first;
	else 
		return pir.second;
}
#else
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, pair<T1, T2>>& 
get(const pair<T1, T2>& pir) noexcept {
	return __pair_get_helper<Index, T1, T2>::get(pir);
}
#endif // MSTL_VERSION_17__
template <typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR const T1& get(const pair<T1, T2>& pir) noexcept {
	return pir.first;
}
template <typename T2, typename T1>
MSTL_NODISCARD MSTL_CONSTEXPR const T2& get(const pair<T1, T2>& pir) noexcept {
	return pir.second;
}


#ifdef MSTL_VERSION_17__
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, pair<T1, T2>>&&
get(pair<T1, T2>&& pir) noexcept {
	if constexpr (Index == 0) 
		return MSTL::forward<T1>(pir.first);
	else 
		return MSTL::forward<T2>(pir.second);
}
#else
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, pair<T1, T2>>&& 
get(pair<T1, T2>&& pir) noexcept {
	return MSTL::forward<tuple_element_t<Index, pair<T1, T2>>>(
		__pair_get_helper<Index, T1, T2>::get(MSTL::forward<pair<T1, T2>>(pir)));
}
#endif // MSTL_VERSION_17__
template <typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR T1&& get(pair<T1, T2>&& pir) noexcept {
	return MSTL::forward<T1>(pir.first);
}
template <typename T2, typename T1>
MSTL_NODISCARD MSTL_CONSTEXPR T2&& get(pair<T1, T2>&& pir) noexcept {
	return MSTL::forward<T2>(pir.second);
}


#ifdef MSTL_VERSION_17__
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, pair<T1, T2>>&&
get(const pair<T1, T2>&& pir) noexcept {
	if constexpr (Index == 0) 
		return MSTL::forward<const T1>(pir.first);
	else 
		return MSTL::forward<const T2>(pir.second);
}
#else
template <size_t Index, typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, pair<T1, T2>>&&
get(const pair<T1, T2>&& pir) noexcept {
	return MSTL::forward<const tuple_element_t<Index, pair<T1, T2>>>(
		__pair_get_helper<Index, T1, T2>::get(MSTL::forward<const pair<T1, T2>>(pir)));
}
#endif // MSTL_VERSION_17__
template <typename T1, typename T2>
MSTL_NODISCARD MSTL_CONSTEXPR const T1&& get(const pair<T1, T2>&& pir) noexcept {
	return MSTL::forward<const T1>(pir.first);
}
template <typename T2, typename T1>
MSTL_NODISCARD MSTL_CONSTEXPR const T2&& get(const pair<T1, T2>&& pir) noexcept {
	return MSTL::forward<const T2>(pir.second);
}


template <typename T, typename... Args>
MSTL_CONSTEXPR20 void construct(T* const ptr, Args&&... args)
noexcept(is_nothrow_constructible_v<T, Args...>) {
	::new (static_cast<void*>(ptr)) T(MSTL::forward<Args>(args)...);
}


template <typename T>
MSTL_CONSTEXPR20 void destroy(T* pointer) noexcept(is_nothrow_destructible_v<T>) {
	pointer->~T();
}

template <typename Iterator, enable_if_t<
	is_fwd_iter_v<Iterator> && !is_trivially_destructible_v<iter_val_t<Iterator>>, int> = 0>
MSTL_CONSTEXPR20 void destroy(Iterator first, Iterator last)
noexcept(is_nothrow_destructible_v<typename iterator_traits<Iterator>::value_type>) {
	for (; first < last; ++first) MSTL::destroy(&*first);
}

template <typename Iterator, enable_if_t<
	is_fwd_iter_v<Iterator> && is_trivially_destructible_v<iter_val_t<Iterator>>, int> = 0>
MSTL_CONSTEXPR20 void destroy(Iterator, Iterator) noexcept {}

#define DESTORY_CHAR_FUNCTION__(OPT) \
    MSTL_CONSTEXPR20 void destroy(OPT*, OPT*) noexcept {}
MSTL_MACRO_RANGE_CHARS(DESTORY_CHAR_FUNCTION__)


template <typename Iterator>
using get_iter_key_t = remove_const_t<typename iterator_traits<Iterator>::value_type::first_type>;
template <typename Iterator>
using get_iter_val_t = typename iterator_traits<Iterator>::value_type::second_type;
template <typename Iterator>
using get_iter_pair_t = pair<add_const_t<typename iterator_traits<Iterator>::value_type::first_type>,
	typename iterator_traits<Iterator>::value_type::second_type>;

MSTL_END_NAMESPACE__
#endif // MSTL_UTILITY_HPP__
