﻿#ifndef MSTL_MATHLIB_H__
#define MSTL_MATHLIB_H__
#include "errorlib.hpp"
#include "type_traits.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_FINAL_CLASS(MathError, ValueError, "Math Function Argument Invalid.")

using maths_t   = int16_t;
using mathus_t	= uint16_t;
using mathi_t	= int32_t;
using mathui_t	= uint32_t;
using mathl_t	= int64_t;
using mathul_t	= uint64_t;
using mathd_t	= float64_t;
using mathld_t	= decimal_t;

namespace constants {
	static constexpr mathld_t EULER = 2.718281828459045L;
	static constexpr mathld_t PI = 3.141592653589793L;  // radian
	static constexpr mathld_t PHI = 1.618033988749895L;
	static constexpr mathld_t SEMI_CIRCLE = 180.0;  // angular
	static constexpr mathld_t CIRCLE = 360.0;
	static constexpr mathld_t EPSILON = 1e-15L;

	static constexpr mathui_t TAYLOR_CONVERGENCE = 10000U;
	static constexpr mathld_t PRECISE_TOLERANCE = TAYLOR_CONVERGENCE * EPSILON;
	static constexpr mathld_t LOW_PRECISE_TOLERANCE = TAYLOR_CONVERGENCE * PRECISE_TOLERANCE;

	static constexpr mathul_t INFINITY_INT = 18446744073709551615ULL;
	static constexpr mathld_t INFINITY_FLT = 1.7976931348623158e+308L;

	static constexpr mathus_t FIBONACCI_COUNT = 50;
	static constexpr mathul_t FIBONACCI_LIST[FIBONACCI_COUNT] = {
		0,			1,			1,			2,			3,
		5,			8,			13,			21,			34,
		55,			89,			144,		233,		377,
		610,		987,		1597,		2584,		4181,
		6765,		10946,		17711,		28657,		46368,
		75025,		121393,		196418,		317811,		514229,
		832040,		1346269,	2178309,	3524578,	5702887,
		9227465,	14930352,	24157817,	39088169,	63245986,
		102334155,	165580141,	267914296,	433494437,	701408733,
		1134903170, 1836311903, 2971215073, 4807526976, 7778742049
	};
}

MSTL_PURE_FUNCTION constexpr mathul_t fibonacci(const mathus_t n) {
	if (n < constants::FIBONACCI_COUNT) return constants::FIBONACCI_LIST[n];
	return fibonacci(n - 1) + fibonacci(n - 2);
}
MSTL_PURE_FUNCTION constexpr mathul_t leonardo(const mathus_t n) {
	return 2 * fibonacci(n + 1) - 1;
}

MSTL_PURE_FUNCTION constexpr mathld_t angular2radian(const mathld_t angular) noexcept {
	return angular * constants::PI / constants::SEMI_CIRCLE;
}
MSTL_PURE_FUNCTION constexpr mathld_t radian2angular(const mathld_t radian) noexcept {
	return radian * (constants::SEMI_CIRCLE / constants::PI);
}

template <typename T, enable_if_t<is_signed_v<T>, int> = 0>
MSTL_CONST_FUNCTION constexpr T opposite(T x) noexcept {
	return -x;
}
template <typename T, enable_if_t<is_signed_v<T>, int> = 0>
MSTL_CONST_FUNCTION constexpr T absolute(T x) noexcept {
	return x > 0 ? x : -x;
}

template <typename T>
MSTL_CONST_FUNCTION constexpr decltype(auto) sum_n(T x) noexcept {
	return x;
}

template <typename First, typename... Rests, enable_if_t<(sizeof...(Rests) > 0), int> = 0>
MSTL_CONST_FUNCTION constexpr decltype(auto) sum_n(First first, Rests... args) noexcept {
	return first + sum_n(args...);
}

template <typename... Args, enable_if_t<(sizeof...(Args) > 0), int> = 0>
MSTL_CONST_FUNCTION constexpr mathld_t average(Args... args) noexcept {
	return sum_n(args...) * 1.0 / sizeof...(Args);
}

template <typename T, enable_if_t<is_integral_v<T> && is_unsigned_v<T>, int> = 0>
MSTL_CONST_FUNCTION constexpr T gcd(T m, T n) noexcept { // greatest common divisor
	while (n != 0) {
		T t = m % n;
		m = n;
		n = t;
	}
	return m;
}

template <typename T, enable_if_t<is_integral_v<T> && is_unsigned_v<T>, int> = 0>
MSTL_CONST_FUNCTION  T lcm(T m, T n) noexcept { // least common multiple
	return m * n / _MSTL gcd(m, n);
}

inline mathld_t float_mod(const mathld_t x, const mathld_t y) {
	if (y == 0) Exception(MathError("zero can not be dividend."));
	const mathld_t result = x - static_cast<int>(x / y) * y;
	return result;
}


template <typename T>
MSTL_CONST_FUNCTION constexpr T square(T x) noexcept { return x * x; }

template <typename T>
MSTL_CONST_FUNCTION constexpr T cube(T x) noexcept { return _MSTL square(x) * x; }

template <typename T, enable_if_t<is_integral_v<T>, int> = 0>
MSTL_CONST_FUNCTION constexpr mathl_t power(T x, mathui_t n) noexcept {
	if (n == 0) return 1;
	mathl_t result = 1;
	T base = x;
	while (n > 0) {
		if (n % 2 == 1)
			result *= base;
		base *= base;
		n /= 2;
	}
	return result;
}

template <typename T, enable_if_t<is_floating_point_v<T>, int> = 0>
MSTL_CONST_FUNCTION constexpr mathld_t power(T x, mathui_t n) noexcept {
	if (n == 0) return 1;
	mathld_t result = 1.0;
	T base = x;
	while (n > 0) {
		if (n % 2 == 1)
			result *= base;
		base *= base;
		n /= 2;
	}
	return result;
}

MSTL_PURE_FUNCTION constexpr mathld_t exponential(const mathui_t n) noexcept {
	return power(constants::EULER, n);
}

MSTL_PURE_FUNCTION constexpr mathld_t logarithm_e(const mathld_t x) noexcept {
	mathui_t N = constants::TAYLOR_CONVERGENCE;
	const mathld_t a = (x - 1) / (x + 1);
	const mathld_t a_sqrt = a * a;
	mathld_t nk = 2 * N + 1;
	mathld_t y = 1.0 / nk;
	while (N--) {
		nk -= 2;
		y = 1.0 / nk + a_sqrt * y;
	}
	return 2.0 * a * y;
}

MSTL_PURE_FUNCTION constexpr mathld_t logarithm(const mathld_t x, const mathui_t base) noexcept {
	return logarithm_e(x) / logarithm_e(base);
}
MSTL_PURE_FUNCTION constexpr mathld_t logarithm_2(const mathld_t x) noexcept {
	return logarithm(x, 2);
}
MSTL_PURE_FUNCTION constexpr mathld_t logarithm_10(const mathld_t x) noexcept {
	return logarithm(x, 10);
}
MSTL_CONST_FUNCTION constexpr mathul_t cursory_lg2(mathul_t x) noexcept {
	mathul_t k = 0;
	for (; x > 1; x >>= 1) ++k;
	return k;
}

MSTL_PURE_FUNCTION constexpr mathld_t
square_root(const mathld_t x, const mathld_t precise = constants::PRECISE_TOLERANCE) noexcept {
	mathld_t t = 0.0;
	mathld_t result = x;
	while (absolute(result - t) > precise) {
		t = result;
		result = 0.5 * (t + x / t);
	}
	return result;
}
MSTL_PURE_FUNCTION constexpr mathld_t
cube_root(const mathld_t x, const mathld_t precise = constants::PRECISE_TOLERANCE) noexcept {
	mathld_t t = 0.0;
	mathld_t result = x;
	while (absolute(result - t) > precise) {
		t = result;
		result = (2 * t + x / (t * t)) / 3;
	}
	return result;
}

MSTL_CONST_FUNCTION constexpr mathul_t factorial(const mathui_t n) noexcept {
	mathul_t h = 1;
	for (mathui_t i = 1; i <= n; i++)
		h *= i;
	return h;
}

// a bit down to the nearest digit, > 0 operates on decimal places, and 0 on integer places.
MSTL_CONST_FUNCTION constexpr mathld_t floor_bit(const mathld_t x, const mathui_t bit) noexcept {
	const mathld_t times = power(10.0, bit);
	const auto int_part = x * times;
	if (x < 0 && x * times * 10 / 10.0 != int_part)
		return (int_part - 1) / times;
	return int_part / times;
}
MSTL_CONST_FUNCTION constexpr mathld_t ceil_bit(const mathld_t x, const mathui_t bit) noexcept {
	const mathld_t times = power(10.0, bit);
	const auto int_part = x * times;
	if (x > 0 && x * times * 10 / 10.0 != int_part)
		return (int_part + 1) / times;
	return int_part / times;
}

MSTL_CONST_FUNCTION constexpr mathld_t round_bit(const mathld_t x, const mathui_t bit) noexcept {
	return x < 0 ? ceil_bit(x - 0.5 / power(10.0, bit), bit) 
		: floor_bit(x + 0.5 / power(10.0, bit), bit);
}
MSTL_CONST_FUNCTION constexpr mathld_t truncate_bit(const mathld_t x, const mathui_t bit) noexcept {
	return x < 0 ? ceil_bit(x, bit) : floor_bit(x, bit);
}
MSTL_CONST_FUNCTION constexpr mathld_t floor(const mathld_t x, const mathui_t bit) noexcept {
	const mathld_t times = power(10.0, bit);
	const auto int_part = x * times;
	if (x < 0 && x * times != int_part)
		return (int_part - 1) / times;
	return int_part / times;
}
MSTL_CONST_FUNCTION constexpr mathld_t ceil(const mathld_t x, const mathui_t bit) noexcept {
	const mathld_t times = power(10.0, bit);
	const auto int_part = x * times;
	if (x > 0 && x * times != int_part)
		return (int_part + 1) / times;
	return int_part / times;
}

MSTL_CONST_FUNCTION constexpr mathld_t round(const mathld_t x, const mathui_t bit) noexcept {
	return x < 0 ? ceil(x - 0.5 / power(10.0, bit), bit)
		: floor(x + 0.5 / power(10.0, bit), bit);
}
MSTL_CONST_FUNCTION constexpr mathld_t truncate(const mathld_t x, const int bit) noexcept {
	return x < 0 ? ceil(x, bit) : floor(x, bit);
}

inline bool around_multiple(mathld_t x, mathld_t axis, const mathld_t toler = constants::PRECISE_TOLERANCE) {
	axis = absolute(axis);
	if (axis < constants::PRECISE_TOLERANCE) Exception(MathError("Axis Cannot be 0"));
	x = absolute(x);
	const mathld_t multi = _MSTL round(x / axis, 0) * axis;
	return absolute(x - multi) < toler;
}

inline bool around_pi(const mathld_t x, const mathld_t toler = constants::LOW_PRECISE_TOLERANCE) {
	return around_multiple(x, constants::PI, toler);
}
inline bool around_zero(const mathld_t x, const mathld_t toler = constants::PRECISE_TOLERANCE) {
	return around_multiple(x, 0, toler);
}

MSTL_CONST_FUNCTION constexpr mathld_t remainder(const mathld_t x, const mathld_t y) noexcept {
	return x - y * (x / y);
}
MSTL_CONST_FUNCTION constexpr mathld_t float_part(const mathld_t x) noexcept {
	return x - static_cast<mathl_t>(x);
}
MSTL_CONST_FUNCTION constexpr mathld_t divided_float(mathld_t x, mathl_t* int_ptr) noexcept {
	*int_ptr = static_cast<mathl_t>(x);
	x -= *int_ptr;
	return x;
}

MSTL_PURE_FUNCTION constexpr mathld_t sine(const mathld_t x) noexcept {
	mathld_t i = 1;
	mathi_t neg = 1;
	mathld_t sum = 0;
	mathld_t idx = x;
	mathld_t fac = 1;
	mathld_t taylor = x;
	do {
		fac = fac * (i + 1) * (i + 2);
		idx *= x * x;
		neg = -neg;
		sum = idx / fac * neg;
		taylor += sum;
		i += 2;
	} while (absolute(sum) > constants::EPSILON);
	return taylor;
}

MSTL_PURE_FUNCTION constexpr mathld_t cosine(const mathld_t x) noexcept {
	return sine(constants::PI / 2.0 - x);
}
inline mathld_t tangent(const mathld_t x) {
	if (around_pi(x - constants::PI / 2.0)) Exception(MathError("Tangent Range Exceeded"));
	return sine(x) / cosine(x);
}
inline mathld_t cotangent(const mathld_t x) {
	return 1.0 / tangent(x);
}

inline mathld_t arcsine(const mathld_t x) {
	if (x > 1 || x < -1) Exception(MathError("Arcsine Range Exceeded"));
	mathld_t y = x;
	mathld_t tmp = x;
	mathld_t N = 0.0;
	while (++N < constants::TAYLOR_CONVERGENCE) {
		tmp *= x * x * (2 * N - 1) * (2 * N - 1) / ((2 * N + 1) * (2 * N));
		y += tmp;
	}
	return y;
}

inline mathld_t arccosine(const mathld_t x) {
	if (x > 1 || x < -1) Exception(MathError("Arccosine Range Exceeded"));
	return constants::PI / 2.0 - arcsine(x);
}

MSTL_PURE_FUNCTION constexpr mathld_t __arctangent_taylor(const mathld_t x) noexcept {
	mathld_t y = 0.0;
	mathld_t tmp = x;
	mathld_t N = 1.0;
	while (tmp > constants::EPSILON || tmp < -constants::EPSILON) {
		y += tmp;
		N += 2;
		tmp *= -x * x * (2 * N - 2) / (2 * N - 1);
	}
	return y;
}

MSTL_PURE_FUNCTION constexpr mathld_t arctangent(const mathld_t x) noexcept {
	if (x > 1)
		return constants::PI / 2 - __arctangent_taylor(1 / x);
	if (x < -1)
		return -(constants::PI / 2 - __arctangent_taylor(1 / -x));
	return __arctangent_taylor(x);
}

MSTL_END_NAMESPACE__
#endif