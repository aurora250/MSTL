#ifndef MSTL_ALGOBASE_HPP__
#define MSTL_ALGOBASE_HPP__
#include "iterator.hpp"
#include "algobase.hpp"
#include "utility.hpp"
#include "errorlib.h"
#include "concepts.hpp"
#include "macro_ranges.h"
MSTL_BEGIN_NAMESPACE__

// equal
template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR bool equal(Iterator1 first1, Iterator1 last1, Iterator2 first2) noexcept {
	for (; first1 != last1; ++first1, ++first2) {
		if (*first1 != *first2) return false;
	}
	return true;
}

template <typename Iterator1, typename Iterator2, typename BinaryPredicate>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR bool equal(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, BinaryPredicate binary_pred) noexcept {
	for (; first1 != last1; ++first1, ++first2) {
		if (!binary_pred(*first1, *first2)) return false;
	}
	return true;
}

// fill
template <typename Iterator, typename T>
	requires(forward_iterator<Iterator>)
MSTL_CONSTEXPR void fill(Iterator first, Iterator last, const T& value) {
	for (; first != last; ++first) *first = value;
}

template <typename Iterator, typename T>
	requires(forward_iterator<Iterator>)
MSTL_CONSTEXPR Iterator fill_n(Iterator first, size_t n, const T& value) {
	for (; n > 0; --n, ++first) *first = value;
	return first;
}

// iter swap
template <typename Iterator1, typename Iterator2>
	requires(forward_iterator<Iterator1> && forward_iterator<Iterator2>)
MSTL_CONSTEXPR void iter_swap(Iterator1 a, Iterator2 b)
noexcept(noexcept(MSTL::swap(*a, *b))) {
	MSTL::swap(*a, *b);
}

// maximum
template <typename T>
MSTL_CONSTEXPR const T& max(const T& a, const T& b) {
	return a < b ? b : a;
}

template <typename T, typename Compare>
MSTL_CONSTEXPR const T& max(const T& a, const T& b, Compare comp) {
	return comp(a, b) ? b : a;
}

// minimum
template <typename T>
MSTL_CONSTEXPR const T& min(const T& a, const T& b) {
	return b < a ? b : a;
}

template <typename T, typename Compare>
MSTL_CONSTEXPR const T& min(const T& a, const T& b, Compare comp) {
	return comp(b, a) ? b : a;
}

// minmax
template<typename Iterator>
	requires(forward_iterator<Iterator>)
pair<typename iterator_traits<Iterator>::value_type, typename iterator_traits<Iterator>::value_type>
minmax(Iterator first, Iterator last) {
	using T = typename iterator_traits<Iterator>::value_type;
	if (first == last) {
		if constexpr (!is_default_constructible_v<T>)
			Exception(StopIterator("minmax cannot compare empty ranges."));
		return MSTL::make_pair(T(), T());
	}
	T min_val = *first;
	T max_val = *first;

	++first;
	for (; first != last; ++first) {
		if (*first < min_val) 
			min_val = *first;
		else if (*first > max_val) 
			max_val = *first;
	}
	return MSTL::make_pair(min_val, max_val);
}

template<typename Iterator, typename Compare>
	requires(forward_iterator<Iterator>)
pair<typename iterator_traits<Iterator>::value_type, typename iterator_traits<Iterator>::value_type>
minmax(Iterator first, Iterator last, Compare comp) {
	using T = typename iterator_traits<Iterator>::value_type;
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

// mediam
template <class T>
MSTL_CONSTEXPR const T& median(const T& a, const T& b, const T& c) {
	if (a < b)
		if (b < c)
			return b;
		else if (a < c)
			return c;
		else
			return a;
	else if (a < c)
		return a;
	else if (b < c)
		return c;
	else
		return b;
}

template <class T, class Compare>
MSTL_CONSTEXPR const T& median(const T& a, const T& b, const T& c, Compare comp) {
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

// max element
template <typename Iterator>
	requires(forward_iterator<Iterator>)
MSTL_CONSTEXPR Iterator max_element(Iterator first, Iterator last) {
	if (first == last) return first;
	Iterator result = first;
	while (++first != last)
		if (*result < *first) result = first;
	return result;
}

template <typename Iterator, typename Compare>
	requires(forward_iterator<Iterator>)
MSTL_CONSTEXPR Iterator max_element(Iterator first, Iterator last, Compare comp) {
	if (first == last) return first;
	Iterator result = first;
	while (++first != last)
		if (comp(*result, *first)) result = first;
	return result;
}

// min element
template <typename Iterator>
	requires(forward_iterator<Iterator>)
MSTL_CONSTEXPR Iterator min_element(Iterator first, Iterator last) {
	if (first == last) return first;
	Iterator result = first;
	while (++first != last)
		if (*first < *result) result = first;
	return result;
}

template <typename Iterator, typename Compare>
	requires(forward_iterator<Iterator>)
MSTL_CONSTEXPR Iterator min_element(Iterator first, Iterator last, Compare comp) {
	if (first == last) return first;
	Iterator result = first;
	while (++first != last)
		if (comp(*first, *result)) result = first;
	return result;
}

// minmax element
template <typename Iterator>
	requires(forward_iterator<Iterator>)
MSTL_CONSTEXPR pair<Iterator, Iterator> minmax_element(Iterator first, Iterator last) {
	Iterator min = MSTL::min_element(first, last);
	Iterator max = MSTL::max_element(first, last);
	return MSTL::make_pair(min, max);
}

template <typename Iterator, typename Compare>
	requires(forward_iterator<Iterator>)
MSTL_CONSTEXPR pair<Iterator, Iterator> minmax_element(Iterator first, Iterator last, Compare comp) {
	Iterator min = MSTL::min_element(first, last, comp);
	Iterator max = MSTL::max_element(first, last, comp);
	return MSTL::make_pair(min, max);
}

// lexicographical compare
template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR bool lexicographical_compare(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Iterator2 last2) noexcept {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (*first1 < *first2) return true;
		if (*first2 < *first1) return false;
	}
	return first1 == last1 && first2 != last2;
}

template <typename Iterator1, typename Iterator2, typename Compare>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR bool lexicographical_compare(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Iterator2 last2, Compare comp) noexcept {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (comp(*first1, *first2)) return true;
		if (comp(*first2, *first1)) return false;
	}
	return first1 == last1 && first2 != last2;
}

inline MSTL_NODISCARD bool lexicographical_compare(const unsigned char* first1,
	const unsigned char* last1, const unsigned char* first2, const unsigned char* last2) noexcept {
	const size_t len1 = last1 - first1;
	const size_t len2 = last2 - first2;
	const int result = MSTL::memcmp(first1, first2, (int)MSTL::min(len1, len1));
	return result != 0 ? result < 0 : len1 < len2;
}

// mismatch
template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
pair<Iterator1, Iterator2>
MSTL_CONSTEXPR mismatch(Iterator1 first1, Iterator1 last1, Iterator2 first2) {
	while (first1 != last1 && *first1 == *first2) {
		++first1; ++first2;
	}
	return MSTL::make_pair<Iterator1, Iterator2>(first1, first2);
}

template <typename Iterator1, typename Iterator2, typename Compare>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
pair<Iterator1, Iterator2>
MSTL_CONSTEXPR mismatch(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Compare comp) {
	while (first1 != last1 && comp(*first1, *first2)) {
		++first1; ++first2;
	}
	return MSTL::make_pair<Iterator1, Iterator2>(first1, first2);
}

// copy

template <typename Iterator1, typename Iterator2, typename Distance>
	requires(random_access_iterator<Iterator1> && random_access_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 copy_distance_aux(Iterator1 first, Iterator1 last, Iterator2 result, Distance*) {
	for (Distance n = last - first; n > 0; --n, ++result, ++first) 
		*result = *first;
	return result;
}

template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 copy_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
	for (; first != last; ++result, ++first) *result = *first;
	return result;
}

template <typename Iterator1, typename Iterator2>
	requires(random_access_iterator<Iterator1> && random_access_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 copy_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
	return MSTL::copy_distance_aux(first, last, result, MSTL::distance_type(first));
}

template <typename T>
	requires(trivially_copy_assignable<T>)
MSTL_CONSTEXPR T* copy_trivial_aux(const T* first, const T* last, T* result) {
	MSTL::memmove(result, first, (int)(sizeof(T) * (last - first)));
	return result + (last - first);
}

template <typename T>
	requires(!trivially_copy_assignable<T>)
MSTL_CONSTEXPR T* copy_trivial_aux(const T* first, const T* last, T* result) {
	return MSTL::copy_distance_aux(first, last, result, (ptrdiff_t*)0);
}

template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
struct copy_dispatch {
	MSTL_CONSTEXPR Iterator2 operator ()(Iterator1 first, Iterator1 last, Iterator2 result) {
		return MSTL::copy_aux(first, last, result);
	}
};
template <typename T>
struct copy_dispatch<T*, T*> {
	MSTL_CONSTEXPR T* operator ()(T* first, T* last, T* result) {
		return MSTL::copy_trivial_aux(first, last, result);
	}
};
template <typename T>
struct copy_dispatch<const T*, T*> {
	MSTL_CONSTEXPR T* operator ()(const T* first, const T* last, T* result) {
		return MSTL::copy_trivial_aux(first, last, result);
	}
};

template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> &&  input_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 copy(Iterator1 first, Iterator1 last, Iterator2 result) {
	return copy_dispatch<Iterator1, Iterator2>()(first, last, result);
}

#define COPY_CHAR_FUNCTION__(OPT) \
inline OPT* copy(const OPT* first, const OPT* last, OPT* result) { \
	MSTL::memmove(result, first, (int)(sizeof(OPT) * (last - first))); \
	return result + (last - first); \
}
MSTL_MACRO_RANGE_CHARS(COPY_CHAR_FUNCTION__)


// copy n

template < typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
pair<Iterator1, Iterator2> copy_n(Iterator1 first, size_t count, Iterator2 result) {
	for (; count > 0; --count, ++first, ++result)
		*result = *first;
	return pair<Iterator1, Iterator2>(first, result);
}

template <typename Iterator1, typename Iterator2>
	requires(random_access_iterator<Iterator1> && input_iterator<Iterator2>)
pair<Iterator1, Iterator2> copy_n(Iterator1 first, size_t count, Iterator2 result) {
	Iterator1 last = first + count;
	return pair<Iterator1, Iterator2>(last, MSTL::copy(first, last, result));
}


// copy backward

template <typename Iterator1, typename Iterator2>
	requires(bidirectional_iterator<Iterator1> && bidirectional_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 copy_backward_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
	while (first != last) *--result = *--last;
	return result;
}

template <typename T>
	requires(trivially_copy_assignable<T>)
MSTL_CONSTEXPR T* copy_backward_trivial_aux(const T* first, const T* last, T* result) {
	const ptrdiff_t N = last - first;
	MSTL::memmove(result - N, first, (int)(sizeof(T) * N));
	return result - N;
}

template <typename T>
	requires(!trivially_copy_assignable<T>)
MSTL_CONSTEXPR T* copy_backward_trivial_aux(const T* first, const T* last, T* result) {
	return MSTL::copy_backward_aux(first, last, result);
}

template <typename Iterator1, typename Iterator2>
	requires(bidirectional_iterator<Iterator1> && bidirectional_iterator<Iterator2>)
struct copy_backward_dispatch {
	MSTL_CONSTEXPR Iterator2 operator()(Iterator1 first, Iterator1 last, Iterator2 result) {
		return MSTL::copy_backward_aux(first, last, result);
	}
};
template <typename T>
struct copy_backward_dispatch<T*, T*> {
	MSTL_CONSTEXPR T* operator()(T* first, T* last, T* result) {
		return MSTL::copy_backward_trivial_aux(first, last, result);
	}
};
template <typename T>
struct copy_backward_dispatch<const T*, T*> {
	MSTL_CONSTEXPR T* operator()(const T* first, const T* last, T* result) {
		return MSTL::copy_backward_trivial_aux(first, last, result);
	}
};

template <typename Iterator1, typename Iterator2>
	requires(bidirectional_iterator<Iterator1> && bidirectional_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 copy_backward(Iterator1 first, Iterator1 last, Iterator2 result) {
	return copy_backward_dispatch<Iterator1, Iterator2>()(first, last, result);
}

#define COPY_BACKWARD_CHAR_FUNCTION__(OPT) \
inline OPT* copy_backward(const OPT* first, const OPT* last, OPT* result) { \
	const ptrdiff_t n = last - first; \
	MSTL::memmove(result, first, (int)(sizeof(OPT) * n)); \
	return result - n; \
}
MSTL_MACRO_RANGE_CHARS(COPY_BACKWARD_CHAR_FUNCTION__)


// move

template <typename Iterator1, typename Iterator2, typename Distance>
	requires(random_access_iterator<Iterator1>&& random_access_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 move_distance_aux(Iterator1 first, Iterator1 last, Iterator2 result, Distance*) {
	for (Distance n = last - first; n > 0; --n, ++result, ++first)
		*result = MSTL::move(*first);
	return result;
}

template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1>&& input_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 move_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
	for (; first != last; ++result, ++first) *result = MSTL::move(*first);
	return result;
}

template <typename Iterator1, typename Iterator2>
	requires(random_access_iterator<Iterator1>&& random_access_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 move_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
	return MSTL::move_distance_aux(first, last, result, MSTL::distance_type(first));
}

template <typename T>
	requires(is_trivially_move_assignable_v<T>)
MSTL_CONSTEXPR T* move_trivial_aux(const T* first, const T* last, T* result) {
	MSTL::memmove(result, first, (int)(sizeof(T) * (last - first)));
	return result + (last - first);
}

template <typename T>
	requires(!is_trivially_move_assignable_v<T>)
MSTL_CONSTEXPR T* move_trivial_aux(const T* first, const T* last, T* result) {
	return MSTL::move_distance_aux(first, last, result, (ptrdiff_t*)0);
}

template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1>&& input_iterator<Iterator2>)
struct move_dispatch {
	MSTL_CONSTEXPR Iterator2 operator ()(Iterator1 first, Iterator1 last, Iterator2 result) {
		return MSTL::move_aux(first, last, result);
	}
};
template <typename T>
struct move_dispatch<T*, T*> {
	MSTL_CONSTEXPR T* operator ()(T* first, T* last, T* result) {
		return MSTL::move_trivial_aux(first, last, result);
	}
};
template <typename T>
struct move_dispatch<const T*, T*> {
	MSTL_CONSTEXPR T* operator ()(const T* first, const T* last, T* result) {
		return MSTL::move_trivial_aux(first, last, result);
	}
};

template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1>&& input_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 move(Iterator1 first, Iterator1 last, Iterator2 result) {
	return move_dispatch<Iterator1, Iterator2>()(first, last, result);
}

#define MOVE_CHAR_FUNCTION__(OPT) \
inline OPT* move(const OPT* first, const OPT* last, OPT* result) { \
	MSTL::memmove(result, first, (int)(sizeof(OPT) * (last - first))); \
	return result + (last - first); \
}
MSTL_MACRO_RANGE_CHARS(MOVE_CHAR_FUNCTION__)


// move backward

template <typename Iterator1, typename Iterator2>
	requires(bidirectional_iterator<Iterator1>&& bidirectional_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 move_backward_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
	while (first != last) *--result = std::move(*--last);
	return result;
}

template <typename T>
	requires(is_trivially_move_assignable_v<T>)
MSTL_CONSTEXPR T* move_backward_trivial_aux(const T* first, const T* last, T* result) {
	const ptrdiff_t N = last - first;
	MSTL::memmove(result - N, first, (int)(sizeof(T) * N));
	return result - N;
}

template <typename T>
	requires(!is_trivially_move_assignable_v<T>)
MSTL_CONSTEXPR T* move_backward_trivial_aux(const T* first, const T* last, T* result) {
	return MSTL::move_backward_aux(first, last, result);
}

template <typename Iterator1, typename Iterator2>
	requires(bidirectional_iterator<Iterator1>&& bidirectional_iterator<Iterator2>)
struct move_backward_dispatch {
	MSTL_CONSTEXPR Iterator2 operator()(Iterator1 first, Iterator1 last, Iterator2 result) {
		return MSTL::move_backward_aux(first, last, result);
	}
};
template <typename T>
struct move_backward_dispatch<T*, T*> {
	MSTL_CONSTEXPR T* operator()(T* first, T* last, T* result) {
		return MSTL::move_backward_trivial_aux(first, last, result);
	}
};
template <typename T>
struct move_backward_dispatch<const T*, T*> {
	MSTL_CONSTEXPR T* operator()(const T* first, const T* last, T* result) {
		return MSTL::move_backward_trivial_aux(first, last, result);
	}
};

template <typename Iterator1, typename Iterator2>
	requires(bidirectional_iterator<Iterator1>&& bidirectional_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 move_backward(Iterator1 first, Iterator1 last, Iterator2 result) {
	return move_backward_dispatch<Iterator1, Iterator2>()(first, last, result);
}

#define MOVE_BACKWARD_CHAR_FUNCTION__(OPT) \
inline OPT* move_backward(const OPT* first, const OPT* last, OPT* result) { \
    const ptrdiff_t n = last - first; \
    MSTL::memmove(result - n, first, (int)(sizeof(OPT) * n)); \
    return result - n; \
}
MSTL_MACRO_RANGE_CHARS(MOVE_BACKWARD_CHAR_FUNCTION__)

MSTL_END_NAMESPACE__
#endif // MSTL_ALGOBASE_HPP__
