#ifndef MSTL_HEAP_HPP__
#define MSTL_HEAP_HPP__
#include "mathlib.h"
#include "vector.hpp"
MSTL_BEGIN_NAMESPACE__

// is heap
template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
bool is_heap(Iterator first, Iterator last) {
	using Distance = iter_dif_t<Iterator>;
	Distance n = MSTL::distance(first, last);
	Distance parent = 0;
	for (Distance child = 1; child < n; ++child) {
		if (*(first + parent) < *(first + child)) 
			return false;
		if ((child & 1) == 0) ++parent;
	}
	return true;
}

template <typename Iterator, typename Compare, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
bool is_heap(Iterator first, Iterator last, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	Distance n = MSTL::distance(first, last);
	Distance parent = 0;
	for (Distance child = 1; child < n; ++child) {
		if (comp(*(first + parent), *(first + child)))
			return false;
		if ((child & 1) == 0) ++parent;
	}
	return true;
}

// is heap until
template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
Iterator is_heap_until(Iterator first, Iterator last) {
	using Distance = iter_dif_t<Iterator>;
	Distance n = MSTL::distance(first, last);
	Distance parent = 0;
	for (Distance child = 1; child < n; ++child) {
		if (*(first + parent) < *(first + child)) {
			return first + child;
		}
		if ((child & 1) == 0) ++parent;
	}
	return last;
}

template <typename Iterator, typename Compare, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
Iterator is_heap_until(Iterator first, Iterator last, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	Distance n = MSTL::distance(first, last);
	Distance parent = 0;
	for (Distance child = 1; child < n; ++child) {
		if (comp(*(first + parent), *(first + child))) {
			return first + child;
		}
		if ((child & 1) == 0) ++parent;
	}
	return last;
}

// push heap
template <typename Iterator, typename Distance, typename T, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void push_heap_aux(Iterator first, Distance hole_index, Distance top_index, T value) {
	Distance parent = (hole_index - 1) / 2;
	while (hole_index > top_index && *(first + parent) < value) {
		*(first + hole_index) = *(first + parent);
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
	*(first + hole_index) = value;
}

template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
inline void push_heap(Iterator first, Iterator last) {
	using Distance = iter_dif_t<Iterator>;
	MSTL::push_heap_aux(first, Distance((last - first) - 1), Distance(0), *(last - 1));
}

template <typename Iterator, typename Distance, typename T, typename Compare, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void push_heap_aux(Iterator first, Distance hole_index, Distance top_index, T value, Compare comp) {
	Distance parent = (hole_index - 1) / 2;
	while (hole_index > top_index && comp(*(first + parent), value)) {
		*(first + hole_index) = *(first + parent);
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
	*(first + hole_index) = value;
}

template <typename Iterator, typename Compare, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
inline void push_heap(Iterator first, Iterator last, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	using T = iter_val_t<Iterator>;
	MSTL::push_heap_aux(first, Distance((last - first) - 1), Distance(0), *(last - 1), comp);
}
// adjust heap
template <typename Iterator, typename Distance, typename T, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
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

template <typename Iterator, typename Distance, typename T, typename Compare, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
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
template <typename Iterator, typename T, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void pop_heap_aux(Iterator first, Iterator last, Iterator result, T value) {
	using Distance = iter_dif_t<Iterator>;
	*result = *first;
	MSTL::adjust_heap(first, Distance(0), Distance(last - first), value);
}

template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
inline void pop_heap(Iterator first, Iterator last) {
	--last;
	MSTL::pop_heap_aux(first, last, last, *last);
}

template <typename Iterator, typename T, typename Compare, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void pop_heap_aux(Iterator first, Iterator last, Iterator result, T value, Compare comp) {
	using Distance = iter_dif_t<Iterator>;
	*result = *first;
	MSTL::adjust_heap(first, Distance(0), Distance(last - first), value, comp);
}

template <typename Iterator, typename Compare, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
inline void pop_heap(Iterator first, Iterator last, Compare comp) {
	--last;
	MSTL::pop_heap_aux(first, last, last, *last, comp);
}
// sort heap
template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void sort_heap(Iterator first, Iterator last) {
	while (last - first > 1) 
		MSTL::pop_heap(first, last--);
}

template <typename Iterator, typename Compare, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void sort_heap(Iterator first, Iterator last, Compare comp) {
	while (last - first > 1)
		MSTL::pop_heap(first, last--, comp);
}
// make heap
template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void make_heap(Iterator first, Iterator last) {
	if (last - first < 2) return;
	using Distance = iter_dif_t<Iterator>;
	Distance len = last - first;
	Distance parent = (len - 2) / 2;
	while (true) {
		MSTL::adjust_heap(first, parent, len, *(first + parent));
		if (parent == 0) return;
		--parent;
	}
}

template <typename Iterator, typename Compare, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void make_heap(Iterator first, Iterator last, Compare comp) {
	if (last - first < 2) return;
	using Distance = iter_dif_t<Iterator>;
	Distance len = last - first;
	Distance parent = (len - 2) / 2;
	while (true) {
		MSTL::adjust_heap(first, parent, len, *(first + parent), comp);
		if (parent == 0) return;
		--parent;
	}
}


template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void adjust_leonardo_heap(Iterator first, size_t current_heap, int level_index, vector<int>& levels) {
	size_t child_heap1;
	size_t child_heap2;
	while (level_index > 0) {
		size_t prev_heap = current_heap - LEONARDO_LIST[levels[level_index]];
		if (*(first + current_heap) < *(first + prev_heap)) {
			if (levels[level_index] > 1) {
				child_heap1 = current_heap - 1 - LEONARDO_LIST[levels[level_index] - 2];
				child_heap2 = current_heap - 1;
				if (*(first + prev_heap) < *(first + child_heap1)) break;
				if (*(first + prev_heap) < *(first + child_heap2)) break;
			}
			MSTL::iter_swap(first + current_heap, first + prev_heap);
			current_heap = prev_heap;
			--level_index;
		}
		else {
			break;
		}
	}
	int current_level = levels[level_index];
	while (current_level > 1) {
		size_t max_child = current_heap;
		child_heap1 = current_heap - 1 - LEONARDO_LIST[current_level - 2];
		child_heap2 = current_heap - 1;

		if (*(first + max_child) < *(first + child_heap1)) max_child = child_heap1;
		if (*(first + max_child) < *(first + child_heap2)) max_child = child_heap2;

		if (max_child == child_heap1) {
			MSTL::iter_swap(first + current_heap, first + child_heap1);
			current_heap = child_heap1;
			--current_level;
		}
		else if (max_child == child_heap2) {
			MSTL::iter_swap(first + current_heap, first + child_heap2);
			current_heap = child_heap2;
			current_level -= 2;
		}
		else {
			break;
		}
	}
}

template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void push_leonardo_heap(Iterator first, Iterator last) {
	if (first == last) return;
	size_t size = MSTL::distance(first, last);
	vector<int> levels = { 1 };
	int toplevel = 0;
	for (size_t i = 1; i < size - 1; ++i) {
		if (toplevel > 0 && levels[toplevel - 1] - levels[toplevel] == 1) {
			--toplevel;
			++levels[toplevel];
		}
		else if (levels[toplevel] != 1) {
			++toplevel;
			levels.push_back(1);
		}
		else {
			++toplevel;
			levels.push_back(0);
		}
	}
	if (toplevel > 0 && levels[toplevel - 1] - levels[toplevel] == 1) {
		--toplevel;
		++levels[toplevel];
	}
	else if (levels[toplevel] != 1) {
		++toplevel;
		levels.push_back(1);
	}
	else {
		++toplevel;
		levels.push_back(0);
	}
	MSTL::adjust_leonardo_heap(first, size - 1, toplevel, levels);
}

template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void pop_leonardo_heap(Iterator first, Iterator last) {
	if (first == last) return;
	size_t size = MSTL::distance(first, last);
	vector<int> levels = { 1 };
	int toplevel = 0;
	for (size_t i = 1; i < size; ++i) {
		if (toplevel > 0 && levels[toplevel - 1] - levels[toplevel] == 1) {
			--toplevel;
			++levels[toplevel];
		}
		else if (levels[toplevel] != 1) {
			++toplevel;
			levels.push_back(1);
		}
		else {
			++toplevel;
			levels.push_back(0);
		}
		MSTL::adjust_leonardo_heap(first, i, toplevel, levels);
	}
	if (levels[toplevel] <= 1) {
		--toplevel;
	}
	else {
		--levels[toplevel];
		levels.push_back(levels[toplevel] - 1);
		++toplevel;
		MSTL::adjust_leonardo_heap(first, size - 2 - LEONARDO_LIST[levels[toplevel]], toplevel - 1, levels);
		MSTL::adjust_leonardo_heap(first, size - 2, toplevel, levels);
	}
}

template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void sort_leonardo_heap(Iterator first, Iterator last) {
	if (first == last) return;
	size_t size = MSTL::distance(first, last);
	vector<int> levels = { 1 };
	int toplevel = 0;
	for (size_t i = 1; i < size; ++i) {
		if (toplevel > 0 && levels[toplevel - 1] - levels[toplevel] == 1) {
			--toplevel;
			++levels[toplevel];
		}
		else if (levels[toplevel] != 1) {
			++toplevel;
			levels.push_back(1);
		}
		else {
			++toplevel;
			levels.push_back(0);
		}
		MSTL::adjust_leonardo_heap(first, i, toplevel, levels);
	}
	for (size_t i = size - 2; i > 0; --i) {
		if (levels[toplevel] <= 1) {
			--toplevel;
		}
		else {
			--levels[toplevel];
			levels.push_back(levels[toplevel] - 1);
			++toplevel;

			MSTL::adjust_leonardo_heap(first, i - LEONARDO_LIST[levels[toplevel]], toplevel - 1, levels);
			MSTL::adjust_leonardo_heap(first, i, toplevel, levels);
		}
	}
}

template <typename Iterator, enable_if_t<
	is_rnd_iter_v<Iterator>, int> = 0>
void make_leonardo_heap(Iterator first, Iterator last) {
	if (first == last) return;
	size_t size = MSTL::distance(first, last);
	vector<int> levels = { 1 };
	int toplevel = 0;

	for (size_t i = 1; i < size; ++i) {
		if (toplevel > 0 && levels[toplevel - 1] - levels[toplevel] == 1) {
			--toplevel;
			++levels[toplevel];
		}
		else if (levels[toplevel] != 1) {
			++toplevel;
			levels.push_back(1);
		}
		else {
			++toplevel;
			levels.push_back(0);
		}
		MSTL::adjust_leonardo_heap(first, i, toplevel, levels);
	}
}

MSTL_END_NAMESPACE__

#endif // HEAP_HPP__
