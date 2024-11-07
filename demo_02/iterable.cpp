#include "iterable.h"
#include "error.h"

namespace MSTL {
	const char* const sciterable::__type__ = "sciterable";
	const int sciterable::epos = -1;
	sciterable::sciterable(size_t _size, size_t _capacity) noexcept : _size(_size), _capacity(_capacity) {}
	sciterable::~sciterable() {
		this->_size = this->_capacity = 0;
	}
	inline void sciterable::__show_sc_only(std::ostream& _out) const {
		_out << "size: " << this->_size << std::endl;
		_out << "capacity: " << this->_capacity << std::endl;
	}
	inline bool sciterable::__in_boundary(int _pos) const {
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

	const char* const siterable::__type__ = "siterable";
	const int siterable::epos = -1;
	void siterable::__show_size_only(std::ostream& _out) const {
		_out << "size: " << this->_size << std::endl;
	}
	bool siterable::__in_boundary(int _pos) const {
		if (_pos < 0) return false;
		else return _pos < this->_size ? true : false;
	}
	siterable::siterable(size_t _size) noexcept : _size(_size) {}
	siterable::~siterable() {
		this->_size = 0;
	}
	size_t siterable::size() const {
		return this->_size;
	}
	bool siterable::empty() const {
		return this->_size == 0;
	}
}
