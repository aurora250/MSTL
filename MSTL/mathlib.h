#ifndef MSTL_MATHLIB_H__
#define MSTL_MATHLIB_H__
#include "basiclib.h"
#include "errorlib.h"
#include "functor.hpp"
#include "concepts.hpp"
#include "vector.hpp"

MSTL_BEGIN_NAMESPACE__
using namespace concepts;

typedef int mathi_t;
typedef unsigned int mathui_t;
typedef MSTL_LONG_LONG_TYPE__ mathl_t;
typedef unsigned MSTL_LONG_LONG_TYPE__ mathul_t;
typedef long double mathd_t;
typedef void mathud_t;

extern const mathd_t EULER = 2.718281828459045;
extern const mathd_t PI = 3.141592653589793;  // radian
extern const mathd_t PHI = 1.618033988749895;
extern const mathd_t SEMI_CIRCLE = 180.0;  // angular
extern const mathd_t CIRCLE = 360.0;
extern const mathd_t EPSILON = 1e-15;
extern const mathui_t TAYLOR_CONVERGENCE = 10000;
extern const mathd_t PRECISE_TOLERANCE = TAYLOR_CONVERGENCE * EPSILON;
extern const mathd_t LOW_PRECISE_TOLERANCE = TAYLOR_CONVERGENCE * PRECISE_TOLERANCE;

inline mathd_t angular2radian(mathd_t angular) {
	return (angular * PI / SEMI_CIRCLE);
}
inline mathd_t radian2angular(mathd_t radians) {
	return radians * (SEMI_CIRCLE / PI);
}

template <typename T> 
	requires(SignedNumber<T>)
inline T opposite(T x) {
	return -x;
}
template <typename T, typename... Args>
T sum_n(T first, Args... args) {
	return first + sum_n(args...);
}
template <typename T> 
	requires(SignedNumber<T>)
inline T absolute(T x) {
	return (x > 0) ? x : (-x);
}
template <typename T> 
	requires(Number<T>)
inline mathd_t reciprocal(T x) {
	if (x == 0) Exception(MathError("0 Have No Reciprocal"));
	return mathd_t(1.0 / x);
}

template <typename T>
inline T square(T x) { return x * x; }
template <typename T>
	requires (Integral<T>)
mathl_t power(T x, mathui_t n) {
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
mathd_t power(T x, mathui_t n) {
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
inline mathd_t exponential(mathd_t n) {
	return power(EULER, n);
}
mathd_t logarithm_e(mathd_t x) {
	mathui_t N = TAYLOR_CONVERGENCE;
	mathd_t a = (x - 1) / (x + 1);
	mathd_t a_sqar = square(a);
	mathl_t nk = 2 * N + 1;
	mathd_t y = 1.0 / nk;
	while(N--) {
		nk -= 2;
		y = 1.0 / nk + a_sqar * y;
	}
	return (2.0 * a * y);
}
inline mathd_t logarithm(mathd_t x, mathui_t base) {
	return logarithm_e(x) / logarithm_e(base);
}
inline mathd_t logarithm_2(mathd_t x) {
	return logarithm(x, 2);
}
inline mathd_t logarithm_10(mathd_t x) {
	return logarithm(x, 10);
}

mathd_t square_root(mathd_t x, mathd_t precise) {
	mathd_t t = 0.0;
	mathd_t result = x;
	while (absolute(result - t) > precise) {
		t = result;
		result = 0.5 * (t + x / t);
	}
	return result;
}

mathul_t factorial(mathul_t n) {
	mathul_t h = 1;
	for (mathul_t i = 1; i <= n; i++)
		h *= i;
	return h;
}

mathd_t floor_bit(mathd_t x, mathui_t bit) { // bit 精确到第几位，>0 操作小数位，≤0 操作整数位
	mathd_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x < 0 && (mathl_t)(x * times * 10) / 10.0 != int_part) {
		return (int_part - 1) / times;
	}
	else {
		return int_part / times;
	}
}
mathd_t ceil_bit(mathd_t x, mathui_t bit) {
	mathd_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x > 0 && (mathl_t)(x * times * 10) / 10.0 != int_part) {
		return (int_part + 1) / times;
	}
	else {
		return int_part / times;
	}
}
inline mathd_t round_bit(mathd_t x, mathui_t bit) {
	return x < 0 ? ceil_bit(x - 0.5 / power(10.0, bit), bit) 
		: floor_bit(x + 0.5 / power(10.0, bit), bit);
}
inline mathd_t truncate_bit(mathd_t x, mathui_t bit) {
	return x < 0 ? ceil_bit(x, bit) 
		: floor_bit(x, bit);
}
mathd_t floor(mathd_t x, mathui_t bit) {
	mathd_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x < 0 && x * times != int_part) 
		return (int_part - 1) / times;
	else 
		return int_part / times;
}
mathd_t ceil(mathd_t x, mathui_t bit) {
	mathd_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x > 0 && x * times != int_part) 
		return (int_part + 1) / times;
	else 
		return int_part / times;
}
inline mathd_t round(mathd_t x, mathui_t bit) {
	return x < 0 ? ceil(x - 0.5 / power(10.0, bit), bit) 
		: floor(x + 0.5 / power(10.0, bit), bit);
}
inline mathd_t truncate(mathd_t x, int bit) {
	return x < 0 ? ceil(x, bit) 
		: floor(x, bit);
}

bool around_multiple(mathd_t x, mathd_t axis, mathd_t toler = PRECISE_TOLERANCE) {
	x = absolute(x);
	axis = absolute(axis);
	if (axis < PRECISE_TOLERANCE) Exception(MathError("Axis Cannot be 0"));
	mathd_t multi = MSTL::round(x / axis, 0) * axis;
	return absolute(x - multi) < toler;
}
inline bool around_pi(mathd_t x, mathd_t toler = LOW_PRECISE_TOLERANCE) {
	return around_multiple(x, PI, toler);
}

inline mathd_t remainder(mathd_t x, mathd_t y) {
	return (x - y * (x / y));
}
inline mathd_t float_part(mathd_t x) {
	return (x - (mathl_t)x);
}
inline mathd_t divided_float(mathd_t x, mathl_t* ptr) {
	*ptr = mathl_t(x);
	x -= (*ptr);
	return x;
}

mathd_t sine(mathd_t x) {
	mathul_t i = 1; 
	int neg = 1;
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
	} while (absolute(sum) > EPSILON);
	return taylor;
}
inline mathd_t cosine(mathd_t x) {
	x = (PI / 2) - x;
	return sine(x);
}
inline mathd_t tangent(mathd_t x) {
	if (around_pi(x - PI / 2)) Exception(MathError("Tangent Range Exceeded"));
	return (sine(x) / cosine(x));
}
inline mathd_t cotangent(mathd_t x) {
	return (1 / tangent(x));
}

mathd_t arcsine(mathd_t x) {
	if (x > 1 || x < -1) Exception(MathError("Arcsine Range Exceeded"));
	mathd_t y = x;
	mathd_t tmp = x;
	mathul_t N = 0;
	while (++N < TAYLOR_CONVERGENCE) {
		tmp *= (x * x * (2 * N - 1) * (2 * N - 1)) / ((2 * N + 1) * (2 * N));
		y += tmp;
	}
	return y;
}
inline mathd_t arccosine(mathd_t x) {
	return PI / 2 - arcsine(x);
}
mathd_t arctangent_taylor(mathd_t x) {
	mathd_t y = 0.0;
	mathd_t tmp = x;
	mathul_t N = 1;
	while (tmp > EPSILON || tmp < -EPSILON) {
		y += tmp;
		N += 2;
		tmp *= -x * x * (2 * N - 2) / (2 * N - 1);
	}
	return y;
}
inline mathd_t arctangent(mathd_t x) {
	if (x > 1)
		return PI / 2 - arctangent_taylor(1 / x);
	else if (x < -1)
		return -PI / 2 - arctangent_taylor(1 / x);
	else
		return arctangent_taylor(x);
}

MSTL_END_NAMESPACE__
#endif