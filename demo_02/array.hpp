#ifndef MSTL_ARRAY_HPP__
#define MSTL_ARRAY_HPP__
#include "container.h"
#include "error.h"
#include "basiclib.h"
#include "memory.hpp"
#include <iostream>
#include <initializer_list>

MSTL_BEGIN_NAMESPACE__
template <typename T, size_t N, typename Alloc = simple_alloc<T, std::allocator<T>>>
class array : public container {
public:
	typedef T					value_type;
	typedef T& reference;
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef const T& const_reference;
	typedef array<T, N, Alloc>	self;

	static const char* const __type__;

	void __det__(std::ostream& _out = std::cout) const {
		split_line(_out);
		_out << "type: " << __type__ << std::endl;
		_out << "check type: " << check_type<self>() << std::endl;
		this->__show_size_only(_out);
		_out << "data: " << std::flush;
		this->__show_data_only(_out);
		_out << std::endl;
		split_line(_out);
	}
private:
	typedef Alloc data_allocator;

	T* _data;
	data_allocator alloc;

	inline void __copy_ptr_only(self&& _arr) {
		for (size_t i = 0; i < _arr._size; i++) {
			std::swap(this->_data[i], _arr._data[i]);
		}
	}
	inline void __copy_ptr_only(const self& _arr) {
		for (size_t i = 0; i < _arr._size; i++) {
			this->_data[i] = _arr._data[i];
		}
	}
	inline void __range_check(int _pos) const {
		Exception(this->__in_boundary(_pos), error_map[__error::RangeErr]);
	}
	inline void __show_data_only(std::ostream& _out) const {
		size_t _band = this->_size - 1;
		_out << '[' << std::flush;
		for (size_t i = 0; i < this->_size; i++) {
			_out << this->_data[i] << std::flush;
			if (i != _band) _out << ", " << std::flush;
		}
		_out << ']' << std::flush;
	}
public:
	explicit array() noexcept : container(N == 0 ? 1 : N) {
		this->_data = new T[N == 0 ? 1 : N];
	}
	explicit array(const std::initializer_list<T>& _lis) noexcept : container(N == 0 ? 1 : N) {
		this->_data = new T[N == 0 ? 1 : N];
		auto _iter = _lis.begin();
		for (size_t i = 0; i < this->_size; i++) {
			this->_data[i] = (*_iter);
			_iter++;
		}

	}
	explicit array(self&& _arr) noexcept : container(_arr._size) {
		this->_data = new T[N == 0 ? 1 : N];
		this->_copy_ptr_only(std::forward<self>(_arr));
	}
	explicit array(const self& _arr) noexcept : container(_arr._size) {
		this->_data = new T[N == 0 ? 1 : N];
		this->__copy_ptr_only(_arr);
	}
	self& operator = (self&& _arr) {
		if (this == &_arr) return *this;
		this->_set_sc_only(_arr._size, _arr._capacity);
		this->_copy_ptr_only(std::forward<self>(_arr));
		return *this;
	}
	~array() {
		delete[] this->_data;
		this->_data = nullptr;
	}
	void swap(self&& _arr) noexcept {
		using std::swap;
		swap(this->_data, _arr._data);
		swap(this->_size, _arr._size);
	}
	T& back() {
		return this->_data[not N == 0 ? N - 1 : 0];
	}
	T& front() {
		return this->_data[0];
	}
	const T& at(int _pos) const {
		this->__range_check(_pos);
		return this->_data[_pos];
	}
	T& at(int _pos) {
		return const_cast<T&>(
			static_cast<const self&>(*this).at(_pos)
			);
	}
	int find(const T&& _tar, int _start = 0, int _end = -1) const {
		this->__range_check(_start);
		size_t _fin = 0;
		if (_end == this->epos) _fin = this->_size;
		else if (this->__in_boundary(_end)) _fin = _end;
		else Exception(error_map[__error::RangeErr]);
		for (size_t i = _start; i < _fin; i++) {
			if (this->_data[i] == _tar) return i;
		}
		return this->epos;
	}
	const_iterator const_begin() const {
		return this->_data;
	}
	const_iterator const_end() const {
		return this->_data + this->_size;
	}
	iterator begin() {
		return this->_data;
	}
	iterator end() {
		return this->_data + this->_size;
	}
	T& operator [](int _pos) {
		return this->at(_pos);
	}
	const T& operator [](int _pos) const {
		return this->at(_pos);
	}
};
template <typename T, size_t N, typename Alloc>
const char* const array<T, N, Alloc>::__type__ = "array";

template <typename T, size_t N, typename Alloc>
inline bool operator ==(const array<T, N, Alloc>& lh, const array<T, N, Alloc>& rh){
	return equal(lh.const_begin(), lh.const_end(), rh.const_begin());
}
template <typename T, size_t N, typename Alloc>
inline bool operator !=(const array<T, N, Alloc>& lh, const array<T, N, Alloc>& rh) {
	return not equal(lh.const_begin(), lh.const_end(), rh.const_begin());
}

template <typename T, size_t N, typename Alloc>
std::ostream& operator <<(std::ostream& _out, const array<T, N, Alloc>& _arr) {
	_arr.__show_data_only(_out);
	return _out;
}
MSTL_END_NAMESPACE__

#endif // MSTL_ARRAY_HPP__

