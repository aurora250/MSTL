#include "basiclib.h"
#include <assert.h>

MSTL_BEGIN_NAMESPACE__

const size_t MSTL_SPLIT_LENGHT = 15;

void split_line(std::ostream& _out, size_t _size) {
	while (_size) _out << '-', _size--;
	_out << std::endl;
}
void* memcpy(void* _dest, void* _rsc, int _byte) {
	assert(_dest && _rsc);
	void* _ret = _dest;
	while (_byte--) {
		*(char*)_dest = *(char*)_rsc;
		_dest = (char*)_dest + 1;
		_rsc = (char*)_rsc + 1;
	}
	return _ret;
}
int memcmp(const void* _dest, const void* _rsc, int _byte) {
	assert(_dest && _rsc);
	while (_byte--) {
		if (*(char*)_dest != *(char*)_rsc) return *(char*)_dest - *(char*)_rsc;
		_dest = (char*)_dest + 1;
		_rsc = (char*)_rsc + 1;
	}
	return 0;
}
void* memmove(void* _dest, const void* _rsc, int _byte) {
	assert(_dest && _rsc);
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
void* memset(void* _dest, int _val, int _size) {
	assert(_dest);
	void* ret = (char*)_dest;
	while (_size--)
	{
		*(char*)_dest = _val;
		_dest = (char*)_dest + 1;
	}
	return ret;
}

size_t strlen(const char* _str) {
	if (*_str != '\0') return strlen(_str + 1) + 1;
	else return 0;
}
char* strcpy(char* _dest, const char* _sou) {
	char* _ret = _dest;
	while (*_sou != '\0') {
		*_dest = *_sou;
		_dest++;
		_sou++;
	}
	*_dest = *_sou;
	return _ret;
}
int strcmp(const char* _des, const char* _sou) {
	while (*_des == *_sou) {
		if (*_des == '\0') return 0;
		_des++;
		_sou++;
	}
	if (*_des > *_sou) return 1;
	else return -1;
}
const char* strstr(const char* _dest, const char* _sou) {
	assert(_dest && _sou);
	const char* _s1 = _dest;
	const char* _s2 = _sou;
	const char* _cur = _dest;
	while (*_cur) {
		_s1 = _cur;
		_s2 = _sou;
		while (*_s1 && *_s2 && (*_s1 == *_s2)) {
			_s1++;
			_s2++;
		}
		if (*_s2 == '\0') return _cur;
		_cur++;
	}
	return NULL;
}
char* memstr(char* _data, int _len, char* _sub) {
	if (_data == NULL || _len <= 0 || _sub == NULL) return NULL;
	if (*_sub == '\0') return NULL;
	int _sublen = strlen(_sub);
	int i;
	char* _cur = _data;
	int _las = _len - _sublen + 1;
	for (i = 0; i < _las; i++) {
		if (*_cur == *_sub) {
			if (memcmp(_cur, _sub, _sublen) == 0) return _cur;
		}
		_cur++;
	}
	return NULL;
}

size_t deque_buf_size(size_t n, size_t sz) {
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : 1);
}

MSTL_END_NAMESPACE__
