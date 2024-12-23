#ifndef MSTL_FUNCTOR_HPP__
#define MSTL_FUNCTOR_HPP__
#include "basiclib.h"
#include "concepts.hpp"

MSTL_BEGIN_NAMESPACE__
using namespace concepts;

template <typename Arg, typename Result>
struct unary_function {
	typedef Arg		argument_type;
	typedef Result  result_type;
};
template <typename Arg1, typename Arg2, typename Result>
struct binary_function {
	typedef Arg1	first_argument_type;
	typedef Arg2	second_argument_type;
	typedef Result  result_type;
};

//arithmetic:
template <typename T> requires(Addable<T, T>)
struct plus : public binary_function<T, T, T> {
	T operator()(const T& x, const T& y) const { return x + y; }
};
template <typename T>
struct minus : public binary_function<T, T, T> {
	T operator()(const T& x, const T& y) const { return x - y; }
};
template <typename T>
struct multiplies : public binary_function<T, T, T> {
	T operator()(const T& x, const T& y) const { return x * y; }
};
template <typename T>
struct divides : public binary_function<T, T, T> {
	T operator()(const T& x, const T& y) const { return x / y; }
};
template <typename T>
struct modulus : public binary_function<T, T, T> {
	T operator()(const T& x, const T& y) const { return x % y; }
};
template <typename T>
struct negate : public unary_function<T, T> {
	T operator()(const T& x) const { return -x; }
};

//identity element:
template <typename T>
inline T identity_element(plus<T>) { return T(0); }
template <typename T>
inline T indetity_element(multiplies<T>) { return T(1); }

//relational:
template <typename T>
struct equal_to : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x == y; }
};
template <typename T>
struct not_equal_to : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x != y; }
};
template <typename T>
struct greater : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x > y; }
};
template <typename T>
struct less : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x < y; }
};
template <typename T>
struct greater_equal : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x >= y; }
};
template <typename T>
struct less_equal : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x <= y; }
};

//logical:
template <typename T>
struct logical_and : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x && y; }
};
template <typename T>
struct logical_or : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x || y; }
};
template <typename T>
struct logical_not : public unary_function<T, bool> {
	bool operator()(const T& x) const { return !x; }
};

//others:
template <typename T>
struct identity : public unary_function<T, T> {
	const T& operator()(const T& x) const { return x; }
};

template <typename Pair>
struct select1st : public unary_function<Pair, typename Pair::first_type> {
	typename Pair::first_type& operator()(const Pair& x) const { return x.first; }
};
template <typename Pair>
struct select2nd : public unary_function<Pair, typename Pair::second_type> {
	typename Pair::second_type& operator()(const Pair& x) const { return x.second; }
};
template <typename Arg1, typename Arg2>
struct project1st : public binary_function<Arg1, Arg2, Arg1> {
	Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
};
template <typename Arg1, typename Arg2>
struct project2nd : public binary_function<Arg1, Arg2, Arg2> {
	Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
};


struct display {
	const display& operator ()() { return *this; }
	void operator()(const auto& value) { std::cout << value << std::flush; }
};
struct display_enter {
	const display_enter& operator ()() { return *this; }
	void operator ()(const auto& value) { std::cout << value << std::endl; }
};

MSTL_END_NAMESPACE__

#endif // MSTL_FUNCTOR_HPP__
