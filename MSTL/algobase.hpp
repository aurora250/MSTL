#ifndef MSTL_ALGOBASE_HPP__
#define MSTL_ALGOBASE_HPP__
#include "iterator.hpp"
#include "utility.hpp"
#include "functor.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Iterator1, typename Iterator2, typename BinaryPredicate, enable_if_t<
	is_input_iter_v<Iterator1> && is_input_iter_v<Iterator2>, int> = 0>
MSTL_NODISCARD MSTL_CONSTEXPR bool equal(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, BinaryPredicate binary_pred) {
	for (; first1 != last1; ++first1, ++first2) {
		if (!binary_pred(*first1, *first2)) return false;
	}
	return true;
}

template <typename Iterator1, typename Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool equal(Iterator1 first1, Iterator1 last1, Iterator2 first2) {
	return MSTL::equal(first1, last1, first2, MSTL::equal_to<iter_val_t<Iterator1>>());
}

template <typename Iterator, typename T, enable_if_t<
	is_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR void fill(Iterator first, Iterator last, const T& value) {
	for (; first != last; ++first) *first = value;
}

template <typename Iterator, typename T, enable_if_t<
	is_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR Iterator fill_n(Iterator first, size_t n, const T& value) {
	for (; n > 0; --n, ++first) *first = value;
	return first;
}

template <typename Iterator1, typename Iterator2, enable_if_t<
	is_input_iter_v<Iterator1> && is_input_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR void iter_swap(Iterator1 a, Iterator2 b)
noexcept(noexcept(MSTL::swap(*a, *b))) {
	MSTL::swap(*a, *b);
}

template <typename T, typename Compare>
MSTL_CONSTEXPR const T& max(const T& a, const T& b, Compare comp)
noexcept(noexcept(comp(a, b))) {
	return comp(a, b) ? b : a;
}

template <typename T>
MSTL_CONSTEXPR const T& max(const T& a, const T& b) 
noexcept(noexcept(a < b)) {
	return a < b ? b : a;
}

template <typename T, typename Compare>
MSTL_CONSTEXPR const T& min(const T& a, const T& b, Compare comp)
noexcept(noexcept(comp(b, a))) {
	return comp(b, a) ? b : a;
}

template <typename T>
MSTL_CONSTEXPR const T& min(const T& a, const T& b) 
noexcept(noexcept(b < a)) {
	return b < a ? b : a;
}

template <typename Iterator, typename Compare, enable_if_t<
	is_input_iter_v<Iterator>, int> = 0>
pair<iter_val_t<Iterator>, iter_val_t<Iterator>>
minmax(Iterator first, Iterator last, Compare comp) {
	using T = iter_val_t<Iterator>;
	if (first == last) {
		if constexpr (!is_default_constructible_v<T>)
			Exception(StopIterator("minmax cannot compare empty ranges."));
		return MSTL::make_pair(T(), T());
	}
	T min_val = *first;
	T max_val = *first;

	++first;
	for (; first != last; ++first) {
		if (comp(*first, min_val)) 
			min_val = *first;
		else if (!comp(*first, max_val)) 
			max_val = *first;
	}
	return MSTL::make_pair(min_val, max_val);
}

template <typename Iterator>
pair<iter_val_t<Iterator>, iter_val_t<Iterator>> minmax(Iterator first, Iterator last) {
	return MSTL::minmax(first, last, MSTL::less<iter_val_t<Iterator>>());
}

template <typename T, typename Compare>
MSTL_CONSTEXPR const T& median(const T& a, const T& b, const T& c, Compare comp) 
noexcept(noexcept(comp(a, b))) {
	if (comp(a, b))
		if (comp(b, c))
			return b;
		else if (comp(a, c))
			return c;
		else
			return a;
	else if (comp(a, c))
		return a;
	else if (comp(b, c))
		return c;
	else
		return b;
}

template <typename T>
MSTL_CONSTEXPR const T& median(const T& a, const T& b, const T& c) 
noexcept(noexcept(MSTL::median(a, b, c, MSTL::less<T>()))) {
	return MSTL::median(a, b, c, MSTL::less<T>());
}

template <typename Iterator, typename Compare, enable_if_t<
	is_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR Iterator max_element(Iterator first, Iterator last, Compare comp) {
	if (first == last) return first;
	Iterator result = first;
	while (++first != last)
		if (comp(*result, *first)) result = first;
	return result;
}

template <typename Iterator>
MSTL_CONSTEXPR Iterator max_element(Iterator first, Iterator last) {
	return MSTL::max_element(first, last, MSTL::less<iter_val_t<Iterator>>());
}

template <typename Iterator, typename Compare, enable_if_t<
	is_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR Iterator min_element(Iterator first, Iterator last, Compare comp) {
	if (first == last) return first;
	Iterator result = first;
	while (++first != last)
		if (comp(*first, *result)) result = first;
	return result;
}

template <typename Iterator>
MSTL_CONSTEXPR Iterator min_element(Iterator first, Iterator last) {
	return MSTL::min_element(first, last, MSTL::less<iter_val_t<Iterator>>());
}

template <typename Iterator, typename Compare, enable_if_t<
	is_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR pair<Iterator, Iterator> minmax_element(Iterator first, Iterator last, Compare comp) {
	Iterator min = MSTL::min_element(first, last, comp);
	Iterator max = MSTL::max_element(first, last, comp);
	return MSTL::make_pair(min, max);
}

template <typename Iterator>
MSTL_CONSTEXPR pair<Iterator, Iterator> minmax_element(Iterator first, Iterator last) {
	return MSTL::minmax_element(first, last, MSTL::less<iter_val_t<Iterator>>());
}

template <typename T, typename Compare>
const T& clamp(const T& value, const T& lower, const T& upper, Compare comp) 
noexcept(noexcept(comp(value, lower))) {
	if (comp(value, lower))
		return lower;
	else if (comp(upper, value))
		return upper;
	return value;
}

template <typename T>
const T& clamp(const T& value, const T& lower, const T& upper) 
noexcept(noexcept(MSTL::clamp(value, lower, upper, MSTL::less<T>()))) {
	return MSTL::clamp(value, lower, upper, MSTL::less<T>());
}

template <typename Iterator1, typename Iterator2, typename Compare, enable_if_t<
	is_input_iter_v<Iterator1> && is_input_iter_v<Iterator2>, int> = 0>
MSTL_NODISCARD MSTL_CONSTEXPR bool lexicographical_compare(
	Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2, Compare comp) {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (comp(*first1, *first2)) return true;
		if (comp(*first2, *first1)) return false;
	}
	return first1 == last1 && first2 != last2;
}

template <typename Iterator1, typename Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool lexicographical_compare(
	Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	return MSTL::lexicographical_compare(first1, last1, first2, last2, MSTL::less<iter_val_t<Iterator1>>());
}

inline MSTL_NODISCARD bool lexicographical_compare(const byte_t* first1,
	const byte_t* last1, const byte_t* first2, const byte_t* last2) {
	const size_t len1 = last1 - first1;
	const size_t len2 = last2 - first2;
	const int result = MSTL::memcmp(first1, first2, MSTL::min(len1, len2));
	return result != 0 ? result < 0 : len1 < len2;
}

template <typename Iterator1, typename Iterator2, typename Compare, enable_if_t<
	is_input_iter_v<Iterator1> && is_input_iter_v<Iterator2>, int> = 0>
pair<Iterator1, Iterator2>
MSTL_CONSTEXPR mismatch(Iterator1 first1, Iterator1 last1, Iterator2 first2, Compare comp) {
	while (first1 != last1 && comp(*first1, *first2)) {
		++first1; ++first2;
	}
	return MSTL::make_pair<Iterator1, Iterator2>(first1, first2);
}

template <typename Iterator1, typename Iterator2>
pair<Iterator1, Iterator2> MSTL_CONSTEXPR mismatch(Iterator1 first1, Iterator1 last1, Iterator2 first2) {
	return MSTL::mismatch(first1, last1, first2, MSTL::equal_to<iter_val_t<Iterator1>>());
}

template <typename Iterator1, typename Iterator2, typename Distance, enable_if_t<
	is_rnd_iter_v<Iterator1> && is_rnd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR Iterator2 __copy_distance_aux(Iterator1 first, Iterator1 last, Iterator2 result, Distance*) {
	for (Distance n = last - first; n > 0; --n, ++result, ++first) 
		*result = *first;
	return result;
}

template <typename Iterator1, typename Iterator2, enable_if_t<
	is_input_iter_v<Iterator1> && is_input_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR Iterator2 __copy_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
	if constexpr (is_rnd_iter_v<Iterator1> && is_rnd_iter_v<Iterator2>)
		return MSTL::__copy_distance_aux(first, last, result, MSTL::distance_type(first));
	else {
		for (; first != last; ++result, ++first) *result = *first;
		return result;
	}
}

template <typename T>
MSTL_CONSTEXPR T* __copy_trivial_aux(const T* first, const T* last, T* result) {
	if constexpr (is_trivially_copy_assignable_v<T>) {
		MSTL::memmove(result, first, (int)(sizeof(T) * (last - first)));
		return result + (last - first);
	}
	else 
		return MSTL::__copy_distance_aux(first, last, result, (ptrdiff_t*)0);
}

template <typename Iterator1, typename Iterator2>
struct copy_dispatch {
	MSTL_CONSTEXPR Iterator2 operator ()(Iterator1 first, Iterator1 last, Iterator2 result) {
		return MSTL::__copy_aux(first, last, result);
	}
};
template <typename T>
struct copy_dispatch<T*, T*> {
	MSTL_CONSTEXPR T* operator ()(T* first, T* last, T* result) {
		return MSTL::__copy_trivial_aux(first, last, result);
	}
};
template <typename T>
struct copy_dispatch<const T*, T*> {
	MSTL_CONSTEXPR T* operator ()(const T* first, const T* last, T* result) {
		return MSTL::__copy_trivial_aux(first, last, result);
	}
};

template <typename Iterator1, typename Iterator2>
MSTL_CONSTEXPR Iterator2 copy(Iterator1 first, Iterator1 last, Iterator2 result) {
	return copy_dispatch<Iterator1, Iterator2>()(first, last, result);
}

inline byte_t* copy(const byte_t* first, const byte_t* last, byte_t* result) {
	MSTL::memmove(result, first, static_cast<size_t>(last - first));
	return result + (last - first);
}

template < typename Iterator1, typename Iterator2, enable_if_t<
	is_input_iter_v<Iterator1> && is_input_iter_v<Iterator2>, int> = 0>
pair<Iterator1, Iterator2> copy_n(Iterator1 first, size_t count, Iterator2 result) {
	if constexpr (is_rnd_iter_v<Iterator1>) {
		Iterator1 last = first + count;
		return pair<Iterator1, Iterator2>(last, MSTL::copy(first, last, result));
	}
	else {
		for (; count > 0; --count, ++first, ++result)
			*result = *first;
		return pair<Iterator1, Iterator2>(first, result);
	}
}

template <typename Iterator1, typename Iterator2, enable_if_t<
	is_bid_iter_v<Iterator1> && is_bid_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR Iterator2 __copy_backward_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
	while (first != last) *--result = *--last;
	return result;
}

template <typename T>
MSTL_CONSTEXPR T* __copy_backward_trivial_aux(const T* first, const T* last, T* result) {
	if constexpr (is_trivially_copy_assignable_v<T>) {
		const ptrdiff_t N = last - first;
		MSTL::memmove(result - N, first, (int)(sizeof(T) * N));
		return result - N;
	}
	else 
		return MSTL::__copy_backward_aux(first, last, result);
}

template <typename Iterator1, typename Iterator2>
struct copy_backward_dispatch {
	MSTL_CONSTEXPR Iterator2 operator()(Iterator1 first, Iterator1 last, Iterator2 result) {
		return MSTL::__copy_backward_aux(first, last, result);
	}
};
template <typename T>
struct copy_backward_dispatch<T*, T*> {
	MSTL_CONSTEXPR T* operator()(T* first, T* last, T* result) {
		return MSTL::__copy_backward_trivial_aux(first, last, result);
	}
};
template <typename T>
struct copy_backward_dispatch<const T*, T*> {
	MSTL_CONSTEXPR T* operator()(const T* first, const T* last, T* result) {
		return MSTL::__copy_backward_trivial_aux(first, last, result);
	}
};

template <typename Iterator1, typename Iterator2, enable_if_t<
	is_rnd_iter_v<Iterator1> && is_rnd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR Iterator2 copy_backward(Iterator1 first, Iterator1 last, Iterator2 result) {
	return copy_backward_dispatch<Iterator1, Iterator2>()(first, last, result);
}

inline byte_t* copy_backward(const byte_t* first, const byte_t* last, byte_t* result) {
	const size_t n = last - first;
	MSTL::memmove(result, first, n);
	return result - n;
}

template <typename Iterator1, typename Iterator2, typename Distance, enable_if_t<
	is_rnd_iter_v<Iterator1> && is_rnd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR Iterator2 __move_distance_aux(Iterator1 first, Iterator1 last, Iterator2 result, Distance*) {
	for (Distance n = last - first; n > 0; --n, ++result, ++first)
		*result = MSTL::move(*first);
	return result;
}

template <typename Iterator1, typename Iterator2, enable_if_t<
	is_input_iter_v<Iterator1> && is_input_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR Iterator2 __move_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
	if constexpr (is_rnd_iter_v<Iterator1> && is_rnd_iter_v<Iterator2>) 
		return MSTL::__move_distance_aux(first, last, result, MSTL::distance_type(first));
	else {
		for (; first != last; ++result, ++first) *result = MSTL::move(*first);
		return result;
	}
}

template <typename T>
MSTL_CONSTEXPR T* __move_trivial_aux(const T* first, const T* last, T* result) {
	if constexpr (is_trivially_move_assignable_v<T>) {
		MSTL::memmove(result, first, (int)(sizeof(T) * (last - first)));
		return result + (last - first);
	}
	else 
		return MSTL::__move_distance_aux(first, last, result, (ptrdiff_t*)0);
}

template <typename Iterator1, typename Iterator2>
struct move_dispatch {
	MSTL_CONSTEXPR Iterator2 operator ()(Iterator1 first, Iterator1 last, Iterator2 result) {
		return MSTL::__move_aux(first, last, result);
	}
};
template <typename T>
struct move_dispatch<T*, T*> {
	MSTL_CONSTEXPR T* operator ()(T* first, T* last, T* result) {
		return MSTL::__move_trivial_aux(first, last, result);
	}
};
template <typename T>
struct move_dispatch<const T*, T*> {
	MSTL_CONSTEXPR T* operator ()(const T* first, const T* last, T* result) {
		return MSTL::__move_trivial_aux(first, last, result);
	}
};

template <typename Iterator1, typename Iterator2>
MSTL_CONSTEXPR Iterator2 move(Iterator1 first, Iterator1 last, Iterator2 result) {
	return move_dispatch<Iterator1, Iterator2>()(first, last, result);
}

inline byte_t* move(const byte_t* first, const byte_t* last, byte_t* result) {
	MSTL::memmove(result, first, static_cast<size_t>(last - first));
	return result + (last - first);
}

template <typename Iterator1, typename Iterator2, enable_if_t<
	is_bid_iter_v<Iterator1> && is_bid_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR Iterator2 __move_backward_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
	while (first != last) *--result = std::move(*--last);
	return result;
}

template <typename T>
MSTL_CONSTEXPR T* __move_backward_trivial_aux(const T* first, const T* last, T* result) {
	if constexpr (is_trivially_move_assignable_v<T>) {
		const ptrdiff_t N = last - first;
		MSTL::memmove(result - N, first, (int)(sizeof(T) * N));
		return result - N;
	}
	else 
		return MSTL::__move_backward_aux(first, last, result);
}

template <typename Iterator1, typename Iterator2>
struct move_backward_dispatch {
	MSTL_CONSTEXPR Iterator2 operator()(Iterator1 first, Iterator1 last, Iterator2 result) {
		return MSTL::__move_backward_aux(first, last, result);
	}
};
template <typename T>
struct move_backward_dispatch<T*, T*> {
	MSTL_CONSTEXPR T* operator()(T* first, T* last, T* result) {
		return MSTL::__move_backward_trivial_aux(first, last, result);
	}
};
template <typename T>
struct move_backward_dispatch<const T*, T*> {
	MSTL_CONSTEXPR T* operator()(const T* first, const T* last, T* result) {
		return MSTL::__move_backward_trivial_aux(first, last, result);
	}
};

template <typename Iterator1, typename Iterator2>
MSTL_CONSTEXPR Iterator2 move_backward(Iterator1 first, Iterator1 last, Iterator2 result) {
	return move_backward_dispatch<Iterator1, Iterator2>()(first, last, result);
}

inline byte_t* move_backward(const byte_t* first, const byte_t* last, byte_t* result) {
    const size_t n = last - first;
    MSTL::memmove(result - n, first, n);
    return result - n;
}

MSTL_END_NAMESPACE__
#endif // MSTL_ALGOBASE_HPP__
