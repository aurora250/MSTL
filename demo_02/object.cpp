#include "object.h"
#include "basiclib.h"

namespace MSTL {
	const char* const object::__type__ = "object";
	object::object() noexcept {};
	object::~object() = default;

	const char* const NaN::__type__ = "NaN";
	NaN::NaN() noexcept {};
	NaN::~NaN() = default;
	void NaN::__det__(std::ostream& _out = std::cout) const {
		split_line(_out);
		_out << "type: " << __type__ << std::endl;
		_out << "data: " << std::flush;
		this->_show_data_only(_out);
		_out << std::endl;
		split_line(_out);
	}
	void NaN::_show_data_only(std::ostream& _out) const {
		_out << "NaN" << std::flush;
	}
	bool NaN::empty() const {
		return true;
	}
}