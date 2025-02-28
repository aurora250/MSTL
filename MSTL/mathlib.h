#ifndef MSTL_MATHLIB_H__
#define MSTL_MATHLIB_H__
#include "errorlib.h"
#include "type_traits.hpp"
MSTL_BEGIN_NAMESPACE__

// 0 ~ 2^16 - 1
using mathus_t	= unsigned short;
// -2^16 ~ 2^16 - 1
using mathi_t	= int;
// 0 ~ 2^32 - 1
using mathui_t	= unsigned int;
// -2^32 ~ 2^32 - 1
using mathl_t	= MSTL_LLT;
// 0 ~ 2^64 - 1
using mathul_t	= unsigned MSTL_LLT;
// 2.3E - 308 ~ 1.7E + 308
using mathd_t	= double;
// 3.4E - 4932 ~ 1.1E + 4932
using mathld_t	= long double;
// invalid
using mathud_t	= void;
// invalid
using mathuld_t = void;

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
	static constexpr mathul_t INFINITY_ULL = 18446744073709551615ULL;
	static constexpr mathld_t INFINITY_LD = 1.7976931348623158e+308L;
};

static constexpr mathus_t LEONARDO_COUNT__ = 46;
static constexpr mathui_t LEONARDO_LIST[LEONARDO_COUNT__] = {
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

constexpr size_t leonardo(mathus_t n) {
	if (n == 0 || n == 1) return 1;
	return leonardo(n - 1) + leonardo(n - 2) + 1;
}

constexpr size_t fibonacci(mathus_t n) {
	if (n == 0 || n == 1) return 1;
	return fibonacci(n - 1) + fibonacci(n - 2);
}

constexpr mathld_t angular2radian(mathld_t angular) noexcept {
	return (angular * constants::PI / constants::SEMI_CIRCLE);
}
constexpr mathld_t radian2angular(mathld_t radians) noexcept {
	return radians * (constants::SEMI_CIRCLE / constants::PI);
}

template <typename T, enable_if_t<is_signed_v<T>, int> = 0>
constexpr T opposite(T x) noexcept {
	return -x;
}
template <typename T, enable_if_t<is_signed_v<T>, int> = 0>
constexpr T absolute(T x) noexcept {
	return (x > 0) ? x : (-x);
}

template <typename T>
constexpr decltype(auto) sum_n(T x) noexcept {
	return x;
}
template <typename First, typename... Rests, enable_if_t<(sizeof...(Rests) > 0), int> = 0>
constexpr decltype(auto) sum_n(First first, Rests... args) noexcept {
	return first + sum_n(args...);
}

template <typename... Args, enable_if_t<(sizeof...(Args) > 0), int> = 0>
constexpr mathld_t average(Args... args) noexcept {
	return sum_n(args...) * 1.0 / (sizeof...(Args));
}

template <typename T, enable_if_t<is_integral_v<T> && is_unsigned_v<T>, int> = 0>
constexpr T gcd(T m, T n) noexcept { // greatest common divisor
	while (n != 0) {
		T t = m % n;
		m = n;
		n = t;
	}
	return m;
}
template <typename T, enable_if_t<is_integral_v<T> && is_unsigned_v<T>, int> = 0>
constexpr T lcm(T m, T n) noexcept { // least common multiple
	return m * n / MSTL::gcd(m, n);
}

template <typename T>
constexpr T square(T x) noexcept { return x * x; }
template <typename T>
constexpr T cube(T x) noexcept { return MSTL::square(x) * x; }

template <typename T, enable_if_t<is_integral_v<T>, int> = 0>
constexpr mathl_t power(T x, mathui_t n) noexcept {
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
template <typename T, enable_if_t<is_floating_point_v<T>, int> = 0>
constexpr mathld_t power(T x, mathui_t n) noexcept {
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

constexpr mathld_t exponential(mathui_t n) noexcept {
	return power(constants::EULER, n);
}

constexpr mathld_t logarithm_e(mathld_t x) noexcept {
	mathui_t N = constants::TAYLOR_CONVERGENCE;
	mathld_t a = (x - 1) / (x + 1);
	mathld_t a_sqar = a * a;
	mathl_t nk = 2 * N + 1;
	mathld_t y = 1.0 / nk;
	while (N--) {
		nk -= 2;
		y = 1.0 / nk + a_sqar * y;
	}
	return (2.0 * a * y);
}

constexpr mathld_t logarithm(mathld_t x, mathui_t base) noexcept {
	return logarithm_e(x) / logarithm_e(base);
}
constexpr mathld_t logarithm_2(mathld_t x) noexcept {
	return logarithm(x, 2);
}
constexpr mathld_t logarithm_10(mathld_t x) noexcept {
	return logarithm(x, 10);
}
constexpr mathul_t cursory_lg2(mathul_t x) noexcept {
	mathul_t k = 0;
	for (; x > 1; x >>= 1) ++k;
	return k;
}

constexpr mathld_t square_root(mathld_t x, mathld_t precise = constants::PRECISE_TOLERANCE) noexcept {
	mathld_t t = 0.0;
	mathld_t result = x;
	while (absolute(result - t) > precise) {
		t = result;
		result = 0.5 * (t + x / t);
	}
	return result;
}
constexpr mathld_t cube_root(mathld_t x, mathld_t precise = constants::PRECISE_TOLERANCE) noexcept {
	mathld_t t = 0.0;
	mathld_t result = x;
	while (absolute(result - t) > precise) {
		t = result;
		result = (2 * t + x / (t * t)) / 3;
	}
	return result;
}

constexpr mathul_t factorial(mathui_t n) noexcept {
	mathul_t h = 1;
	for (mathui_t i = 1; i <= n; i++)
		h *= i;
	return h;
}

// bit down to the nearest digit, > 0 operates on decimal places, and ¡Ü 0 on integer places.
constexpr mathld_t floor_bit(mathld_t x, mathui_t bit) noexcept {
	mathld_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x < 0 && (mathl_t)(x * times * 10) / 10.0 != int_part) 
		return (int_part - 1) / times;
	else 
		return int_part / times;
}
constexpr mathld_t ceil_bit(mathld_t x, mathui_t bit) noexcept {
	mathld_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x > 0 && (mathl_t)(x * times * 10) / 10.0 != int_part) 
		return (int_part + 1) / times;
	else 
		return int_part / times;
}

constexpr mathld_t round_bit(mathld_t x, mathui_t bit) noexcept {
	return x < 0 ? ceil_bit(x - 0.5 / power(10.0, bit), bit) 
		: floor_bit(x + 0.5 / power(10.0, bit), bit);
}
constexpr mathld_t truncate_bit(mathld_t x, mathui_t bit) noexcept {
	return x < 0 ? ceil_bit(x, bit) : floor_bit(x, bit);
}
constexpr mathld_t floor(mathld_t x, mathui_t bit) noexcept {
	mathld_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x < 0 && x * times != int_part)
		return (int_part - 1) / times;
	else
		return int_part / times;
}
constexpr mathld_t ceil(mathld_t x, mathui_t bit) noexcept {
	mathld_t times = power(10.0, bit);
	mathl_t int_part = (mathl_t)(x * times);
	if (x > 0 && x * times != int_part)
		return (int_part + 1) / times;
	else
		return int_part / times;
}

constexpr mathld_t round(mathld_t x, mathui_t bit) noexcept {
	return x < 0 ? ceil(x - 0.5 / power(10.0, bit), bit)
		: floor(x + 0.5 / power(10.0, bit), bit);
}
constexpr mathld_t truncate(mathld_t x, int bit) noexcept {
	return x < 0 ? ceil(x, bit) : floor(x, bit);
}

bool around_multiple(mathld_t x, mathld_t axis, mathld_t toler = constants::PRECISE_TOLERANCE);

inline bool around_pi(mathld_t x, mathld_t toler = constants::LOW_PRECISE_TOLERANCE) {
	return around_multiple(x, constants::PI, toler);
}
inline bool around_zero(mathld_t x, mathld_t toler = constants::PRECISE_TOLERANCE) {
	return around_multiple(x, 0.0L, toler);
}

constexpr mathld_t remainder(mathld_t x, mathld_t y) noexcept {
	return (x - y * (x / y));
}
constexpr mathld_t float_part(mathld_t x) noexcept {
	return (x - mathl_t(x));
}
constexpr mathld_t divided_float(mathld_t x, mathl_t* int_ptr) noexcept {
	*int_ptr = mathl_t(x);
	x -= (*int_ptr);
	return x;
}

constexpr mathld_t sine(mathld_t x) noexcept {
	mathul_t i = 1;
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

constexpr mathld_t cosine(mathld_t x) noexcept {
	return sine(constants::PI / 2.0 - x);
}
inline mathld_t tangent(mathld_t x) {
	if (around_pi(x - constants::PI / 2.0)) Exception(ValueError("Tangent Range Exceeded"));
	return (sine(x) / cosine(x));
}
inline mathld_t cotangent(mathld_t x) {
	return 1.0 / tangent(x);
}

mathld_t arcsine(mathld_t x);

inline mathld_t arccosine(mathld_t x) {
	if (x > 1 || x < -1) Exception(ValueError("Arccosine Range Exceeded"));
	return constants::PI / 2.0 - arcsine(x);
}

constexpr mathld_t __arctangent_taylor(mathld_t x) noexcept {
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

constexpr mathld_t arctangent(mathld_t x) noexcept {
	if (x > 1)
		return constants::PI / 2 - __arctangent_taylor(1 / x);
	else if (x < -1)
		return -constants::PI / 2 - __arctangent_taylor(1 / x);
	else
		return __arctangent_taylor(x);
}

MSTL_END_NAMESPACE__
#endif