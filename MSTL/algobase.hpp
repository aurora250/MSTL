#ifndef MSTL_ALGOBASE_HPP__
#define MSTL_ALGOBASE_HPP__
#include "iterator.hpp"
#include "basiclib.h"
#include "pair.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

template <typename Iterator1, typename Iterator2>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2>)
MSTL_CONSTEXPR bool equal(Iterator1 first1, Iterator1 last1, Iterator2 first2) {
	for (; first1 != last1; ++first1, ++first2) {
		if (*first1 != *first2) return false;
	}
	return true;
}
template <typename Iterator1, typename Iterator2, typename BinaryPredicate>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2> && BinaryFunction<BinaryPredicate>)
MSTL_CONSTEXPR bool equal(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, BinaryPredicate binary_pred) {
	for (; first1 != last1; ++first1, ++first2) {
		if (!binary_pred(*first1, *first2)) return false;
	}
	return true;
}

template <typename Iterator, typename T>
	requires(ForwardIterator<Iterator>)
MSTL_CONSTEXPR void fill(Iterator first, Iterator last, T&& value) {
	for (; first != last; ++first) *first = value;
}
template <typename Iterator, typename T>
	requires(ForwardIterator<Iterator>)
MSTL_CONSTEXPR Iterator fill_n(Iterator first, size_t n, T&& value) {
	for (; n > 0; --n, ++first) *first = value;
	return first;
}

template <typename Iterator1, typename Iterator2>
	requires(ForwardIterator<Iterator1> && ForwardIterator<Iterator2>)
MSTL_CONSTEXPR void iter_swap(Iterator1 a, Iterator2 b) {
	using T = typename std::iterator_traits<Iterator1>::value_type;
	T tmp = *a;
	*a = *b;
	*b = tmp;
}

template <typename T>
MSTL_CONSTEXPR const T& maximum(const T& a, const T& b) {
	return a < b ? b : a;
}
template <typename T, typename Compare>
	requires(BinaryFunction<Compare>)
MSTL_CONSTEXPR const T& maximum(const T& a, const T& b, Compare comp) {
	return comp(a, b) ? b : a;
}
template <typename T>
MSTL_CONSTEXPR const T& minimum(const T& a, const T& b) {
	return b < a ? b : a;
}
template <typename T, typename Compare>
	requires(BinaryFunction<Compare>)
MSTL_CONSTEXPR const T& minimum(const T& a, const T& b, Compare comp) {
	return comp(b, a) ? b : a;
}

template <typename Iterator1, typename Iterator2>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2>)
MSTL_CONSTEXPR bool lexicographical_compare(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Iterator2 last2) {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (*first1 < *first2) return true;
		if (*first2 < *first1) return false;
	}
	return first1 == last1 && first2 != last2;
}
template <typename Iterator1, typename Iterator2, typename Compare>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2> && BinaryFunction<Compare>)
MSTL_CONSTEXPR bool lexicographical_compare(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Iterator2 last2, Compare comp) {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (comp(*first1, *first2)) return true;
		if (comp(*first2, *first1)) return false;
	}
	return first1 == last1 && first2 != last2;
}
inline bool lexicographical_compare(const unsigned char* first1, const unsigned char* last1,
	const unsigned char* first2, const unsigned char* last2) {
	const size_t len1 = last1 - first1;
	const size_t len2 = last2 - first2;
	const int result = MSTL::memcmp(first1, first2, (int)MSTL::minimum(len1, len1));
	return result != 0 ? result < 0 : len1 < len2;
}

template <typename Iterator1, typename Iterator2>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2>)
pair<Iterator1, Iterator2>
MSTL_CONSTEXPR mismatch(Iterator1 first1, Iterator1 last1, Iterator2 first2) {
	while (first1 != last1 && *first1 == *first2) {
		++first1; ++first2;
	}
	return MSTL::make_pair<Iterator1, Iterator2>(first1, first2);
}
template <typename Iterator1, typename Iterator2, typename Compare>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2> && BinaryFunction<Compare>)
pair<Iterator1, Iterator2>
MSTL_CONSTEXPR mismatch(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Compare comp) {
	while (first1 != last1 && comp(*first1, *first2)) {
		++first1; ++first2;
	}
	return MSTL::make_pair<Iterator1, Iterator2>(first1, first2);
}

// copy: 
template <typename Iterator1, typename Iterator2, typename Distance>
	requires(RandomAccessIterator<Iterator1> && RandomAccessIterator<Iterator2> && Number<Distance>)
MSTL_CONSTEXPR Iterator2 __copy_d(Iterator1 first, Iterator1 last, Iterator2 result, Distance*) {
	for (Distance n = last - first; n > 0; --n, ++result, ++first) 
		*result = *first;
	return result;
}
template <typename Iterator1, typename Iterator2>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 __copy(Iterator1 first, Iterator1 last, Iterator2 result) {
	for (; first != last; ++result, ++first) *result = *first;
	return result;
}
template <typename Iterator1, typename Iterator2>
	requires(RandomAccessIterator<Iterator1> && RandomAccessIterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 __copy(Iterator1 first, Iterator1 last, Iterator2 result) {
	return MSTL::__copy_d(first, last, result, distance_type(first));
}
template <typename T>
	requires(TrivialCopyAssignable<T>)
MSTL_CONSTEXPR T* __copy_t(const T* first, const T* last, T* result) {
	MSTL::memmove(result, first, (int)(sizeof(T) * (last - first)));
	return result + (last - first);
}
template <typename T>
	requires(!TrivialCopyAssignable<T>)
MSTL_CONSTEXPR T* __copy_t(const T* first, const T* last, T* result) {
	return MSTL::__copy_d(first, last, result, (ptrdiff_t*)0);
}
template <typename Iterator1, typename Iterator2>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2>)
struct __copy_dispatch {
	MSTL_CONSTEXPR Iterator2 operator ()(Iterator1 first, Iterator1 last, Iterator2 result) {
		return MSTL::__copy(first, last, result);
	}
};
template <typename T>
struct __copy_dispatch<T*, T*> {
	MSTL_CONSTEXPR T* operator ()(T* first, T* last, T* result) {
		return MSTL::__copy_t(first, last, result);
	}
};
template <typename T>
struct __copy_dispatch<const T*, T*> {
	MSTL_CONSTEXPR T* operator ()(const T* first, const T* last, T* result) {
		return MSTL::__copy_t(first, last, result);
	}
};
template <typename Iterator1, typename Iterator2>
	requires(InputIterator<Iterator1> &&  InputIterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 copy(Iterator1 first, Iterator1 last, Iterator2 result) {
	return __copy_dispatch<Iterator1, Iterator2>()(first, last, result);
}
inline char* copy(const char* first, const char* last, char* result) {
	MSTL::memmove(result, first, (int)(last - first));
	return result + (last - first);
}
inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
	MSTL::memmove(result, first, (int)(sizeof(wchar_t) * (last - first)));
	return result + (last - first);
}

// copy_backward:
template <class Iterator1, class Iterator2>
	requires(BidirectionalIterator<Iterator1> && BidirectionalIterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 __copy_backward(Iterator1 first, Iterator1 last, Iterator2 result) {
	while (first != last) *--result = *--last;
	return result;
}
template <class T>
	requires(TrivialCopyAssignable<T>)
MSTL_CONSTEXPR T* __copy_backward_t(const T* first, const T* last, T* result) {
	const ptrdiff_t N = last - first;
	MSTL::memmove(result - N, first, (int)(sizeof(T) * N));
	return result - N;
}
template <class T>
	requires(!TrivialCopyAssignable<T>)
MSTL_CONSTEXPR T* __copy_backward_t(const T* first, const T* last, T* result) {
	return MSTL::__copy_backward(first, last, result);
}
template <class Iterator1, class Iterator2>
	requires(BidirectionalIterator<Iterator1> && BidirectionalIterator<Iterator2>)
struct __copy_backward_dispatch {
	MSTL_CONSTEXPR Iterator2 operator()(Iterator1 first, Iterator1 last, Iterator2 result) {
		return MSTL::__copy_backward(first, last, result);
	}
};
template <class T>
struct __copy_backward_dispatch<T*, T*> {
	MSTL_CONSTEXPR T* operator()(T* first, T* last, T* result) {
		return MSTL::__copy_backward_t(first, last, result);
	}
};
template <class T>
struct __copy_backward_dispatch<const T*, T*> {
	MSTL_CONSTEXPR T* operator()(const T* first, const T* last, T* result) {
		return MSTL::__copy_backward_t(first, last, result);
	}
};
template <class Iterator1, class Iterator2>
	requires(BidirectionalIterator<Iterator1> && BidirectionalIterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 copy_backward(Iterator1 first, Iterator1 last, Iterator2 result) {
	return __copy_backward_dispatch<Iterator1, Iterator2>()(first, last, result);
}
MSTL_END_NAMESPACE__

#endif // MSTL_ALGOBASE_HPP__
