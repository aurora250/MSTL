#ifndef MSTL_FUNCTOR_HPP__
#define MSTL_FUNCTOR_HPP__
#include "type_traits.hpp"
MSTL_BEGIN_NAMESPACE__

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

template <typename T = void>
struct plus : public binary_function<T, T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x, const T& y) const
		noexcept(noexcept(MSTL::declcopy<T>(x + y))) {
		return x + y;
	}
};
template <typename T = void>
struct minus : public binary_function<T, T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x, const T& y) const 
		noexcept(noexcept(MSTL::declcopy<T>(x - y))) {
		return x - y;
	}
};
template <typename T = void>
struct multiplies : public binary_function<T, T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x, const T& y) const 
		noexcept(noexcept(MSTL::declcopy<T>(x * y))) {
		return x * y;
	}
};
template <typename T = void>
struct divides : public binary_function<T, T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x, const T& y) const
		noexcept(noexcept(MSTL::declcopy<T>(x / y))) {
		return x / y;
	}
};
template <typename T = void>
struct modulus : public binary_function<T, T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x, const T& y) const
		noexcept(noexcept(MSTL::declcopy<T>(x % y))) {
		return x % y;
	}
};
template <typename T = void>
struct negate : public unary_function<T, T> {
	MSTL_NODISCARD MSTL_CONSTEXPR T operator()(const T& x) const
		noexcept(noexcept(MSTL::declcopy<T>(-x))) {
		return -x;
	}
};

//relational:

template <typename T = void>
struct equal_to : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const 
		noexcept(noexcept(MSTL::declcopy<bool>(x == y))) {
		return x == y;
	}
};
template <>
struct equal_to<void> {
	template <typename T1, typename T2>
	MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) operator()(T1&& x, T2&& y) const
		noexcept(noexcept(static_cast<T1&&>(x) == static_cast<T2&&>(y))) {
		return static_cast<T1&&>(x) == static_cast<T2&&>(y);
	}
};

template <typename T = void>
struct not_equal_to : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const
		noexcept(noexcept(MSTL::declcopy<bool>(x != y))) {
		return x != y;
	}
};
template <>
struct not_equal_to<void> {
	template <typename T1, typename T2>
	MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) operator()(T1&& x, T2&& y) const
		noexcept(noexcept(static_cast<T1&&>(x) != static_cast<T2&&>(y))) {
		return static_cast<T1&&>(x) != static_cast<T2&&>(y);
	}
};

template <typename T = void>
struct greater : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const
		noexcept(noexcept(MSTL::declcopy<bool>(x > y))) {
		return x > y;
	}
};
template <>
struct greater<void> {
	template <typename T1, typename T2>
	MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) operator()(T1&& x, T2&& y) const
		noexcept(noexcept(static_cast<T1&&>(x) > static_cast<T2&&>(y))) {
		return static_cast<T1&&>(x) > static_cast<T2&&>(y);
	}
};

template <typename T = void>
struct less : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const
		noexcept(noexcept(MSTL::declcopy<bool>(x < y))) {
		return x < y;
	}
};
template <>
struct less<void> {
	template <typename T1, typename T2>
	MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) operator()(T1&& x, T2&& y) const
		noexcept(noexcept(static_cast<T1&&>(x) < static_cast<T2&&>(y))) {
		return static_cast<T1&&>(x) < static_cast<T2&&>(y);
	}
};

template <typename T = void>
struct greater_equal : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const
		noexcept(noexcept(MSTL::declcopy<bool>(x >= y))) {
		return x >= y;
	}
};
template <>
struct greater_equal<void> {
	template <typename T1, typename T2>
	MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) operator()(T1&& x, T2&& y) const
		noexcept(noexcept(static_cast<T1&&>(x) >= static_cast<T2&&>(y))) {
		return static_cast<T1&&>(x) >= static_cast<T2&&>(y);
	}
};

template <typename T = void>
struct less_equal : public binary_function<T, T, bool> {
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator()(const T& x, const T& y) const
		noexcept(noexcept(MSTL::declcopy<bool>(x <= y))) {
		return x <= y;
	}
};
template <>
struct less_equal<void> {
	template <typename T1, typename T2>
	MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) operator()(T1&& x, T2&& y) const
		noexcept(noexcept(static_cast<T1&&>(x) <= static_cast<T2&&>(y))) {
		return static_cast<T1&&>(x) <= static_cast<T2&&>(y);
	}
};

//others:
template <typename T>
struct identity : public unary_function<T, T> {
	template <typename U = T>
	MSTL_NODISCARD MSTL_CONSTEXPR U&& operator()(U&& x) const noexcept {
		return MSTL::forward<U>(x);
	}
};

template <typename Pair>
	requires(is_pair_v<Pair>)
struct select1st : public unary_function<Pair, typename Pair::first_type> {
	MSTL_NODISCARD MSTL_CONSTEXPR typename Pair::first_type&& 
		operator()(Pair&& x) const noexcept(noexcept(MSTL::forward<typename Pair::first_type>(x.first))) {
		return MSTL::forward<typename Pair::first_type>(x.first);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const typename Pair::first_type& 
		operator()(const Pair& x) const noexcept {
		return x.first;
	}
};

template <typename Pair>
	requires(is_pair_v<Pair>)
struct select2nd : public unary_function<Pair, typename Pair::second_type> {
	MSTL_NODISCARD MSTL_CONSTEXPR typename Pair::second_type&&
		operator()(Pair&& x) const noexcept(noexcept(MSTL::forward<typename Pair::second_type>(x.second))) {
		return MSTL::forward<typename Pair::second_type>(x.second);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const typename Pair::second_type&
		operator()(const Pair& x) const noexcept {
		return x.second;
	}
};

struct display {
	typedef display self;

	display(const char* finish = "", const char* split = "")
		noexcept(is_nothrow_constructible_v<std::string, const char*>)
		: split_(split), finish_(finish) {}
	explicit display(const std::string& split, const std::string& finish) 
		noexcept(is_nothrow_copy_constructible_v<std::string>)
		: split_(split), finish_(finish) {}
	explicit display(std::string&& split, std::string&& finish)
		noexcept(is_nothrow_move_constructible_v<std::string>)
		: split_(MSTL::forward<std::string>(split)), finish_(MSTL::forward<std::string>(finish)) {}

	template <typename... T>
	self& operator ()(T&&... value) {
		((std::cout << MSTL::forward<T>(value) << split_), ...);
		std::cout << finish_ << std::flush;
		return *this;
	}
	std::string split_;
	std::string finish_;
};

MSTL_END_NAMESPACE__

#endif // MSTL_FUNCTOR_HPP__
