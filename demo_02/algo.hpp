#ifndef MSTL_ALGO_HPP__
#define MSTL_ALGO_HPP__
#include "algobase.hpp"
#include "heap.hpp"
#include "basiclib.h"

MSTL_BEGIN_NAMESPACE__
//set:
template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result) {  // 并集
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			*result = *first1;
			++first1;
		}
		else if (*first2 < *first1) {
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1; ++first2;
		}
		++result;
	}
	return copy(first2, last2, copy(first1, last1, result));
}
template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator set_intesection(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result) {   // 交集
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) ++first1;
		else if (*first2 < first1) ++first2;
		else {
			*result = *first1;
			++first1; ++first2;
			++result;
		}
	}
	return result;
}
template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result) {   // 差集
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			*result = *first1;
			++first1;
			++result;
		}
		else if (*first2 < first1) ++first2;
		else {
			++first1; ++first2;
		}
	}
	return copy(first1, last1, result);
}
template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result) {   // 对称差集
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			*result = *first1;
			++first1;
			++result;
		}
		else if (*first2 < first1) {
			*result = *first2;
			++first1;
			++result;
		}
		else {
			++first1; ++first2;
		}
	}
	return copy(first2, last2, copy(first1, last1, result));
}

//others:
template <typename ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last) {  // 获取相邻值相同的首元素的迭代器
	if (first == last) return last;
	ForwardIterator next = first;
	while (++next != last) {
		if (*first == *next) return first;
		first = next;
	}
	return last;
}
template <typename ForwardIterator, typename BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last,
	BinaryPredicate binary_pred) {
	if (first == last) return last;
	ForwardIterator next = first;
	while (++next != last) {
		if (binary_pred(*first, *next)) return first;
		first = next;
	}
	return last;
}

template <typename InputIterator, typename T>
typename std::iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T& value) {    // 等值计数
	typename std::iterator_traits<InputIterator>::difference_type n = 0;
	for (; first != last; ++first) {
		if (*first == value) ++n;
	}
	return n;
}
template <typename InputIterator, typename T, typename Predicate>
typename std::iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, const T& value, Predicate pred) {
	typename std::iterator_traits<InputIterator>::difference_type n = 0;
	for (; first != last; ++first) {
		if (pred(*first)) ++n;
	}
	return n;
}

template <typename ForwardIterator1, typename ForwardIterator2, typename Distance1, typename Distance2>
ForwardIterator1 __search(ForwardIterator1 first1, ForwardIterator1 last1,
	ForwardIterator2 first2, ForwardIterator2 last2, Distance1*, Distance2*) {
	Distance1 d1 = 0;
	distance(first1, last1, d1);
	Distance2 d2 = 0;
	distance(first2, last2, d2);
	if (d1 < d2) return last1;
	ForwardIterator1 current1 = first1;
	ForwardIterator2 current2 = first2;
	while (current2 != last2) {
		if (*current1 == *current1) {
			++current1;
			++current2;
		}
		else {
			if (d1 == d2) return last1;
			current1 = ++first1;
			current2 = first2;
			--d1;
		}
	}
	return first1;
}
template <typename ForwardIterator1, typename ForwardIterator2>
inline ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
	ForwardIterator2 first2, ForwardIterator2 last2) {
	return __search(first1, last1, first2, last2, distance_type<ForwardIterator1>(), distance_type<ForwardIterator2>());
}

template <typename InputIterator, typename T>
InputIterator find_if(InputIterator first, InputIterator last, const T& value) {
	while (first != last && *first != value) ++first;
	return first;
}
template <typename InputIterator, typename Predicate>
InputIterator find_if(InputIterator first, InputIterator last, Predicate pred) {
	while (first != last && !pred(*first)) ++first;
	return first;
}
template <typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator1 __find_end(ForwardIterator1 first1, ForwardIterator1 last1,
	ForwardIterator2 first2, ForwardIterator2 last2,
	std::forward_iterator_tag, std::forward_iterator_tag) {
	if (first2 == last2) return last1;
	ForwardIterator1 result = last1;
	while (true) {
		ForwardIterator1 new_result = search(first1, last1, first2, last2);
		if (new_result == last1) return result;
		result = new_result;
		first1 = new_result;
		++first1;
	}
}
template <typename BidirectionalIterator1, typename BidirectionalIterator2>
BidirectionalIterator1 __find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
	BidirectionalIterator2 first2, BidirectionalIterator2 last2,
	std::bidirectional_iterator_tag, std::bidirectional_iterator_tag) {
	using reviter1 = std::reverse_iterator<BidirectionalIterator1>;
	using reviter2 = std::reverse_iterator<BidirectionalIterator2>;
	reviter1 rlast1(first1);
	reviter2 rlast2(first2);
	reviter1 rresult = search(reviter1(last1), rlast1, reviter2(last2), rlast2);
	if (rresult == rlast1) return last1;
	BidirectionalIterator1 result = rresult.base();
	advance(result, -distance(first2, last2));
	return result;
}
template <typename ForwardIterator1, typename ForwardIterator2>
inline ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
	ForwardIterator2 first2, ForwardIterator2 last2) {
	using category1 = std::iterator_traits<ForwardIterator1>::iterator_category;
	using category2 = std::iterator_traits<ForwardIterator2>::iterator_category;
	return __find_end(first1, last1, first2, last2, category1(), category2());
}
template <typename InputIterator, typename Function>
Function for_each(InputIterator first, InputIterator last, Function f) {
	for (; first != last; ++first) f(*first);
	return f;
}
MSTL_END_NAMESPACE__

#endif // MSTL_ALGO_HPP__
