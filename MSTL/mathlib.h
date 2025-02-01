#ifndef MSTL_MATHLIB_H__
#define MSTL_MATHLIB_H__
#include "errorlib.h"
#include "functor.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__

// 0 ~ 2^16-1
typedef unsigned short mathus_t;
// -2^16 ~ 2^16-1
typedef int mathi_t;
// 0 ~ 2^32-1
typedef unsigned int mathui_t;
// -2^32 ~ 2^32-1
typedef MSTL_LONG_LONG_TYPE__ mathl_t;
// 0 ~ 2^64-1
typedef unsigned MSTL_LONG_LONG_TYPE__ mathul_t;
// 2.3E-308 ~ 1.7E+308
typedef double mathd_t;
// 3.4E-4932 ~ 1.1E+4932
typedef long double mathld_t;
// invalid
typedef void mathud_t;
// invalid
typedef void mathuld_t;

struct constants {
	static MSTL_CONSTEXPR mathld_t EULER = 2.718281828459045L;
	static MSTL_CONSTEXPR mathld_t PI = 3.141592653589793L;  // radian
	static MSTL_CONSTEXPR mathld_t PHI = 1.618033988749895L;
	static MSTL_CONSTEXPR mathld_t SEMI_CIRCLE = 180.0;  // angular
	static MSTL_CONSTEXPR mathld_t CIRCLE = 360.0;
	static MSTL_CONSTEXPR mathld_t EPSILON = 1e-15L;
	static MSTL_CONSTEXPR mathui_t TAYLOR_CONVERGENCE = 10000U;
	static MSTL_CONSTEXPR mathld_t PRECISE_TOLERANCE = TAYLOR_CONVERGENCE * EPSILON;
	static MSTL_CONSTEXPR mathld_t LOW_PRECISE_TOLERANCE = TAYLOR_CONVERGENCE * PRECISE_TOLERANCE;
	static MSTL_CONSTEXPR mathul_t INFINITY_ULL = 18446744073709551615ULL;
	static MSTL_CONSTEXPR mathld_t INFINITY_LD = 1.7976931348623158e+308L;
};

static MSTL_CONSTEXPR mathus_t PRIMER_COUNT__ = 28;
static MSTL_CONSTEXPR mathui_t PRIME_LIST[PRIMER_COUNT__] = {
  53,         97,           193,         389,       769,
  1543,       3079,         6151,        12289,     24593,
  49157,      98317,        196613,      393241,    786433,
  1572869,    3145739,      6291469,     12582917,  25165843,
  50331653,   100663319,    201326611,   402653189, 805306457,
  1610612741, 3221225473u,  4294967291u
};

static MSTL_CONSTEXPR mathus_t LEONARDO_COUNT__ = 46;
static MSTL_CONSTEXPR mathui_t LEONARDO_LIST[LEONARDO_COUNT__] = {
	1,			1,			3,			5,			9, 
	15,			25,			41,			67,			109,
	177,		287,		465,		753,		1219,
	1973,		3193,		5167,		8361,		13529,
	21891,		35421,		57313,		92735,		150049, 
	242785,		392835,		635621,		1028457,	1664079,
	2692537,	4356617,	7049155,	11405773,	18454929,
	29860703,	48315633,	78176337,	126491971,	204668309,
	331160281,	535828591,	866988873,	1402817465, 2269806339u, 
	3672623805u
};

MSTL_CONSTEXPR size_t leonardo(mathus_t n) {
	if (n == 0 || n == 1) return 1;
	return leonardo(n - 1) + leonardo(n - 2) + 1;
}

MSTL_CONSTEXPR size_t fibonacci(mathus_t n) {
	if (n == 0 || n == 1) return 1;
	return fibonacci(n - 1) + fibonacci(n - 2);
}

MSTL_CONSTEXPR mathld_t angular2radian(mathld_t angular) noexcept {
	return (angular * constants::PI / constants::SEMI_CIRCLE);
}
MSTL_CONSTEXPR mathld_t radian2angular(mathld_t radians) noexcept {
	return radians * (constants::SEMI_CIRCLE / constants::PI);
}

template <typename T> 
	requires(signed_number<T>)
MSTL_CONSTEXPR T opposite(T x) noexcept {
	return -x;
}
template <typename T>
MSTL_CONSTEXPR decltype(auto) sum_n(T x) noexcept {
	return x;
}
template <typename T, typename... Args>
	requires(sizeof...(Args) > 0)
MSTL_CONSTEXPR decltype(auto) sum_n(T first, Args... args) noexcept {
	return first + sum_n(args...);
}
template <typename T> 
	requires(signed_number<T>)
MSTL_CONSTEXPR T absolute(T x) noexcept {
	return (x > 0) ? x : (-x);
}

template <typename T>
	requires(integral<T>)
MSTL_CONSTEXPR T gcd(T m, T n) noexcept { // greatest common divisor
	while (n != 0) {
		T t = m % n;
		m = n;
		n = t;
	}
	return m;
}
template <typename T>
	requires(integral<T>)
MSTL_CONSTEXPR T lcm(T m, T n) noexcept { // least common multiple
	return MSTL::absolute(m * n) / MSTL::gcd(m, n);
}

template <typename T>
MSTL_CONSTEXPR T square(T x) noexcept { return x * x; }
template <typename T>
MSTL_CONSTEXPR T cube(T x) noexcept { return MSTL::square(x) * x; }
template <typename T>
	requires (integral<T>)
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
	requires (floating_point<T>)
MSTL_CONSTEXPR mathld_t power(T x, mathui_t n) noexcept {
	if (n == 0) return mathld_t(1);
	mathld_t result = 1.0;
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
MSTL_CONSTEXPR mathld_t exponential(mathui_t n) noexcept {
	return power(constants::EULER, n);
}
MSTL_CONSTEXPR mathld_t logarithm_e(mathld_t x) noexcept {
	mathui_t N = constants::TAYLOR_CONVERGENCE;
	mathld_t a = (x - 1) / (x + 1);
	mathld_t a_sqar = square(a);
	mathl_t nk = 2 * N + 1;
	mathld_t y = 1.0 / nk;
	while (N--) {
		nk -= 2;
		y = 1.0 / nk + a_sqar * y;
	}
	return (2.0 * a * y);
}

MSTL_CONSTEXPR mathld_t logarithm(mathld_t x, mathui_t base) noexcept {
	return logarithm_e(x) / logarithm_e(base);
}
MSTL_CONSTEXPR mathld_t logarithm_2(mathld_t x) noexcept {
	return logarithm(x, 2);
}
MSTL_CONSTEXPR mathld_t logarithm_10(mathld_t x) noexcept {
	return logarithm(x, 10);
}
MSTL_CONSTEXPR mathul_t cursory_lg2(mathul_t x) noexcept {
	mathul_t k = 0;
	for (; x > 1; x >>= 1) ++k;
	return k;
}

MSTL_CONSTEXPR mathld_t square_root(mathld_t x, mathld_t precise) noexcept {
	mathld_t t = 0.0;
	mathld_t result = x;
	while (absolute(result - t) > precise) {
		t = result;
		result = 0.5 * (t + x / t);
	}
	return result;
}

MSTL_CONSTEXPR mathld_t cube_root(mathld_t x, mathld_t precise) noexcept {
	mathld_t t = 0.0;
	mathld_t result = x;
	while (absolute(result - t) > precise) {
		t = result;
		result = (2 * t + x / (t * t)) / 3;
	}
	return result;
}

MSTL_CONSTEXPR mathul_t factorial(mathul_t n) noexcept {
	mathul_t h = 1;
	for (mathul_t i = 1; i <= n; i++)
		h *= i;
	return h;
}

// bit 精确到第几位，>0 操作小数位，≤0 操作整数位
MSTL_CONSTEXPR mathld_t floor_bit(mathld_t x, mathui_t bit) noexcept {
	mathld_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x < 0 && (mathl_t)(x * times * 10) / 10.0 != int_part) {
		return (int_part - 1) / times;
	}
	else {
		return int_part / times;
	}
}
MSTL_CONSTEXPR mathld_t ceil_bit(mathld_t x, mathui_t bit) noexcept {
	mathld_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x > 0 && (mathl_t)(x * times * 10) / 10.0 != int_part) {
		return (int_part + 1) / times;
	}
	else {
		return int_part / times;
	}
}

MSTL_CONSTEXPR mathld_t round_bit(mathld_t x, mathui_t bit) noexcept {
	return x < 0 ? ceil_bit(x - 0.5 / power(10.0, bit), bit) 
		: floor_bit(x + 0.5 / power(10.0, bit), bit);
}
MSTL_CONSTEXPR mathld_t truncate_bit(mathld_t x, mathui_t bit) noexcept {
	return x < 0 ? ceil_bit(x, bit) : floor_bit(x, bit);
}
MSTL_CONSTEXPR mathld_t floor(mathld_t x, mathui_t bit) noexcept {
	mathld_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x < 0 && x * times != int_part)
		return (int_part - 1) / times;
	else
		return int_part / times;
}
MSTL_CONSTEXPR mathld_t ceil(mathld_t x, mathui_t bit) noexcept {
	mathld_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x > 0 && x * times != int_part)
		return (int_part + 1) / times;
	else
		return int_part / times;
}

MSTL_CONSTEXPR mathld_t round(mathld_t x, mathui_t bit) noexcept {
	return x < 0 ? ceil(x - 0.5 / power(10.0, bit), bit)
		: floor(x + 0.5 / power(10.0, bit), bit);
}
MSTL_CONSTEXPR mathld_t truncate(mathld_t x, int bit) noexcept {
	return x < 0 ? ceil(x, bit) : floor(x, bit);
}

inline bool around_multiple(mathld_t x, mathld_t axis, mathld_t toler = constants::PRECISE_TOLERANCE) {
	axis = absolute(axis);
	if (axis < constants::PRECISE_TOLERANCE) Exception(ValueError("Axis Cannot be 0"));
	x = absolute(x);
	mathld_t multi = MSTL::round(x / axis, 0) * axis;
	return absolute(x - multi) < toler;
}

inline bool around_pi(mathld_t x, mathld_t toler = constants::LOW_PRECISE_TOLERANCE) {
	return around_multiple(x, constants::PI, toler);
}
inline bool around_zero(mathld_t x, mathld_t toler = constants::PRECISE_TOLERANCE) {
	return around_multiple(x, 0.0L, toler);
}

MSTL_CONSTEXPR mathld_t remainder(mathld_t x, mathld_t y) noexcept {
	return (x - y * (x / y));
}
MSTL_CONSTEXPR mathld_t float_part(mathld_t x) noexcept {
	return (x - mathl_t(x));
}
MSTL_CONSTEXPR mathld_t divided_float(mathld_t x, mathl_t* int_ptr) noexcept {
	*int_ptr = mathl_t(x);
	x -= (*int_ptr);
	return x;
}

MSTL_CONSTEXPR mathld_t sine(mathld_t x) noexcept {
	mathul_t i = 1;
	mathi_t neg = 1;
	mathld_t sum;
	mathld_t idx = x;
	mathld_t fac = 1;
	mathld_t taylor = x;
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

MSTL_CONSTEXPR mathld_t cosine(mathld_t x) noexcept {
	return sine((constants::PI / 2.0) - x);
}
inline mathld_t tangent(mathld_t x) {
	if (around_pi(x - constants::PI / 2.0)) Exception(ValueError("Tangent Range Exceeded"));
	return (sine(x) / cosine(x));
}
inline mathld_t cotangent(mathld_t x) {
	return 1.0 / tangent(x);
}

inline mathld_t arcsine(mathld_t x) {
	if (x > 1 || x < -1) Exception(ValueError("Arcsine Range Exceeded"));
	mathld_t y = x;
	mathld_t tmp = x;
	mathul_t N = 0;
	while (++N < constants::TAYLOR_CONVERGENCE) {
		tmp *= (x * x * (2 * N - 1) * (2 * N - 1)) / ((2 * N + 1) * (2 * N));
		y += tmp;
	}
	return y;
}

inline mathld_t arccosine(mathld_t x) {
	if (x > 1 || x < -1) Exception(ValueError("Arccosine Range Exceeded"));
	return constants::PI / 2.0 - arcsine(x);
}
MSTL_CONSTEXPR mathld_t arctangent_taylor(mathld_t x) noexcept {
	mathld_t y = 0.0;
	mathld_t tmp = x;
	mathul_t N = 1;
	while (tmp > constants::EPSILON || tmp < -constants::EPSILON) {
		y += tmp;
		N += 2;
		tmp *= -x * x * (2 * N - 2) / (2 * N - 1);
	}
	return y;
}

MSTL_CONSTEXPR mathld_t arctangent(mathld_t x) noexcept {
	if (x > 1)
		return constants::PI / 2 - arctangent_taylor(1 / x);
	else if (x < -1)
		return -constants::PI / 2 - arctangent_taylor(1 / x);
	else
		return arctangent_taylor(x);
}

MSTL_END_NAMESPACE__
#endif