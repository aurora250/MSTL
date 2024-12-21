#include "errorlib.h"

MSTL_BEGIN_NAMESPACE__

Error::Error(cstring _info, cstring _type) noexcept : _info(_info), _type(_type) {}
Error::~Error() {};
const_cstring Error::__type__ = TO_STRING(Error);

StopIterator::StopIterator(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring StopIterator::__type__ = TO_STRING(StopIterator);

AssertError::AssertError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring AssertError::__type__ = TO_STRING(AssertError);

AttributeError::AttributeError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring AttributeError::__type__ = TO_STRING(AttributeError);

MemoryError::MemoryError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring MemoryError::__type__ = TO_STRING(MemoryError);

ValueError::ValueError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring ValueError::__type__ = TO_STRING(ValueError);

RangeError::RangeError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring RangeError::__type__ = TO_STRING(RangeError);

SQLError::SQLError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring SQLError::__type__ = TO_STRING(SQLError);

MathError::MathError(cstring _info) noexcept : Error(_info, __type__) {}
const_cstring MathError::__type__ = TO_STRING(MathError);

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
		if (_err._type == TO_STRING(Error)) Assert(false);
		else Exception(_err);
	}
}

inline void Assert(bool _boolean, const char* _info) { // useless in release
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
