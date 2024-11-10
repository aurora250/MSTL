#ifndef MSTL_NUMERIC_HPP__
#define MSTL_NUMERIC_HPP__
#include "functor.hpp"

MSTL_BEGIN_NAMESPACE__

template <typename InputIterator, typename T>
T accumulate(InputIterator first, InputIterator second, T init) {
	for (; first != second; ++first) init = init + *first;   // 将迭代器内每个值累加到init上
	return init;
}
template <typename InputIterator, typename T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator second, T init, BinaryOperation binary_op) {
	for (; first != second; ++first) init = binary_op(init, *first);
	return init;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result) {
	using T = std::iterator_traits<InputIterator>::value_type;
	if (first == last) return result;
	*result = *first;
	T value = *first;
	while (++first != last) {
		T tem = *first;
		*++result = tem - value;    // 获取迭代器内的差值
		value = tem;
	}
	return ++result;
}
template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation binary_op) {
	if (first == last) return result;
	using T = std::iterator_traits<InputIterator>::value_type;
	*result = *first;
	T value = *first;
	while (++first != last) {
		T tem = *first;
		*++result = binary_op(tem, value);
		value = tem;
	}
	return ++result;
}

template <typename InputIterator1, typename InputIterator2, typename T>
T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init) {
	for (; first1 != last1; ++first1, ++first2) init = init + (*first1 * *first2);   // 获取两个迭代器内积之和
	return init;
}
template <typename InputIterator1, typename InputIterator2, typename T,
	typename BinaryOperation1, typename BinaryOperation2>
T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init,
	BinaryOperation1 binary_op1, BinaryOperation2 binary_op2) {
	for (; first1 != last1; ++first1, ++first2) init = binary_op1(init, binary_op2(*first1, *first2));
	return init;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator  last, OutputIterator result) {
	if (first == last) return result;
	using T = std::iterator_traits<InputIterator>::value_type;
	*result = *first;
	T value = *first;
	while (++first != last) {
		value = value + *first;   // 获取迭代器部分和
		*++result = value;
	}
	return ++result;
}
template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
OutputIterator partial_sum(InputIterator first, InputIterator  last, OutputIterator result, BinaryOperation binary_op) {
	if (first == last) return result;
	using T = std::iterator_traits<InputIterator>::value_type;
	*result = *first;
	T value = *first;
	while (++first != last) {
		value = binary_op(value, *first);
		*++result = value;
	}
	return ++result;
}

template <typename T, typename Integer>
inline T power(T x, Integer n) {
	return power(x, n, multiplies<T>());
}
template <typename T, typename Integer, typename MonoidOperation>
T power(T x, Integer n, MonoidOperation op) {
	if (n == 0) return identity_element(multiplies<T>());
	while ((n & 1) == 0) {
		n >>= 1;
		x = op(x, x);
	}
	T result = x;
	x >>= 1;  // 右移一位并赋值
	while (n != 0) {
		x = op(x, x);
		if ((n & 1) != 0) result = op(result, x);
		n >>= 1;
	}
	return result;
}

template <typename ForwardIterator, typename T>
void iota(ForwardIterator first, ForwardIterator last, T value) {
	while (first != last) *first++ = value++;   // 设置迭代器部分为value累进
}

MSTL_END_NAMESPACE__

#endif // MSTL_NUMERIC_HPP__

