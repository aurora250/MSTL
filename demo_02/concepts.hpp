#ifndef MSTL_CONCEPTS_HPP__
#define MSTL_CONCEPTS_HPP__
#include <concepts>
#include <type_traits>
#include <iostream>
#include "basiclib.h"

MSTL_BEGIN_NAMESPACE__

namespace concepts {
#ifdef _HAS_CXX20
	template <typename T>
	concept MSTL_TYPE = requires(T _mstl) {
		T::__type__;
		_mstl.__det__;
	};
	template <typename T, typename B>
	concept Derived = std::is_base_of<B, T>::value;

	template <typename T>
	concept VoidT = std::is_void<T>::value;
	template <typename T>
	concept NVoidT = not std::is_void<T>::value;
	template <typename T>
	concept Reference = std::is_reference<T>::value;
	template <typename T>
	concept Rightvalue = std::is_rvalue_reference<T>::value;
	template <typename T>
	concept Pointer = std::is_pointer<T>::value;
	template <typename T>
	concept Handler = std::is_reference<T>::value ||
		std::is_rvalue_reference<T>::value || std::is_pointer<T>::value;
	template <typename T>
	concept NHandler = (not std::is_reference<T>::value) &&
		(not std::is_rvalue_reference<T>::value) && (not std::is_pointer<T>::value);
	template <typename T>
	concept Const = std::is_const<T>::value;
	template <typename T>
	concept Volatile = std::is_volatile<T>::value;

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
	concept Swapable = std::swappable<T>;
	template <typename T>
	concept Trivial = std::is_trivial_v<T>;

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
	concept Integral = std::is_integral<T>::value;
	template <typename T>
	concept Float = std::is_floating_point<T>::value;
	template <typename T>
	concept Number = std::integral<T> || std::floating_point<T> || std::unsigned_integral<T>;
#endif // _HAS_CXX20
}

MSTL_END_NAMESPACE__

#endif // MSTL_CONCEPTS_HPP__
