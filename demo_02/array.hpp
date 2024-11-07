#ifndef ARRAY_HPP
#define ARRAY_HPP
#include "iterable.h"
#include "error.h"
#include "basiclib.h"
#include <iostream>
#include <initializer_list>

namespace MSTL {
	template <typename T, size_t N>
	class array : public sciterable {
	public:
		typedef T* iterator;
		typedef const T* const_iterator;
		static const char* const __type__;
		void __det__(std::ostream& _out = std::cout) const {
			split_line(_out);
			_out << "type: " << __type__ << std::endl;
			this->_show_sc_only(_out);
			_out << "data: " << std::flush;
			this->_show_data_only(_out);
			_out << std::endl;
			split_line(_out);
		}
	private:
		T* _data;

		inline void _copy_ptr_only(array<T, N>&& _arr) {
			for (size_t i = 0; i < _arr._size; i++) {
				std::swap(this->_data[i], _arr._data[i]);
			}
		}
		inline void _copy_ptr_only(const array<T, N>& _arr) {
			for (size_t i = 0; i < _arr._size; i++) {
				this->_data[i] = _arr._data[i];
			}
		}
		inline void _range_check(int _pos) const {
			Exception(this->_in_boundary(_pos), new RangeError());
		}
		inline void _show_data_only(std::ostream& _out) const {
			size_t _band = this->_size - 1;
			_out << '[' << std::flush;
			for (size_t i = 0; i < this->_size; i++) {
				_out << this->_data[i] << std::flush;
				if (i != _band) _out << ", " << std::flush;
			}
			_out << ']' << std::flush;
		}
	public:
		explicit array() noexcept : sciterable(N == 0 ? 1 : N, N == 0 ? 1 : N) {
			this->_data = new T[N == 0 ? 1 : N];
		}
		explicit array(const std::initializer_list<T>& _lis) noexcept : sciterable(N == 0 ? 1 : N, N == 0 ? 1 : N) {
			this->_data = new T[N == 0 ? 1 : N];
			auto _iter = _lis.begin();
			for (size_t i = 0; i < this->_size; i++) {
				this->_data[i] = (*_iter);
				_iter++;
			}
			
		}
		explicit array(array<T, N>&& _arr) noexcept : sciterable(_arr._size, _arr._capacity) {
			this->_data = new T[N == 0 ? 1 : N];
			this->_copy_ptr_only(std::forward<array<T, N>>(_arr));
		}
		explicit array(const array<T, N>& _arr) noexcept : sciterable(_arr._size, _arr._capacity) {
			this->_data = new T[N == 0 ? 1 : N];
			this->_copy_ptr_only(_arr);
		}
		array<T, N>& operator = (array<T, N>&& _arr) {
			if (this == &_arr) return *this;
			this->_set_sc_only(_arr._size, _arr._capacity);
			this->_copy_ptr_only(std::forward<array<T, N>>(_arr));
			return *this;
		}
		~array() {
			this->_size = this->_capacity = 0;
			delete[] this->_data;
			this->_data = nullptr;
		}
		void swap(array<T, N>&& _arr) noexcept {
			using std::swap;
			swap(this->_data, _arr._data);
			swap(this->_size, _arr._size);
			swap(this->_capacity, _arr._capacity);
		}
		void data() const {
			this->_show_data_only();
		}
		size_t capacity() const {
			return this->_size;
		}
		T& back() {
			return this->_data[not N == 0 ? N - 1 : 0];
		}
		T& front() {
			return this->_data[0];
		}
		const T& at(int _pos) const {
			this->_range_check(_pos);
			return this->_data[_pos];
		}
		T& at(int _pos) {
			return const_cast<T&>(
				static_cast<const array<T, N>&>(*this).at(_pos)
			);
		}
		int find(const T&& _tar, int _start = 0, int _end = -1) const {
			this->_range_check(_start);
			size_t _fin = 0;
			if (_end == this->epos) _fin = this->_size;
			else if (this->_in_boundary(_end)) _fin = _end;
			else Exception(new RangeError());
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
		bool operator ==(const array<T, N>& _arr) const {
			for (size_t i = 0; i < this->_size; i++) {
				if (this->_data[i] != _arr._data[i]) return false;
			}
			return true;
		}
		bool operator !=(const array<T, N>& _arr) const {
			return not (*this == _arr);
		}
	};
	template<typename T, size_t N>
	const char* const array<T, N>::__type__ = "array";
}

template <typename T, size_t N>
std::ostream& operator <<(std::ostream& _out, const MSTL::array<T, N>& _arr) {
	_arr._show_data_only(_out);
	return _out;
}

#endif

