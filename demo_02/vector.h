#ifndef VECTOR_H
#define VECTOR_H
#include "iterable.h"
#include "basiclib.h"
#include <iostream>

namespace MSTL {
	template<typename T>
	class vector : public sciterable {
	public:
		typedef T* iterator;
		typedef const T* const_iterator;
		static const char* __type__;
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

		inline void _show_data_only(std::ostream& _out) const {
			for (size_t i = 0; i < this->_size; i++) {
				_out << this->_data[i];
			}
		}
		inline void _range_check(int _pos) const {
			Exception(this->_in_boundary(_pos));
		}
		inline bool _in_boundary(int _pos) const {
			if (_pos < 0)return false;
			else return _pos < this->_size ? true : false;
		}
	public:
		explicit vector(const T* _ptr = nullptr) {
			if (_ptr) this->_data = nullptr;
			else {
				this->_data = new T[this->_capacity];

			}
		}
		explicit vector(const vector<T>& _vec);
		explicit vector(vector<T>& _vec);
		explicit vector(const std::initializer_list<T>& _lis);
		vector<T>& operator =(const vector<T>& _vec);
		~vector() {}
		void reserve(int _cap);
		void push(const T& _val);
		T& pop();
		void append(const T* _val);
		void append(const vector<T>& _vec);
		void insert(int _pos, int _cpte, const T& _val);
		void insert(int _pos, const T* _ptr);
		void insert(int _pos, const vector<T>& _vec);
		void erase(int _pos, int _len = epos);
		void clear();
		decltype(auto) copy(int _pos = 0, int _len = epos);
		void swap(vector<T>& _vec) noexcept;
		void resize(int _cap, const T& _val);
		int find(const T& _tar, int _pos = 0);
		int find(const T* _tar, int _pos = 0);

		iterator begin();
		iterator end();
		const_iterator const_begin() const;
		const_iterator const_end() const;

		T& operator [](int _pos);
		const T& operator[] (int _pos) const;
		vector<T>& operator +=(const T& _val);
		vector<T>& operator +=(const T* _ptr);
		vector<T>& operator +=(const vector<T>& _vec);
		bool operator <(const vector<T>& _vec) const;
		bool operator >(const vector<T>& _vec) const;
		bool operator <=(const vector<T>& _vec) const;
		bool operator >=(const vector<T>& _vec) const;
		bool operator !=(const vector<T>& _vec) const;
		bool operator ==(const vector<T>& _vec) const;
		friend std::ostream& operator <<(std::ostream& _out, const vector<T>& _vec);
	};
	template <typename T>
	const char* vector<T>::__type__ = "vector";

	template <typename T>
	inline size_t origin_arr_len(const T* _ptr) {
		return sizeof(_ptr) / sizeof(_ptr[0]);
	}
}

#endif
