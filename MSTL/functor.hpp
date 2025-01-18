#ifndef MSTL_FUNCTOR_HPP__
#define MSTL_FUNCTOR_HPP__
#include "basiclib.h"
#include "concepts.hpp"

MSTL_BEGIN_NAMESPACE__
using namespace concepts;

template <typename Arg, typename Result>
struct unary_function {
	typedef Arg		argument_type;
	typedef Result  result_type;
};
template <typename Arg1, typename Arg2, typename Result>
struct binary_function {
	typedef Arg1	first_argument_type;
	typedef Arg2	second_argument_type;
	typedef Result  result_type;
};

//arithmetic:
template <typename T>
struct plus : public binary_function<T, T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x, const T& y) const noexcept {
		return x + y;
	}
};
template <typename T>
struct minus : public binary_function<T, T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x, const T& y) const noexcept {
		return x - y;
	}
};
template <typename T>
struct multiplies : public binary_function<T, T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x, const T& y) const noexcept {
		return x * y;
	}
};
template <typename T>
struct divides : public binary_function<T, T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x, const T& y) const noexcept {
		return x / y;
	}
};
template <typename T>
struct modulus : public binary_function<T, T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x, const T& y) const noexcept {
		return x % y;
	}
};
template <typename T>
struct negate : public unary_function<T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x) const noexcept {
		return -x;
	}
};

//identity element:
template <typename T>
MSTL_CONSTEXPR T identity_element(plus<T>) { return T(0); }
template <typename T>
MSTL_CONSTEXPR T indetity_element(multiplies<T>) { return T(1); }

//relational:
template <typename T>
struct equal_to : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const noexcept {
		return x == y;
	}
};
template <typename T>
struct not_equal_to : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const noexcept {
		return x != y;
	}
};
template <typename T>
struct greater : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const noexcept {
		return x > y;
	}
};
template <typename T>
struct less : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const noexcept {
		return x < y;
	}
};
template <typename T>
struct greater_equal : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const noexcept {
		return x >= y;
	}
};
template <typename T>
struct less_equal : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const noexcept {
		return x <= y;
	}
};

//logical:
template <typename T>
struct logical_and : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const noexcept {
		return x && y;
	}
};
template <typename T>
struct logical_or : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const noexcept {
		return x || y;
	}
};
template <typename T>
struct logical_not : public unary_function<T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x) const noexcept {
		return !x;
	}
};

//others:
template <typename T>
struct identity : public unary_function<T, T> {
	template <typename U = T>
	MSTL_NODISCARD MSTL_CONSTEXPR U&& operator()(U&& x) const noexcept {
		return std::forward<U>(x);
	}
};

template <typename Pair>
	requires(PairLike<Pair>)
struct select1st : public unary_function<Pair, typename Pair::first_type> {
	MSTL_NODISCARD MSTL_CONSTEXPR typename Pair::first_type&& operator()(
		Pair&& x) const noexcept {
		return std::forward<typename Pair::first_type>(x.first);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const typename Pair::first_type& operator()(
		const Pair& x) const noexcept {
		return x.first;
	}
};
template <typename Pair>
	requires(PairLike<Pair>)
struct select2nd : public unary_function<Pair, typename Pair::second_type> {
	MSTL_NODISCARD MSTL_CONSTEXPR typename Pair::second_type&& operator()(
		Pair&& x) const noexcept {
		return std::forward<typename Pair::second_type>(x.second);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const typename Pair::second_type& operator()(
		const Pair& x) const noexcept {
		return x.second;
	}
};

struct display {
	typedef display self;

	display(const char* finish = "", const char* split = "")
		noexcept(NothrowConstructibleFrom<std::string, const char*>)
		: split_(split), finish_(finish) {}
	explicit display(const std::string& split, const std::string& finish) 
		noexcept(NothrowCopyConstructible<std::string>)
		: split_(split), finish_(finish) {}
	explicit display(std::string&& split, std::string&& finish)
		noexcept(NothrowMoveConstructible<std::string>)
		: split_(std::forward<std::string>(split)), finish_(std::forward<std::string>(finish)) {}

	template <typename... T>
	self& operator ()(T&&... value) {
		((std::cout << std::forward<T>(value) << split_), ...);
		std::cout << finish_ << std::flush;
		return *this;
	}
	std::string split_;
	std::string finish_;
};

MSTL_END_NAMESPACE__

#endif // MSTL_FUNCTOR_HPP__
