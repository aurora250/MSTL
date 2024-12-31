#ifndef MSTL_SORT_HPP__
#define MSTL_SORT_HPP__
#include "basiclib.h"
#include "algobase.hpp"
#include "heap.hpp"
#include "iterator.hpp"
#include "concepts.hpp"
#include "tempbuf.hpp"
MSTL_BEGIN_NAMESPACE__

// bubble sort
template <typename Iterator> 
    requires(concepts::BidirectionalIterator<Iterator>)
void bubble_sort(Iterator begin, Iterator end) {
    auto reverse_end = std::make_reverse_iterator(begin);
    auto reverse_start = std::make_reverse_iterator(--end);
    for (auto iter = reverse_start; iter != reverse_end; ++iter) {
        bool notfinished = false;
        auto current_end = iter.base();
        auto inner_start = begin;
        for (auto it = inner_start; it != current_end; ++it) {
            if (*it > *(it + 1)) {
                MSTL::iter_swap(it, it + 1);
                notfinished = true;
            }
        }
        if (!notfinished) break;
    }
}

// select sort
template <typename Iterator> 
    requires(concepts::ForwardIterator<Iterator>)
void select_sort(Iterator begin, Iterator end) {
    Iterator min;
    for (Iterator i = begin; i != end; ++i) {
        min = i;
        for (Iterator j = i + 1; j != end; ++j) {
            if (*j < *min) {
                min = j;
            }
        }
        MSTL::iter_swap(i, min);
    }
}

// shell sort
template <typename Iterator> 
    requires(concepts::RandomAccessIterator<Iterator>)
void shell_sort(Iterator start, Iterator end) {
    auto dist = MSTL::distance(start, end);
    for (auto gap = dist / 2; gap > 0; gap /= 2) {
        for (auto i = start; i != start + gap; ++i) {
            auto j = i + gap;
            for (; j < end && *j < *i; j += gap) {
                *(j - gap) = *j;
            }
            *(j - gap) = *i;
        }
    }
}

// partial sort (heap sort)
template <class RandomAccessIterator, class T>
void __partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, T*) {
    MSTL::make_heap(first, middle);
	for (RandomAccessIterator i = middle; i < last; ++i) {
		if (*i < *first)
			__pop_heap(first, middle, i, T(*i), (distance_type)(first));
	}
	MSTL::sort_heap(first, middle);
}
template <class RandomAccessIterator>
inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last) {
	__partial_sort(first, middle, last, (value_type)(first));
}

template <class RandomAccessIterator, class T, class Compare>
void __partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
	T*, Compare comp) {
    MSTL::make_heap(first, middle, comp);
	for (RandomAccessIterator i = middle; i < last; ++i) {
		if (comp(*i, *first))
			__pop_heap(first, middle, i, T(*i), comp, (distance_type)(first));
	}
    MSTL::sort_heap(first, middle, comp);
}
template <class RandomAccessIterator, class Compare>
inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
	Compare comp) {
	__partial_sort(first, middle, last, (value_type)(first), comp);
}

template <class InputIterator, class RandomAccessIterator, class Distance, class T>
RandomAccessIterator __partial_sort_copy(InputIterator first, InputIterator last,
	RandomAccessIterator result_first, RandomAccessIterator result_last, Distance*, T*) {
	if (result_first == result_last) return result_last;
	RandomAccessIterator result_real_last = result_first;
	while (first != last && result_real_last != result_last) {
		*result_real_last = *first;
		++result_real_last;
		++first;
	}
    MSTL::make_heap(result_first, result_real_last);
	while (first != last) {
		if (*first < *result_first)
			__adjust_heap(result_first, Distance(0),
				Distance(result_real_last - result_first), T(*first));
		++first;
	}
    MSTL::sort_heap(result_first, result_real_last);
	return result_real_last;
}
template <class InputIterator, class RandomAccessIterator>
inline RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last,
	RandomAccessIterator result_first, RandomAccessIterator result_last) {
	return __partial_sort_copy(first, last, result_first, result_last,
		(distance_type)(result_first), (value_type)(first));
}

template <class InputIterator, class RandomAccessIterator, class Compare, class Distance, class T>
RandomAccessIterator __partial_sort_copy(InputIterator first, InputIterator last,
	RandomAccessIterator result_first, RandomAccessIterator result_last, Compare comp, Distance*, T*) {
	if (result_first == result_last) return result_last;
	RandomAccessIterator result_real_last = result_first;
	while (first != last && result_real_last != result_last) {
		*result_real_last = *first;
		++result_real_last;
		++first;
	}
    MSTL::make_heap(result_first, result_real_last, comp);
	while (first != last) {
		if (comp(*first, *result_first)) {
			__adjust_heap(result_first, Distance(0), Distance(result_real_last - result_first),
				T(*first), comp);
		}
		++first;
	}
    MSTL::sort_heap(result_first, result_real_last, comp);
	return result_real_last;
}
template <typename InputIterator, typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last,
	RandomAccessIterator result_first, RandomAccessIterator result_last, Compare comp) {
	return __partial_sort_copy(first, last, result_first, result_last, comp,
		(distance_type)(result_first), (value_type)(first));
}



template <class RandomAccessIterator, class T>
RandomAccessIterator __unguarded_partition(RandomAccessIterator first, 
    RandomAccessIterator last, T pivot) {
    while (true) {
        while (*first < pivot) ++first;
        --last;
        while (pivot < *last) --last;
        if (!(first < last)) return first;
        MSTL::iter_swap(first, last);
        ++first;
    }
}

template <class RandomAccessIterator, class T, class Compare>
RandomAccessIterator __unguarded_partition(RandomAccessIterator first,
    RandomAccessIterator last, T pivot, Compare comp) {
    while (1) {
        while (comp(*first, pivot)) ++first;
        --last;
        while (comp(pivot, *last)) --last;
        if (!(first < last)) return first;
        MSTL::iter_swap(first, last);
        ++first;
    }
}

static const int threshold__ = 16;

template <class RandomAccessIterator, class T>
void __unguarded_linear_insert(RandomAccessIterator last, T value) {
    RandomAccessIterator next = last;
    --next;
    while (value < *next) {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

template <class RandomAccessIterator, class T, class Compare>
void __unguarded_linear_insert(RandomAccessIterator last, T value, Compare comp) {
    RandomAccessIterator next = last;
    --next;
    while (comp(value, *next)) {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

template <class RandomAccessIterator, class T>
inline void __linear_insert(RandomAccessIterator first, RandomAccessIterator last, T*) {
    T value = *last;
    if (value < *first) {
        MSTL::copy_backward(first, last, last + 1);
        *first = value;
    }
    else
        __unguarded_linear_insert(last, value);
}

template <class RandomAccessIterator, class T, class Compare>
inline void __linear_insert(RandomAccessIterator first, RandomAccessIterator last, T*, Compare comp) {
    T value = *last;
    if (comp(value, *first)) {
        MSTL::copy_backward(first, last, last + 1);
        *first = value;
    }
    else
        __unguarded_linear_insert(last, value, comp);
}

template <class RandomAccessIterator>
void insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
    if (first == last) return;
    for (RandomAccessIterator i = first + 1; i != last; ++i)
        __linear_insert(first, i, (value_type)(first));
}

template <class RandomAccessIterator, class Compare>
void insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (first == last) return;
    for (RandomAccessIterator i = first + 1; i != last; ++i)
        __linear_insert(first, i, (value_type)(first), comp);
}

template <class RandomAccessIterator, class T>
void __unguarded_insertion_sort_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
    for (RandomAccessIterator i = first; i != last; ++i)
        __unguarded_linear_insert(i, T(*i));
}

template <class RandomAccessIterator>
inline void __unguarded_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
    __unguarded_insertion_sort_aux(first, last, (value_type)(first));
}

template <class RandomAccessIterator, class T, class Compare>
void __unguarded_insertion_sort_aux(RandomAccessIterator first,
    RandomAccessIterator last, T*, Compare comp) {
    for (RandomAccessIterator i = first; i != last; ++i)
        __unguarded_linear_insert(i, T(*i), comp);
}

template <class RandomAccessIterator, class Compare>
inline void __unguarded_insertion_sort(RandomAccessIterator first,
    RandomAccessIterator last, Compare comp) {
    __unguarded_insertion_sort_aux(first, last, (value_type)(first), comp);
}

template <class RandomAccessIterator>
void __final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
    if (last - first > threshold__) {
        MSTL::insertion_sort(first, first + threshold__);
        __unguarded_insertion_sort(first + threshold__, last);
    }
    else
        MSTL::insertion_sort(first, last);
}

template <class RandomAccessIterator, class Compare>
void __final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (last - first > threshold__) {
        MSTL::insertion_sort(first, first + threshold__, comp);
        __unguarded_insertion_sort(first + threshold__, last, comp);
    }
    else
        MSTL::insertion_sort(first, last, comp);
}

inline size_t __lg(size_t n) {
    size_t k;
    for (k = 0; n > 1; n >>= 1) ++k;
    return k;
}
template <class T>
MSTL_CONSTEXPR inline const T& __median(const T& a, const T& b, const T& c) {
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
MSTL_CONSTEXPR inline const T& __median(const T& a, const T& b, const T& c, Compare comp) {
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

template <class RandomAccessIterator, class T, class Size>
void __introsort_loop(RandomAccessIterator first, RandomAccessIterator last, T*, Size depth_limit) {
    while (last - first > threshold__) {
        if (depth_limit == 0) {
            MSTL::partial_sort(first, last, last);
            return;
        }
        --depth_limit;
        RandomAccessIterator cut = __unguarded_partition(
            first, last, T(
                __median(*first, *(first + (last - first) / 2), *(last - 1))
            ));
        __introsort_loop(cut, last, (value_type)(first), depth_limit);
        last = cut;
    }
}

template <class RandomAccessIterator, class T, class Compare>
void __introsort_loop(RandomAccessIterator first, RandomAccessIterator last,
    T*, size_t depth_limit, Compare comp) {
    while (last - first > threshold__) {
        if (depth_limit == 0) {
            MSTL::partial_sort(first, last, last, comp);
            return;
        }
        --depth_limit;
        RandomAccessIterator cut = __unguarded_partition(
            first, last, T(
                __median(*first, *(first + (last - first) / 2), *(last - 1), comp)
            ), comp);
        __introsort_loop(cut, last, (value_type)(first), depth_limit, comp);
        last = cut;
    }
}

template <class RandomAccessIterator>
inline void sort(RandomAccessIterator first, RandomAccessIterator last) {
    if (first != last) {
        __introsort_loop(first, last, (value_type)(first), (__lg)(last - first) * 2);
        __final_insertion_sort(first, last);
    }
}

template <class RandomAccessIterator, class Compare>
inline void sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (first != last) {
        __introsort_loop(first, last, (value_type)(first), (__lg)(last - first) * 2, comp);
        __final_insertion_sort(first, last, comp);
    }
}

MSTL_END_NAMESPACE__
#endif // MSTL_SORT_HPP__
