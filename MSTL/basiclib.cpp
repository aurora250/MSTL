#include "basiclib.h"
#include "errorlib.h"

MSTL_BEGIN_NAMESPACE__

void split_line(std::ostream& _out, size_t _size) {
	while (_size) _out << '-', _size--;
	_out << std::endl;
}
void* memcpy(void* _dest, void* _rsc, int _byte) {
	Exception(_dest && _rsc, new StopIterator());
	void* _ret = _dest;
	while (_byte--) {
		*(char*)_dest = *(char*)_rsc;
		_dest = (char*)_dest + 1;
		_rsc = (char*)_rsc + 1;
	}
	return _ret;
}
int memcmp(const void* _dest, const void* _rsc, int _byte) {
	Exception(_dest && _rsc, new StopIterator());
	while (_byte--) {
		if (*(char*)_dest != *(char*)_rsc) return *(char*)_dest - *(char*)_rsc;
		_dest = (char*)_dest + 1;
		_rsc = (char*)_rsc + 1;
	}
	return 0;
}
void* memmove(void* _dest, const void* _rsc, int _byte) {
	Exception(_dest && _rsc, new StopIterator());
	void* _ret = _dest;
	if (_dest < _rsc) {
		while (_byte--) {
			*(char*)_dest = *(char*)_rsc;
			_dest = (char*)_dest + 1;
			_rsc = (char*)_rsc + 1;
		}
	}
	else if (_dest > _rsc) {
		while (_byte--) {
			*((char*)_dest + _byte) = *((char*)_rsc + _byte);
		}
	}
	return _ret;
}
void* memset(void* _dest, int _val, size_t _size) {
	Exception(_dest, new StopIterator());
	void* ret = (char*)_dest;
	while (_size--)
	{
		*(char*)_dest = _val;
		_dest = (char*)_dest + 1;
	}
	return ret;
}
size_t deque_buf_size(size_t n, size_t sz) {
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : 1);
}

MSTL_END_NAMESPACE__
