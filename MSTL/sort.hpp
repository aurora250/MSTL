#ifndef MSTL_SORT_HPP__
#define MSTL_SORT_HPP__
#include "basiclib.h"
#include "algo.hpp"
#include "heap.hpp"
#include "tempbuf.hpp"
MSTL_BEGIN_NAMESPACE__

// bubble sort
//template <typename BidirectionalIterator>
//void bubble_sort(BidirectionalIterator start, BidirectionalIterator end) {
//    bool notfinished;
//    auto before_start = --start;
//    for (auto iter = end, --iter; iter != before_start; --iter) {
//        notfinished = false;
//        for (auto it = start; it != iter; it++) {
//            if (*it > *++it) {
//                (iter_swap)(it, ++it);
//                notfinished = true;
//            }
//        }
//        if (not notfinished) break;
//    }
//}

// partial sort (heap sort)
template <class RandomAccessIterator, class T>
void __partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, T*) {
	make_heap(first, middle);
	for (RandomAccessIterator i = middle; i < last; ++i) {
		if (*i < *first)
			__pop_heap(first, middle, i, T(*i), (distance_type)(first));
	}
	sort_heap(first, middle);
}
template <class RandomAccessIterator>
inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last) {
	__partial_sort(first, middle, last, (value_type)(first));
}

template <class RandomAccessIterator, class T, class Compare>
void __partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
	T*, Compare comp) {
	make_heap(first, middle, comp);
	for (RandomAccessIterator i = middle; i < last; ++i) {
		if (comp(*i, *first))
			__pop_heap(first, middle, i, T(*i), comp, (distance_type)(first));
	}
	sort_heap(first, middle, comp);
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
	make_heap(result_first, result_real_last);
	while (first != last) {
		if (*first < *result_first)
			__adjust_heap(result_first, Distance(0),
				Distance(result_real_last - result_first), T(*first));
		++first;
	}
	sort_heap(result_first, result_real_last);
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
	make_heap(result_first, result_real_last, comp);
	while (first != last) {
		if (comp(*first, *result_first)) {
			__adjust_heap(result_first, Distance(0), Distance(result_real_last - result_first),
				T(*first), comp);
		}
		++first;
	}
	sort_heap(result_first, result_real_last, comp);
	return result_real_last;
}
template <typename InputIterator, typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last,
	RandomAccessIterator result_first, RandomAccessIterator result_last, Compare comp) {
	return __partial_sort_copy(first, last, result_first, result_last, comp,
		(distance_type)(result_first), (value_type)(first));
}


template <class RandomAccessIterator, class T>
RandomAccessIterator __unguarded_partition(RandomAccessIterator first, RandomAccessIterator last, T pivot) {
    while (true) {
        while (*first < pivot) ++first;
        --last;
        while (pivot < *last) --last;
        if (!(first < last)) return first;
        (iter_swap)(first, last);
        ++first;
    }
}

template <class RandomAccessIterator, class T, class Compare>
RandomAccessIterator __unguarded_partition(RandomAccessIterator first,
    RandomAccessIterator last,
    T pivot, Compare comp) {
    while (1) {
        while (comp(*first, pivot)) ++first;
        --last;
        while (comp(pivot, *last)) --last;
        if (!(first < last)) return first;
        iter_swap(first, last);
        ++first;
    }
}

const int __stl_threshold = 16;


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
void __unguarded_linear_insert(RandomAccessIterator last, T value,
    Compare comp) {
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
inline void __linear_insert(RandomAccessIterator first,
    RandomAccessIterator last, T*) {
    T value = *last;
    if (value < *first) {
        copy_backward(first, last, last + 1);
        *first = value;
    }
    else
        __unguarded_linear_insert(last, value);
}

template <class RandomAccessIterator, class T, class Compare>
inline void __linear_insert(RandomAccessIterator first,
    RandomAccessIterator last, T*, Compare comp) {
    T value = *last;
    if (comp(value, *first)) {
        copy_backward(first, last, last + 1);
        *first = value;
    }
    else
        __unguarded_linear_insert(last, value, comp);
}

template <class RandomAccessIterator>
void insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
    if (first == last) return;
    for (RandomAccessIterator i = first + 1; i != last; ++i)
        __linear_insert(first, i, value_type(first));
}

template <class RandomAccessIterator, class Compare>
void insertion_sort(RandomAccessIterator first,
    RandomAccessIterator last, Compare comp) {
    if (first == last) return;
    for (RandomAccessIterator i = first + 1; i != last; ++i)
        __linear_insert(first, i, value_type(first), comp);
}

template <class RandomAccessIterator, class T>
void __unguarded_insertion_sort_aux(RandomAccessIterator first,
    RandomAccessIterator last, T*) {
    for (RandomAccessIterator i = first; i != last; ++i)
        __unguarded_linear_insert(i, T(*i));
}

template <class RandomAccessIterator>
inline void __unguarded_insertion_sort(RandomAccessIterator first,
    RandomAccessIterator last) {
    __unguarded_insertion_sort_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class T, class Compare>
void __unguarded_insertion_sort_aux(RandomAccessIterator first,
    RandomAccessIterator last,
    T*, Compare comp) {
    for (RandomAccessIterator i = first; i != last; ++i)
        __unguarded_linear_insert(i, T(*i), comp);
}

template <class RandomAccessIterator, class Compare>
inline void __unguarded_insertion_sort(RandomAccessIterator first,
    RandomAccessIterator last,
    Compare comp) {
    __unguarded_insertion_sort_aux(first, last, value_type(first), comp);
}

template <class RandomAccessIterator>
void __final_insertion_sort(RandomAccessIterator first,
    RandomAccessIterator last) {
    if (last - first > __stl_threshold) {
        insertion_sort(first, first + __stl_threshold);
        __unguarded_insertion_sort(first + __stl_threshold, last);
    }
    else
        insertion_sort(first, last);
}

template <class RandomAccessIterator, class Compare>
void __final_insertion_sort(RandomAccessIterator first,
    RandomAccessIterator last, Compare comp) {
    if (last - first > __stl_threshold) {
        insertion_sort(first, first + __stl_threshold, comp);
        __unguarded_insertion_sort(first + __stl_threshold, last, comp);
    }
    else
        insertion_sort(first, last, comp);
}

inline size_t __lg(size_t n) {
    size_t k;
    for (k = 0; n > 1; n >>= 1) ++k;
    return k;
}

template <class RandomAccessIterator, class T, class Size>
void __introsort_loop(RandomAccessIterator first,
    RandomAccessIterator last, T*,
    Size depth_limit) {
    while (last - first > __stl_threshold) {
        if (depth_limit == 0) {
            partial_sort(first, last, last);
            return;
        }
        --depth_limit;
        RandomAccessIterator cut = __unguarded_partition
        (first, last, T(__median(*first, *(first + (last - first) / 2),
            *(last - 1))));
        __introsort_loop(cut, last, value_type(first), depth_limit);
        last = cut;
    }
}

template <class RandomAccessIterator, class T, class Size, class Compare>
void __introsort_loop(RandomAccessIterator first,
    RandomAccessIterator last, T*,
    Size depth_limit, Compare comp) {
    while (last - first > __stl_threshold) {
        if (depth_limit == 0) {
            partial_sort(first, last, last, comp);
            return;
        }
        --depth_limit;
        RandomAccessIterator cut = __unguarded_partition
        (first, last, T(__median(*first, *(first + (last - first) / 2),
            *(last - 1), comp)), comp);
        __introsort_loop(cut, last, value_type(first), depth_limit, comp);
        last = cut;
    }
}

template <class RandomAccessIterator>
inline void sort(RandomAccessIterator first, RandomAccessIterator last) {
    if (first != last) {
        __introsort_loop(first, last, value_type(first), __lg(last - first) * 2);
        __final_insertion_sort(first, last);
    }
}

template <class RandomAccessIterator, class Compare>
inline void sort(RandomAccessIterator first, RandomAccessIterator last,
    Compare comp) {
    if (first != last) {
        __introsort_loop(first, last, value_type(first), __lg(last - first) * 2,
            comp);
        __final_insertion_sort(first, last, comp);
    }
}

MSTL_END_NAMESPACE__
#endif // MSTL_SORT_HPP__
