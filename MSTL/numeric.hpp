#ifndef MSTL_NUMERIC_HPP__
#define MSTL_NUMERIC_HPP__
#include <thread>
#include "functor.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__

// accumulate
template <typename Iterator, typename T>
	requires(input_iterator<Iterator>)
MSTL_CONSTEXPR T accumulate(Iterator first, Iterator second, T init) {
	for (; first != second; ++first) init = init + *first;
	return init;
}

template <typename Iterator, typename T, class BinaryOperation>
	requires(input_iterator<Iterator>)
MSTL_CONSTEXPR T accumulate(Iterator first, Iterator second, T init, BinaryOperation binary_op) {
	for (; first != second; ++first) init = binary_op(init, *first);
	return init;
}

// reduce
template <typename Iterator, typename BinaryOp, typename Result, size_t Threshhold = 10>
void reduce(Iterator first, Iterator last, BinaryOp op, Result& res) {
	size_t dist = distance(first, last);
	if (dist <= Threshhold) {
		for (Iterator it = first; it != last; ++it) 
			res = op(res, *it);
	}
	else {
		Iterator mid = next(first, dist / 2);
		Result l_res = res, r_res = res;
		std::thread r_thd(reduce<Iterator, BinaryOp, Result, Threshhold>, mid, last, op, MSTL::ref(r_res));
		reduce(first, mid, op, l_res);
		r_thd.join();
		res = op(l_res, r_res);
	}
}

// transform reduce
template <typename Iterator, typename UnaryOp, typename BinaryOp, typename Result, size_t Threshhold = 10>
void transform_reduce(Iterator first, Iterator last, UnaryOp transform, BinaryOp reduce, Result& res) {
	size_t dist = MSTL::distance(first, last);
	if (dist <= Threshhold) {
		for (Iterator it = first; it != last; ++it)
			res = reduce(res, transform(*it));
	}
	else {
		Iterator mid = MSTL::next(first, dist / 2);
		Result l_res = (initialize<Result>)(), r_res = (initialize<Result>)();
		std::thread r_thd(transform_reduce<Iterator, UnaryOp, BinaryOp, Result, Threshhold>,
			mid, last, transform, reduce, MSTL::ref(r_res));
		(transform_reduce)(first, mid, transform, reduce, l_res);
		r_thd.join();
		res = reduce(res, reduce(l_res, r_res));
	}
}

// adjacent difference
template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 adjacent_difference(Iterator1 first, Iterator1 last, Iterator2 result) {
	using T = typename iterator_traits<Iterator1>::value_type;
	if (first == last) return result;
	*result = *first;
	T value = *first;
	while (++first != last) {
		T tem = *first;
		*++result = tem - value;
		value = tem;
	}
	return ++result;
}

template <typename Iterator1, typename Iterator2, typename BinaryOperation>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 adjacent_difference(Iterator1 first, Iterator1 last,
	Iterator2 result, BinaryOperation binary_op) {
	if (first == last) return result;
	using T = typename iterator_traits<Iterator1>::value_type;
	*result = *first;
	T value = *first;
	while (++first != last) {
		T tem = *first;
		*++result = binary_op(tem, value);
		value = tem;
	}
	return ++result;
}

// inner product
template <typename Iterator1, typename Iterator2, typename T>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_CONSTEXPR T inner_product(Iterator1 first1, Iterator1 last1, Iterator2 first2, T init) {
	for (; first1 != last1; ++first1, ++first2) 
		init = init + ((*first1) * (*first2));
	return init;
}

template <typename Iterator1, typename Iterator2, typename T,
	typename BinaryOperation1, typename BinaryOperation2> 
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_CONSTEXPR T inner_product(Iterator1 first1, Iterator1 last1, Iterator2 first2, T init,
	BinaryOperation1 binary_op1, BinaryOperation2 binary_op2) {
	for (; first1 != last1; ++first1, ++first2) 
		init = binary_op1(init, binary_op2(*first1, *first2));
	return init;
}

// partial sum
template <typename Iterator1, typename Iterator2>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 partial_sum(Iterator1 first, Iterator1  last, Iterator2 result) {
	if (first == last) return result;
	using T = typename iterator_traits<Iterator1>::value_type;
	*result = *first;
	T value = *first;
	while (++first != last) {
		value = value + *first;
		*++result = value;
	}
	return ++result;
}

template <typename Iterator1, typename Iterator2, typename BinaryOperation>
	requires(input_iterator<Iterator1> && input_iterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 partial_sum(
	Iterator1 first, Iterator1 last, Iterator2 result, BinaryOperation binary_op) {
	if (first == last) return result;
	using T = typename iterator_traits<Iterator1>::value_type;
	*result = *first;
	T value = *first;
	while (++first != last) {
		value = binary_op(value, *first);
		*++result = value;
	}
	return ++result;
}

// iota
template <class ForwardIter, class T>
void iota(ForwardIter first, ForwardIter last, T value) {
	while (first != last) {
		*first++ = value;
		++value;
	}
}

MSTL_END_NAMESPACE__
#endif // MSTL_NUMERIC_HPP__
