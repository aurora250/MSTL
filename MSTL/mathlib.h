#ifndef MSTL_MATHLIB_H__
#define MSTL_MATHLIB_H__
#include "basiclib.h"
#include "errorlib.h"
#include "functor.hpp"
#include "concepts.hpp"

MSTL_BEGIN_NAMESPACE__
using namespace concepts;

typedef int mathi_t;
typedef unsigned int mathui_t;
typedef MSTL_LONG_LONG_TYPE__ mathl_t;
typedef unsigned MSTL_LONG_LONG_TYPE__ mathul_t;
typedef long double mathd_t;
typedef void mathud_t;

struct constants {
	static MSTL_CONSTEXPR mathd_t EULER = 2.718281828459045L;
	static MSTL_CONSTEXPR mathd_t PI = 3.141592653589793L;  // radian
	static MSTL_CONSTEXPR mathd_t PHI = 1.618033988749895L;
	static MSTL_CONSTEXPR mathd_t SEMI_CIRCLE = 180.0;  // angular
	static MSTL_CONSTEXPR mathd_t CIRCLE = 360.0;
	static MSTL_CONSTEXPR mathd_t EPSILON = 1e-15L;
	static MSTL_CONSTEXPR mathui_t TAYLOR_CONVERGENCE = 10000U;
	static MSTL_CONSTEXPR mathd_t PRECISE_TOLERANCE = TAYLOR_CONVERGENCE * EPSILON;
	static MSTL_CONSTEXPR mathd_t LOW_PRECISE_TOLERANCE = TAYLOR_CONVERGENCE * PRECISE_TOLERANCE;
	static MSTL_CONSTEXPR mathul_t INFINITY_ULL = 18446744073709551615ULL;
	static MSTL_CONSTEXPR mathd_t INFINITY_LD = 1.7976931348623158e+308L;
};

MSTL_CONSTEXPR mathd_t angular2radian(mathd_t angular) noexcept {
	return (angular * constants::PI / constants::SEMI_CIRCLE);
}
MSTL_CONSTEXPR mathd_t radian2angular(mathd_t radians) noexcept {
	return radians * (constants::SEMI_CIRCLE / constants::PI);
}

template <typename T> 
	requires(SignedNumber<T>)
MSTL_CONSTEXPR T opposite(T x) noexcept {
	return -x;
}
template <typename T, typename... Args>
MSTL_CONSTEXPR T sum_n(T first, Args... args) noexcept {
	return first + sum_n(args...);
}
template <typename T> 
	requires(SignedNumber<T>)
MSTL_CONSTEXPR T absolute(T x) noexcept {
	return (x > 0) ? x : (-x);
}
template <typename T> 
	requires(Number<T>)
inline mathd_t reciprocal(T x) {
	if (x == 0) Exception(MathError("x cannot be zero"));
	return mathd_t(1.0 / x);
}

template <typename T>
MSTL_CONSTEXPR T square(T x) noexcept { return x * x; }
template <typename T>
	requires (Integral<T>)
MSTL_CONSTEXPR mathl_t power(T x, mathui_t n) noexcept {
	if (n == 0) return mathl_t(1);
	mathl_t result = 1;
	T base = x;
	while (n > 0) {
		if (n % 2 == 1) {
			result *= base;
		}
		base *= base;
		n /= 2;
	}
	return result;
}
template <typename T>
	requires (Float<T>)
mathd_t MSTL_CONSTEXPR power(T x, mathui_t n) noexcept {
	if (n == 0) return mathd_t(1);
	mathd_t result = 1.0;
	T base = x;
	while (n > 0) {
		if (n % 2 == 1) {
			result *= base;
		}
		base *= base;
		n /= 2;
	}
	return result;
}
MSTL_CONSTEXPR mathd_t exponential(mathui_t n) noexcept {
	return power(constants::EULER, n);
}
MSTL_CONSTEXPR mathd_t logarithm_e(mathd_t x) noexcept {
	mathui_t N = constants::TAYLOR_CONVERGENCE;
	mathd_t a = (x - 1) / (x + 1);
	mathd_t a_sqar = square(a);
	mathl_t nk = 2 * N + 1;
	mathd_t y = 1.0 / nk;
	while (N--) {
		nk -= 2;
		y = 1.0 / nk + a_sqar * y;
	}
	return (2.0 * a * y);
}

MSTL_CONSTEXPR mathd_t logarithm(mathd_t x, mathui_t base) noexcept {
	return logarithm_e(x) / logarithm_e(base);
}
MSTL_CONSTEXPR mathd_t logarithm_2(mathd_t x) noexcept {
	return logarithm(x, 2);
}
MSTL_CONSTEXPR mathd_t logarithm_10(mathd_t x) noexcept {
	return logarithm(x, 10);
}

MSTL_CONSTEXPR mathd_t square_root(mathd_t x, mathd_t precise) noexcept {
	mathd_t t = 0.0;
	mathd_t result = x;
	while (absolute(result - t) > precise) {
		t = result;
		result = 0.5 * (t + x / t);
	}
	return result;
}

MSTL_CONSTEXPR mathul_t factorial(mathul_t n) noexcept {
	mathul_t h = 1;
	for (mathul_t i = 1; i <= n; i++)
		h *= i;
	return h;
}

MSTL_CONSTEXPR mathd_t floor_bit(mathd_t x, mathui_t bit) noexcept { // bit 精确到第几位，>0 操作小数位，≤0 操作整数位
	mathd_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x < 0 && (mathl_t)(x * times * 10) / 10.0 != int_part) {
		return (int_part - 1) / times;
	}
	else {
		return int_part / times;
	}
}
MSTL_CONSTEXPR mathd_t ceil_bit(mathd_t x, mathui_t bit) noexcept {
	mathd_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x > 0 && (mathl_t)(x * times * 10) / 10.0 != int_part) {
		return (int_part + 1) / times;
	}
	else {
		return int_part / times;
	}
}

MSTL_CONSTEXPR mathd_t round_bit(mathd_t x, mathui_t bit) noexcept {
	return x < 0 ? ceil_bit(x - 0.5 / power(10.0, bit), bit) 
		: floor_bit(x + 0.5 / power(10.0, bit), bit);
}
MSTL_CONSTEXPR mathd_t truncate_bit(mathd_t x, mathui_t bit) noexcept {
	return x < 0 ? ceil_bit(x, bit) 
		: floor_bit(x, bit);
}
MSTL_CONSTEXPR mathd_t floor(mathd_t x, mathui_t bit) noexcept {
	mathd_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x < 0 && x * times != int_part)
		return (int_part - 1) / times;
	else
		return int_part / times;
}
MSTL_CONSTEXPR mathd_t ceil(mathd_t x, mathui_t bit) noexcept {
	mathd_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x > 0 && x * times != int_part)
		return (int_part + 1) / times;
	else
		return int_part / times;
}

MSTL_CONSTEXPR mathd_t round(mathd_t x, mathui_t bit) noexcept {
	return x < 0 ? ceil(x - 0.5 / power(10.0, bit), bit) 
		: floor(x + 0.5 / power(10.0, bit), bit);
}
MSTL_CONSTEXPR mathd_t truncate(mathd_t x, int bit) noexcept {
	return x < 0 ? ceil(x, bit) 
		: floor(x, bit);
}

inline bool around_multiple(
	mathd_t x, mathd_t axis, mathd_t toler = constants::PRECISE_TOLERANCE) {
	x = absolute(x);
	axis = absolute(axis);
	if (axis < constants::PRECISE_TOLERANCE) Exception(MathError("Axis Cannot be 0"));
	mathd_t multi = MSTL::round(x / axis, 0) * axis;
	return absolute(x - multi) < toler;
}

inline bool around_pi(mathd_t x, mathd_t toler = constants::LOW_PRECISE_TOLERANCE) {
	return around_multiple(x, constants::PI, toler);
}
inline bool around_zero(mathd_t x, mathd_t toler = constants::PRECISE_TOLERANCE) {
	return around_multiple(x, 0.0L, toler);
}

MSTL_CONSTEXPR mathd_t remainder(mathd_t x, mathd_t y) noexcept {
	return (x - y * (x / y));
}
MSTL_CONSTEXPR mathd_t float_part(mathd_t x) noexcept {
	return (x - (mathl_t)x);
}
MSTL_CONSTEXPR mathd_t divided_float(mathd_t x, mathl_t* ptr) noexcept {
	*ptr = mathl_t(x);
	x -= (*ptr);
	return x;
}

MSTL_CONSTEXPR mathd_t sine(mathd_t x) noexcept {
	mathul_t i = 1;
	mathi_t neg = 1;
	mathd_t sum;
	mathd_t idx = x;
	mathd_t fac = 1;
	mathd_t taylor = x;
	do {
		fac = fac * (i + 1) * (i + 2);
		idx *= power(x, 2);
		neg = opposite(neg);
		sum = idx / fac * neg;
		taylor += sum;
		i += 2;
	} while (absolute(sum) > constants::EPSILON);
	return taylor;
}

MSTL_CONSTEXPR mathd_t cosine(mathd_t x) noexcept {
	return sine((constants::PI / 2.0) - x);
}
inline mathd_t tangent(mathd_t x) {
	if (around_pi(x - constants::PI / 2.0)) Exception(MathError("Tangent Range Exceeded"));
	return (sine(x) / cosine(x));
}
inline mathd_t cotangent(mathd_t x) {
	return 1.0 / tangent(x);
}

inline mathd_t arcsine(mathd_t x) {
	if (x > 1 || x < -1) Exception(MathError("Arcsine Range Exceeded"));
	mathd_t y = x;
	mathd_t tmp = x;
	mathul_t N = 0;
	while (++N < constants::TAYLOR_CONVERGENCE) {
		tmp *= (x * x * (2 * N - 1) * (2 * N - 1)) / ((2 * N + 1) * (2 * N));
		y += tmp;
	}
	return y;
}

inline mathd_t arccosine(mathd_t x) {
	if (x > 1 || x < -1) Exception(MathError("Arccosine Range Exceeded"));
	return constants::PI / 2.0 - arcsine(x);
}
MSTL_CONSTEXPR mathd_t arctangent_taylor(mathd_t x) noexcept {
	mathd_t y = 0.0;
	mathd_t tmp = x;
	mathul_t N = 1;
	while (tmp > constants::EPSILON || tmp < -constants::EPSILON) {
		y += tmp;
		N += 2;
		tmp *= -x * x * (2 * N - 2) / (2 * N - 1);
	}
	return y;
}

MSTL_CONSTEXPR mathd_t arctangent(mathd_t x) noexcept {
	if (x > 1)
		return constants::PI / 2 - arctangent_taylor(1 / x);
	else if (x < -1)
		return -constants::PI / 2 - arctangent_taylor(1 / x);
	else
		return arctangent_taylor(x);
}

MSTL_END_NAMESPACE__
#endif