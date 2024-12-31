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

extern const mathd_t EULER;
extern const mathd_t PI;
extern const mathd_t PHI;
extern const mathd_t SEMI_CIRCLE;
extern const mathd_t CIRCLE;
extern const mathd_t EPSILON;
extern const mathui_t TAYLOR_CONVERGENCE;
extern const mathd_t PRECISE_TOLERANCE;
extern const mathd_t LOW_PRECISE_TOLERANCE;

inline MSTL_CONSTEXPR mathd_t angular2radian(mathd_t angular) {
	return (angular * PI / SEMI_CIRCLE);
}
inline MSTL_CONSTEXPR mathd_t radian2angular(mathd_t radians) {
	return radians * (SEMI_CIRCLE / PI);
}

template <typename T> 
	requires(SignedNumber<T>)
inline MSTL_CONSTEXPR T opposite(T x) {
	return -x;
}
template <typename T, typename... Args>
T MSTL_CONSTEXPR sum_n(T first, Args... args) {
	return first + sum_n(args...);
}
template <typename T> 
	requires(SignedNumber<T>)
inline MSTL_CONSTEXPR T absolute(T x) {
	return (x > 0) ? x : (-x);
}
template <typename T> 
	requires(Number<T>)
inline MSTL_CONSTEXPR mathd_t reciprocal(T x) {
	if (x == 0) Exception(MathError("0 Have No Reciprocal"));
	return mathd_t(1.0 / x);
}

template <typename T>
inline MSTL_CONSTEXPR T square(T x) { return x * x; }
template <typename T>
	requires (Integral<T>)
MSTL_CONSTEXPR mathl_t power(T x, mathui_t n) {
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
mathd_t MSTL_CONSTEXPR power(T x, mathui_t n) {
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
inline MSTL_CONSTEXPR mathd_t exponential(mathui_t n) {
	return power(EULER, n);
}
mathd_t logarithm_e(mathd_t x);

inline mathd_t logarithm(mathd_t x, mathui_t base) {
	return logarithm_e(x) / logarithm_e(base);
}
inline mathd_t logarithm_2(mathd_t x) {
	return logarithm(x, 2);
}
inline mathd_t logarithm_10(mathd_t x) {
	return logarithm(x, 10);
}

mathd_t square_root(mathd_t x, mathd_t precise);

mathul_t factorial(mathul_t n);

mathd_t floor_bit(mathd_t x, mathui_t bit);
mathd_t ceil_bit(mathd_t x, mathui_t bit);

inline mathd_t round_bit(mathd_t x, mathui_t bit) {
	return x < 0 ? ceil_bit(x - 0.5 / power(10.0, bit), bit) 
		: floor_bit(x + 0.5 / power(10.0, bit), bit);
}
inline mathd_t truncate_bit(mathd_t x, mathui_t bit) {
	return x < 0 ? ceil_bit(x, bit) 
		: floor_bit(x, bit);
}
mathd_t floor(mathd_t x, mathui_t bit);
mathd_t ceil(mathd_t x, mathui_t bit);

inline mathd_t round(mathd_t x, mathui_t bit) {
	return x < 0 ? ceil(x - 0.5 / power(10.0, bit), bit) 
		: floor(x + 0.5 / power(10.0, bit), bit);
}
inline mathd_t truncate(mathd_t x, int bit) {
	return x < 0 ? ceil(x, bit) 
		: floor(x, bit);
}

bool around_multiple(mathd_t x, mathd_t axis, mathd_t toler = PRECISE_TOLERANCE);

inline bool around_pi(mathd_t x, mathd_t toler = LOW_PRECISE_TOLERANCE) {
	return around_multiple(x, PI, toler);
}

inline MSTL_CONSTEXPR mathd_t remainder(mathd_t x, mathd_t y) {
	return (x - y * (x / y));
}
inline MSTL_CONSTEXPR mathd_t float_part(mathd_t x) {
	return (x - (mathl_t)x);
}
inline MSTL_CONSTEXPR mathd_t divided_float(mathd_t x, mathl_t* ptr) {
	*ptr = mathl_t(x);
	x -= (*ptr);
	return x;
}

mathd_t sine(mathd_t x);

inline mathd_t cosine(mathd_t x) {
	return sine((PI / 2) - x);
}
inline mathd_t tangent(mathd_t x) {
	if (around_pi(x - PI / 2)) Exception(MathError("Tangent Range Exceeded"));
	return (sine(x) / cosine(x));
}
inline mathd_t cotangent(mathd_t x) {
	return reciprocal(tangent(x));
}

mathd_t arcsine(mathd_t x);

inline mathd_t arccosine(mathd_t x) {
	if (x > 1 || x < -1) Exception(MathError("Arccosine Range Exceeded"));
	return PI / 2.0 - arcsine(x);
}
mathd_t arctangent_taylor(mathd_t x);

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