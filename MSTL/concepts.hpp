#ifndef MSTL_CONCEPTS_HPP__
#define MSTL_CONCEPTS_HPP__
#include "type_traits.hpp"
MSTL_BEGIN_NAMESPACE__

template <class T1, class T2>
concept same_as = is_same_v<T1, T2> && is_same_v<T2, T1>;

template <class T1, class T2>
concept common_reference_with = requires {
	typename common_reference_t<T1, T2>;
	typename common_reference_t<T2, T1>;
}
&& same_as<common_reference_t<T1, T2>, common_reference_t<T2, T1>>
&& convertible_to<T1, common_reference_t<T1, T2>>
&& convertible_to<T2, common_reference_t<T1, T2>>;

template <class T1, class T2>
concept common_with = requires { typename common_type_t<T1, T2>; typename common_type_t<T2, T1>; }
&& same_as<common_type_t<T1, T2>, common_type_t<T2, T1>> && requires {
	static_cast<common_type_t<T1, T2>>(MSTL::declval<T1>());
	static_cast<common_type_t<T1, T2>>(MSTL::declval<T2>());
} && common_reference_with<add_lvalue_reference_t<const T1>, add_lvalue_reference_t<const T2>>
&& common_reference_with<add_lvalue_reference_t<common_type_t<T1, T2>>,
	common_reference_t<add_lvalue_reference_t<const T1>, add_lvalue_reference_t<const T2>>>;

template <class Derived, class Base>
concept derived_from = is_base_of_v<Base, Derived>
&& is_convertible_to_v<const volatile Derived*, const volatile Base*>;

template <class _Ty>
concept integral = is_integral_v<_Ty>;
template <class _Ty>
concept signed_integral = integral<_Ty> && static_cast<_Ty>(-1) < static_cast<_Ty>(0);
template <typename T>
concept signed_number = is_signed_v<T>;
template <class _Ty>
concept unsigned_integral = integral<_Ty> && !signed_integral<_Ty>;
template <class _Ty>
concept floating_point = is_floating_point_v<_Ty>;


template <typename T>
concept trivially_destructible = is_trivially_destructible_v<T>;
template <typename T>
concept nothrow_destructible = is_nothrow_destructible_v<T>;
template <typename T>
concept destructible = is_destructible_v<T>;


template<typename T, typename... Args>
concept constructible_from = is_constructible_v<T, Args...>;
template<typename T>
concept default_constructible = is_default_constructible_v<T>;
template<typename T>
concept implicit_constructible = is_implicitly_default_constructible_v<T>;
template <class T>
concept move_constructible = is_move_constructible_v<T>;
template<typename T>
concept copy_constructible = move_constructible<T>
&& constructible_from<T, T&>&& convertible_to<T&, T>
&& constructible_from<T, const T&>&& convertible_to<const T&, T>
&& constructible_from<T, const T>&& convertible_to<const T, T>;
template<typename T>
concept nothrow_default_constructible = is_nothrow_default_constructible_v<T>;
template<typename T>
concept nothrow_copy_constructible = is_nothrow_copy_constructible_v<T>;
template<typename T>
concept nothrow_move_constructible = is_nothrow_move_constructible_v<T>;
template<typename T, typename... Args>
concept nothrow_constructible_from = is_nothrow_constructible_v<T, Args...>;


template <class T>
concept default_initializable = constructible_from<T> && requires {
	T{};
	::new (static_cast<void*>(nullptr)) T;
};


template <typename T>
concept swappable = is_swappable_v<T>;
template <typename T>
concept nothrow_swappable = is_nothrow_swappable_v<T>;


template <class To, class From>
concept assignable_from = is_lvalue_reference_v<To>
&& common_reference_with<const remove_reference_t<To>&, const remove_reference_t<From>&>
&& requires(To x, From&& y) {
	{ x = static_cast<From&&>(y) } -> same_as<To>;
};
template<typename T>
concept copy_assignable = is_copy_assignable_v<T>;
template <typename T>
concept trivially_copy_assignable = is_trivially_copy_assignable_v<T>;
template<typename T>
concept move_assignable = is_move_assignable_v<T>;
template <typename T>
concept trivial_move_assignable = is_trivially_move_assignable_v<T>;
template <typename T>
concept TrivialAssignable = trivially_copy_assignable<T> || trivial_move_assignable<T>;


template <class T>
concept movable = is_object_v<T>
&& move_constructible<T>
&& assignable_from<T&, T>
&& swappable<T>;

template <class T>
concept copyable = copy_constructible<T>
&& movable<T>
&& assignable_from<T&, T&>
&& assignable_from<T&, const T&>
&& assignable_from<T&, const T>;



template<typename To, typename From>
concept NothrowAssignableFrom = is_nothrow_assignable_v<To, From>;
template<typename T>
concept NothrowCopyAssignable = is_nothrow_copy_assignable_v<T>;
template<typename T>
concept NothrowMoveAssignable = is_nothrow_move_assignable_v<T>;


template <class T1, class T2>
concept one_way_equality_comparable =
	requires(const remove_reference_t<T1>& x, const remove_reference_t<T2>& y) {
		{ x == y } -> convertible_to<bool>;
		{ x != y } -> convertible_to<bool>;
};


template <class T1, class T2>
concept both_equality_comparable =
one_way_equality_comparable<T1, T2>&& one_way_equality_comparable<T2, T1>;

template <class T>
concept equality_comparable = one_way_equality_comparable<T, T>;

template <class T1, class T2>
concept equality_comparable_with = equality_comparable<T1> && equality_comparable<T2>
&& common_reference_with<const remove_reference_t<T1>&, const remove_reference_t<T2>&>
&& equality_comparable<common_reference_t<const remove_reference_t<T1>&, const remove_reference_t<T2>&>>
&& both_equality_comparable<T1, T2>;


template <class T1, class T2>
concept one_way_ordered = requires(const remove_reference_t<T1>& x, const remove_reference_t<T2>& y) {
	{ x < y } -> convertible_to<bool>;
	{ x > y } -> convertible_to<bool>;
	{ x <= y } -> convertible_to<bool>;
	{ x >= y } -> convertible_to<bool>;
};

template <class T1, class T2>
concept both_ordered_with = one_way_ordered<T1, T2>&& one_way_ordered<T2, T1>;

template <class T>
concept totally_ordered = equality_comparable<T> && one_way_ordered<T, T>;

template <class T1, class T2>
concept totally_ordered_with = totally_ordered<T1> && totally_ordered<T2>
&& equality_comparable_with<T1, T2>
&& totally_ordered<common_reference_t<const remove_reference_t<T1>&, const remove_reference_t<T2>&>>
&& both_ordered_with<T1, T2>;


template <class T>
concept semiregular = copyable<T> && default_initializable<T>;
template <class T>
concept regular = semiregular<T> && equality_comparable<T>;


template<typename T>
concept is_printable = requires(T t) {
	{ std::cout << t } -> convertible_to<std::ostream&>;
};

template <typename T>
concept is_detailable = requires(const T & c) {
	{ c.cbegin() } -> convertible_to<typename T::const_iterator>;
	{ c.cend() } -> convertible_to<typename T::const_iterator>;
	{ c.size() } -> convertible_to<size_t>;
		requires is_printable<typename std::iterator_traits<typename T::const_iterator>::value_type>;
};


template <typename T>
concept iterator_typedef = requires() {
	typename std::iterator_traits<T>::iterator_category;
	typename std::iterator_traits<T>::value_type;
	typename std::iterator_traits<T>::difference_type;
	typename std::iterator_traits<T>::pointer;
	typename std::iterator_traits<T>::reference;
};

template<typename Iterator>
concept input_iterator = iterator_typedef<Iterator> && requires(Iterator it1, Iterator it2) {
	{ *it1 } -> convertible_to<typename std::iterator_traits<Iterator>::value_type>;
	{ ++it1 } -> same_as<Iterator&>;
	{ it1++ } -> same_as<Iterator>;
	{ it1 != it2 } -> convertible_to<bool>;
	{ it1 == it2 } -> convertible_to<bool>;
};
template<typename Iterator>
concept forward_iterator = input_iterator<Iterator> && requires(Iterator it1, Iterator it2) {
	{ it1 < it2 } -> convertible_to<bool>;
	{ it1 <= it2 } -> convertible_to<bool>;
	{ it1 > it2 } -> convertible_to<bool>;
	{ it1 >= it2 } -> convertible_to<bool>;
	{ (it1 - it2) } -> convertible_to<typename std::iterator_traits<Iterator>::difference_type>;
};
template<typename Iterator>
concept bidirectional_iterator = forward_iterator<Iterator> && requires(Iterator it) {
	{ --it } -> same_as<Iterator&>;
	{ it-- } -> same_as<Iterator>;
};
template<typename Iterator>
concept random_access_iterator = bidirectional_iterator<Iterator> &&
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
concept is_pair_v = requires(T p) {
	typename T::first_type;
	typename T::second_type;
	p.first;
	p.second;
};

template <typename Func, typename Arg>
concept is_hash_v = requires(Func f, Arg a) {
	{ f(a) } -> convertible_to<size_t>;
};

MSTL_END_NAMESPACE__
#endif // MSTL_CONCEPTS_HPP__
