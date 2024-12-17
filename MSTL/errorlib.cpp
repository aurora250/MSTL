#include "errorlib.h"

MSTL_BEGIN_NAMESPACE__

const_cstring MSTL_NOTERROR__ = "NotError";
const std::shared_ptr<MemoryError> MSTL_MEMORY_ERROR_PTR__ = std::make_shared<MemoryError>(MemoryError());

Error::Error(cstring _info, cstring _type) noexcept : _info(_info), _type(_type) {}
Error::~Error() {};
const_cstring Error::__type__ = "Error";

StopIterator::StopIterator(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring StopIterator::__type__ = "StopIterator";

AssertError::AssertError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring AssertError::__type__ = "AssertError";

AttributeError::AttributeError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring AttributeError::__type__ = "AttributeError";

MemoryError::MemoryError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring MemoryError::__type__ = "MemoryError";

ValueError::ValueError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring ValueError::__type__ = "ValueError";

RangeError::RangeError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring RangeError::__type__ = "RangeError";

SQLError::SQLError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring SQLError::__type__ = "SQLError";

NotError::NotError() noexcept : Error(__type__, __type__) {}
const_cstring NotError::__type__ = MSTL_NOTERROR__;

void __show_data_only(const Error& e, std::ostream& _out) {
	_out << "Exception : (" << e._type << ") " << e._info << std::flush;
}

std::ostream& operator <<(std::ostream& _out, const Error& err) {
	__show_data_only(err, _out);
	return _out;
}

void Exception(const Error& _err) {
	__show_data_only(_err, std::cerr);
	std::cerr << std::endl;
	throw _err;
}

void Exception(bool _boolean, const Error& _err) {
	if (_boolean) return;
	else {
		if (_err._type == MSTL_NOTERROR__) Assert(false);
		else Exception(_err);
	}
}

inline void Assert(bool _boolean, const char* _info) {
	if (_boolean) return;
	else Exception(AssertError(_info));
}

void Exit(bool _abort, void(* _func)(void)) {
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
