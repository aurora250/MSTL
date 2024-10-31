#ifndef MATH_H
#define MATH_H
#include "basiclib.h"

namespace MSTL {
	namespace math {
		template<typename T>
		extern const T& min(const T&& _l, const T&& _r) {
			if (_l > _r) return _r;
			else return _l;
		}
		template<typename T>
		extern const T& max(const T&& _l, const T&& _r) {
			if (_l > _r) return _l;
			else return _r;
		}
		auto sum_all(const MSTL::concepts::Number auto & ...args);
	}
}

#endif

