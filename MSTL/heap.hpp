#ifndef MSTL_HEAP_HPP__
#define MSTL_HEAP_HPP__
#include "basiclib.h"
#include "type_traits.hpp"

MSTL_BEGIN_NAMESPACE__

template <typename RandomAccessIterator, typename Distance, typename T>
void __push_heap(RandomAccessIterator first, Distance hole_index, Distance top_index, T value) {
	Distance parent = (hole_index - 1) / 2;
	while (hole_index > top_index && *(first + parent) < value) {
		*(first + hole_index) = *(first + parent);
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
	*(first + hole_index) = value;
}
template <typename RandomAccessIterator, typename Distance, typename T>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*) {
	__push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
}
template <typename RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
	__push_heap_aux(first, last, (distance_type)(first), (value_type)(first));
}
template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void __push_heap(RandomAccessIterator first, Distance holeIndex,
	Distance topIndex, T value, Compare comp) {
	Distance parent = (holeIndex - 1) / 2;
	while (holeIndex > topIndex && comp(*(first + parent), value)) {
		*(first + holeIndex) = *(first + parent);
		holeIndex = parent;
		parent = (holeIndex - 1) / 2;
	}
	*(first + holeIndex) = value;
}
template <typename RandomAccessIterator, typename Compare, typename Distance, typename T>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, 
	Compare comp, Distance*, T*) {
	__push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)), comp);
}
template <typename RandomAccessIterator, typename Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last,
	Compare comp) {
	__push_heap_aux(first, last, comp, (distance_type)(first), (value_type)(first));
}
template <typename RandomAccessIterator, typename Distance, typename T>
void __adjust_heap(RandomAccessIterator first, Distance hole_index, Distance len, T value) {
	Distance top_index = hole_index;
	Distance second_child = 2 * hole_index + 2;
	while (second_child < len) {
		if (*(first + second_child) < *(first + (second_child - 1))) --second_child;
		*(first + hole_index) = *(first + second_child);
		hole_index = second_child;
		second_child = 2 * (second_child + 1);
	}
	if (second_child == len) {
		*(first + hole_index) = *(first + (second_child - 1));
		hole_index = second_child - 1;
	}
	__push_heap(first, hole_index, top_index, value);
}
template <typename RandomAccessIterator, typename Distance, typename T>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
	RandomAccessIterator result, T value, Distance*) {
	*result = *first;
	__adjust_heap(first, Distance(0), Distance(last - first), value);
}
template <typename RandomAccessIterator, typename T>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
	__pop_heap(first, last - 1, last - 1, T(*(last - 1)), (distance_type)(first));
}
template <typename RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
	__pop_heap_aux(first, last, (value_type)(first));
}
template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value, Compare comp) {
	Distance topIndex = holeIndex;
	Distance secondChild = 2 * holeIndex + 2;
	while (secondChild < len) {
		if (comp(*(first + secondChild), *(first + (secondChild - 1))))
			secondChild--;
		*(first + holeIndex) = *(first + secondChild);
		holeIndex = secondChild;
		secondChild = 2 * (secondChild + 1);
	}
	if (secondChild == len) {
		*(first + holeIndex) = *(first + (secondChild - 1));
		holeIndex = secondChild - 1;
	}
	__push_heap(first, holeIndex, topIndex, value, comp);
}
template <typename RandomAccessIterator, typename T, typename Compare, typename Distance>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
	RandomAccessIterator result, T value, Compare comp, Distance*) {
	*result = *first;
	__adjust_heap(first, Distance(0), Distance(last - first), value, comp);
}
template <typename RandomAccessIterator, typename T, typename Compare>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*, Compare comp) {
	__pop_heap(first, last - 1, last - 1, T(*(last - 1)), comp, (distance_type)(first));
}
template <typename RandomAccessIterator, typename Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last,
	Compare comp) {
	__pop_heap_aux(first, last, (value_type)(first), comp);
}
template <typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
	while (last - first > 1) MSTL::pop_heap(first, last--);
}
template <typename RandomAccessIterator, typename Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last,
	Compare comp) {
	while (last - first > 1) MSTL::pop_heap(first, last--, comp);
}
template <typename RandomAccessIterator, typename Distance, typename T>
void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*) {
	if (last - first < 2)return;
	Distance len = last - first;
	Distance parent = (len - 2) / 2;
	while (true) {
		__adjust_heap(first, parent, len, T(*(first + parent)));
		if (parent == 0) return;
		--parent;
	}
}
template <typename RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
	__make_heap(first, last, (value_type)(first), (distance_type)(last));
}
template <typename RandomAccessIterator, typename Compare, typename T, typename Distance>
void __make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp, T*, Distance*) {
	if (last - first < 2) return;
	Distance len = last - first;
	Distance parent = (len - 2) / 2;
	while (true) {
		__adjust_heap(first, parent, len, T(*(first + parent)), comp);
		if (parent == 0) return;
		parent--;
	}
}
template <typename RandomAccessIterator, typename Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last,
	Compare comp) {
	__make_heap(first, last, comp, (value_type)(first), (distance_type)(first));
}

MSTL_END_NAMESPACE__

#endif // HEAP_HPP__
