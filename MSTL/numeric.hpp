#ifndef MSTL_NUMERIC_HPP__
#define MSTL_NUMERIC_HPP__
#include <thread>
#include "functor.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Iterator, typename T, typename BinaryOperation, enable_if_t<
	is_input_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR T accumulate(Iterator first, Iterator second, T init, BinaryOperation binary_op) {
	for (; first != second; ++first) init = binary_op(init, *first);
	return init;
}

template <typename Iterator, typename T>
MSTL_CONSTEXPR T accumulate(Iterator first, Iterator second, T init) {
	return MSTL::accumulate(first, second, init, MSTL::plus<T>());
}

template <typename Iterator, typename BinaryOperation, typename Result, 
	size_t Threshhold = 10, enable_if_t<is_input_iter_v<Iterator>, int> = 0>
void reduce(Iterator first, Iterator last, BinaryOperation op, Result& res) {
	size_t dist = MSTL::distance(first, last);
	if (dist <= Threshhold) {
		for (Iterator it = first; it != last; ++it) 
			res = op(res, *it);
	}
	else {
		Iterator mid = next(first, dist / 2);
		Result l_res = res, r_res = res;
		std::thread r_thd(reduce<Iterator, BinaryOperation, Result, Threshhold>, mid, last, op, MSTL::ref(r_res));
		MSTL::reduce(first, mid, op, l_res);
		r_thd.join();
		res = op(l_res, r_res);
	}
}

template <typename Iterator, typename UnaryOperation, typename BinaryOp, typename Result,
	size_t Threshhold = 10, enable_if_t<is_input_iter_v<Iterator>, int> = 0>
void transform_reduce(Iterator first, Iterator last, UnaryOperation transform, BinaryOp reduce, Result& res) {
	size_t dist = MSTL::distance(first, last);
	if (dist <= Threshhold) {
		for (Iterator it = first; it != last; ++it)
			res = reduce(res, transform(*it));
	}
	else {
		Iterator mid = MSTL::next(first, dist / 2);
		Result l_res = MSTL::initialize<Result>(), r_res = MSTL::initialize<Result>();
		std::thread r_thd(transform_reduce<Iterator, UnaryOperation, BinaryOp, Result, Threshhold>,
			mid, last, transform, reduce, MSTL::ref(r_res));
		MSTL::transform_reduce(first, mid, transform, reduce, l_res);
		r_thd.join();
		res = reduce(res, reduce(l_res, r_res));
	}
}

template <typename Iterator1, typename Iterator2, typename BinaryOperation, enable_if_t<
	is_input_iter_v<Iterator1> && is_input_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR Iterator2 adjacent_difference(Iterator1 first, Iterator1 last,
	Iterator2 result, BinaryOperation binary_op) {
	if (first == last) return result;
	using T = iter_val_t<Iterator1>;
	*result = *first;
	T value = *first;
	while (++first != last) {
		T tem = *first;
		*++result = binary_op(tem, value);
		value = tem;
	}
	return ++result;
}

template <typename Iterator1, typename Iterator2>
MSTL_CONSTEXPR Iterator2 adjacent_difference(Iterator1 first, Iterator1 last, Iterator2 result) {
	return MSTL::adjacent_difference(first, last, result, MSTL::minus<iter_val_t<Iterator1>>());
}

template <typename Iterator1, typename Iterator2, typename T,
	typename BinaryOperation1, typename BinaryOperation2, enable_if_t<
	is_input_iter_v<Iterator1> && is_input_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR T inner_product(Iterator1 first1, Iterator1 last1, Iterator2 first2, T init,
	BinaryOperation1 binary_op1, BinaryOperation2 binary_op2) {
	for (; first1 != last1; ++first1, ++first2) 
		init = binary_op1(init, binary_op2(*first1, *first2));
	return init;
}

template <typename Iterator1, typename Iterator2, typename T>
MSTL_CONSTEXPR T inner_product(Iterator1 first1, Iterator1 last1, Iterator2 first2, T init) {
	return MSTL::inner_product(first1, last1, first2, init, MSTL::plus<T>(), MSTL::multiplies<T>());
}

template <typename Iterator1, typename Iterator2, typename BinaryOperation, enable_if_t<
	is_input_iter_v<Iterator1> && is_input_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR Iterator2 partial_sum(
	Iterator1 first, Iterator1 last, Iterator2 result, BinaryOperation binary_op) {
	if (first == last) return result;
	*result = *first;
	iter_val_t<Iterator1> value = *first;
	while (++first != last) {
		value = binary_op(value, *first);
		*++result = value;
	}
	return ++result;
}

template <typename Iterator1, typename Iterator2>
MSTL_CONSTEXPR Iterator2 partial_sum(Iterator1 first, Iterator1 last, Iterator2 result) {
	return MSTL::partial_sum(first, last, result, MSTL::plus<iter_val_t<Iterator1>>());
}

template <typename Iterator, typename T, enable_if_t<
	is_input_iter_v<Iterator>, int> = 0>
void iota(Iterator first, Iterator last, T value) {
	while (first != last) {
		*first++ = value;
		++value;
	}
}

MSTL_END_NAMESPACE__
#endif // MSTL_NUMERIC_HPP__
