#ifndef VECTOR_H
#define VECTOR_H
#include "base_container.h"
#include "basiclib.h"
#include <iostream>
#include "initializer_list"
#include "memmory.hpp"
#include "error.h"

namespace MSTL {
	template<class T, class Alloc = simple_alloc<T>>
	class vector : container {
	public:
		typedef T					value_type;
		typedef T*					pointer;
		typedef T*					iterator;
		typedef const T*			const_iterator;
		typedef T&					reference;
		typedef const T&			const_reference;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;
		typedef vector<T, Alloc>	self;

		static const char* __type__;

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

		iterator start;
		iterator finish;
		iterator end_of_storage;
		data_allocator alloc;

		inline void __show_data_only(std::ostream& _out) const {
			for (auto iter = const_begin(); iter != const_end(); iter++) {
				_out << *iter;
			}
		}
		inline void __range_check(int _pos) const {
			Exception(this->__in_boundary(_pos), error_map[__error::RangeErr]);
		}
		inline bool __in_boundary(int _pos) const {
			if (_pos < 0) return false;
			else return _pos < this->_size ? true : false;
		}
		void fill_initialize(size_type n, const_reference value) {
			start = allocate_and_fill(n, value);
			finish = start + n;
			end_of_storage = finish;
			_size = n;
		}
		iterator allocate_and_fill(size_type n, const_reference x) {
			iterator result = alloc.allocate(n);
			uninitalized_fill_n(result, n, x);
			return result;
		}
		void deallocate() {
			if (start) alloc.deallocate(start, end_of_storage - start);
			start = nullptr; finish = nullptr; end_of_storage = nullptr;
			_size = 0;
		}
		void insert_aux(iterator position, const_reference x) {
			if (finish != end_of_storage) {
				construct(finish, *(finish - 1));
				++finish;
				value_type x_copy = x;
				copy_backward(position, finish - 2, finish - 1);
				*position = x_copy;
				return;
			}
			const size_type old_size = size();
			const size_type len = old_size != 0 ? 2 * old_size : 1;
			iterator new_start = alloc.allocate(len);
			iterator new_finish = new_start;
			try {
				new_finish = uninitialized_copy(start, position, new_start);
				construct(new_finish, x);
				++new_finish;
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw Exception(__error::MemErr);
			}
			destory(begin(), end());
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	public:
		vector() : start(0), finish(0), end_of_storage(0), alloc() {}
		explicit vector(size_type n) { fill_initialize(n, T()); }
		vector(size_type n, const_reference value) { fill_initialize(n, value); }
		vector(int n, const_reference value) { fill_initialize(n, value); }
		vector(long n, const_reference value) { fill_initialize(n, value); }
		vector(const self& v) : start(0), finish(0), end_of_storage(0) {
			start = new T[v.capacity()];
			for (size_t i = 0; i < v.size(); i++) start[i] = v[i];
			finish = start + v.size();
			end_of_storage = start + v.capacity();
		}
		vector(const std::initializer_list<T>& l) : start(0), finish(0), end_of_storage(0) {
			start = new T[l.size()];
			for (auto iter = l.begin(); iter != l.end(); iter++) {
				*start = *iter;
				++start;
			}
			finish = start + l.size();
			end_of_storage = finish;
			_size = l.size();
		}
		self& operator =(const self& v) {
			if (*this == v) return *this;
			delete[] start;
			start = new T[v.capacity()];
			for (size_t i = 0; i < v.size(); i++) start[i] = v[i];
			finish = start + v.size();
			end_of_storage = start + v.capacity();
			return *this;
		}
		~vector() {
			destroy(start, finish);
			deallocate();
		}

		iterator begin() { return start; }
		iterator end() { return finish; }
		const_iterator const_begin() const { return start; }
		const_iterator const_end() const { return finish; }
		size_type size() const { return size_type(finish - start); }
		size_type capacity() const { return size_type(end_of_storage - begin()); }
		bool empty() const { return start == finish; }
		reference operator[](size_type n) { return *(begin() + n); }
		reference front() { return *begin(); }
		reference back() { return *(end() - 1); }

		void reserve(size_type n)
		{
			if (n > capacity())
			{
				size_t sz = size();
				T* tmp = new T[n];
				if (start) {
					for (size_t i = 0; i < sz; i++) tmp[i] = start[i];
					delete[] start;
				}
				start = tmp;
				finish = start + sz;
				end_of_storage = start + n;
			}
		}
		void push_back(const_reference val) {
			if (finish != end_of_storage) {
				construct(finish, val);
				++finish;
			}
			else insert_aux(end(), val);
		}
		void pop_back() {
			destroy(finish);
			--finish;
		}
		iterator erase(iterator first, iterator last) {
			iterator i = copy(last, finish, first);
			destroy(i, finish);
			finish = finish - (last - first);
			return finish;
		}
		iterator erase(iterator position) {
			if (position + 1 != end()) copy(position + 1, finish, position);
			--finish;
			destory(finish);
			return position;
		}
		void clear() { erase(begin(), end()); }
		void insert(iterator position, size_type n, const T& x) {
			if (n == 0) return;
			if (size_type(end_of_storage - finish) >= n) {
				T x_copy = x;
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				if (elems_after > n) {
					uninitialized_copy(finish - n, finish, finish);
					finish += n;
					copy_backward(position, old_finish - n, old_finish);
					fill(position, position + n, x_copy);
				}
				else {
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n;
					uninitialized_copy(position, old_finish, finish);
					finish += elems_after;
					fill(position, old_finish, x_copy);
				}
			}
			else {
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);
				iterator new_start = alloc.allocate(len);
				iterator new_finish = new_start;
				try {
					new_finish = uninitialized_copy(start,position,new_start);
					new_finish = uninitialized_fill_n(new_finish,n,x);
					new_finish = uninitialized_copy(position,finish,new_finish);
				}
				catch (...) {
					destroy(new_start, new_finish);
					alloc.deallocate(new_start, len);
					Exception(__error::MemErr);
				}
				destroy(start, finish);
				deallocate();
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;
			}
		}
	};
	template <typename T, typename Alloc>
	const char* vector<T, Alloc>::__type__ = "vector";
}

#endif
