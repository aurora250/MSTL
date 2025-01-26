#ifndef MSTL_HEAP_HPP__
#define MSTL_HEAP_HPP__
#include "iterator.hpp"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

// push heap
template <typename Iterator, typename Distance, typename T>
	requires(RandomAccessIterator<Iterator>)
void push_heap_aux(Iterator first, Distance hole_index, Distance top_index, T value) {
	Distance parent = (hole_index - 1) / 2;
	while (hole_index > top_index && *(first + parent) < value) {
		*(first + hole_index) = *(first + parent);
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
	*(first + hole_index) = value;
}

template <typename Iterator>
	requires(RandomAccessIterator<Iterator>)
inline void push_heap(Iterator first, Iterator last) {
	using Distance = std::iterator_traits<Iterator>::difference_type;
	MSTL::push_heap_aux(first, Distance((last - first) - 1), Distance(0), *(last - 1));
}

template <typename Iterator, typename Distance, typename T, typename Compare>
	requires(RandomAccessIterator<Iterator>)
void push_heap_aux(Iterator first, Distance hole_index, Distance top_index, T value, Compare comp) {
	Distance parent = (hole_index - 1) / 2;
	while (hole_index > top_index && comp(*(first + parent), value)) {
		*(first + hole_index) = *(first + parent);
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
	*(first + hole_index) = value;
}

template <typename Iterator, typename Compare>
	requires(RandomAccessIterator<Iterator>)
inline void push_heap(Iterator first, Iterator last, Compare comp) {
	using Distance = std::iterator_traits<Iterator>::difference_type;
	using T = std::iterator_traits<Iterator>::value_type;
	MSTL::push_heap_aux(first, Distance((last - first) - 1), Distance(0), *(last - 1), comp);
}
// adjust heap
template <typename Iterator, typename Distance, typename T>
	requires(RandomAccessIterator<Iterator>)
void adjust_heap(Iterator first, Distance hole_index, Distance len, T value) {
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
	MSTL::push_heap_aux(first, hole_index, top_index, value);
}

template <typename Iterator, typename Distance, typename T, typename Compare>
	requires(RandomAccessIterator<Iterator>)
void adjust_heap(Iterator first, Distance holeIndex, Distance len, T value, Compare comp) {
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
	MSTL::push_heap_aux(first, holeIndex, topIndex, value, comp);
}
// pop heap
template <typename Iterator, typename T>
	requires(RandomAccessIterator<Iterator>)
void pop_heap_aux(Iterator first, Iterator last, Iterator result, T value) {
	using Distance = std::iterator_traits<Iterator>::difference_type;
	*result = *first;
	MSTL::adjust_heap(first, Distance(0), Distance(last - first), value);
}

template <typename Iterator>
	requires(RandomAccessIterator<Iterator>)
inline void pop_heap(Iterator first, Iterator last) {
	using Distance = std::iterator_traits<Iterator>::difference_type;
	--last;
	MSTL::pop_heap_aux(first, last, last, *last);
}

template <typename Iterator, typename T, typename Compare>
	requires(RandomAccessIterator<Iterator>)
void pop_heap_aux(Iterator first, Iterator last, Iterator result, T value, Compare comp) {
	using Distance = std::iterator_traits<Iterator>::difference_type;
	*result = *first;
	MSTL::adjust_heap(first, Distance(0), Distance(last - first), value, comp);
}

template <typename Iterator, typename Compare>
	requires(RandomAccessIterator<Iterator>)
inline void pop_heap(Iterator first, Iterator last, Compare comp) {
	using Distance = std::iterator_traits<Iterator>::difference_type;
	--last;
	MSTL::pop_heap_aux(first, last, last, *last, comp);
}
// sort heap
template <typename Iterator>
	requires(RandomAccessIterator<Iterator>)
void sort_heap(Iterator first, Iterator last) {
	while (last - first > 1) 
		MSTL::pop_heap(first, last--);
}

template <typename Iterator, typename Compare>
	requires(RandomAccessIterator<Iterator>)
void sort_heap(Iterator first, Iterator last, Compare comp) {
	while (last - first > 1)
		MSTL::pop_heap(first, last--, comp);
}
// make heap
template <typename Iterator>
	requires(RandomAccessIterator<Iterator>)
void make_heap(Iterator first, Iterator last) {
	if (last - first < 2) return;
	using Distance = std::iterator_traits<Iterator>::difference_type;
	Distance len = last - first;
	Distance parent = (len - 2) / 2;
	while (true) {
		MSTL::adjust_heap(first, parent, len, *(first + parent));
		if (parent == 0) return;
		--parent;
	}
}

template <typename Iterator, typename Compare>
	requires(RandomAccessIterator<Iterator>)
void make_heap(Iterator first, Iterator last, Compare comp) {
	if (last - first < 2) return;
	using Distance = std::iterator_traits<Iterator>::difference_type;
	Distance len = last - first;
	Distance parent = (len - 2) / 2;
	while (true) {
		MSTL::adjust_heap(first, parent, len, *(first + parent), comp);
		if (parent == 0) return;
		--parent;
	}
}

MSTL_END_NAMESPACE__

#endif // HEAP_HPP__
