#ifndef MSTL_CONCEPTS_HPP__
#define MSTL_CONCEPTS_HPP__
#include "type_traits.hpp"
#include <concepts>
MSTL_BEGIN_NAMESPACE__
MSTL_SET_CONCEPTS__

template <typename B, typename D>
concept Derived = is_base_of_v<B, D>;

template <typename T>
concept Void = is_void_v<T>;
template <typename T>
concept Reference = is_reference_v<T>;
template <typename T>
concept Pointer = is_pointer_v<T>;
template <typename T>
concept Handler = Reference<T> || Pointer<T>;
template <typename T>
concept Const = is_const_v<T>;
template <typename T>
concept Volatile = is_volatile_v<T>;
template <typename T>
concept Function = is_function_v<T>;

template <typename T>
concept Integral = is_integral_v<T>;
template <typename T>
concept Float = is_floating_point_v<T>;
template <typename T>
concept Number = is_arithmetic_v<T>;
template <typename T>
concept SignedNumber = is_signed_v<T>;

template <typename From, typename To>
concept Convertible = is_convertible_v<From, To>;
template <typename T, typename U>
concept SameTo = is_same_v<T, U>;

template<typename T>
concept DefaultConstructible = is_default_constructible_v<T>;
template<typename T>
concept DefaultImplicitConstructible = is_implicitly_default_constructible_v<T>;
template<typename T>
concept CopyConstructible = is_copy_constructible_v<T>;
template<typename T, typename... Args>
concept ConstructibleFrom = is_constructible_v<T, Args...>;

template<typename To, typename From>
concept AssignableFrom = is_assignable_v<To, From>;
template<typename T>
concept CopyAssignable = is_copy_assignable_v<T>;
template <typename T>
concept TrivialCopyAssignable = is_trivially_copy_assignable_v<T>;
template<typename T>
concept MoveAssignable = is_move_assignable_v<T>;
template <typename T>
concept TrivialMoveAssignable = is_trivially_move_assignable_v<T>;
template <typename T>
concept TrivialAssignable = TrivialCopyAssignable<T> || TrivialMoveAssignable<T>;

template<typename T>
concept NothrowDefaultConstructible = is_nothrow_default_constructible_v<T>;
template<typename T>
concept NothrowCopyConstructible = is_nothrow_copy_constructible_v<T>;
template<typename T>
concept NothrowMoveConstructible = is_nothrow_move_constructible_v<T>;
template<typename T, typename... Args>
concept NothrowConstructibleFrom = is_nothrow_constructible_v<T, Args...>;

template<typename To, typename From>
concept NothrowAssignableFrom = is_nothrow_assignable_v<To, From>;
template<typename T>
concept NothrowCopyAssignable = is_nothrow_copy_assignable_v<T>;
template<typename T>
concept NothrowMoveAssignable = is_nothrow_move_assignable_v<T>;

template <typename T>
concept Swappable = is_swappable_v<T>;
template <typename T>
concept NothrowSwappable = is_nothrow_swappable_v<T>;

template <typename T>
concept TrivialDestructible = is_trivially_destructible_v<T>;
template <typename T>
concept NothrowDestructable = is_nothrow_destructible_v<T>;

template<typename T>
concept Printable = requires(const T & t) {
	{ std::cout << t } -> convertible_to<std::ostream&>;
};
template <typename T>
concept IteratorTypedef = requires() {
	typename std::iterator_traits<T>::iterator_category;
	typename std::iterator_traits<T>::value_type;
	typename std::iterator_traits<T>::difference_type;
	typename std::iterator_traits<T>::pointer;
	typename std::iterator_traits<T>::reference;
};

template <typename T>
concept Detailable = !Pointer<T> && requires(const T & c) {
	{ c.cbegin() } -> convertible_to<typename T::const_iterator>;
	{ c.cend() } -> convertible_to<typename T::const_iterator>;
	{ c.size() } -> convertible_to<size_t>;
		requires Printable<typename std::iterator_traits<typename T::const_iterator>::value_type>;
};

template<typename Iterator>
concept InputIterator = IteratorTypedef<Iterator> && requires(Iterator it1, Iterator it2) {
	{ *it1 } -> convertible_to<typename std::iterator_traits<Iterator>::value_type>;
	{ ++it1 } -> std::same_as<Iterator&>;
	{ it1++ } -> std::same_as<Iterator>;
	{ it1 != it2 } -> convertible_to<bool>;
	{ it1 == it2 } -> convertible_to<bool>;
};
template<typename Iterator>
concept ForwardIterator = InputIterator<Iterator> && requires(Iterator it1, Iterator it2) {
	{ it1 < it2 } -> convertible_to<bool>;
	{ it1 <= it2 } -> convertible_to<bool>;
	{ it1 > it2 } -> convertible_to<bool>;
	{ it1 >= it2 } -> convertible_to<bool>;
	{ (it1 - it2) } -> convertible_to<typename std::iterator_traits<Iterator>::difference_type>;
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
	{ it1 + n } -> convertible_to<Iterator>;
	{ n + it1 } -> convertible_to<Iterator>;
	{ it1 - n } -> convertible_to<Iterator>;
	{ it1 += n } -> convertible_to<Iterator>;
	{ it1 -= n } -> convertible_to<Iterator>;
	{ it2 - it1 } -> convertible_to<typename std::iterator_traits<Iterator>::difference_type>;
	{ it1[n] } -> convertible_to<typename std::iterator_traits<Iterator>::value_type>;
};

template <typename T>
concept PairLike = requires(T p) {
	typename T::first_type;
	typename T::second_type;
	p.first;
	p.second;
};

template<typename T>
concept BinaryFunction = requires(T f, typename T::first_argument_type a1, typename T::second_argument_type a2) {
	f(a1, a2);
};
template <typename T>
concept UnaryFunction = requires(T f, typename T::argument_type a) {
	f(a);
};
template <typename Func, typename Arg>
concept HashFunction = requires(Func f, Arg a) {
	{ f(a) } -> convertible_to<size_t>;
};
template <typename Func, typename Arg>
concept SelectPairFirstFunction = PairLike<Arg> && requires(Func f, const Arg & a) {
	{ f(a) } -> convertible_to<decltype(a.first)>;
};
template <typename Func, typename Arg>
concept SelectPairSecondFunction = PairLike<Arg> && requires(Func f, const Arg & a) {
	{ f(a) } -> convertible_to<decltype(a.second)>;
};
template <typename Func, typename Arg>
concept SelectPairFunction = SelectPairFirstFunction<Func, Arg> || SelectPairSecondFunction<Func, Arg>;

MSTL_END_CONCEPTS__
MSTL_END_NAMESPACE__

#endif // MSTL_CONCEPTS_HPP__
