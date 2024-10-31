#include "math.h"

namespace MSTL {
	namespace math {
		auto sum_all(const MSTL::concepts::Number auto & ...args) { return (... + args); }
	}
}