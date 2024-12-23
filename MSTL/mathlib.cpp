#include "mathlib.h"
MSTL_BEGIN_NAMESPACE__

const mathd_t EULER = 2.718281828459045;
const mathd_t PI = 3.141592653589793;  // radian
const mathd_t PHI = 1.618033988749895;
const mathd_t SEMI_CIRCLE = 180.0;  // angular
const mathd_t CIRCLE = 360.0;
const mathd_t EPSILON = 1e-15;
const mathui_t TAYLOR_CONVERGENCE = 10000;
const mathd_t PRECISE_TOLERANCE = TAYLOR_CONVERGENCE * EPSILON;
const mathd_t LOW_PRECISE_TOLERANCE = TAYLOR_CONVERGENCE * PRECISE_TOLERANCE;

mathd_t logarithm_e(mathd_t x) {
	mathui_t N = TAYLOR_CONVERGENCE;
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
bool around_multiple(mathd_t x, mathd_t axis, mathd_t toler) {
	x = absolute(x);
	axis = absolute(axis);
	if (axis < PRECISE_TOLERANCE) Exception(MathError("Axis Cannot be 0"));
	mathd_t multi = MSTL::round(x / axis, 0) * axis;
	return absolute(x - multi) < toler;
}
mathd_t sine(mathd_t x) {
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
	} while (absolute(sum) > EPSILON);
	return taylor;
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

MSTL_END_NAMESPACE__
