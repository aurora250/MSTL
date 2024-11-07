#include "error.h"
#include "basiclib.h"
#include "depositary.h"

namespace MSTL {
	Error::Error(cstring _info, cstring _type) noexcept : _info(_info), _type(_type) {}
	Error::~Error() {};
	void Error::__det__(std::ostream& _out) const {
		split_line(_out);
		_out << "type: " << this->_type << std::endl;
		this->__show_data_only(_out);
		_out << std::endl;
		split_line(_out);
	}
	inline void Error::__show_data_only(std::ostream& _out) const {
		_out << this->_info << std::flush;
	}
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

	void Exception(Error* _err) {
		// _out_set<const char*>({ "Exception : (" , _err->_type, ") ",_err->_info }, true, std::cerr);
		std::cerr << "Exception : (" << _err->_type << ") " << _err->_info << std::endl;
		throw* _err;
	}

	inline void Exception(bool _boolean, Error* _err) {
		if (_boolean) return;
		else {
			if (_err == nullptr) Assert(false);
			else Exception(_err);
		}
	}

	void Assert(bool _boolean, const char* _info) {
		if (_boolean) return;
		else Exception(new AssertError(_info));
	}

	void Exit(bool _abort, void(__cdecl* _func)(void)) {
		if (_func) {
			if (_abort)
				std::cout << "Abort failed! Function should be called first." << std::endl;
			std::atexit(_func);
		}
		else {
			if (_abort) std::abort();
			else std::exit(1);
		}
	}
}