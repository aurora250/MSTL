#include "basiclib.h"
#include <assert.h>
MSTL_BEGIN_NAMESPACE__

void split_line(std::ostream& out, uint32_t size, char split_type) {
	while (size--) out << split_type;
	out << std::endl;
}

void* memcpy(void* _dest, const void* _rsc, size_t _byte) {
	assert(_dest && _rsc);
	void* _ret = _dest;
	while (_byte--) {
		*(char*)_dest = *(char*)_rsc;
		_dest = (char*)_dest + 1;
		_rsc = (char*)_rsc + 1;
	}
	return _ret;
}

wchar_t* wmemcpy(wchar_t* _dest, const wchar_t* _rsc, size_t _count) {
	assert(_dest && _rsc);
	wchar_t* _ret = _dest;
	while (_count--) {
		*_dest = *_rsc;
		++_dest;
		++_rsc;
	}
	return _ret;
}

int memcmp(const void* _dest, const void* _rsc, size_t _byte) {
	assert(_dest && _rsc);
	while (_byte--) {
		if (*(char*)_dest != *(char*)_rsc) return (int)(*(char*)_dest - *(char*)_rsc);
		_dest = (char*)_dest + 1;
		_rsc = (char*)_rsc + 1;
	}
	return 0;
}

int wmemcmp(const wchar_t* _dest, const wchar_t* _rsc, size_t _count) {
	assert(_dest && _rsc);
	while (_count--) {
		if (*_dest != *_rsc) return (int)(*_dest - *_rsc);
		++_dest;
		++_rsc;
	}
	return 0;
}

void* memchr(const void* _dest, int _val, size_t _byte) {
	assert(_dest);
	const char* p = (const char*)_dest;
	while (_byte--) {
		if (*p == (char)_val) {
			return (void*)p;
		}
		p++;
	}
	return nullptr;
}

wchar_t* wmemchr(const wchar_t* _dest, wchar_t _val, size_t _count) {
	assert(_dest);
	const wchar_t* p = _dest;
	while (_count--) {
		if (*p == _val) {
			return const_cast<wchar_t*>(p);
		}
		p++;
	}
	return nullptr;
}

void* memmove(void* _dest, const void* _rsc, size_t _byte) {
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

wchar_t* wmemmove(wchar_t* _dest, const wchar_t* _src, size_t _count) {
	assert(_dest && _src);
	return (wchar_t*)memmove(_dest, _src, _count * sizeof(wchar_t));
}

void* memset(void* _dest, int _val, size_t _size) {
	assert(_dest);
	void* ret = (char*)_dest;
	while (_size--) {
		*(char*)_dest = _val;
		_dest = (char*)_dest + 1;
	}
	return ret;
}

wchar_t* wmemset(wchar_t* _dest, wchar_t _val, size_t _count) {
	assert(_dest);
	wchar_t* ret = _dest;
	while (_count--) {
		*_dest = _val;
		_dest++;
	}
	return ret;
}


int strlen(const char* _str) {
	if (*_str != '\0') return strlen(_str + 1) + 1;
	else return 0;
}

int wcslen(const wchar_t* _str) {
	if (*_str != L'\0') return wcslen(_str + 1) + 1;
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
	return nullptr;
}

char* memstr(char* _data, int _len, char* _sub) {
	if (_data == nullptr || _len <= 0 || _sub == nullptr) return nullptr;
	if (*_sub == '\0') return nullptr;
	int _sublen = strlen(_sub);
	char* _cur = _data;
	int _las = _len - _sublen + 1;
	for (int i = 0; i < _las; i++) {
		if (*_cur == *_sub) {
			if (memcmp(_cur, _sub, _sublen) == 0) return _cur;
		}
		_cur++;
	}
	return nullptr;
}

MSTL_END_NAMESPACE__
