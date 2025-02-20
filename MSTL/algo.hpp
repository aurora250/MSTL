#ifndef MSTL_ALGO_HPP__
#define MSTL_ALGO_HPP__
#include "mathlib.h"
#include "memory.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Iterator, typename Predicate>
	requires(input_iterator<Iterator>)
bool all_of(Iterator first, Iterator last, Predicate pred) {
	for (; first != last; ++first) {
		if (!pred(*first))
			return false;
	}
	return true;
}

template <typename Iterator, typename Predicate>
	requires(input_iterator<Iterator>)
bool any_of(Iterator first, Iterator last, Predicate pred) {
	for (; first != last; ++first) {
		if (pred(*first))
			return true;
	}
	return false;
}

template <typename Iterator, typename Predicate>
	requires(input_iterator<Iterator>)
bool none_of(Iterator first, Iterator last, Predicate pred) {
	for (; first != last; ++first) {
		if (pred(*first))
			return false;
	}
	return true;
}

// union of set
template <typename Iterator1, typename Iterator2, typename Iterator3>
	requires(forward_iterator<Iterator1> && forward_iterator<Iterator2> && forward_iterator<Iterator3>)
Iterator3 set_union(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2, Iterator3 result) {
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
	return MSTL::copy(first2, last2, MSTL::copy(first1, last1, result));
}

// intersection of set
template <typename Iterator1, typename Iterator2, typename Iterator3>
	requires(forward_iterator<Iterator1> && forward_iterator<Iterator2> && forward_iterator<Iterator3>)
Iterator3 set_intersection(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Iterator2 last2, Iterator3 result) {
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

// difference of set
template <typename Iterator1, typename Iterator2, typename Iterator3>
	requires(forward_iterator<Iterator1> && forward_iterator<Iterator2> && forward_iterator<Iterator3>)
Iterator3 set_difference(Iterator1 first1, Iterator1 last1, 
	Iterator2 first2, Iterator2 last2, Iterator3 result) {
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
	return MSTL::copy(first1, last1, result);
}

// symmetric difference of set
template <typename Iterator1, typename Iterator2, typename Iterator3>
	requires(forward_iterator<Iterator1> && forward_iterator<Iterator2> && forward_iterator<Iterator3>)
Iterator3 set_symmetric_difference(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Iterator2 last2, Iterator3 result) {
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
	return MSTL::copy(first2, last2, MSTL::copy(first1, last1, result));
}

// adjacent_find
template <typename Iterator, enable_if_t<is_input_iter<Iterator>, int> = 0>
Iterator adjacent_find(Iterator first, Iterator last) {
	if (first == last) return last;
	Iterator next = first;
	while (++next != last) {
		if (*first == *next) return first;
		first = next;
	}
	return last;
}
template <typename Iterator, typename BinaryPredicate, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator adjacent_find(Iterator first, Iterator last, BinaryPredicate binary_pred) {
	if (first == last) return last;
	Iterator next = first;
	while (++next != last) {
		if (binary_pred(*first, *next)) return first;
		first = next;
	}
	return last;
}

// count
template <typename Iterator, typename T, enable_if_t<is_input_iter<Iterator>, int> = 0>
iter_dif_t<Iterator> count(Iterator first, Iterator last, const T& value) {
	iter_dif_t n = 0;
	for (; first != last; ++first) {
		if (*first == value) ++n;
	}
	return n;
}

template <typename Iterator, typename T, typename BinaryPredicate, enable_if_t<is_input_iter<Iterator>, int> = 0>
iter_dif_t<Iterator> count_if(Iterator first, Iterator last, const T& value, BinaryPredicate pred) {
	iter_dif_t<Iterator> n = 0;
	for (; first != last; ++first) {
		if (pred(*first, value)) ++n;
	}
	return n;
}

// find
template <typename Iterator, typename T, enable_if_t<is_input_iter<Iterator>, int> = 0>
Iterator find(Iterator first, Iterator last, const T& value) {
	while (first != last && *first != value) ++first;
	return first;
}

template <typename Iterator, typename T, enable_if_t<is_input_iter<Iterator>, int> = 0>
Iterator find_if(Iterator first, Iterator last, const T& value) {
	while (first != last && *first != value) ++first;
	return first;
}

template <typename Iterator, typename Predicate, enable_if_t<is_input_iter<Iterator>, int> = 0>
Iterator find_if(Iterator first, Iterator last, Predicate pred) {
	while (first != last && !pred(*first)) ++first;
	return first;
}

template <typename Iterator, typename Predicate, enable_if_t<is_input_iter<Iterator>, int> = 0>
Iterator find_if_not(Iterator first, Iterator last, Predicate pred) {
	while (first != last && pred(*first))
		++first;
	return first;
}

// search
template <typename Iterator1, typename Iterator2>
	requires(forward_iterator<Iterator1> && forward_iterator<Iterator2>)
Iterator1 search(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	iter_dif_t<Iterator1> d1 = MSTL::distance(first1, last1);
	iter_dif_t<Iterator2> d2 = MSTL::distance(first2, last2);
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
	requires(forward_iterator<Iterator1> && forward_iterator<Iterator2>)
Iterator1 search(Iterator1 first1, Iterator1 last1, Iterator2 first2,
	Iterator2 last2, BinaryPredicate binary_pred) {
	iter_dif_t<Iterator1> d1 = MSTL::distance(first1, last1);
	iter_dif_t<Iterator2> d2 = MSTL::distance(first2, last2);
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

template <typename Iterator, typename T, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator search_n(Iterator first, Iterator last, size_t count, const T& value) {
	first = MSTL::find(first, last, value);
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
			first = MSTL::find(i, last, value);
	}
	return last;
}

template <typename Iterator, typename T, typename BinaryPredicate, enable_if_t<is_input_iter<Iterator>, int> = 0>
Iterator search_n(Iterator first, Iterator last, size_t count,
	const T& value, BinaryPredicate binary_pred) {
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

// find place
template <typename Iterator1, typename Iterator2>
	requires(forward_iterator<Iterator1>&& forward_iterator<Iterator2>)
Iterator1 find_end(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	if (first2 == last2) return last1;
	Iterator1 result = last1;
	while (true) {
		Iterator1 new_result = MSTL::search(first1, last1, first2, last2);
		if (new_result == last1) return result;
		result = new_result;
		first1 = new_result;
		++first1;
	}
}

template <typename Iterator1, typename Iterator2>
	requires(bidirectional_iterator<Iterator1>&& bidirectional_iterator<Iterator2>)
Iterator1 find_end(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	using reviter1 = typename MSTL::reverse_iterator<Iterator1>;
	using reviter2 = typename MSTL::reverse_iterator<Iterator2>;
	reviter1 rlast1(first1);
	reviter2 rlast2(first2);
	reviter1 rresult = MSTL::search(reviter1(last1), rlast1, reviter2(last2), rlast2);
	if (rresult == rlast1) return last1;
	Iterator1 result = rresult.base();
	MSTL::advance(result, -distance(first2, last2));
	return result;
}

template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1>&& forward_iterator<Iterator2>)
Iterator1 find_first_of(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	for (; first1 != last1; ++first1)
		for (Iterator2 iter = first2; iter != last2; ++iter)
			if (*first1 == *iter) return first1;
	return last1;
}

template <typename Iterator1, typename Iterator2, typename BinaryPredicate>
	requires(input_iterator<Iterator1>&& forward_iterator<Iterator2>)
Iterator1 find_first_of(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Iterator2 last2, BinaryPredicate comp) {
	for (; first1 != last1; ++first1)
		for (Iterator2 iter = first2; iter != last2; ++iter)
			if (comp(*first1, *iter)) return first1;
	return last1;
}

// for each
template <typename Iterator, typename Function, enable_if_t<is_input_iter<Iterator>, int> = 0>
Function for_each(Iterator first, Iterator last, Function f) {
	for (; first != last; ++first) f(*first);
	return f;
}

template<typename Iterator, typename Function, enable_if_t<is_input_iter<Iterator>, int> = 0>
Iterator for_each_n(Iterator first, size_t n, Function f) {
	for (size_t i = 0; i < n; i++) {
		f(*first);
		++first;
	}
	return first;
}

// generate
template <typename Iterator, typename Generator, enable_if_t<is_input_iter<Iterator>, int> = 0>
void generate(Iterator first, Iterator last, Generator gen) {
	for (; first != last; ++first)
		*first = gen();
}

template <typename Iterator, typename Generator, enable_if_t<is_input_iter<Iterator>, int> = 0>
Iterator generate_n(Iterator first, size_t n, Generator gen) {
	for (; n > 0; --n, ++first)
		*first = gen();
	return first;
}

// includes
template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
bool includes(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	while (first1 != last1 && first2 != last2)
		if (*first2 < *first1)
			return false;
		else if (*first1 < *first2)
			++first1;
		else
			++first1, ++first2;
	return first2 == last2;
}

template <typename Iterator1, typename Iterator2, typename Compare>
	requires(input_iterator<Iterator1>&& input_iterator<Iterator2>)
bool includes(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2, Compare comp) {
	while (first1 != last1 && first2 != last2)
		if (comp(*first2, *first1))
			return false;
		else if (comp(*first1, *first2))
			++first1;
		else
			++first1, ++first2;
	return first2 == last2;
}

// merge
template <typename Iterator1, typename Iterator2, typename Iterator3>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2> && input_iterator<Iterator3>)
Iterator3 merge(Iterator1 first1, Iterator1 last1, Iterator2 first2,
	Iterator2 last2, Iterator3 result) {
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
	return MSTL::copy(first2, last2, MSTL::copy(first1, last1, result));
}

template <typename Iterator1, typename Iterator2, typename Iterator3, typename Compare>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>&& input_iterator<Iterator3>)
Iterator3 merge(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2,
	Iterator3 result, Compare comp) {
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
	return MSTL::copy(first2, last2, MSTL::copy(first1, last1, result));
}

// unstable partition
template <typename Iterator, typename Predicate, enable_if_t<is_bid_iter<Iterator>, int> = 0>
Iterator partition(Iterator first, Iterator last, Predicate pred) {
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
		MSTL::iter_swap(first, last);
		++first;
	}
}

// remove
template <typename Iterator1, typename Iterator2, typename T>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
Iterator2 remove_copy(Iterator1 first, Iterator1 last, Iterator2 result, const T& value) {
	for (; first != last; ++first)
		if (*first != value) {
			*result = *first;
			++result;
		}
	return result;
}

template <typename Iterator1, typename Iterator2, typename Predicate>
	requires(input_iterator<Iterator1>&& input_iterator<Iterator2>)
Iterator2 remove_copy_if(Iterator1 first, Iterator1 last, Iterator2 result, Predicate pred) {
	for (; first != last; ++first)
		if (!pred(*first)) {
			*result = *first;
			++result;
		}
	return result;
}

template <typename Iterator, typename T, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator remove(Iterator first, Iterator last, const T& value) {
	first = MSTL::find(first, last, value);
	Iterator next = first;
	return first == last ? first : MSTL::remove_copy(++next, last, first, value);
}

template <typename Iterator, typename Predicate, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator remove_if(Iterator first, Iterator last, Predicate pred) {
	first = MSTL::find_if(first, last, pred);
	Iterator next = first;
	return first == last ? first : MSTL::remove_copy_if(++next, last, first, pred);
}

// replace
template <typename Iterator1, typename Iterator2, typename T>
	requires(input_iterator<Iterator1>&& input_iterator<Iterator2>)
Iterator2 replace_copy(Iterator1 first, Iterator1 last, Iterator2 result,
	const T& old_value, const T& new_value) {
	for (; first != last; ++first, ++result)
		*result = (*first == old_value ? new_value : *first);
	return result;
}

template <typename Iterator1, typename Iterator2, typename Predicate, typename T>
	requires(input_iterator<Iterator1>&& input_iterator<Iterator2>)
Iterator2 replace_copy_if(Iterator1 first, Iterator1 last,
	Iterator2 result, Predicate pred, const T& new_value) {
	for (; first != last; ++first, ++result)
		*result = pred(*first) ? new_value : *first;
	return result;
}

template <typename Iterator, typename T, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
void replace(Iterator first, Iterator last, const T& old_value, const T& new_value) {
	for (; first != last; ++first)
		if (*first == old_value) *first = new_value;
}

template <typename Iterator, typename Predicate, typename T, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
void replace_if(Iterator first, Iterator last, Predicate pred, const T& new_value) {
	for (; first != last; ++first)
		if (pred(*first)) *first = new_value;
}

// reserve
template <typename Iterator, enable_if_t<is_bid_iter<Iterator>, int> = 0>
void reverse(Iterator first, Iterator last) {
	while (true)
		if (first == last || first == --last) return;
		else {
			--last;
			MSTL::iter_swap(first, last);
			++first;
		}
}

template <typename Iterator, enable_if_t<is_rnd_iter<Iterator>, int> = 0>
void reverse(Iterator first, Iterator last) {
	while (first < last) {
		--last;
		MSTL::iter_swap(first, last);
		++first;
	}
}

// rotate
template <typename Iterator, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
void rotate(Iterator first, Iterator middle, Iterator last) {
	if (first == middle || middle == last) return;
	for (Iterator i = middle; ;) {
		MSTL::iter_swap(first, i);
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

template <typename Iterator, enable_if_t<is_bid_iter<Iterator>, int> = 0>
void rotate(Iterator first, Iterator middle, Iterator last) {
	if (first == middle || middle == last) return;
	MSTL::reverse(first, middle);
	MSTL::reverse(middle, last);
	MSTL::reverse(first, last);
}

template <typename Iterator, typename Distance, enable_if_t<is_rnd_iter<Iterator>, int> = 0>
void rotate_cycle_aux(Iterator first, Iterator last, Iterator initial, Distance shift) {
	iter_val_t<Iterator> value = *initial;
	Iterator ptr1 = initial;
	Iterator ptr2 = ptr1 + shift;
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

template <typename Iterator, typename Distance, enable_if_t<is_rnd_iter<Iterator>, int> = 0>
void rotate(Iterator first, Iterator middle, Iterator last) {
	if (first == middle || middle == last) return;
	iter_dif_t<Iterator> n = MSTL::gcd(last - first, middle - first);
	while (n--)
		(rotate_cycle_aux)(first, last, first + n, middle - first);
}

template <typename Iterator1, typename Iterator2>
	requires(forward_iterator<Iterator1> && forward_iterator<Iterator2>)
Iterator2 rotate_copy(Iterator1 first, Iterator1 middle, Iterator1 last, Iterator2 result) {
	return MSTL::copy(first, middle, MSTL::copy(middle, last, result));
}

// swap ranges
template <typename Iterator1, typename Iterator2>
	requires(forward_iterator<Iterator1>&& forward_iterator<Iterator2>)
Iterator2 swap_ranges(Iterator1 first1, Iterator1 last1, Iterator2 first2) {
	for (; first1 != last1; ++first1, ++first2)
		MSTL::iter_swap(first1, first2);
	return first2;
}

// transform
template <typename Iterator1, typename Iterator2, typename UnaryOperation>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
Iterator2 transform(Iterator1 first, Iterator1 last, Iterator2 result, UnaryOperation op) {
	for (; first != last; ++first, ++result)
		*result = op(*first);
	return result;
}

template <typename Iterator1, typename Iterator2, typename Iterator3, typename BinaryOperation>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2> && input_iterator<Iterator3>)
Iterator3 transform(Iterator1 first1, Iterator1 last1, Iterator2 first2,
	Iterator3 result, BinaryOperation binary_op) {
	for (; first1 != last1; ++first1, ++first2, ++result)
		*result = binary_op(*first1, *first2);
	return result;
}

// unique always stable
template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> && forward_iterator<Iterator2>)
Iterator2 unique_copy(Iterator1 first, Iterator1 last, Iterator2 result) {
	if (first == last) return result;
	*result = *first;
	while (++first != last)
		if (*result != *first) *++result = *first;
	return ++result;
}

template <typename Iterator1, typename Iterator2, typename BinaryPredicate>
	requires(input_iterator<Iterator1>&& forward_iterator<Iterator2>)
Iterator2 unique_copy(Iterator1 first, Iterator1 last, 
	Iterator2 result, BinaryPredicate binary_pred) {
	if (first == last) return result;
	*result = *first;
	while (++first != last)
		if (!binary_pred(*result, *first)) *++result = *first;
	return ++result;
}

template <typename Iterator, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator unique(Iterator first, Iterator last) {
	first = MSTL::adjacent_find(first, last);
	return MSTL::unique_copy(first, last, first);
}

template <typename Iterator, typename BinaryPredicate, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator unique(Iterator first, Iterator last, BinaryPredicate binary_pred) {
	first = MSTL::adjacent_find(first, last, binary_pred);
	return MSTL::unique_copy(first, last, first, binary_pred);
}

// lower bound
template <typename Iterator, typename T, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator lower_bound(Iterator first, Iterator last, const T& value) noexcept {
	using Distance = iter_dif_t<Iterator>;
	Distance len = MSTL::distance(first, last);
	Distance half;
	Iterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		MSTL::advance(middle, half);
		if (*middle < value) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else len = half;
	}
	return first;
}

template <typename Iterator, typename T, enable_if_t<is_rnd_iter<Iterator>, int> = 0>
Iterator lower_bound(Iterator first, Iterator last, const T& value) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = last - first;
	Distance half;
	Iterator middle;
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

template <typename Iterator, typename T, typename Compare, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator lower_bound(Iterator first, Iterator last, const T& value, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = MSTL::distance(first, last);
	Distance half;
	Iterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		MSTL::advance(middle, half);
		if (comp(*middle, value)) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else len = half;
	}
	return first;
}

template <typename Iterator, typename T, typename Compare, enable_if_t<is_rnd_iter<Iterator>, int> = 0>
Iterator lower_bound(Iterator first, Iterator last, const T& value, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = last - first;
	Distance half;
	Iterator middle;
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

// upper bound
template <typename Iterator, typename T, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator upper_bound(Iterator first, Iterator last, const T& value) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = MSTL::distance(first, last);
	Distance half;
	Iterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		MSTL::advance(middle, half);
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

template <typename Iterator, typename T, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator upper_bound(Iterator first, Iterator last, const T& value) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = last - first;
	Distance half;
	Iterator middle;
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

template <typename Iterator, typename T, typename Compare, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
Iterator upper_bound(Iterator first, Iterator last, const T& value, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = distance(first, last);
	Distance half;
	Iterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		MSTL::advance(middle, half);
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

template <typename Iterator, typename T, typename Compare, enable_if_t<is_rnd_iter<Iterator>, int> = 0>
Iterator upper_bound(Iterator first, Iterator last, const T& value, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = last - first;
	Distance half;
	Iterator middle;
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

// binary search
template <typename Iterator, typename T, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
bool binary_search(Iterator first, Iterator last, const T& value) {
	Iterator i = MSTL::lower_bound(first, last, value);
	return i != last && !(value < *i);
}

template <typename Iterator, typename T, typename Compare, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
bool binary_search(Iterator first, Iterator last, const T& value, Compare comp) {
	Iterator i = MSTL::lower_bound(first, last, value, comp);
	return i != last && !comp(value, *i);
}

// is permutation
template <class Iterator1, class Iterator2, class BinaryPred>
	requires(bidirectional_iterator<Iterator1>&& bidirectional_iterator<Iterator2>)
bool is_permutation(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	iter_dif_t<Iterator1> len1 = MSTL::distance(first1, last1);
	iter_dif_t<Iterator2> len2 = MSTL::distance(first2, last2);
	if (len1 != len2) return false;

	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (!(*first1 == *first2)) break;
	}
	if (first1 == last1) return true;

	for (Iterator1 i = first1; i != last1; ++i) {
		bool is_repeate = false;
		for (Iterator1 j = first1; j != i; ++j) {
			if (*i == *j) {
				is_repeate = true;
				break;
			}
		}

		if (!is_repeate) {
			uint32_t c2 = 0;
			for (Iterator2 j = first2; j != last2; ++j) {
				if (*i == *j)
					++c2;
			}
			if (c2 == 0) return false;

			uint32_t c1 = 1;
			Iterator1 j = i;
			for (++j; j != last1; ++j) {
				if (*i == *j)
					++c1;
			}
			if (c1 != c2) return false;
		}
	}
	return true;
}

template <class Iterator1, class Iterator2, class BinaryPred>
	requires(bidirectional_iterator<Iterator1> && bidirectional_iterator<Iterator2>)
bool is_permutation(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2, BinaryPred pred) {
	iter_dif_t<Iterator1> len1 = MSTL::distance(first1, last1);
	iter_dif_t<Iterator2> len2 = MSTL::distance(first2, last2);
	if (len1 != len2)
		return false;
	for (; first1 != last1 && first2 != last2; ++first1, (void) ++first2) {
		if (!pred(*first1, *first2))
			break;
	}
	if (first1 == last1)
		return true;
	for (Iterator1 i = first1; i != last1; ++i) {
		bool is_repeated = false;
		for (Iterator1 j = first1; j != i; ++j) {
			if (pred(*j, *i)) {
				is_repeated = true;
				break;
			}
		}

		if (!is_repeated) {
			size_t c2 = 0;
			for (Iterator2 j = first2; j != last2; ++j) {
				if (pred(*i, *j))
					++c2;
			}
			if (c2 == 0)
				return false;
			size_t c1 = 1;
			Iterator1 j = i;
			for (++j; j != last1; ++j) {
				if (pred(*i, *j))
					++c1;
			}
			if (c1 != c2)
				return false;
		}
	}
	return true;
}

// next permutation
template <typename Iterator, enable_if_t<is_bid_iter<Iterator>, int> = 0>
bool next_permutation(Iterator first, Iterator last) {
	if (first == last) return false;
	Iterator i = first;
	++i;
	if (i == last) return false;
	i = last;
	--i;
	for (;;) {
		Iterator ii = i;
		--i;
		if (*i < *ii) {
			Iterator j = last;
			while (!(*i < *--j));
			MSTL::iter_swap(i, j);
			MSTL::reverse(ii, last);
			return true;
		}
		if (i == first) {
			MSTL::reverse(first, last);
			return false;
		}
	}
}

template <typename Iterator, typename Compare, enable_if_t<is_bid_iter<Iterator>, int> = 0>
bool next_permutation(Iterator first, Iterator last, Compare comp) {
	if (first == last) return false;
	Iterator i = first;
	++i;
	if (i == last) return false;
	i = last;
	--i;
	for (;;) {
		Iterator ii = i;
		--i;
		if (comp(*i, *ii)) {
			Iterator j = last;
			while (!comp(*i, *--j));
			MSTL::iter_swap(i, j);
			MSTL::reverse(ii, last);
			return true;
		}
		if (i == first) {
			MSTL::reverse(first, last);
			return false;
		}
	}
}

// prev permutation
template <typename Iterator, enable_if_t<is_bid_iter<Iterator>, int> = 0>
bool prev_permutation(Iterator first, Iterator last) {
	if (first == last) return false;
	Iterator i = first;
	++i;
	if (i == last) return false;
	i = last;
	--i;
	for (;;) {
		Iterator ii = i;
		--i;
		if (*ii < *i) {
			Iterator j = last;
			while (!(*--j < *i));
			MSTL::iter_swap(i, j);
			MSTL::reverse(ii, last);
			return true;
		}
		if (i == first) {
			MSTL::reverse(first, last);
			return false;
		}
	}
}

template <typename Iterator, typename Compare, enable_if_t<is_bid_iter<Iterator>, int> = 0>
bool prev_permutation(Iterator first, Iterator last, Compare comp) {
	if (first == last) return false;
	Iterator i = first;
	++i;
	if (i == last) return false;
	i = last;
	--i;
	for (;;) {
		Iterator ii = i;
		--i;
		if (comp(*ii, *i)) {
			Iterator j = last;
			while (!comp(*--j, *i));
			MSTL::iter_swap(i, j);
			MSTL::reverse(ii, last);
			return true;
		}
		if (i == first) {
			MSTL::reverse(first, last);
			return false;
		}
	}
}

// shuffle
template <typename Iterator, enable_if_t<is_rnd_iter<Iterator>, int> = 0>
void shuffle(Iterator first, Iterator last) {
	if (first == last) return;
	for (Iterator i = MSTL::next(first); i != last; ++i) {
		Iterator j = MSTL::next(first, std::rand() % ((i - first) + 1));
		MSTL::iter_swap(i, j);
	}
}

template <typename Iterator, typename Generator, enable_if_t<is_rnd_iter<Iterator>, int> = 0>
void shuffle(Iterator first, Iterator last, Generator& rand) {
	if (first == last) return;
	for (Iterator i = MSTL::next(first); i != last; ++i) {
		Iterator j = MSTL::next(first, rand((i - first) + 1));
		MSTL::iter_swap(i, j);
	}
}

// equal range
template <typename Iterator, typename T, enable_if_t<is_fwd_iter<Iterator>, int> = 0>
pair<Iterator, Iterator> equal_range(Iterator first, Iterator last, const T& value) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = distance(first, last);
	Distance half;
	Iterator middle, left, right;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		MSTL::advance(middle, half);
		if (*middle < value) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else if (value < *middle)
			len = half;
		else {
			left = MSTL::lower_bound(first, middle, value);
			MSTL::advance(first, len);
			right = MSTL::upper_bound(++middle, first, value);
			return pair<Iterator, Iterator>(left, right);
		}
	}
	return pair<Iterator, Iterator>(first, first);
}

template <typename Iterator, typename T, typename Distance>
	requires(random_access_iterator<Iterator>)
pair<Iterator, Iterator> equal_range(Iterator first, Iterator last, const T& value) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = last - first;
	Distance half;
	Iterator middle, left, right;
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
			left = MSTL::lower_bound(first, middle, value);
			right = MSTL::upper_bound(++middle, first + len, value);
			return pair<Iterator, Iterator>(left, right);
		}
	}
	return pair<Iterator, Iterator>(first, first);
}

template <typename Iterator, typename T, typename Compare>
	requires(forward_iterator<Iterator>)
pair<Iterator, Iterator> equal_range(Iterator first, Iterator last, const T& value, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = distance(first, last);
	Distance half;
	Iterator middle, left, right;
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
			left = MSTL::lower_bound(first, middle, value, comp);
			MSTL::advance(first, len);
			right = MSTL::upper_bound(++middle, first, value, comp);
			return pair<Iterator, Iterator>(left, right);
		}
	}
	return pair<Iterator, Iterator>(first, first);
}

template <typename Iterator, typename T, typename Compare>
	requires(random_access_iterator<Iterator>)
pair<Iterator, Iterator> equal_range(Iterator first, Iterator last, const T& value, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = last - first;
	Distance half;
	Iterator middle, left, right;
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
			left = MSTL::lower_bound(first, middle, value, comp);
			right = MSTL::upper_bound(++middle, first + len, value, comp);
			return pair<Iterator, Iterator>(left, right);
		}
	}
	return pair<Iterator, Iterator>(first, first);
}

// inplace merge always stable
template <class Iterator, class Distance>
	requires(bidirectional_iterator<Iterator>)
void merge_without_buffer_aux(Iterator first, Iterator middle,
	Iterator last, Distance len1, Distance len2) {
	if (len1 == 0 || len2 == 0) return;
	if (len1 + len2 == 2) {
		if (*middle < *first) MSTL::iter_swap(first, middle);
		return;
	}
	Iterator first_cut = first;
	Iterator second_cut = middle;
	Distance len11 = 0;
	Distance len22 = 0;
	if (len1 > len2) {
		len11 = len1 / 2;
		MSTL::advance(first_cut, len11);
		second_cut = MSTL::lower_bound(middle, last, *first_cut);
		len22 = MSTL::distance(middle, second_cut);
	}
	else {
		len22 = len2 / 2;
		MSTL::advance(second_cut, len22);
		first_cut = MSTL::upper_bound(first, middle, *second_cut);
		len11 = MSTL::distance(first, first_cut);
	}
	MSTL::rotate(first_cut, middle, second_cut);
	Iterator new_middle = first_cut;
	MSTL::advance(new_middle, len22);
	MSTL::merge_without_buffer_aux(first, first_cut, new_middle, len11, len22);
	MSTL::merge_without_buffer_aux(new_middle, second_cut, last, len1 - len11, len2 - len22);
}

template <class Iterator, class Distance, class Compare>
	requires(bidirectional_iterator<Iterator>)
void merge_without_buffer_aux(Iterator first, Iterator middle, Iterator last,
	Distance len1, Distance len2, Compare comp) {
	if (len1 == 0 || len2 == 0) return;
	if (len1 + len2 == 2) {
		if (comp(*middle, *first)) MSTL::iter_swap(first, middle);
		return;
	}
	Iterator first_cut = first;
	Iterator second_cut = middle;
	Distance len11 = 0;
	Distance len22 = 0;
	if (len1 > len2) {
		len11 = len1 / 2;
		MSTL::advance(first_cut, len11);
		second_cut = MSTL::lower_bound(middle, last, *first_cut, comp);
		len22 = MSTL::distance(middle, second_cut);
	}
	else {
		len22 = len2 / 2;
		MSTL::advance(second_cut, len22);
		first_cut = MSTL::upper_bound(first, middle, *second_cut, comp);
		len11 = MSTL::distance(first, first_cut);
	}
	MSTL::rotate(first_cut, middle, second_cut);
	Iterator new_middle = first_cut;
	MSTL::advance(new_middle, len22);
	MSTL::merge_without_buffer_aux(first, first_cut, new_middle, len11, len22, comp);
	MSTL::merge_without_buffer_aux(new_middle, second_cut, last, len1 - len11, len2 - len22, comp);
}

template <class Iterator1, class Iterator2, class Distance>
	requires(bidirectional_iterator<Iterator1> && bidirectional_iterator<Iterator2>)
Iterator1 rotate_with_buffer_aux(Iterator1 first, Iterator1 middle, Iterator1 last,
	Distance len1, Distance len2, Iterator2 buffer, Distance buffer_size) {
	Iterator2 buffer_end;
	if (len1 > len2 && len2 <= buffer_size) {
		buffer_end = MSTL::copy(middle, last, buffer);
		MSTL::copy_backward(first, middle, last);
		return MSTL::copy(buffer, buffer_end, first);
	}
	else if (len1 <= buffer_size) {
		buffer_end = MSTL::copy(first, middle, buffer);
		MSTL::copy(middle, last, first);
		return MSTL::copy_backward(buffer, buffer_end, last);
	}
	else {
		MSTL::rotate(first, middle, last);
		MSTL::advance(first, len2);
		return first;
	}
}

template <class Iterator, class Distance, class Pointer>
	requires(bidirectional_iterator<Iterator>)
void merge_with_buffer_aux(Iterator first, Iterator middle, Iterator last,
	Distance len1, Distance len2, Pointer buffer, Distance buffer_size) {
	if (len1 <= len2 && len1 <= buffer_size) {
		Pointer end_buffer = MSTL::copy(first, middle, buffer);
		MSTL::merge(buffer, end_buffer, middle, last, first);
	}
	else if (len2 <= buffer_size) {
		Pointer end_buffer = MSTL::copy(middle, last, buffer);
		if (first == middle) {
			MSTL::copy_backward(buffer, end_buffer, last);
			return;
		}
		if (buffer == end_buffer) {
			MSTL::copy_backward(first, middle, last);
			return;
		}
		--middle;
		--end_buffer;
		while (true) {
			if (*end_buffer < *middle) {
				*--last = *middle;
				if (first == middle) {
					MSTL::copy_backward(buffer, ++end_buffer, last);
					return;
				}
				--middle;
			}
			else {
				*--last = *end_buffer;
				if (buffer == end_buffer) {
					MSTL::copy_backward(first, ++middle, last);
					return;
				}
				--end_buffer;
			}
		}
	}
	else {
		Iterator first_cut = first;
		Iterator second_cut = middle;
		Distance len11 = 0;
		Distance len22 = 0;
		if (len1 > len2) {
			len11 = len1 / 2;
			MSTL::advance(first_cut, len11);
			second_cut = MSTL::lower_bound(middle, last, *first_cut);
			len22 = MSTL::distance(middle, second_cut);
		}
		else {
			len22 = len2 / 2;
			MSTL::advance(second_cut, len22);
			first_cut = MSTL::upper_bound(first, middle, *second_cut);
			len11 = MSTL::distance(first, first_cut);
		}
		Iterator new_middle = MSTL::rotate_with_buffer_aux(
			first_cut, middle, second_cut, len1 - len11, len22, buffer, buffer_size);

		MSTL::merge_with_buffer_aux(
			first, first_cut, new_middle, len11, len22, buffer, buffer_size);
		MSTL::merge_with_buffer_aux(
			new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, buffer_size);
	}
}

template <class Iterator, class Distance, class Pointer, class Compare>
	requires(bidirectional_iterator<Iterator>)
void merge_with_buffer_aux(Iterator first, Iterator middle, Iterator last,
	Distance len1, Distance len2, Pointer buffer, Distance buffer_size, Compare comp) {
	if (len1 <= len2 && len1 <= buffer_size) {
		Pointer end_buffer = MSTL::copy(first, middle, buffer);
		MSTL::merge(buffer, end_buffer, middle, last, first, comp);
	}
	else if (len2 <= buffer_size) {
		Pointer end_buffer = MSTL::copy(middle, last, buffer);
		if (first == middle) {
			MSTL::copy_backward(buffer, end_buffer, last);
			return;
		}
		if (buffer == end_buffer) {
			MSTL::copy_backward(first, middle, last);
			return;
		}
		--middle;
		--end_buffer;
		while (true) {
			if (comp(*end_buffer, *middle)) {
				*--last = *middle;
				if (first == middle) {
					MSTL::copy_backward(buffer, ++end_buffer, last);
					return;
				}
				--middle;
			}
			else {
				*--last = *end_buffer;
				if (buffer == end_buffer) {
					MSTL::copy_backward(first, ++middle, last);
					return;
				}
				--end_buffer;
			}
		}
	}
	else {
		Iterator first_cut = first;
		Iterator second_cut = middle;
		Distance len11 = 0;
		Distance len22 = 0;
		if (len1 > len2) {
			len11 = len1 / 2;
			MSTL::advance(first_cut, len11);
			second_cut = MSTL::lower_bound(middle, last, *first_cut, comp);
			len22 = MSTL::distance(middle, second_cut);
		}
		else {
			len22 = len2 / 2;
			MSTL::advance(second_cut, len22);
			first_cut = MSTL::upper_bound(first, middle, *second_cut, comp);
			len11 = MSTL::distance(first, first_cut);
		}
		Iterator new_middle = MSTL::rotate_with_buffer_aux(
			first_cut, middle, second_cut, len1 - len11, len22, buffer, buffer_size);

		MSTL::merge_with_buffer_aux(
			first, first_cut, new_middle, len11, len22, buffer, buffer_size, comp);
		MSTL::merge_with_buffer_aux(
			new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, buffer_size, comp);
	}
}

template <class Iterator>
	requires(bidirectional_iterator<Iterator>)
void inplace_merge(Iterator first, Iterator middle, Iterator last) {
	if (first == middle || middle == last) return;
	using Distance = iter_dif_t<Iterator>;
	Distance len1 = MSTL::distance(first, middle);
	Distance len2 = MSTL::distance(middle, last);
	temporary_buffer<Iterator> buffer(first, last);
	if (buffer.begin() == 0)
		MSTL::merge_without_buffer_aux(first, middle, last, len1, len2);
	else
		MSTL::merge_with_buffer_aux(
			first, middle, last, len1, len2, buffer.begin(), Distance(buffer.size()));
}

template <class Iterator, class Compare>
	requires(bidirectional_iterator<Iterator>)
void inplace_merge(Iterator first, Iterator middle, Iterator last, Compare comp) {
	if (first == middle || middle == last) return;
	using Distance = iter_dif_t<Iterator>;
	Distance len1 = MSTL::distance(first, middle);
	Distance len2 = MSTL::distance(middle, last);
	temporary_buffer<Iterator> buffer(first, last);
	if (buffer.begin() == 0)
		MSTL::merge_without_buffer_aux(first, middle, last, len1, len2, comp);
	else
		MSTL::merge_with_buffer_aux(
			first, middle, last, len1, len2, buffer.begin(), Distance(buffer.size()), comp);
}

MSTL_END_NAMESPACE__

#endif // MSTL_ALGO_HPP__
