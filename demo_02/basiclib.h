#ifndef BASICLIB_H
#define BASICLIB_H
#include <initializer_list>
#include <iostream>

#define to_definestr(v) #v

#if _HAS_CXX20  
#include <concepts>
namespace MSTL {
	namespace concepts {
		template <typename T>
		concept MSTL_TYPE = requires(T _mstl) {
			T::__type__;
			_mstl.__det__;
			_mstl._show_data_only();
			_mstl.empty();
		};
		template <typename T, typename B>
		concept Derived = std::is_base_of<B, T>::value;
		template<typename T>
		constexpr bool SWAPABLE = requires(T a, T b) {
			a.swap(b);
		};

		template<typename T>
		concept Hashable = requires(T a) {
			{ std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
		};
		template <typename T>
		concept Comparable = requires(T _l, T _r) {
			_l > _r;
			_l < _r;
			_l >= _r;
			_l <= _r;
			_l == _r;
			_l != _r;
		};
		template <typename T>
		concept Outable = requires(T _val) {
			requires (std::is_same<decltype(std::cout << _val), std::ostream&>::_val == true);
		};
		template <typename T>
		concept Smaller_Bit_Int = requires {
			requires sizeof(T) <= sizeof(int);
		};
		template<size_t N>
		concept Even = requires {
			requires (N % 2 == 0);
		};
		template<size_t N>
		concept Positive = requires {
			requires (N > 0);
		};

		template <typename T>
		concept NVoidT = not std::is_void<T>::value;
		template <typename T>
		concept Integral = std::is_integral<T>::value;
		template <typename T>
		concept Float = std::is_floating_point<T>::value;
		template <typename T>
		concept Number = std::integral<T> || std::floating_point<T>; 
		template <typename T>
		concept Trivial = std::is_trivial_v<T>;
	}

	enum Boolean {
		False = 0, True = 1
	};

	template<typename T>
	inline void ostream_set(const std::initializer_list<T>& _vals,
		bool _enter = false, std::ostream& _out = std::cout,
		const char* _join = nullptr) {
		auto _beg = _vals.begin();
		static_assert(std::is_same<decltype(std::cout << *_beg), std::ostream&>::_beg, "Type not printable");
		size_t _size = _vals.size();
		if (not _size == 0) {
			if (_join) {
				for (; _beg != _vals.end(); _beg++) {
					_out << (*_beg) << std::flush;
					if (--_size != 0)
						_out << _join << std::flush;
				}
				// or:
				// #include <iterator>  #include <algorithm>
				// std::copy(_vals.begin(), _vals.end(), std::ostream_iterator(std::cout, " "))
			}
			else {
				for (; _beg != _vals.end(); _beg++)
					_out << (*_beg) << std::flush;
			}
		}
		if (_enter) _out << std::endl;
	}
}
#endif

namespace MSTL {
	extern void repect_ostm_str(const char* _str, std::ostream& _out = std::cout, size_t _size = 10);
	extern void repect_ostm_chr(char _chr, std::ostream& _out = std::cout, size_t _size = 10);
	extern void split_line(std::ostream& _out = std::cout, size_t _size = 10);

	extern void* memcpy(void* _dest, void* _rsc, int _byte);
	extern int memcmp(const void* _dest, const void* _rsc, int _byte);
	extern void* memmove(void* _dest, const void* _rsc, int _byte);
	extern void* memset(void* _dest, int _val, size_t _byte);
}

#define SPLIT_LINE MSTL::split_line()

#endif

