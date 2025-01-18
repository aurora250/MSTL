#ifndef MSTL_NUMERIC_HPP__
#define MSTL_NUMERIC_HPP__
#include "functor.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

template <typename Iterator, typename T>
	requires(InputIterator<Iterator>)
MSTL_CONSTEXPR T accumulate(Iterator first, Iterator second, T init)
noexcept(noexcept(first != second) && noexcept(++first) && noexcept(init = init + *first)) {
	for (; first != second; ++first) init = init + *first;
	return init;
}
template <typename Iterator, typename T, class BinaryOperation>
	requires(InputIterator<Iterator> && BinaryFunction<BinaryOperation>)
MSTL_CONSTEXPR T accumulate(Iterator first, Iterator second, T init, BinaryOperation binary_op) {
	for (; first != second; ++first) init = binary_op(init, *first);
	return init;
}

template <typename Iterator1, typename Iterator2>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 adjacent_difference(Iterator1 first, Iterator1 last, Iterator2 result) {
	using T = typename std::iterator_traits<Iterator1>::value_type;
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
	requires(InputIterator<Iterator1> && InputIterator<Iterator2> && BinaryFunction<BinaryOperation>)
MSTL_CONSTEXPR Iterator2 adjacent_difference(Iterator1 first, Iterator1 last,
	Iterator2 result, BinaryOperation binary_op) {
	if (first == last) return result;
	using T = typename std::iterator_traits<Iterator1>::value_type;
	*result = *first;
	T value = *first;
	while (++first != last) {
		T tem = *first;
		*++result = binary_op(tem, value);
		value = tem;
	}
	return ++result;
}

template <typename Iterator1, typename Iterator2, typename T>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2>)
MSTL_CONSTEXPR T inner_product(Iterator1 first1, Iterator1 last1, Iterator2 first2, T init) {
	for (; first1 != last1; ++first1, ++first2) 
		init = init + ((*first1) * (*first2));
	return init;
}
template <typename Iterator1, typename Iterator2, typename T,
	typename BinaryOperation1, typename BinaryOperation2> 
	requires(InputIterator<Iterator1> && InputIterator<Iterator2>
		&& BinaryFunction<BinaryOperation1> && BinaryFunction<BinaryOperation2>)
MSTL_CONSTEXPR T inner_product(Iterator1 first1, Iterator1 last1, Iterator2 first2, T init,
	BinaryOperation1 binary_op1, BinaryOperation2 binary_op2) {
	for (; first1 != last1; ++first1, ++first2) 
		init = binary_op1(init, binary_op2(*first1, *first2));
	return init;
}

template <typename Iterator1, typename Iterator2>
	requires(InputIterator<Iterator1>&& InputIterator<Iterator2>)
MSTL_CONSTEXPR Iterator2 partial_sum(Iterator1 first, Iterator1  last, Iterator2 result) {
	if (first == last) return result;
	using T = typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<Iterator1>::value_type;
	*result = *first;
	T value = *first;
	while (++first != last) {
		value = value + *first;
		*++result = value;
	}
	return ++result;
}
template <typename Iterator1, typename Iterator2, typename BinaryOperation>
	requires(InputIterator<Iterator1> && InputIterator<Iterator2>&& BinaryFunction<BinaryOperation>)
MSTL_CONSTEXPR Iterator2 partial_sum(
	Iterator1 first, Iterator1 last, Iterator2 result, BinaryOperation binary_op) {
	if (first == last) return result;
	using T = typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<Iterator1>::value_type;
	*result = *first;
	T value = *first;
	while (++first != last) {
		value = binary_op(value, *first);
		*++result = value;
	}
	return ++result;
}

MSTL_END_NAMESPACE__

#endif // MSTL_NUMERIC_HPP__
