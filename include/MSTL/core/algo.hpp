#ifndef MSTL_ALGO_HPP__
#define MSTL_ALGO_HPP__
#include "mathlib.hpp"
#include "memory.hpp"
#include "heap.hpp"
#include "random.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Iterator, typename Predicate, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 bool all_of(Iterator first, Iterator last, Predicate pred) {
	for (; first != last; ++first) {
		if (!pred(*first))
			return false;
	}
	return true;
}

template <typename Iterator, typename Predicate, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 bool any_of(Iterator first, Iterator last, Predicate pred) {
	for (; first != last; ++first) {
		if (pred(*first))
			return true;
	}
	return false;
}

template <typename Iterator, typename Predicate, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 bool none_of(Iterator first, Iterator last, Predicate pred) {
	for (; first != last; ++first) {
		if (pred(*first))
			return false;
	}
	return true;
}

template <typename Iterator1, typename Iterator2, typename Iterator3,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2> && is_ranges_fwd_iter_v<Iterator3>, int> = 0>
MSTL_CONSTEXPR20 Iterator3 set_union(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2, Iterator3 result) {
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
	return _MSTL copy(first2, last2, _MSTL copy(first1, last1, result));
}

template <typename Iterator1, typename Iterator2, typename Iterator3,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2> && is_ranges_fwd_iter_v<Iterator3>, int> = 0>
MSTL_CONSTEXPR20 Iterator3 set_intersection(Iterator1 first1, Iterator1 last1,
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

template <typename Iterator1, typename Iterator2, typename Iterator3,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2> && is_ranges_fwd_iter_v<Iterator3>, int> = 0>
MSTL_CONSTEXPR20 Iterator3 set_difference(Iterator1 first1, Iterator1 last1,
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
	return _MSTL copy(first1, last1, result);
}

template <typename Iterator1, typename Iterator2, typename Iterator3,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2> && is_ranges_fwd_iter_v<Iterator3>, int> = 0>
MSTL_CONSTEXPR20 Iterator3 set_symmetric_difference(Iterator1 first1, Iterator1 last1,
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
	return _MSTL copy(first2, last2, _MSTL copy(first1, last1, result));
}

template <typename Iterator, typename BinaryPredicate, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator adjacent_find(Iterator first, Iterator last, BinaryPredicate binary_pred) {
	if (first == last) return last;
	Iterator next = first;
	while (++next != last) {
		if (binary_pred(*first, *next)) return first;
		first = next;
	}
	return last;
}

template <typename Iterator, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator adjacent_find(Iterator first, Iterator last) {
	return _MSTL adjacent_find(first, last, _MSTL equal_to<iter_val_t<Iterator>>());
}

template <typename Iterator, typename T, typename BinaryPredicate,
	enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 iter_dif_t<Iterator> count_if(Iterator first, Iterator last, const T& value, BinaryPredicate pred) {
	iter_dif_t<Iterator> n = 0;
	for (; first != last; ++first) {
		if (pred(*first, value)) ++n;
	}
	return n;
}

template <typename Iterator, typename T>
MSTL_CONSTEXPR20 iter_dif_t<Iterator> count(Iterator first, Iterator last, const T& value) {
	return _MSTL count_if(first, last, value, _MSTL equal_to<iter_val_t<Iterator>>());
}

template <typename Iterator, typename T, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator find_if(Iterator first, Iterator last, const T& value) {
	while (first != last && *first != value) ++first;
	return first;
}

template <typename Iterator, typename T>
MSTL_NODISCARD MSTL_CONSTEXPR20 Iterator find(Iterator first, Iterator last, const T& value) {
	return _MSTL find_if(first, last, value);
}

template <typename Iterator, typename Predicate, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator find_if(Iterator first, Iterator last, Predicate pred) {
	while (first != last && !pred(*first)) ++first;
	return first;
}

template <typename Iterator, typename Predicate, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator find_if_not(Iterator first, Iterator last, Predicate pred) {
	while (first != last && pred(*first))
		++first;
	return first;
}

template <typename Iterator1, typename Iterator2, typename BinaryPredicate,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator1 search(Iterator1 first1, Iterator1 last1, Iterator2 first2,
	Iterator2 last2, BinaryPredicate binary_pred) {
	iter_dif_t<Iterator1> d1 = _MSTL distance(first1, last1);
	iter_dif_t<Iterator2> d2 = _MSTL distance(first2, last2);
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

template <typename Iterator1, typename Iterator2>
MSTL_CONSTEXPR20 Iterator1 search(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	return _MSTL search(first1, last1, first2, last2, _MSTL equal_to<iter_val_t<Iterator1>>());
}

template <typename Iterator, typename T, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator search_n(Iterator first, Iterator last, size_t count, const T& value) {
	first = _MSTL find(first, last, value);
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
			first = _MSTL find(i, last, value);
	}
	return last;
}

template <typename Iterator, typename T, typename BinaryPredicate, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator search_n(Iterator first, Iterator last, const size_t count, const T& value, BinaryPredicate binary_pred) {
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

#ifndef MSTL_VERSION_17__
template <typename Iterator1, typename Iterator2,
	enable_if_t<is_ranges_bid_iter_v<Iterator1> && is_ranges_bid_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator1 __find_end_aux(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	using reviter1 = _MSTL reverse_iterator<Iterator1>;
	using reviter2 = _MSTL reverse_iterator<Iterator2>;
	reviter1 rlast1(first1);
	reviter2 rlast2(first2);
	reviter1 rresult = _MSTL search(reviter1(last1), rlast1, reviter2(last2), rlast2);
	if (rresult == rlast1) return last1;
	Iterator1 result = rresult.base();
	_MSTL advance(result, -distance(first2, last2));
	return result;
}
template <typename Iterator1, typename Iterator2,
	enable_if_t<!(is_ranges_bid_iter_v<Iterator1> && is_ranges_bid_iter_v<Iterator2>), int> = 0>
MSTL_CONSTEXPR20 Iterator1 __find_end_aux(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	Iterator1 result = last1;
	while (true) {
		Iterator1 new_result = _MSTL search(first1, last1, first2, last2);
		if (new_result == last1) return result;
		result = new_result;
		first1 = new_result;
		++first1;
	}
}
template <typename Iterator1, typename Iterator2,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator1 find_end(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	if (first2 == last2) return last1;
	return _MSTL __find_end_aux(first1, last1, first2, last2);
}
#else
template <typename Iterator1, typename Iterator2,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator1 find_end(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	if (first2 == last2) return last1;
	if constexpr (is_ranges_bid_iter_v<Iterator1> && is_ranges_bid_iter_v<Iterator2>) {
		using reviter1 = _MSTL reverse_iterator<Iterator1>;
		using reviter2 = _MSTL reverse_iterator<Iterator2>;

		reviter1 rlast1(first1);
		reviter2 rlast2(first2);
		reviter1 rresult = _MSTL search(reviter1(last1), rlast1, reviter2(last2), rlast2);
		if (rresult == rlast1) return last1;

		Iterator1 result = rresult.base();
		_MSTL advance(result, -distance(first2, last2));
		return result;
	}
	else {
		Iterator1 result = last1;
		while (true) {
			Iterator1 new_result = _MSTL search(first1, last1, first2, last2);
			if (new_result == last1) return result;
			result = new_result;
			first1 = new_result;
			++first1;
		}
	}
}
#endif // MSTL_VERSION_17__

template <typename Iterator1, typename Iterator2, typename BinaryPredicate,
	enable_if_t<is_ranges_input_iter_v<Iterator1> && is_ranges_input_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator1 find_first_of(Iterator1 first1, Iterator1 last1,
	Iterator2 first2, Iterator2 last2, BinaryPredicate comp) {
	for (; first1 != last1; ++first1)
		for (Iterator2 iter = first2; iter != last2; ++iter)
			if (comp(*first1, *iter)) return first1;
	return last1;
}

template <typename Iterator1, typename Iterator2>
MSTL_CONSTEXPR20 Iterator1 find_first_of(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	return _MSTL find_first_of(first1, last1, first2, last2, _MSTL equal_to<iter_val_t<Iterator1>>());
}

template <typename Iterator, typename Function, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Function for_each(Iterator first, Iterator last, Function f) {
	for (; first != last; ++first) f(*first);
	return f;
}

template<typename Iterator, typename Function, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator for_each_n(Iterator first, size_t n, Function f) {
	for (size_t i = 0; i < n; i++) {
		f(*first);
		++first;
	}
	return first;
}

template <typename Iterator, typename Generator, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 void generate(Iterator first, Iterator last, Generator gen) {
	for (; first != last; ++first)
		*first = gen();
}

template <typename Iterator, typename Generator, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator generate_n(Iterator first, size_t n, Generator gen) {
	for (; n > 0; --n, ++first)
		*first = gen();
	return first;
}

template <typename Iterator1, typename Iterator2, typename Compare,
	enable_if_t<is_ranges_input_iter_v<Iterator1> && is_ranges_input_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 bool includes(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2, Compare comp) {
	while (first1 != last1 && first2 != last2) {
		if (comp(*first2, *first1))
			return false;
		if (comp(*first1, *first2))
			++first1;
		else
			++first1, ++first2;
	}
	return first2 == last2;
}

template <typename Iterator1, typename Iterator2>
MSTL_CONSTEXPR20 bool includes(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	return _MSTL includes(first1, last1, first2, last2, _MSTL less<iter_val_t<Iterator1>>());
}

template <typename Iterator1, typename Iterator2, typename Iterator3, typename Compare,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2> && is_ranges_fwd_iter_v<Iterator3>, int> = 0>
MSTL_CONSTEXPR20 Iterator3 merge(Iterator1 first1, Iterator1 last1, Iterator2 first2,
	Iterator2 last2, Iterator3 result, Compare comp) {
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
	return _MSTL copy(first2, last2, _MSTL copy(first1, last1, result));
}

template <typename Iterator1, typename Iterator2, typename Iterator3>
MSTL_CONSTEXPR20 Iterator3 merge(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2, Iterator3 result) {
	return _MSTL merge(first1, last1, first2, last2, result, _MSTL less<iter_val_t<Iterator1>>());
}

template <typename Iterator, typename Predicate, enable_if_t<is_ranges_bid_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator partition(Iterator first, Iterator last, Predicate pred) {
	while (true) {
		while (true) {
			if (first == last)
				return first;
			if (pred(*first))
				++first;
			else
				break;
		}
		--last;
		while (true) {
			if (first == last)
				return first;
			if (!pred(*last))
				--last;
			else
				break;
		}
		_MSTL iter_swap(first, last);
		++first;
	}
}

template <typename Iterator, typename T, typename Compare, enable_if_t<is_ranges_rnd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator lomuto_partition(Iterator first, Iterator last, const T& pivot, Compare comp) {
	while (first < last) {
		while (comp(*first, pivot)) ++first;
		--last;
		while (comp(pivot, *last)) --last;
		if (!(first < last)) break;
		_MSTL iter_swap(first, last);
		++first;
	}
	return first;
}

template <typename Iterator, typename T>
MSTL_CONSTEXPR20 Iterator lomuto_partition(Iterator first, Iterator last, const T& pivot) {
	return _MSTL lomuto_partition(first, last, pivot);
}

template <typename Iterator1, typename Iterator2, typename T,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 remove_copy(Iterator1 first, Iterator1 last, Iterator2 result, const T& value) {
	for (; first != last; ++first)
		if (*first != value) {
			*result = *first;
			++result;
		}
	return result;
}

template <typename Iterator1, typename Iterator2, typename Predicate,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 remove_copy_if(Iterator1 first, Iterator1 last, Iterator2 result, Predicate pred) {
	for (; first != last; ++first)
		if (!pred(*first)) {
			*result = *first;
			++result;
		}
	return result;
}

template <typename Iterator, typename T, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator remove(Iterator first, Iterator last, const T& value) {
	first = _MSTL find(first, last, value);
	Iterator next = first;
	return first == last ? first : _MSTL remove_copy(++next, last, first, value);
}

template <typename Iterator, typename Predicate, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator remove_if(Iterator first, Iterator last, Predicate pred) {
	first = _MSTL find_if(first, last, pred);
	Iterator next = first;
	return first == last ? first : _MSTL remove_copy_if(++next, last, first, pred);
}

template <typename Container, typename U,
	enable_if_t<is_same_v<typename Container::value_type, U>, int> = 0>
MSTL_CONSTEXPR20 decltype(auto) erase(Container& cont, const U& value) {
	const auto old_size = cont.size();
	const auto end = cont.end();
	auto removed = _MSTL remove_if(cont.begin(), end,
		[&value](const auto& iter) { return *iter == value; });
	cont.erase(removed, end);
	return old_size - cont.size();
}

template <typename Container, typename Predicate>
MSTL_CONSTEXPR20 decltype(auto) erase_if(Container& cont, Predicate pred) {
	const auto old_size = cont.size();
	const auto end = cont.end();
	auto removed = _MSTL remove_if(cont.begin(), end,
		[ref_pred = _MSTL ref(pred)](const auto& iter) { return ref_pred(*iter); });
	cont.erase(removed, end);
	return old_size - cont.size();
}

template <typename Iterator1, typename Iterator2, typename T,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 replace_copy(Iterator1 first, Iterator1 last, Iterator2 result,
	const T& old_value, const T& new_value) {
	for (; first != last; ++first, ++result)
		*result = *first == old_value ? new_value : *first;
	return result;
}

template <typename Iterator1, typename Iterator2, typename Predicate, typename T,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 replace_copy_if(Iterator1 first, Iterator1 last, Iterator2 result,
	Predicate pred, const T& new_value) {
	for (; first != last; ++first, ++result)
		*result = pred(*first) ? new_value : *first;
	return result;
}

template <typename Iterator, typename T, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 void replace(Iterator first, Iterator last, const T& old_value, const T& new_value) {
	for (; first != last; ++first)
		if (*first == old_value) *first = new_value;
}

template <typename Iterator, typename Predicate, typename T, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 void replace_if(Iterator first, Iterator last, Predicate pred, const T& new_value) {
	for (; first != last; ++first)
		if (pred(*first)) *first = new_value;
}

#ifndef MSTL_VERSION_17__
template <typename Iterator, enable_if_t<is_ranges_rnd_iter_v<Iterator>, int> = 0>
void __reverse_aux(Iterator first, Iterator last) {
	while (first < last) {
		--last;
		_MSTL iter_swap(first, last);
		++first;
	}
}
template <typename Iterator, enable_if_t<!is_ranges_rnd_iter_v<Iterator>, int> = 0>
void __reverse_aux(Iterator first, Iterator last) {
	while (true) {
		if (first == last || first == --last) return;
		--last;
		_MSTL iter_swap(first, last);
		++first;
	}
}
template <typename Iterator, enable_if_t<is_ranges_bid_iter_v<Iterator>, int> = 0>
void reverse(Iterator first, Iterator last) {
	_MSTL __reverse_aux(first, last);
}
#else
template <typename Iterator, enable_if_t<is_ranges_bid_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 void reverse(Iterator first, Iterator last) {
	if constexpr (is_ranges_rnd_iter_v<Iterator>) {
		while (first < last) {
			--last;
			_MSTL iter_swap(first, last);
			++first;
		}
	}
	else {
		while (true) {
			if (first == last || first == --last) return;
			--last;
			_MSTL iter_swap(first, last);
			++first;
		}
	}
}
#endif // MSTL_VERSION_17__

#ifndef MSTL_VERSION_17__
template <typename Iterator, enable_if_t<!is_ranges_bid_iter_v<Iterator>, int> = 0>
void __rotate_aux_dispatch(Iterator first, Iterator middle, Iterator last) {
	for (Iterator i = middle; ;) {
		_MSTL iter_swap(first, i);
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
template <typename Iterator, enable_if_t<is_ranges_bid_iter_v<Iterator>, int> = 0>
void __rotate_aux_dispatch(Iterator first, Iterator middle, Iterator last) {
	_MSTL reverse(first, middle);
	_MSTL reverse(middle, last);
	_MSTL reverse(first, last);
}
template <typename Iterator, enable_if_t<!is_ranges_rnd_iter_v<Iterator>, int> = 0>
void __rotate_aux(Iterator first, Iterator middle, Iterator last) {
	_MSTL __rotate_aux_dispatch(first, middle, last);
}
#else
template <typename Iterator, enable_if_t<!is_ranges_rnd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 void __rotate_aux(Iterator first, Iterator middle, Iterator last) {
	if (first == middle || middle == last) return;
	if constexpr (is_ranges_bid_iter_v<Iterator>) {
		_MSTL reverse(first, middle);
		_MSTL reverse(middle, last);
		_MSTL reverse(first, last);
	}
	else {
		for (Iterator i = middle; ;) {
			_MSTL iter_swap(first, i);
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
}
#endif // MSTL_VERSION_17__

template <typename Iterator, typename Distance>
MSTL_CONSTEXPR20 void __rotate_cycle_aux(Iterator first, Iterator last, Iterator initial, Distance shift) {
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

template <typename Iterator, enable_if_t<is_ranges_rnd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 void __rotate_aux(Iterator first, Iterator middle, Iterator last) {
	iter_dif_t<Iterator> n = _MSTL gcd(last - first, middle - first);
	while (n--)
		_MSTL __rotate_cycle_aux(first, last, first + n, middle - first);
}

template <typename Iterator, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 void rotate(Iterator first, Iterator middle, Iterator last) {
	if (first == middle || middle == last) return;
	_MSTL __rotate_aux(first, middle, last);
}

template <typename Iterator1, typename Iterator2,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 rotate_copy(Iterator1 first, Iterator1 middle, Iterator1 last, Iterator2 result) {
	return _MSTL copy(first, middle, _MSTL copy(middle, last, result));
}

template <typename Iterator,
	enable_if_t<is_ranges_fwd_iter_v<Iterator> && is_default_constructible_v<iter_val_t<Iterator>>, int> = 0>
MSTL_CONSTEXPR20 void shift_left(Iterator first, Iterator last, size_t n) {
	if (first == last || n == 0) return;
	if (n >= _MSTL distance(first, last)) {
		for (; first != last; ++first) {
			*first = _MSTL initialize<iter_val_t<Iterator>>();
		}
		return;
	}
	Iterator new_first = _MSTL next(first, n);
	_MSTL copy(new_first, last, first);
	Iterator end = _MSTL prev(last, -n);
	for (; end != last; ++end) {
		*end = _MSTL initialize<iter_val_t<Iterator>>();
	}
}

template<typename Iterator,
	enable_if_t<is_ranges_bid_iter_v<Iterator> && is_default_constructible_v<iter_val_t<Iterator>>, int> = 0>
MSTL_CONSTEXPR20 void shift_right(Iterator first, Iterator last, size_t n) {
	if (first == last || n == 0) return;
	if (n >= _MSTL distance(first, last)) {
		for (; first != last; ++first) {
			*first = _MSTL initialize<iter_val_t<Iterator>>();
		}
		return;
	}
	auto new_last = _MSTL prev(last, -n);
	_MSTL move_backward(first, new_last, last);
	auto end = _MSTL next(first, n);
	for (; first != end; ++first) {
		*first = _MSTL initialize<iter_val_t<Iterator>>();
	}
}

template <typename Iterator1, typename Iterator2,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 swap_ranges(Iterator1 first1, Iterator1 last1, Iterator2 first2) {
	for (; first1 != last1; ++first1, ++first2)
		_MSTL iter_swap(first1, first2);
	return first2;
}

template <typename Iterator1, typename Iterator2, typename UnaryOperation,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 transform(Iterator1 first, Iterator1 last, Iterator2 result, UnaryOperation op) {
	for (; first != last; ++first, ++result)
		*result = op(*first);
	return result;
}

template <typename Iterator1, typename Iterator2, typename Iterator3, typename BinaryOperation,
	enable_if_t<is_ranges_fwd_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2> && is_ranges_fwd_iter_v<Iterator3>, int> = 0>
MSTL_CONSTEXPR20 Iterator3 transform(Iterator1 first1, Iterator1 last1, Iterator2 first2,
	Iterator3 result, BinaryOperation binary_op) {
	for (; first1 != last1; ++first1, ++first2, ++result)
		*result = binary_op(*first1, *first2);
	return result;
}

template <typename Iterator1, typename Iterator2, typename BinaryPredicate,
	enable_if_t<is_ranges_input_iter_v<Iterator1> && is_ranges_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 unique_copy(Iterator1 first, Iterator1 last,
	Iterator2 result, BinaryPredicate binary_pred) {
	if (first == last) return result;
	*result = *first;
	while (++first != last)
		if (!binary_pred(*result, *first)) *++result = *first;
	return ++result;
}

template <typename Iterator1, typename Iterator2>
MSTL_CONSTEXPR20 Iterator2 unique_copy(Iterator1 first, Iterator1 last, Iterator2 result) {
	return _MSTL unique_copy(first, last, result, _MSTL equal_to<iter_val_t<Iterator1>>());
}

template <typename Iterator, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator unique(Iterator first, Iterator last) {
	first = _MSTL adjacent_find(first, last);
	return _MSTL unique_copy(first, last, first);
}

template <typename Iterator, typename BinaryPredicate, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator unique(Iterator first, Iterator last, BinaryPredicate binary_pred) {
	first = _MSTL adjacent_find(first, last, binary_pred);
	return _MSTL unique_copy(first, last, first, binary_pred);
}

template <typename Iterator, typename T, typename Compare, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator lower_bound(Iterator first, Iterator last, const T& value, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = _MSTL distance(first, last);
	Distance half;
	Iterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		_MSTL advance(middle, half);
		if (comp(*middle, value)) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else len = half;
	}
	return first;
}

template <typename Iterator, typename T>
MSTL_CONSTEXPR20 Iterator lower_bound(Iterator first, Iterator last, const T& value) {
	return _MSTL lower_bound(first, last, value, _MSTL less<iter_val_t<Iterator>>());
}

template <typename Iterator, typename T, typename Compare, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator upper_bound(Iterator first, Iterator last, const T& value, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	Distance len = _MSTL distance(first, last);
	Distance half;
	Iterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		_MSTL advance(middle, half);
		if (comp(value, *middle)) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else len = half;
	}
	return first;
}

template <typename Iterator, typename T>
MSTL_CONSTEXPR20 Iterator upper_bound(Iterator first, Iterator last, const T& value) {
	return _MSTL upper_bound(first, last, value, _MSTL greater<iter_val_t<Iterator>>());
}

template <typename Iterator, typename T, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 bool binary_search(Iterator first, Iterator last, const T& value) {
	Iterator i = _MSTL lower_bound(first, last, value);
	return i != last && !(value < *i);
}

template <typename Iterator, typename T, typename Compare, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 bool binary_search(Iterator first, Iterator last, const T& value, Compare comp) {
	Iterator i = _MSTL lower_bound(first, last, value, comp);
	return i != last && !comp(value, *i);
}

template <typename Iterator1, typename Iterator2, typename BinaryPred,
	enable_if_t<is_ranges_bid_iter_v<Iterator1> && is_ranges_bid_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 bool is_permutation(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2, BinaryPred pred) {
	iter_dif_t<Iterator1> len1 = _MSTL distance(first1, last1);
	iter_dif_t<Iterator2> len2 = _MSTL distance(first2, last2);
	if (len1 != len2) return false;
	for (; first1 != last1 && first2 != last2; ++first1, (void) ++first2) {
		if (!pred(*first1, *first2)) break;
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
			if (c2 == 0) return false;
			size_t c1 = 1;
			Iterator1 j = i;
			for (++j; j != last1; ++j) {
				if (pred(*i, *j))
					++c1;
			}
			if (c1 != c2) return false;
		}
	}
	return true;
}

template <typename Iterator1, typename Iterator2>
MSTL_CONSTEXPR20 bool is_permutation(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) {
	return _MSTL is_permutation(first1, last1, first2, last2, _MSTL equal_to<iter_val_t<Iterator1>>());
}

template <typename Iterator, typename Compare, enable_if_t<is_ranges_bid_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 bool next_permutation(Iterator first, Iterator last, Compare comp) {
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
			while (!comp(*i, *--j)) {}
			_MSTL iter_swap(i, j);
			_MSTL reverse(ii, last);
			return true;
		}
		if (i == first) {
			_MSTL reverse(first, last);
			return false;
		}
	}
}

template <typename Iterator>
MSTL_CONSTEXPR20 bool next_permutation(Iterator first, Iterator last) {
	return _MSTL next_permutation(first, last, _MSTL less<iter_val_t<Iterator>>());
}

template <typename Iterator, typename Compare, enable_if_t<is_ranges_bid_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 bool prev_permutation(Iterator first, Iterator last, Compare comp) {
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
			while (!comp(*--j, *i)) {}
			_MSTL iter_swap(i, j);
			_MSTL reverse(ii, last);
			return true;
		}
		if (i == first) {
			_MSTL reverse(first, last);
			return false;
		}
	}
}

template <typename Iterator>
MSTL_CONSTEXPR20 bool prev_permutation(Iterator first, Iterator last) {
	return _MSTL prev_permutation(first, last, _MSTL less<iter_val_t<Iterator>>());
}


// based on Fisher-Yates algorithm to shuffle elements
template <typename Iterator, enable_if_t<is_ranges_rnd_iter_v<Iterator>, int> = 0>
void shuffle(Iterator first, Iterator last) {
	if (first == last) return;
    _MSTL random_lcd rng;
    for (Iterator i = _MSTL next(first); i != last; ++i) {
        auto distance = _MSTL distance(first, i);
        auto random_idx = rng.next_int(0, static_cast<int>(distance));
        Iterator j = _MSTL next(first, random_idx);
        _MSTL iter_swap(i, j);
    }
}

template <typename Iterator, typename Generator, enable_if_t<is_ranges_rnd_iter_v<Iterator>, int> = 0>
void shuffle(Iterator first, Iterator last, Generator& rand) {
	if (first == last) return;
	for (Iterator i = _MSTL next(first); i != last; ++i) {
		Iterator j = _MSTL next(first, rand(i - first + 1));
		_MSTL iter_swap(i, j);
	}
}

template <typename Iterator, typename T, typename Compare, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 pair<Iterator, Iterator> equal_range(Iterator first, Iterator last, const T& value, Compare comp) {
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
			left = _MSTL lower_bound(first, middle, value, comp);
			_MSTL advance(first, len);
			right = _MSTL upper_bound(++middle, first, value, comp);
			return pair<Iterator, Iterator>(left, right);
		}
	}
	return pair<Iterator, Iterator>(first, first);
}

template <typename Iterator, typename T>
MSTL_CONSTEXPR20 pair<Iterator, Iterator> equal_range(Iterator first, Iterator last, const T& value) {
	return _MSTL equal_range(first, last, value, _MSTL less<iter_val_t<Iterator>>());
}

template <typename Iterator, typename Distance, typename Compare>
MSTL_CONSTEXPR20 void __merge_without_buffer_aux(Iterator first, Iterator middle, Iterator last,
	Distance len1, Distance len2, Compare comp) {
	if (len1 == 0 || len2 == 0) return;
	if (len1 + len2 == 2) {
		if (comp(*middle, *first)) _MSTL iter_swap(first, middle);
		return;
	}
	Iterator first_cut = first;
	Iterator second_cut = middle;
	Distance len11 = 0;
	Distance len22 = 0;
	if (len1 > len2) {
		len11 = len1 / 2;
		_MSTL advance(first_cut, len11);
		second_cut = _MSTL lower_bound(middle, last, *first_cut, comp);
		len22 = _MSTL distance(middle, second_cut);
	}
	else {
		len22 = len2 / 2;
		_MSTL advance(second_cut, len22);
		first_cut = _MSTL upper_bound(first, middle, *second_cut, comp);
		len11 = _MSTL distance(first, first_cut);
	}
	_MSTL rotate(first_cut, middle, second_cut);
	Iterator new_middle = first_cut;
	_MSTL advance(new_middle, len22);
	_MSTL __merge_without_buffer_aux(first, first_cut, new_middle, len11, len22, comp);
	_MSTL __merge_without_buffer_aux(new_middle, second_cut, last, len1 - len11, len2 - len22, comp);
}

template <typename Iterator1, typename Iterator2, typename Distance>
MSTL_CONSTEXPR20 Iterator1 __rotate_with_buffer_aux(Iterator1 first, Iterator1 middle, Iterator1 last,
	Distance len1, Distance len2, Iterator2 buffer, Distance buffer_size) {
	Iterator2 buffer_end;
	if (len1 > len2 && len2 <= buffer_size) {
		buffer_end = _MSTL copy(middle, last, buffer);
		_MSTL copy_backward(first, middle, last);
		return _MSTL copy(buffer, buffer_end, first);
	}
	if (len1 <= buffer_size) {
		buffer_end = _MSTL copy(first, middle, buffer);
		_MSTL copy(middle, last, first);
		return _MSTL copy_backward(buffer, buffer_end, last);
	}
	_MSTL rotate(first, middle, last);
	_MSTL advance(first, len2);
	return first;
}

template <typename Iterator, typename Distance, typename Pointer, typename Compare>
MSTL_CONSTEXPR20 void __merge_with_buffer_aux(Iterator first, Iterator middle, Iterator last,
	Distance len1, Distance len2, Pointer buffer, Distance buffer_size, Compare comp) {
	if (len1 <= len2 && len1 <= buffer_size) {
		Pointer end_buffer = _MSTL copy(first, middle, buffer);
		_MSTL merge(buffer, end_buffer, middle, last, first, comp);
	}
	else if (len2 <= buffer_size) {
		Pointer end_buffer = _MSTL copy(middle, last, buffer);
		if (first == middle) {
			_MSTL copy_backward(buffer, end_buffer, last);
			return;
		}
		if (buffer == end_buffer) {
			_MSTL copy_backward(first, middle, last);
			return;
		}
		--middle;
		--end_buffer;
		while (true) {
			if (comp(*end_buffer, *middle)) {
				*--last = *middle;
				if (first == middle) {
					_MSTL copy_backward(buffer, ++end_buffer, last);
					return;
				}
				--middle;
			}
			else {
				*--last = *end_buffer;
				if (buffer == end_buffer) {
					_MSTL copy_backward(first, ++middle, last);
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
			_MSTL advance(first_cut, len11);
			second_cut = _MSTL lower_bound(middle, last, *first_cut, comp);
			len22 = _MSTL distance(middle, second_cut);
		}
		else {
			len22 = len2 / 2;
			_MSTL advance(second_cut, len22);
			first_cut = _MSTL upper_bound(first, middle, *second_cut, comp);
			len11 = _MSTL distance(first, first_cut);
		}
		Iterator new_middle = _MSTL __rotate_with_buffer_aux(
			first_cut, middle, second_cut, len1 - len11, len22, buffer, buffer_size);

		_MSTL __merge_with_buffer_aux(
			first, first_cut, new_middle, len11, len22, buffer, buffer_size, comp);
		_MSTL __merge_with_buffer_aux(
			new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, buffer_size, comp);
	}
}

template <typename Iterator, typename Compare, enable_if_t<is_ranges_bid_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 void inplace_merge(Iterator first, Iterator middle, Iterator last, Compare comp) {
	if (first == middle || middle == last) return;
	using Distance = iter_dif_t<Iterator>;
	Distance len1 = _MSTL distance(first, middle);
	Distance len2 = _MSTL distance(middle, last);
	temporary_buffer<Iterator> buffer(first, last);
	if (buffer.begin() == 0)
		_MSTL __merge_without_buffer_aux(first, middle, last, len1, len2, comp);
	else
		_MSTL __merge_with_buffer_aux(
			first, middle, last, len1, len2, buffer.begin(), Distance(buffer.size()), comp);
}

template <typename Iterator>
MSTL_CONSTEXPR20 void inplace_merge(Iterator first, Iterator middle, Iterator last) {
	return _MSTL inplace_merge(first, middle, last, _MSTL less<iter_val_t<Iterator>>());
}


template<typename Iterator, typename Compare, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
bool is_sorted(Iterator first, Iterator last, Compare comp) {
	if (first == last) return true;
	Iterator next = _MSTL next(first);
	for (; next != last; ++first, ++next) {
		if (comp(*next, *first)) {
			return false;
		}
	}
	return true;
}

template <typename Iterator>
bool is_sorted(Iterator first, Iterator last) {
	return is_sorted(first, last, _MSTL less<iter_val_t<Iterator>>());
}

template <typename Iterator, typename Compare, enable_if_t<is_ranges_input_iter_v<Iterator>, int> = 0>
Iterator is_sorted_until(Iterator first, Iterator last, Compare comp) {
	if (first == last) return last;
	Iterator next = _MSTL next(first);
	for (; next != last; ++first, ++next) {
		if (comp(*next, *first))
			return next;
	}
	return last;
}

template <typename Iterator>
Iterator is_sorted_until(Iterator first, Iterator last) {
	return is_sorted_until(first, last, _MSTL less<iter_val_t<Iterator>>());
}


// merge sort : Ot(NlogN) Om(logN) stable
template <typename Iterator, typename Compare, enable_if_t<
    is_ranges_rnd_iter_v<Iterator>, int> = 0>
void merge_sort(Iterator first, Iterator last, Compare comp) {
    using Distance = typename iterator_traits<Iterator>::difference_type;
    Distance n = _MSTL distance(first, last);
    if (n < 2) return;
    Iterator mid = first + n / 2;
    _MSTL merge_sort(first, mid);
    _MSTL merge_sort(mid, last);
    _MSTL inplace_merge(first, mid, last, comp);
}

template <typename Iterator>
void merge_sort(Iterator first, Iterator last) {
    return _MSTL merge_sort(first, last, _MSTL less<iter_val_t<Iterator>>());
}

// partial sort / heap sort : Ot(NlogN) Om(1) unstable
template <typename Iterator, typename Compare, enable_if_t<
    is_ranges_rnd_iter_v<Iterator>, int> = 0>
void partial_sort(Iterator first, Iterator middle, Iterator last, Compare comp) {
    if (first == middle) return;
    _MSTL make_heap(first, middle, comp);
    for (Iterator i = middle; i < last; ++i) {
        if (comp(*i, *first))
            _MSTL pop_heap_aux(first, middle, i, *i, comp);
    }
    _MSTL sort_heap(first, middle, comp);
}

template <typename Iterator>
void partial_sort(Iterator first, Iterator middle, Iterator last) {
    return _MSTL partial_sort(first, middle, last, _MSTL less<iter_val_t<Iterator>>());
}

template <typename Iterator1, typename Iterator2, typename Compare, enable_if_t<
    is_ranges_input_iter_v<Iterator1> && is_ranges_rnd_iter_v<Iterator2>, int> = 0>
Iterator2 partial_sort_copy(Iterator1 first, Iterator1 last,
	Iterator2 result_first, Iterator2 result_last, Compare comp) {
	if (result_first == result_last) return result_last;
    using Distance = typename iterator_traits<Iterator1>::difference_type;
	Iterator2 result_real_last = result_first;
	while (first != last && result_real_last != result_last) {
		*result_real_last = *first;
		++result_real_last;
		++first;
	}
    _MSTL make_heap(result_first, result_real_last, comp);
	while (first != last) {
		if (comp(*first, *result_first)) {
            _MSTL adjust_heap(result_first, Distance(0),
                Distance(result_real_last - result_first), *first, comp);
        }
		++first;
	}
    _MSTL sort_heap(result_first, result_real_last, comp);
	return result_real_last;
}

template <typename Iterator1, typename Iterator2>
Iterator2 partial_sort_copy(
    Iterator1 first, Iterator1 last, Iterator2 result_first, Iterator2 result_last) {
    return _MSTL partial_sort_copy(first, result_first, result_last, _MSTL less<iter_val_t<Iterator1>>());
}


template <typename Iterator, typename T, typename Compare>
void __insertion_sort_aux(Iterator last, T value, Compare comp) {
    Iterator next = last;
    --next;
    while (comp(value, *next)) {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

// insertion sort : Ot(N)~(N^2) Om(1) stable
template <typename Iterator, typename Compare, enable_if_t<
    is_ranges_rnd_iter_v<Iterator>, int> = 0>
void insertion_sort(Iterator first, Iterator last, Compare comp) {
    if (first == last) return;
    using T = typename iterator_traits<Iterator>::value_type;
    for (Iterator i = first + 1; i != last; ++i) {
        T value = *i;
        if (comp(value, *first)) {
            _MSTL copy_backward(first, i, i + 1);
            *first = value;
        }
        else _MSTL __insertion_sort_aux(i, value, comp);
    }
}

template <typename Iterator>
void insertion_sort(Iterator first, Iterator last) {
    return _MSTL insertion_sort(first, last, _MSTL less<iter_val_t<Iterator>>());
}


static constexpr size_t SORT_DISPATCH_THRESHOLD = 16;

// introspective sort : Ot(NlogN) Om(logN) unstable
template <typename Iterator, typename Compare, enable_if_t<
    is_ranges_rnd_iter_v<Iterator>, int> = 0>
void introspective_sort(Iterator first, Iterator last, int depth_limit, Compare comp) {
    while (first < last) {
        if (depth_limit == 0) {
            _MSTL partial_sort(first, last, last, comp);
            return;
        }
        --depth_limit;
        Iterator cut = _MSTL lomuto_partition(
            first, last, _MSTL median(*first, *(first + (last - first) / 2), *(last - 1), comp), comp);
        _MSTL introspective_sort(cut, last, depth_limit, comp);
        last = cut;
    }
}

template <typename Iterator>
void introspective_sort(Iterator first, Iterator last, int depth_limit) {
    return _MSTL introspective_sort(first, last, depth_limit, _MSTL less<iter_val_t<Iterator>>());
}

// quick sort : Ot(NlogN) Om(1) unstable
template <typename Iterator, typename Compare, enable_if_t<
    is_ranges_rnd_iter_v<Iterator>, int> = 0>
void quick_sort(Iterator first, Iterator last, Compare comp) {
    if (first < last) {
        Iterator pov = last - 1;
        Iterator cut = _MSTL lomuto_partition(first, last, *pov, comp);
        _MSTL iter_swap(cut, pov);
        _MSTL quick_sort(first, cut, comp);
        _MSTL quick_sort(cut + 1, last, comp);
    }
}

template <typename Iterator>
void quick_sort(Iterator first, Iterator last) {
    return _MSTL quick_sort(first, last, _MSTL less<iter_val_t<Iterator>>());
}

template <typename Iterator, typename Compare>
void __intro_sort_dispatch(Iterator first, Iterator last, int depth_limit, Compare comp) {
    while (last - first > SORT_DISPATCH_THRESHOLD) {
        if (depth_limit == 0) {
            _MSTL partial_sort(first, last, last, comp);
            return;
        }
        --depth_limit;
        Iterator cut = _MSTL lomuto_partition(
            first, last, _MSTL median(*first, *(first + (last - first) / 2), *(last - 1), comp), comp);
        _MSTL __intro_sort_dispatch(cut, last, depth_limit, comp);
        last = cut;
    }
}

// standard sort : Ot(NlogN) Om(logN) unstable
template <typename Iterator, typename Compare, enable_if_t<
    is_ranges_rnd_iter_v<Iterator>, int> = 0>
void sort(Iterator first, Iterator last, Compare comp) {
    if (first == last) return;
    _MSTL __intro_sort_dispatch(first, last, (int)_MSTL cursory_lg2(last - first) * 2, comp);
    if (last - first > SORT_DISPATCH_THRESHOLD) {
        _MSTL insertion_sort(first, first + SORT_DISPATCH_THRESHOLD, comp);
        for (Iterator i = first + SORT_DISPATCH_THRESHOLD; i != last; ++i)
            _MSTL __insertion_sort_aux(i, *i, comp);
    }
    else
        _MSTL insertion_sort(first, last, comp);
}

template <typename Iterator>
void sort(Iterator first, Iterator last) {
    return _MSTL sort(first, last, _MSTL less<iter_val_t<Iterator>>());
}

// nth element : Ot(N)~(N^2) Om(1) unstable
template <typename Iterator, typename Compare, enable_if_t<
    is_ranges_rnd_iter_v<Iterator>, int> = 0>
void nth_element(Iterator first, Iterator nth, Iterator last, Compare comp) {
    while (last - first > 3) {
        Iterator cut = _MSTL lomuto_partition(
            first, last, _MSTL median(*first, *(first + (last - first) / 2), *(last - 1), comp), comp);
        if (cut <= nth) first = cut;
        else last = cut;
    }
    _MSTL insertion_sort(first, last, comp);
}

template <typename Iterator>
void nth_element(Iterator first, Iterator nth, Iterator last) {
    return _MSTL nth_element(first, nth, last, _MSTL less<iter_val_t<Iterator>>());
}

MSTL_END_NAMESPACE__
#endif // MSTL_ALGO_HPP__
