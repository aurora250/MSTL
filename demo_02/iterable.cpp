#include "iterable.h"
#include "error.h"

namespace MSTL {
	const char* const sciterable::__type__ = "iterable";
	const int sciterable::epos = -1;

	sciterable::sciterable(size_t _size, size_t _capacity) noexcept : _size(_size), _capacity(_capacity) {}

	sciterable::~sciterable() {
		this->_clear_sc_only();
	}

	inline void sciterable::_clear_sc_only() {
		this->_size = this->_capacity = 0;
	}

	inline void sciterable::_show_sc_only(std::ostream& _out) const {
		_out << "size: " << this->_size << std::endl;
		_out << "capacity: " << this->_capacity << std::endl;
	}

	inline void sciterable::_set_sc_only(size_t _size, size_t _cap) {
		this->_size = _size;
		this->_capacity = _cap;
	}

	inline bool sciterable::_in_boundary(int _pos) const {
		if (_pos < 0) return false;
		else return _pos < this->_size ? true : false;
	}

	size_t sciterable::size() const {
		return this->_size;
	}
	size_t sciterable::capacity() const {
		return this->_capacity;
	}
	bool sciterable::empty() const {
		return this->_size == 0;
	}
}
