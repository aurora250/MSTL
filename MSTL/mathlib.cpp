#include "mathlib.h"
MSTL_BEGIN_NAMESPACE__

bool around_multiple(mathld_t x, mathld_t axis, const mathld_t toler) {
	axis = absolute(axis);
	if (axis < constants::PRECISE_TOLERANCE) Exception(MathError("Axis Cannot be 0"));
	x = absolute(x);
	const mathld_t multi = _MSTL round(x / axis, 0) * axis;
	return absolute(x - multi) < toler;
}

mathld_t arcsine(const mathld_t x) {
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

MSTL_END_NAMESPACE__
