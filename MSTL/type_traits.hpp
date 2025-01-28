#ifndef MSTL_TYPE_TRAITS_HPP__
#define MSTL_TYPE_TRAITS_HPP__
#include "basiclib.h"
MSTL_BEGIN_NAMESPACE__

template <class T, T Value>
struct integral_constant {
    static MSTL_CONSTEXPR T value = Value;

    using value_type = T;
    using type = integral_constant;

    MSTL_CONSTEXPR operator value_type() const noexcept {
        return value;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR value_type operator()() const noexcept {
        return value;
    }
};


template <bool Value>
using bool_constant = integral_constant<bool, Value>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;


// Test is false, SFINAF
template <bool Test, class T = void>
struct enable_if {};
// Test is true
template <class T>
struct enable_if<true, T> {
    using type = T;
};
template <bool Test, class T = void>
using enable_if_t = typename enable_if<Test, T>::type;


// Test is true, using type = T1
template <bool Test, class T1, class T2>
struct conditional {
    using type = T1;
};
// Test is false, using type = T2
template <class T1, class T2>
struct conditional<false, T1, T2> {
    using type = T2;
};
template <bool Test, class T1, class T2>
using conditional_t = typename conditional<Test, T1, T2>::type;


// not same type
template <class, class>
constexpr bool is_same_v = false;
// same type
template <class T>
constexpr bool is_same_v<T, T> = true;

template <class T1, class T2>
struct is_same : bool_constant<is_same_v<T1, T2>> {};


template <class T>
struct remove_const {
    using type = T;
};
template <class T>
struct remove_const<const T> {
    using type = T;
};
template <class T>
using remove_const_t = typename remove_const<T>::type;


template <class T>
struct remove_volatile {
    using type = T;
};
template <class T>
struct remove_volatile<volatile T> {
    using type = T;
};
template <class T>
using remove_volatile_t = typename remove_volatile<T>::type;


template <class T>
struct remove_cv {
    using type = T;

    template <template <class> class wrapper>
    using apply_t = wrapper<T>;
};
template <class T>
struct remove_cv<const T> {
    using type = T;

    template <template <class> class wrapper>
    using apply_t = const wrapper<T>;
};
template <class T>
struct remove_cv<volatile T> {
    using type = T;

    template <template <class> class wrapper>
    using apply_t = volatile wrapper<T>;
};
template <class T>
struct remove_cv<const volatile T> {
    using type = T;

    template <template <class> class wrapper>
    using apply_t = const volatile wrapper<T>;
};
template <class T>
using remove_cv_t = typename MSTL::remove_cv<T>::type;


// Test is true, stop recursion and set type
template <bool Test, class first, class...>
struct disjunction_aux {
    using type = first;
};
// Curr`s Test is false, continue checking next
template <class Curr, class Next, class... Rest>
struct disjunction_aux<false, Curr, Next, Rest...> {
    using type = typename disjunction_aux<static_cast<bool>(Next::value), Next, Rest...>::type;
};

// recursion end
template <class...>
struct disjunction : false_type {};
// start recursion
template <class First, class... Rest>
struct disjunction<First, Rest...> : disjunction_aux<static_cast<bool>(First::value), First, Rest...>::type {};

template <class... Args>
constexpr bool disjunction_v = disjunction<Args...>::value;


template <class T, class... Types>
constexpr bool is_any_of_v = (is_same_v<T, Types> || ...);


template <class T>
constexpr bool is_integral_v = is_any_of_v<remove_cv_t<T>,
    bool, char, signed char, unsigned char, wchar_t, 
#ifdef _HAS_CXX20
    char8_t,
#endif
    char16_t, char32_t, short, unsigned short, int, unsigned int, long,
    unsigned long, long long, unsigned long long
>;
template <class T>
struct is_integral : bool_constant<is_integral_v<T>> {};


template <class T>
constexpr bool is_floating_point_v = is_any_of_v<remove_cv_t<T>, float, double, long double>;
template <class T>
struct is_floating_point : bool_constant<is_floating_point_v<T>> {};


template <class T>
constexpr bool is_arithmetic_v = is_integral_v<T> || is_floating_point_v<T>;
template <class T>
struct is_arithmetic : bool_constant<is_arithmetic_v<T>> {};


template <class T>
struct remove_reference {
    using type = T;
    using bind_const_ref_t = const T;
};
template <class T>
struct remove_reference<T&> {
    using type = T;
    using bind_const_ref_t = const T&;
};
template <class T>
struct remove_reference<T&&> {
    using type = T;
    using bind_const_ref_t = const T&&;
};

template <class T>
using remove_reference_t = typename remove_reference<T>::type;
template <class T>
using const_ref_t = typename remove_reference<T>::bind_const_ref_t;


template <class T>
using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;
template <class T>
struct remove_cvref {
    using type = remove_cvref_t<T>;
};


// Test is true, stop recursion and set type
template <bool, class First, class...>
struct conjunction_aux {
    using type = First;
};
// Curr`s Test is true, continue checking next
template <class Curr, class Next, class... Rest>
struct conjunction_aux<true, Curr, Next, Rest...> {
    using type = typename conjunction_aux<static_cast<bool>(Next::value), Next, Rest...>::type;
};

// recursion finished
template <class...>
struct conjunction : true_type {};
// start recursion
template <class First, class... Rest>
struct conjunction<First, Rest...> : conjunction_aux<static_cast<bool>(First::value), First, Rest...>::type {};

template <class... Args>
constexpr bool conjunction_v = conjunction<Args...>::value;


template <class T>
struct negation : bool_constant<!static_cast<bool>(T::value)> {};
template <class T>
constexpr bool negation_v = negation<T>::value;


template <class T>
constexpr bool is_void_v = is_same_v<remove_cv_t<T>, void>;
template <class T>
struct is_void : bool_constant<is_void_v<T>> {};

template <class...>
using void_t = void;


template <class T>
struct identity_aux {
    using type = T;
};
template <class T>
using identity_t = typename identity_aux<T>::type;


template <class T>
struct add_const {
    using type = const T;
};
template <class T>
using add_const_t = typename add_const<T>::type;

_EXPORT_STD template <class _Ty>
struct add_volatile { // add top-level volatile qualifier
    using type = volatile _Ty;
};

_EXPORT_STD template <class _Ty>
using add_volatile_t = typename add_volatile<_Ty>::type;

_EXPORT_STD template <class _Ty>
struct add_cv { // add top-level const and volatile qualifiers
    using type = const volatile _Ty;
};

_EXPORT_STD template <class _Ty>
using add_cv_t = typename add_cv<_Ty>::type;

MSTL_END_NAMESPACE__
#endif // MSTL_TYPE_TRAITS_HPP__
