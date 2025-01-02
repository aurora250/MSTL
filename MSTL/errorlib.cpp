#include "errorlib.h"
MSTL_BEGIN_NAMESPACE__

void Exception(const Error& _err) {
	__show_data_only(_err, std::cerr);
	std::cerr << std::endl;
	throw _err;
}
MSTL_NORETURN void Exit(bool _abort, void(*_func)(void)) {
	if (_func) {
#ifdef MSTL_STATE_DEBUG__
		if (_abort)
			std::cout << "Abort failed! Function should be called first." << std::endl;
#endif
		std::atexit(_func);
	}
	else {
		if (_abort) std::abort();
		else std::exit(1);
	}
}

MSTL_END_NAMESPACE__
