#ifndef MSTL_CONCEPTS_HPP__
#define MSTL_CONCEPTS_HPP__
#include "basiclib.h"
#include <concepts>
#include <type_traits>
MSTL_BEGIN_NAMESPACE__

namespace concepts {
#if defined(MSTL_SUPPORT_CONCEPTS__)
	template <typename B, typename D>
	concept Derived = std::is_base_of<B, D>::value;

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
	concept NPointer = (not std::is_pointer<T>::value);
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

	template<typename T, typename U>
	concept Addable = requires(T a, U b) {
		{ a + b } -> std::convertible_to<decltype(a + b)>;
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
	template <typename T>
	concept SignedNumber = Integral<T> || Float<T>;

	template<typename T>
	concept DefaultConstructible = std::is_default_constructible_v<T>;
	template<typename T>
	concept CopyConstructible = std::is_copy_constructible_v<T>;
	template<typename T, typename U>
	concept ConstructibleFrom = std::is_constructible_v<T, U>;

	template<typename T, typename U>
	concept AssignableFrom = std::is_assignable_v<T&, U>;
	template<typename T>
	concept CopyAssignable = AssignableFrom<T, const T&>;
	template<typename T>
	concept MoveAssignable = AssignableFrom<T, T&&>;

	template<typename T>
	concept NothrowDefaultConstructible = std::is_nothrow_default_constructible_v<T>;
	template<typename T, typename U>
	concept NothrowConstructibleFrom = std::is_nothrow_constructible_v<T, U>;
	template<typename T, typename U>
	concept NothrowAssignableFrom = noexcept(std::declval<T&>() = std::declval<U>());

	template<typename T>
	concept Printable = requires(const T & t) {
		{ std::cout << t } -> std::convertible_to<std::ostream&>;
	};
	template <typename T>
	concept Swappable = std::_Is_swappable<T>::value;
	template <typename T>
	concept IteratorTypedef = requires() {
		typename std::iterator_traits<T>::iterator_category;
		typename std::iterator_traits<T>::value_type;
		typename std::iterator_traits<T>::difference_type;
		typename std::iterator_traits<T>::pointer;
		typename std::iterator_traits<T>::reference;
	};

	template <typename T>
	concept Detailable = NPointer<T> &&  requires(const T& c) {
		{ c.const_begin() } -> std::convertible_to<typename T::const_iterator>;
		{ c.const_end() } -> std::convertible_to<typename T::const_iterator>;
		{ c.size() } -> std::convertible_to<size_t>;
			requires Printable<typename std::iterator_traits<typename T::const_iterator>::value_type>;
	};

	template<typename Iterator>
	concept InputIterator = IteratorTypedef<Iterator> && requires(Iterator it1, Iterator it2) {
		{ *it1 } -> std::convertible_to<typename std::iterator_traits<Iterator>::value_type>;
		{ ++it1 } -> std::same_as<Iterator&>;
		{ it1++ } -> std::same_as<Iterator>;
		{ it1 != it2 } -> std::convertible_to<bool>;
		{ it1 == it2 } -> std::convertible_to<bool>;
		//{ it1 == it1 } -> std::convertible_to<bool>;
	};
	template<typename Iterator>
	concept ForwardIterator = InputIterator<Iterator> && requires(Iterator it1, Iterator it2) {
		{ it1 < it2 } -> std::convertible_to<bool>;
		{ it1 <= it2 } -> std::convertible_to<bool>;
		{ it1 > it2 } -> std::convertible_to<bool>;
		{ it1 >= it2 } -> std::convertible_to<bool>;
		{ std::distance(it1, it2) } -> std::convertible_to<typename std::iterator_traits<Iterator>::difference_type>;
	};
	template<typename Iterator>
	concept BidirectionalIterator = ForwardIterator<Iterator> && requires(Iterator it) {
		{ --it } -> std::same_as<Iterator&>;
		{ it-- } -> std::same_as<Iterator>;
	};
	template<typename Iterator>
	concept RandomAccessIterator = BidirectionalIterator<Iterator> &&
		requires(Iterator it1, Iterator it2, typename std::iterator_traits<Iterator>::difference_type n) 
	{
		{ it1 + n } -> std::same_as<Iterator>;
		{ n + it1 } -> std::same_as<Iterator>;
		{ it1 - n } -> std::same_as<Iterator>;
		{ it1 += n } -> std::same_as<Iterator>;
		{ it1 -= n } -> std::same_as<Iterator>;
		{ it2 - it1 } -> std::convertible_to<typename std::iterator_traits<Iterator>::difference_type>;
		{ it1[n] } -> std::convertible_to<typename std::iterator_traits<Iterator>::value_type>;
	};

	template <typename T>
	concept PairLike = requires(T p) {
		typename T::first_type;
		typename T::second_type;
		p.first;
		p.second;
	};

	template<typename T>
	concept BinaryFunction = requires(T f, typename T::first_argument_type a1, typename T::first_argument_type a2) {
		{ f(a1, a2) } -> std::convertible_to<bool>;
	};
	template <typename T>
	concept UnaryFunction = requires(T f, typename T::argument_type a) {
		{ f(a) } -> std::convertible_to<bool>;
	};
	template <typename Func, typename Arg>
	concept HashFunction = requires(Func f, Arg a) {
		{ f(a) } -> std::convertible_to<size_t>;
	};
	template <typename Func, typename Arg>
	concept SelectPairFirstFunction = PairLike<Arg> && requires(Func f, const Arg& a) {
		{ f(a) } -> std::convertible_to<decltype(a.first)>;
	};
	template <typename Func, typename Arg>
	concept SelectPairSecondFunction = PairLike<Arg> && requires(Func f, const Arg& a) {
		{ f(a) } -> std::convertible_to<decltype(a.second)>;
	};
	template <typename Func, typename Arg>
	concept SelectPairFunction = SelectPairFirstFunction<Func, Arg> || SelectPairSecondFunction<Func, Arg>;

#endif // MSTL_SUPPORT_CONCEPTS__
}

MSTL_END_NAMESPACE__

#endif // MSTL_CONCEPTS_HPP__
