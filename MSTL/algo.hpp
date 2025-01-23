#ifndef MSTL_ALGO_HPP__
#define MSTL_ALGO_HPP__
#include "sort.hpp"
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
template <typename Iterator>
	requires(ForwardIterator<Iterator>)
Iterator adjacent_find(Iterator first, Iterator last) {
	if (first == last) return last;
	Iterator next = first;
	while (++next != last) {
		if (*first == *next) return first;
		first = next;
	}
	return last;
}
template <typename Iterator, typename BinaryPredicate>
	requires(ForwardIterator<Iterator>)
Iterator adjacent_find(Iterator first, Iterator last,
	BinaryPredicate binary_pred) {
	if (first == last) return last;
	Iterator next = first;
	while (++next != last) {
		if (binary_pred(*first, *next)) return first;
		first = next;
	}
	return last;
}

template <typename Iterator, typename T>
	requires(InputIterator<Iterator>)
typename std::iterator_traits<Iterator>::difference_type
count(Iterator first, Iterator last, T&& value) {
	typename std::iterator_traits<Iterator>::difference_type n = 0;
	for (; first != last; ++first) {
		if (*first == value) ++n;
	}
	return n;
}
template <typename Iterator, typename T, typename BinaryPredicate>
	requires(InputIterator<Iterator> && BinaryFunction<BinaryPredicate>)
typename std::iterator_traits<Iterator>::difference_type
count_if(Iterator first, Iterator last, T&& value, BinaryPredicate pred) {
	typename std::iterator_traits<Iterator>::difference_type n = 0;
	for (; first != last; ++first) {
		if (pred(*first, std::forward<T>(value))) ++n;
	}
	return n;
}

template <typename Iterator1, typename Iterator2>
	requires(ForwardIterator<Iterator1> && ForwardIterator<Iterator2>)
Iterator1 search(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	using Distance1 = typename std::iterator_traits<Iterator1>::difference_type;
	using Distance2 = typename std::iterator_traits<Iterator2>::difference_type;
	Distance1 d1 = 0;
	distance(first1, last1, d1);
	Distance2 d2 = 0;
	distance(first2, last2, d2);
	if (d1 < d2) return last1;
	Iterator1 current1 = first1;
	Iterator2 current2 = first2;
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

template <typename Iterator1, typename Iterator2, typename BinaryPredicate>
	requires(ForwardIterator<Iterator1> && ForwardIterator<Iterator2>)
Iterator1 search(Iterator1 first1, Iterator1 last1, Iterator2 first2,
	Iterator2 last2, BinaryPredicate binary_pred) {
	using Distance1 = typename std::iterator_traits<Iterator1>::difference_type;
	using Distance2 = typename std::iterator_traits<Iterator2>::difference_type;
	Distance1 d1 = 0;
	MSTL::distance(first1, last1, d1);
	Distance2 d2 = 0;
	MSTL::distance(first2, last2, d2);
	if (d1 < d2) return last1;
	Iterator1 current1 = first1;
	Iterator2 current2 = first2;
	while (current2 != last2) {
		if (binary_pred(*current1, *current2)) {
			++current1;
			++current2;
		}
		else {
			if (d1 == d2)
				return last1;
			else {
				current1 = ++first1;
				current2 = first2;
				--d1;
			}
		}
	}
	return first1;
}

template <typename Iterator, typename T>
	requires(ForwardIterator<Iterator>)
Iterator search_n(Iterator first, Iterator last, size_t count, const T& value) {
	first = find(first, last, value);
	while (first != last) {
		size_t n = count - 1;
		Iterator i = first;
		++i;
		while (i != last && n != 0 && *i == value) {
			++i;
			--n;
		}
		if (n == 0)
			return first;
		else
			first = find(i, last, value);
	}
	return last;
}
template <typename Iterator, typename T, typename BinaryPredicate>
	requires(ForwardIterator<Iterator>)
Iterator search_n(Iterator first, Iterator last, size_t count,
	T&& value, BinaryPredicate binary_pred) {
	while (first != last) {
		if (binary_pred(*first, value)) break;
		++first;
	}
	while (first != last) {
		size_t n = count - 1;
		Iterator i = first;
		++i;
		while (i != last && n != 0 && binary_pred(*i, value)) {
			++i;
			--n;
		}
		if (n == 0)
			return first;
		else {
			while (i != last) {
				if (binary_pred(*i, value)) break;
				++i;
			}
			first = i;
		}
	}
	return last;
}


template <typename Iterator, typename T>
	requires(InputIterator<Iterator>)
Iterator find(Iterator first, Iterator last, T&& value) {
	while (first != last && *first != value) ++first;
	return first;
}
template <typename Iterator, typename T>
	requires(InputIterator<Iterator>)
Iterator find_if(Iterator first, Iterator last, T&& value) {
	while (first != last && *first != value) ++first;
	return first;
}
template <typename Iterator, typename Predicate>
	requires(InputIterator<Iterator>)
Iterator find_if(Iterator first, Iterator last, Predicate pred) {
	while (first != last && !pred(*first)) ++first;
	return first;
}
template <typename Iterator1, typename Iterator2>
	requires(ForwardIterator<Iterator1> && ForwardIterator<Iterator2>)
Iterator1 find_end(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	if (first2 == last2) return last1;
	Iterator1 result = last1;
	while (true) {
		Iterator1 new_result = (search)(first1, last1, first2, last2);
		if (new_result == last1) return result;
		result = new_result;
		first1 = new_result;
		++first1;
	}
}
template <typename Iterator1, typename Iterator2>
	requires(BidirectionalIterator<Iterator1> && BidirectionalIterator<Iterator2>)
Iterator1 find_end(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	using reviter1 = typename std::reverse_iterator<Iterator1>;
	using reviter2 = typename std::reverse_iterator<Iterator2>;
	reviter1 rlast1(first1);
	reviter2 rlast2(first2);
	reviter1 rresult = (search)(reviter1(last1), rlast1, reviter2(last2), rlast2);
	if (rresult == rlast1) return last1;
	Iterator1 result = rresult.base();
	MSTL::advance(result, -distance(first2, last2));
	return result;
}

template <class Iterator1, class Iterator2>
	requires(InputIterator<Iterator1> && ForwardIterator<Iterator2>)
Iterator1 find_first_of(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	for (; first1 != last1; ++first1)
		for (Iterator2 iter = first2; iter != last2; ++iter)
			if (*first1 == *iter) return first1;
	return last1;
}

template <class Iterator1, class Iterator2, class BinaryPredicate>
	requires(InputIterator<Iterator1> && ForwardIterator<Iterator2>)
Iterator1 find_first_of(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Iterator2 last2, BinaryPredicate comp){
	for (; first1 != last1; ++first1)
		for (Iterator2 iter = first2; iter != last2; ++iter)
			if (comp(*first1, *iter)) return first1;
	return last1;
}

template <typename Iterator, typename Function>
	requires(InputIterator<Iterator>)
Function for_each(Iterator first, Iterator last, Function f) {
	for (; first != last; ++first) f(*first);
	return f;
}

template <class Iterator, class Generator>
	requires(InputIterator<Iterator>)
void generate(Iterator first, Iterator last, Generator gen) {
	for (; first != last; ++first)
		*first = gen();
}
template <class Iterator, class Generator>
	requires(InputIterator<Iterator>)
Iterator generate_n(Iterator first, size_t n, Generator gen) {
	for (; n > 0; --n, ++first)
		*first = gen();
	return first;
}

template <class InputIterator1, class InputIterator2>
bool includes(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2) {
	while (first1 != last1 && first2 != last2)
		if (*first2 < *first1)
			return false;
		else if (*first1 < *first2)
			++first1;
		else
			++first1, ++first2;
	return first2 == last2;
}
template <class InputIterator1, class InputIterator2, class Compare>
bool includes(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2, Compare comp) {
	while (first1 != last1 && first2 != last2)
		if (comp(*first2, *first1))
			return false;
		else if (comp(*first1, *first2))
			++first1;
		else
			++first1, ++first2;
	return first2 == last2;
}

template <class Iterator>
	requires(ForwardIterator<Iterator>)
Iterator max_element(Iterator first, Iterator last) {
	if (first == last) return first;
	Iterator result = first;
	while (++first != last)
		if (*result < *first) result = first;
	return result;
}
template <class Iterator, class Compare>
	requires(ForwardIterator<Iterator>)
Iterator max_element(Iterator first, Iterator last,
	Compare comp) {
	if (first == last) return first;
	Iterator result = first;
	while (++first != last)
		if (comp(*result, *first)) result = first;
	return result;
}

template <class Iterator>
	requires(ForwardIterator<Iterator>)
Iterator min_element(Iterator first, Iterator last) {
	if (first == last) return first;
	Iterator result = first;
	while (++first != last)
		if (*first < *result) result = first;
	return result;
}
template <class Iterator, class Compare>
	requires(ForwardIterator<Iterator>)
Iterator min_element(Iterator first, Iterator last,
	Compare comp) {
	if (first == last) return first;
	Iterator result = first;
	while (++first != last)
		if (comp(*first, *result)) result = first;
	return result;
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
	while (first1 != last1 && first2 != last2) {
		if (*first2 < *first1) {
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1;
		}
		++result;
	}
	return copy(first2, last2, copy(first1, last1, result));
}
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
	while (first1 != last1 && first2 != last2) {
		if (comp(*first2, *first1)) {
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1;
		}
		++result;
	}
	return copy(first2, last2, copy(first1, last1, result));
}

template <class BidirectionalIterator, class Predicate>
BidirectionalIterator partition(BidirectionalIterator first,
	BidirectionalIterator last, Predicate pred) {
	while (true) {
		while (true)
			if (first == last)
				return first;
			else if (pred(*first))
				++first;
			else
				break;
		--last;
		while (true)
			if (first == last)
				return first;
			else if (!pred(*last))
				--last;
			else
				break;
		(iter_swap)(first, last);
		++first;
	}
}

template <class InputIterator, class OutputIterator, class T>
OutputIterator remove_copy(InputIterator first, InputIterator last,
	OutputIterator result, const T& value) {
	for (; first != last; ++first)
		if (*first != value) {
			*result = *first;
			++result;
		}
	return result;
}
template <class InputIterator, class OutputIterator, class Predicate>
OutputIterator remove_copy_if(InputIterator first, InputIterator last,
	OutputIterator result, Predicate pred) {
	for (; first != last; ++first)
		if (!pred(*first)) {
			*result = *first;
			++result;
		}
	return result;
}
template <class ForwardIterator, class T>
ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value) {
	first = find(first, last, value);
	ForwardIterator next = first;
	return first == last ? first : (remove_copy)(++next, last, first, value);
}
template <class ForwardIterator, class Predicate>
ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, Predicate pred) {
	first = find_if(first, last, pred);
	ForwardIterator next = first;
	return first == last ? first : (remove_copy_if)(++next, last, first, pred);
}

template <class InputIterator, class OutputIterator, class T>
OutputIterator replace_copy(InputIterator first, InputIterator last,
	OutputIterator result, const T& old_value, const T& new_value) {
	for (; first != last; ++first, ++result)
		*result = *first == old_value ? new_value : *first;
	return result;
}
template <class Iterator, class OutputIterator, class Predicate, class T>
OutputIterator replace_copy_if(Iterator first, Iterator last,
	OutputIterator result, Predicate pred, const T& new_value) {
	for (; first != last; ++first, ++result)
		*result = pred(*first) ? new_value : *first;
	return result;
}
template <class ForwardIterator, class T>
void replace(ForwardIterator first, ForwardIterator last, const T& old_value, const T& new_value) {
	for (; first != last; ++first)
		if (*first == old_value) *first = new_value;
}
template <class ForwardIterator, class Predicate, class T>
void replace_if(ForwardIterator first, ForwardIterator last, Predicate pred, const T& new_value) {
	for (; first != last; ++first)
		if (pred(*first)) *first = new_value;
}

template <class BidirectionalIterator>
void __reverse(BidirectionalIterator first, BidirectionalIterator last, 
	std::bidirectional_iterator_tag) {
	while (true)
		if (first == last || first == --last) return;
		else (iter_swap)(first++, last);
}
template <class RandomAccessIterator>
void __reverse(RandomAccessIterator first, RandomAccessIterator last,
	std::random_access_iterator_tag) {
	while (first < last) (iter_swap)(first++, --last);
}
template <class BidirectionalIterator>
inline void reverse(BidirectionalIterator first, BidirectionalIterator last) {
	__reverse(first, last, (iterator_category)(first));
}

template <class ForwardIterator, class Distance>
void __rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last,
	Distance*, std::forward_iterator_tag) {
	for (ForwardIterator i = middle; ;) {
		(iter_swap)(first, i);
		++first;
		++i;
		if (first == middle) {
			if (i == last) return;
			middle = i;
		}
		else if (i == last)
			i = middle;
	}
}
template <class BidirectionalIterator, class Distance>
void __rotate(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last,
	Distance*, std::bidirectional_iterator_tag) {
	(reverse)(first, middle);
	(reverse)(middle, last);
	(reverse)(first, last);
}
template <class EuclideanRingElement>
EuclideanRingElement __gcd(EuclideanRingElement m, EuclideanRingElement n) {
	while (n != 0) {
		EuclideanRingElement t = m % n;
		m = n;
		n = t;
	}
	return m;
}
template <class RandomAccessIterator, class Distance, class T>
void __rotate_cycle(RandomAccessIterator first, RandomAccessIterator last,
	RandomAccessIterator initial, Distance shift, T*) {
	T value = *initial;
	RandomAccessIterator ptr1 = initial;
	RandomAccessIterator ptr2 = ptr1 + shift;
	while (ptr2 != initial) {
		*ptr1 = *ptr2;
		ptr1 = ptr2;
		if (last - ptr2 > shift)
			ptr2 += shift;
		else
			ptr2 = first + (shift - (last - ptr2));
	}
	*ptr1 = value;
}
template <class RandomAccessIterator, class Distance>
void __rotate(RandomAccessIterator first, RandomAccessIterator middle,
	RandomAccessIterator last, Distance*, std::random_access_iterator_tag) {
	Distance n = (__gcd)(last - first, middle - first);
	while (n--)
		(__rotate_cycle)(first, last, first + n, middle - first, (value_type)(first));
}
template <class ForwardIterator>
inline void rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last) {
	if (first == middle || middle == last) return;
	__rotate(first, middle, last, (distance_type)(first), (iterator_category)(first));
}
template <class ForwardIterator, class OutputIterator>
OutputIterator rotate_copy(ForwardIterator first, ForwardIterator middle,
	ForwardIterator last, OutputIterator result) {
	return copy(first, middle, copy(middle, last, result));
}

template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2) {
	for (; first1 != last1; ++first1, ++first2)
		(iter_swap)(first1, first2);
	return first2;
}

template <class InputIterator, class OutputIterator, class UnaryOperation>
OutputIterator transform(InputIterator first, InputIterator last, OutputIterator result, UnaryOperation op) {
	for (; first != last; ++first, ++result)
		*result = op(*first);
	return result;
}
template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryOperation>
OutputIterator transform(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, OutputIterator result, BinaryOperation binary_op) {
	for (; first1 != last1; ++first1, ++first2, ++result)
		*result = binary_op(*first1, *first2);
	return result;
}

template <class InputIterator, class ForwardIterator>
ForwardIterator __unique_copy(InputIterator first, InputIterator last, 
	ForwardIterator result, std::forward_iterator_tag) {
	*result = *first;
	while (++first != last)
		if (*result != *first) *++result = *first;
	return ++result;
}
template <class InputIterator, class OutputIterator, class T>
OutputIterator __unique_copy(InputIterator first, InputIterator last, OutputIterator result, T*) {
	T value = *first;
	*result = value;
	while (++first != last) {
		if (value != *first) {
			value = *first;
			*++result = value;
		}
	}
	return ++result;
}

template <class InputIterator, class OutputIterator>
inline OutputIterator __unique_copy(InputIterator first, InputIterator last,
	OutputIterator result, std::output_iterator_tag) {
	return __unique_copy(first, last, result, (value_type)(first));
}
template <class InputIterator, class OutputIterator>
inline OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator result) {
	if (first == last) return result;
	return __unique_copy(first, last, result, (iterator_category)(result));
}
template <class InputIterator, class ForwardIterator, class BinaryPredicate>
ForwardIterator __unique_copy(InputIterator first, InputIterator last, ForwardIterator result,
	BinaryPredicate binary_pred, std::forward_iterator_tag) {
	*result = *first;
	while (++first != last)
		if (!binary_pred(*result, *first)) *++result = *first;
	return ++result;
}
template <class InputIterator, class OutputIterator, class BinaryPredicate, class T>
OutputIterator __unique_copy(InputIterator first, InputIterator last, OutputIterator result,
	BinaryPredicate binary_pred, T*) {
	T value = *first;
	*result = value;
	while (++first != last) {
		if (!binary_pred(value, *first)) {
			value = *first;
			*++result = value;
		}
	}
	return ++result;
}
template <class InputIterator, class OutputIterator, class BinaryPredicate>
inline OutputIterator __unique_copy(InputIterator first, InputIterator last, OutputIterator result,
	BinaryPredicate binary_pred, std::output_iterator_tag) {
	return __unique_copy(first, last, result, binary_pred, (value_type)(first));
}
template <class InputIterator, class OutputIterator, class BinaryPredicate>
inline OutputIterator unique_copy(InputIterator first, InputIterator last,
	OutputIterator result,
	BinaryPredicate binary_pred) {
	if (first == last) return result;
	return __unique_copy(first, last, result, binary_pred, (iterator_category)(result));
}
template <class ForwardIterator>
ForwardIterator unique(ForwardIterator first, ForwardIterator last) {
	first = adjacent_find(first, last);
	return unique_copy(first, last, first);
}
template <class ForwardIterator, class BinaryPredicate>
ForwardIterator unique(ForwardIterator first, ForwardIterator last,
	BinaryPredicate binary_pred) {
	first = adjacent_find(first, last, binary_pred);
	return unique_copy(first, last, first, binary_pred);
}

template <class ForwardIterator, class T, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last,
	const T& value, Distance*, std::forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (*middle < value) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else len = half;
	}
	return first;
}
template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last,
	const T& value,Distance*, std::random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (*middle < value) {
			first = middle + 1;
			len = len - half - 1;
		}
		else len = half;
	}
	return first;
}
template <class ForwardIterator, class T>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value) {
	return __lower_bound(first, last, value, (distance_type)(first), (iterator_category)(first));
}

template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last,
	const T& value, Compare comp, Distance*, std::forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (comp(*middle, value)) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else len = half;
	}
	return first;
}
template <class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last,
	const T& value, Compare comp, Distance*, std::random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (comp(*middle, value)) {
			first = middle + 1;
			len = len - half - 1;
		}
		else len = half;
	}
	return first;
}
template <class ForwardIterator, class T, class Compare>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, 
	const T& value, Compare comp) {
	return __lower_bound(first, last, value, comp, (distance_type)(first), (iterator_category)(first));
}

template <class ForwardIterator, class T, class Distance>
ForwardIterator __upper_bound(ForwardIterator first, ForwardIterator last, const T& value, 
	Distance*, std::forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first;
		(advance)(middle, half);
		if (value < *middle)
			len = half;
		else {
			first = middle;
			++first;
			len = len - half - 1;
		}
	}
	return first;
}
template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last, const T& value,
	Distance*, std::random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (value < *middle)
			len = half;
		else {
			first = middle + 1;
			len = len - half - 1;
		}
	}
	return first;
}
template <class ForwardIterator, class T>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value) {
	return __upper_bound(first, last, value, (distance_type)(first), (iterator_category)(first));
}

template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __upper_bound(ForwardIterator first, ForwardIterator last, const T& value, 
	Compare comp, Distance*, std::forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		(advance)(middle, half);
		if (comp(value, *middle))
			len = half;
		else {
			first = middle;
			++first;
			len = len - half - 1;
		}
	}
	return first;
}
template <class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last,
	const T& value, Compare comp, Distance*, std::random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (comp(value, *middle))
			len = half;
		else {
			first = middle + 1;
			len = len - half - 1;
		}
	}
	return first;
}
template <class ForwardIterator, class T, class Compare>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
	return __upper_bound(first, last, value, comp, (distance_type)(first), (iterator_category)(first));
}

template <class ForwardIterator, class T>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value) {
	ForwardIterator i = lower_bound(first, last, value);
	return i != last && !(value < *i);
}
template <class ForwardIterator, class T, class Compare>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
	ForwardIterator i = lower_bound(first, last, value, comp);
	return i != last && !comp(value, *i);
}

template <class BidirectionalIterator>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last) {
	if (first == last) return false;
	BidirectionalIterator i = first;
	++i;
	if (i == last) return false;
	i = last;
	--i;
	for (;;) {
		BidirectionalIterator ii = i;
		--i;
		if (*i < *ii) {
			BidirectionalIterator j = last;
			while (!(*i < *--j));
			iter_swap(i, j);
			reverse(ii, last);
			return true;
		}
		if (i == first) {
			reverse(first, last);
			return false;
		}
	}
}
template <class BidirectionalIterator, class Compare>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
	if (first == last) return false;
	BidirectionalIterator i = first;
	++i;
	if (i == last) return false;
	i = last;
	--i;
	for (;;) {
		BidirectionalIterator ii = i;
		--i;
		if (comp(*i, *ii)) {
			BidirectionalIterator j = last;
			while (!comp(*i, *--j));
			iter_swap(i, j);
			reverse(ii, last);
			return true;
		}
		if (i == first) {
			reverse(first, last);
			return false;
		}
	}
}

template <class BidirectionalIterator>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last) {
	if (first == last) return false;
	BidirectionalIterator i = first;
	++i;
	if (i == last) return false;
	i = last;
	--i;
	for (;;) {
		BidirectionalIterator ii = i;
		--i;
		if (*ii < *i) {
			BidirectionalIterator j = last;
			while (!(*--j < *i));
			(iter_swap)(i, j);
			(reverse)(ii, last);
			return true;
		}
		if (i == first) {
			(reverse)(first, last);
			return false;
		}
	}
}
template <class BidirectionalIterator, class Compare>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
	if (first == last) return false;
	BidirectionalIterator i = first;
	++i;
	if (i == last) return false;
	i = last;
	--i;
	for (;;) {
		BidirectionalIterator ii = i;
		--i;
		if (comp(*ii, *i)) {
			BidirectionalIterator j = last;
			while (!comp(*--j, *i));
			(iter_swap)(i, j);
			(reverse)(ii, last);
			return true;
		}
		if (i == first) {
			(reverse)(first, last);
			return false;
		}
	}
}

template <class RandomAccessIterator, class Distance>
void __random_shuffle(RandomAccessIterator first, RandomAccessIterator last, Distance*) {
	if (first == last) return;
	for (RandomAccessIterator i = first + 1; i != last; ++i)
		(iter_swap)(i, first + Distance(rand() % ((i - first) + 1)));
}
template <class RandomAccessIterator>
inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last) {
	__random_shuffle(first, last, (distance_type)(first));
}
template <class RandomAccessIterator, class RandomNumberGenerator>
void random_shuffle(RandomAccessIterator first, RandomAccessIterator last, RandomNumberGenerator& rand) {
	if (first == last) return;
	for (RandomAccessIterator i = first + 1; i != last; ++i)
		(iter_swap)(i, first + rand((i - first) + 1));
}

template <class ForwardIterator, class T, class Distance>
pair<ForwardIterator, ForwardIterator>
__equal_range(ForwardIterator first, ForwardIterator last, const T& value,
	Distance*, std::forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle, left, right;

	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (*middle < value) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else if (value < *middle)
			len = half;
		else {
			left = lower_bound(first, middle, value);
			advance(first, len);
			right = upper_bound(++middle, first, value);
			return pair<ForwardIterator, ForwardIterator>(left, right);
		}
	}
	return pair<ForwardIterator, ForwardIterator>(first, first);
}
template <class RandomAccessIterator, class T, class Distance>
pair<RandomAccessIterator, RandomAccessIterator>
__equal_range(RandomAccessIterator first, RandomAccessIterator last,
	const T& value, Distance*, std::random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle, left, right;

	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (*middle < value) {
			first = middle + 1;
			len = len - half - 1;
		}
		else if (value < *middle)
			len = half;
		else {
			left = lower_bound(first, middle, value);
			right = upper_bound(++middle, first + len, value);
			return pair<RandomAccessIterator, RandomAccessIterator>(left,
				right);
		}
	}
	return pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}

template <class ForwardIterator, class T>
inline pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value) {
	return __equal_range(first, last, value, distance_type(first),
		iterator_category(first));
}
template <class ForwardIterator, class T, class Compare, class Distance>
pair<ForwardIterator, ForwardIterator>
__equal_range(ForwardIterator first, ForwardIterator last, const T& value,
	Compare comp, Distance*, std::forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle, left, right;

	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (comp(*middle, value)) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else if (comp(value, *middle))
			len = half;
		else {
			left = lower_bound(first, middle, value, comp);
			advance(first, len);
			right = upper_bound(++middle, first, value, comp);
			return pair<ForwardIterator, ForwardIterator>(left, right);
		}
	}
	return pair<ForwardIterator, ForwardIterator>(first, first);
}
template <class RandomAccessIterator, class T, class Compare, class Distance>
pair<RandomAccessIterator, RandomAccessIterator>
__equal_range(RandomAccessIterator first, RandomAccessIterator last,
	const T& value, Compare comp, Distance*,
	std::random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle, left, right;

	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (comp(*middle, value)) {
			first = middle + 1;
			len = len - half - 1;
		}
		else if (comp(value, *middle))
			len = half;
		else {
			left = lower_bound(first, middle, value, comp);
			right = upper_bound(++middle, first + len, value, comp);
			return pair<RandomAccessIterator, RandomAccessIterator>(left,
				right);
		}
	}
	return pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}
template <class ForwardIterator, class T, class Compare>
inline pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value,
	Compare comp) {
	return __equal_range(first, last, value, comp, distance_type(first),
		iterator_category(first));
}

MSTL_END_NAMESPACE__

#endif // MSTL_ALGO_HPP__
