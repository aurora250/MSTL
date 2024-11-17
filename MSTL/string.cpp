#include "string.h"
#include "errorlib.h"
#include "check_type.h"

MSTL_BEGIN_NAMESPACE__

const char* const string::__type__ = "string";
void string::__det__(std::ostream& _out) const {
	split_line(_out);
	_out << "type: " << __type__ << std::endl;
	_out << "check type: " << check_type<self>() << std::endl;
	this->__show_size_only(_out);
	_out << "data: " << std::flush;
	this->__show_data_only(_out);
	_out << std::endl;
	split_line(_out);
}

inline void string::__show_data_only(std::ostream& _out) const {
	for (size_t i = 0; i < this->_size; i++) {
		_out << this->_data[i];
	}
}
inline void string::__range_check(int _pos) const {
	RangeError* e = new RangeError();
	Exception(this->__in_boundary(_pos), e);
	delete e;
}
inline bool string::__in_boundary(int _pos) const {
	if (_pos < 0) return false;
	else return _pos < this->_size + 1 ? true : false;
}

string::string(const char* _str) : container(strlen(_str)), _capacity(strlen(_str)) {
	this->_data = new char[this->_capacity + 1];
	strcpy(this->_data, _str);
}
string::string(const string& _str) : container(_str._size), _capacity(_str._capacity) {
	this->_data = new char[this->_capacity + 1];
	memcpy(this->_data, _str._data, this->_size + 1);
}
string& string::operator =(const string& _str) {
	if (this == &_str) return *this;
	this->_data = new char[_str._capacity + 1];
	this->_size = _str._size;
	this->_capacity = _str._capacity;
	memcpy(this->_data, _str._data, this->_size + 1);
	return *this;
}
string::~string() {
	if (this->_data) delete[] this->_data;
	this->_data = nullptr;
}
string::iterator string::begin() {
	return this->_data;
}
string::iterator string::end() {
	return this->_data + this->_size;
}
string::const_iterator string::const_begin() const {
	return this->_data;
}
string::const_iterator string::const_end() const {
	return this->_data + this->_size;
}
void string::reserve(int _cap) {
	if (_cap <= this->_capacity) return;
	char* _str = new char[_cap + 1];
	this->_capacity = _cap;
	memcpy(_str, this->_data, _cap + 1);
	delete[] this->_data;
	this->_data = _str;
}
void string::push(char _chr) {
	if (not empty()) this->reserve(this->empty() ? 4 : _capacity * 2);
	this->_data[this->_size] = _chr;
	this->_size++;
	this->_data[this->_size] = '\0';
}
char string::pop() {
	if (this->empty()) return '\0';
	else {
		char _las = this->_data[_size - 1];
		this->erase(this->_size - 1, 1);
		return _las;
	}
}
void string::append(const char* _str) {
	int _len = strlen(_str);
	if (this->_size + _len > _capacity) this->reserve(this->_size + _len);
	strcpy(this->end(), _str);
	this->_size += _len;
}
void string::append(const string& _str) {
	int _len = _str._size;
	if (this->_size + _len > this->_capacity) this->reserve(this->_size + _len);
	strcpy(this->end(), _str._data);
	this->_size += _len;
}
void string::insert(int _pos, int _cpte, char _chr) {
	this->__range_check(_pos);
	if (this->_size + _cpte > this->_capacity) this->reserve(this->_size + _cpte);
	int _end = this->_size;
	while (_end >= _pos && _end != this->epos) {
		this->_data[_end + _cpte] = this->_data[_end];
		_end--;
	}
	for (int i = 0; i < _cpte; i++) this->_data[_pos + i] = _chr;
	this->_size += _cpte;
}
void string::insert(int _pos, const char* _str) {
	this->__range_check(_pos);
	int _len = strlen(_str);
	if (this->_size + _len > this->_capacity) this->reserve(this->_size + _len);
	int _end = this->_size;
	while (_end >= _pos && _end != this->epos) {
		this->_data[_end + _len] = this->_data[_end];
		_end--;
	}
	for (int i = 0; i < _len; i++) this->_data[_pos + i] = _str[i];
	this->_size += _len;
}
void string::insert(int _pos, const string& _str) {
	this->__range_check(_pos);
	int _len = _str._size;
	if (this->_size + _len > this->_capacity) this->reserve(this->_size + _len);
	int _end = this->_size;
	while (_end >= _pos && _end != this->epos) {
		this->_data[_end + _len] = this->_data[_end];
		_end--;
	}
	for (int i = 0; i < _len; i++) this->_data[_pos + i] = _str._data[i];
	this->_size += _len;
}
void string::erase(int _pos, int _len) {
	this->__range_check(_pos);
	if (_len == this->epos || _pos + _len >= this->_size) {
		this->_data[_pos] = '\0';
		this->_size = _pos;
	}
	else {
		int _end = _pos + _len;
		while (_end <= this->_size) this->_data[_pos++] = this->_data[_end++];
		this->_size -= _len;
	}
}
void string::clear() {
	this->_data[0] = '\0';
	this->_size = 0;
}
string& string::copy(int _pos, int _len) {
	this->__range_check(_pos);
	string _str;
	size_t _n = _len;
	if (not this->__in_boundary(_pos + _len) || _len == this->epos) _n = this->_size - _pos;
	for (size_t i = 0; i < _n; i++)	_str += this->_data[_pos + i];
	return _str;
}
void string::swap(string& _str) {
	if (this->_capacity == _str._capacity) std::swap(this->_data, _str._data);
	else if (this->_capacity > _str._capacity) {
		_str.reserve(this->_capacity);
		auto _tem = _str._data;
		_str._data = this->_data;

	}
	std::swap(this->_size, _str._size);
	std::swap(this->_capacity, _str._capacity);
}
void string::resize(int _cap, char _chr) {
	if (_cap < this->_capacity) {
		this->_data[_cap] = '\0';
		this->_size = _cap;
	}
	else {
		this->reserve(_cap);
		for (size_t i = this->_size; i < _cap; i++) this->_data[i] = _chr;
		this->_size = _cap;
		this->_data[this->_size] = '\0';
	}
}
int string::find(char _chr, int _pos) {
	this->__range_check(_pos);
	for (size_t i = 0; i < this->_size; i++) {
		if (this->_data[i] == _chr) return i;
	}
	return this->epos;
}
int string::find(const char* _str, int _pos) {
	this->__range_check(_pos);
	const char* _p = strstr(this->begin() + _pos, _str);
	if (_p) return _p - this->begin();
	else return this->epos;
}
char& string::operator [](int _pos) {
	return const_cast<char&>(
		static_cast<const string&>(*this)[_pos]
		);
}
const char& string::operator [](int _pos) const {
	this->__range_check(_pos);
	if (_pos == this->epos) return _data[_size - 1];
	else return _data[_pos];
}
string& string::operator +=(char _chr) {
	this->push(_chr);
	return *this;
}
string& string::operator +=(const char* _str) {
	this->append(_str);
	return *this;
}
string& string::operator +=(const string& _str) {
	this->append(_str);
	return *this;
}
bool string::operator <(const string& _str) const {
	size_t i1 = 0;
	size_t i2 = 0;
	while (i1 < this->_size && i2 < _str._size) {
		if (this->_data[i1++] == _str._data[i2++]) continue;
		else if (this->_data[i1++] > _str._data[i2++]) return false;
		else return true;
	}
	return this->_size < _str._size;
}
bool string::operator >(const string& _str) const {
	return not (*this < _str && *this == _str);
}
bool string::operator <=(const string& _str) const {
	return not (*this > _str);
}
bool string::operator >=(const string& _str) const {
	return not (*this < _str);
}
bool string::operator !=(const string& _str) const {
	return not (*this == _str);
}
bool string::operator ==(const string& _str) const {
	return this->_size == _str._size && memcpy(this->_data, _str._data, this->_size);
}
std::ostream& operator <<(std::ostream& _out, const string& _str) {
	_str.__show_data_only(_out);
	return _out;
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
const char* strstr(const char* _des, const char* _sou) {
	Exception(_des && _sou, new ValueError("Value is NULL!"));
	const char* _s1 = _des;
	const char* _s2 = _sou;
	const char* _cur = _des;
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
char* memstr(char* _data, size_t _len, char* _sub) {
	if (_data == NULL || _len <= 0 || _sub == NULL) return NULL;
	if (*_sub == '\0') return NULL;
	size_t _sublen = strlen(_sub);
	size_t i;
	char* _cur = _data;
	size_t _las = _len - _sublen + 1;
	for (i = 0; i < _las; i++) {
		if (*_cur == *_sub) {
			if (memcmp(_cur, _sub, _sublen) == 0) return _cur;
		}
		_cur++;
	}
	return NULL;
}

MSTL_END_NAMESPACE__
