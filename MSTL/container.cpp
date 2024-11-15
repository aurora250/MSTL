#include "container.h"

MSTL_BEGIN_NAMESPACE__

const int container::epos = -1;
void container::__show_size_only(std::ostream& _out) const {
	_out << "size: " << this->_size << std::endl;
}
bool container::__in_boundary(int _pos) const {
	if (_pos < 0) return false;
	else return _pos < this->_size ? true : false;
}
container::container(size_type _size) : _size(_size) {}
container::~container() { this->_size = 0; }
container::size_type container::size() const { return this->_size; }
bool container::empty() const { return this->_size == 0; }

MSTL_END_NAMESPACE__
