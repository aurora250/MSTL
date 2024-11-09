#include "basiclib.h"
#include "error.h"

MSTL_BEGIN_NAMESPACE__

inline void repect_ostm_str(const char* _str, std::ostream& _out, size_t _size) throw(ValueError) {
	Exception(_size > 0 && _str, new ValueError());
	while (_size-- != 0) _out << _str;
}
inline void repect_ostm_chr(char _chr, std::ostream& _out, size_t _size) throw(ValueError) {
	Exception(_size > 0 && _chr, new ValueError());
	while (_size-- != 0) _out << _chr;
}
inline void split_line(std::ostream& _out, size_t _size) throw(ValueError) {
	repect_ostm_chr('-', _out);
	_out << std::endl;
}
void* memcpy(void* _dest, void* _rsc, int _byte) throw(ValueError) {
	Exception(_dest && _rsc, new StopIterator());
	void* _ret = _dest;
	while (_byte--) {
		*(char*)_dest = *(char*)_rsc;
		_dest = (char*)_dest + 1;
		_rsc = (char*)_rsc + 1;
	}
	return _ret;
}
int memcmp(const void* _dest, const void* _rsc, int _byte) throw(ValueError) {
	Exception(_dest && _rsc, new StopIterator());
	while (_byte--) {
		if (*(char*)_dest != *(char*)_rsc) return *(char*)_dest - *(char*)_rsc;
		_dest = (char*)_dest + 1;
		_rsc = (char*)_rsc + 1;
	}
	return 0;
}
void* memmove(void* _dest, const void* _rsc, int _byte) throw(ValueError) {
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
void* memset(void* _dest, int _val, size_t _size) throw(ValueError) {
	Exception(_dest, new StopIterator());
	void* ret = (char*)_dest;
	while (_size--)
	{
		*(char*)_dest = _val;
		_dest = (char*)_dest + 1;
	}
	return ret;
}

MSTL_END_NAMESPACE__
