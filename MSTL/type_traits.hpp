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


template <class T>
struct negation : bool_constant<!static_cast<bool>(T::value)> {};
template <class T>
constexpr bool negation_v = negation<T>::value;


// not same type
template <class, class>
constexpr bool is_same_v = false;
// same type
template <class T>
constexpr bool is_same_v<T, T> = true;

template <class T1, class T2>
struct is_same : bool_constant<is_same_v<T1, T2>> {};


template <class T, class... Types>
constexpr bool is_any_of_v = (is_same_v<T, Types> || ...);
template <class T, class... Types>
struct is_any_of : bool_constant<is_any_of_v<T, Types...>> {};


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


// Test is true, stop recursion and set type
template <bool, class first, class...>
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
struct disjunction<First, Rest...>
    : disjunction_aux<static_cast<bool>(First::value), First, Rest...>::type {
};

template <class... Args>
constexpr bool disjunction_v = disjunction<Args...>::value;


// Test is false, stop recursion and set type
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
struct conjunction<First, Rest...>
    : conjunction_aux<static_cast<bool>(First::value), First, Rest...>::type {
};

template <class... Args>
constexpr bool conjunction_v = conjunction<Args...>::value;


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
using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;
template <class T>
struct remove_cvref {
    using type = remove_cvref_t<T>;
};


template <class T>
struct remove_extent {
    using type = T;
};
template <class T, size_t Idx>
struct remove_extent<T[Idx]> {
    using type = T;
};
template <class T>
struct remove_extent<T[]> {
    using type = T;
};

template <class T>
using remove_extent_t = typename remove_extent<T>::type;


template <class T>
struct remove_all_extents {
    using type = T;
};
template <class T, size_t Idx>
struct remove_all_extents<T[Idx]> {
    using type = typename remove_all_extents<T>::type;
};
template <class T>
struct remove_all_extents<T[]> {
    using type = typename remove_all_extents<T>::type;
};

template <class T>
using remove_all_extents_t = typename remove_all_extents<T>::type;


template <class T>
struct remove_pointer {
    using type = T;
};
template <class T>
struct remove_pointer<T*> {
    using type = T;
};
template <class T>
struct remove_pointer<T* const> {
    using type = T;
};
template <class T>
struct remove_pointer<T* volatile> {
    using type = T;
};
template <class T>
struct remove_pointer<T* const volatile> {
    using type = T;
};

template <class T>
using remove_pointer_t = typename remove_pointer<T>::type;



template <class T>
constexpr bool is_void_v = is_same_v<remove_cv_t<T>, void>;
template <class T>
struct is_void : bool_constant<is_void_v<T>> {};

template <class...>
using void_t = void;


template <class T>
constexpr bool is_integral_v = is_any_of_v<remove_cv_t<T>,
    bool, char, signed char, unsigned char, wchar_t, 
#ifdef MSTL_VERSION_20__
    char8_t,
#endif
    char16_t, char32_t, short, unsigned short, int, unsigned int, long,
    unsigned long, long long, unsigned long long
>;
template <class T>
struct is_integral : bool_constant<is_integral_v<T>> {};


template <class T>
constexpr bool is_nonbool_integral = is_integral_v<T> && !is_same_v<remove_cv_t<T>, bool>;


template <class T>
constexpr bool is_floating_point_v = is_any_of_v<remove_cv_t<T>, float, double, long double>;
template <class T>
struct is_floating_point : bool_constant<is_floating_point_v<T>> {};


template <class T>
constexpr bool is_arithmetic_v = is_integral_v<T> || is_floating_point_v<T>;
template <class T>
struct is_arithmetic : bool_constant<is_arithmetic_v<T>> {};


template <typename T, bool = is_integral_v<T>>
struct sign_aux {
    static constexpr bool is_signed = static_cast<remove_cv_t<T>>(-1) < static_cast<remove_cv_t<T>>(0);
    static constexpr bool is_unsigned = !is_signed;
};
template <typename T>
struct sign_aux<T, false> {
    static constexpr bool is_signed = is_floating_point_v<T>;
    static constexpr bool is_unsigned = false;
};

template <typename T>
struct is_signed : bool_constant<sign_aux<T>::is_signed> {};
template <typename T>
constexpr bool is_signed_v = is_signed<T>::value;

template <class T>
struct is_unsigned : bool_constant<sign_aux<T>::is_unsigned> {};
template <class T>
constexpr bool is_unsigned_v = is_unsigned<T>::value;


template <class T>
struct type_identity {
    using type = T;
};
template <class T>
using identity_t = typename type_identity<T>::type;


template <class T>
struct add_const {
    using type = const T;
};
template <class T>
using add_const_t = typename add_const<T>::type;


template <class T>
struct add_volatile {
    using type = volatile T;
};
template <class T>
using add_volatile_t = typename add_volatile<T>::type;


template <class T>
struct add_cv {
    using type = const volatile T;
};
template <class T>
using add_cv_t = typename add_cv<T>::type;


template <class T, class = void>
struct add_reference {
    using lvalue = T;
    using rvalue = T;
};
template <class T>
struct add_reference<T, void_t<T&>> {
    using lvalue = T&;
    using rvalue = T&&;
};

template <class T>
struct add_lvalue_reference {
    using type = typename add_reference<T>::lvalue;
};
template <class T>
using add_lvalue_reference_t = typename add_reference<T>::lvalue;

template <class T>
struct add_rvalue_reference {
    using type = typename add_reference<T>::rvalue;
};
template <class T>
using add_rvalue_reference_t = typename add_reference<T>::rvalue;


// declval only work in no evaluation context (decltype or sizeof) to quickly get rvalue type. 
template <class T>
add_rvalue_reference_t<T> declval() noexcept { MSTL_NO_EVALUATION__ }
// try to copy type T
template <class T>
T declcopy(T) noexcept { MSTL_NO_EVALUATION__ }
// work with is_void_t
template <class T>
void declvoid(T) noexcept { MSTL_NO_EVALUATION__ }


template <class T, class = void>
struct add_pointer {
    using type = T;
};
template <class T>
struct add_pointer<T, void> {
    using type = remove_reference_t<T>*;
};

template <class T>
using add_pointer_t = typename add_pointer<T>::type;


template <class>
constexpr bool is_array_v = false;
template <class T, size_t Idx>
constexpr bool is_array_v<T[Idx]> = true;
template <class T>
constexpr bool is_array_v<T[]> = true;

template <class T>
struct is_array : bool_constant<is_array_v<T>> {};


template <class>
constexpr bool is_bounded_array_v = false;
template <class T, size_t Idx>
constexpr bool is_bounded_array_v<T[Idx]> = true;

template <class T>
struct is_bounded_array : bool_constant<is_bounded_array_v<T>> {};


template <class>
constexpr bool is_unbounded_array_v = false;
template <class T>
constexpr bool is_unbounded_array_v<T[]> = true;

template <class T>
struct is_unbounded_array : bool_constant<is_unbounded_array_v<T>> {};


template <class>
constexpr bool is_lvalue_reference_v = false;
template <class T>
constexpr bool is_lvalue_reference_v<T&> = true;

template <class T>
struct is_lvalue_reference : bool_constant<is_lvalue_reference_v<T>> {};


template <class>
constexpr bool is_rvalue_reference_v = false;
template <class T>
constexpr bool is_rvalue_reference_v<T&&> = true;

template <class T>
struct is_rvalue_reference : bool_constant<is_rvalue_reference_v<T>> {};


template <class>
constexpr bool is_reference_v = false;
template <class T>
constexpr bool is_reference_v<T&> = true;
template <class T>
constexpr bool is_reference_v<T&&> = true;

template <class T>
struct is_reference : bool_constant<is_reference_v<T>> {};


template <class>
constexpr bool is_pointer_v = false;
template <class T>
constexpr bool is_pointer_v<T*> = true;
template <class T>
constexpr bool is_pointer_v<T* const> = true;
template <class T>
constexpr bool is_pointer_v<T* volatile> = true;
template <class T>
constexpr bool is_pointer_v<T* const volatile> = true;

template <class T>
struct is_pointer : bool_constant<is_pointer_v<T>> {};


template <class T>
constexpr bool is_null_pointer_v = is_same_v<remove_cv_t<T>, std::nullptr_t>;
template <class T>
struct is_null_pointer : bool_constant<is_null_pointer_v<T>> {};


template<typename T>
struct is_enum : bool_constant<__is_enum(T)> {};
template <class T>
constexpr bool is_enum_v = is_enum<T>::value;

template <class T>
struct is_union : bool_constant<__is_union(T)> {};
template <class T>
constexpr bool is_union_v = is_union<T>::value;

template <class T>
struct is_class : bool_constant<__is_class(T)> {};
template <class T>
constexpr bool is_class_v = is_class<T>::value;


// literal type
template <class T>
constexpr bool is_fundamental_v = is_arithmetic_v<T> || is_void_v<T> || is_null_pointer_v<T>;
template <class T>
struct is_fundamental : bool_constant<is_fundamental_v<T>> {};

template <class T>
struct is_compound : bool_constant<!is_fundamental_v<T>> {};
template <class T>
constexpr bool is_compound_v = is_compound<T>::value;


template <class From, class To>
struct is_convertible : bool_constant<__is_convertible_to(From, To)> {};
template <class From, class To>
constexpr bool is_convertible_v = is_convertible<From, To>::value;


template <class>
constexpr bool is_const_v = false;
template <class T>
constexpr bool is_const_v<const T> = true;
template <class T>
struct is_const : bool_constant<is_const_v<T>> {};


template <class>
constexpr bool is_volatile_v = false;
template <class T>
constexpr bool is_volatile_v<volatile T> = true;
template <class T>
struct is_volatile : bool_constant<is_volatile_v<T>> {};


// function cannot be const qualified.
template <class T>
constexpr bool is_function_v = !is_const_v<const T> && !is_reference_v<T>;
template <class T>
struct is_function : bool_constant<is_function_v<T>> {};


// object type is not the function, reference or void type. 
// function and reference cannot be const–qualified.
template <class T>
constexpr bool is_object_v = is_const_v<const T> && !is_void_v<T>;
template <class T>
struct is_object : bool_constant<is_object_v<T>> {};


#if defined(MSTL_COMPILE_CLANG__)
template <class T>
struct is_member_function_pointer : bool_constant<__is_member_function_pointer(T)> {};
#else
template <typename>
struct is_member_function_pointer_aux : false_type {};
template <typename T, typename C>
struct is_member_function_pointer_aux<T C::*> : is_function<T> {};
template<typename T>
struct is_member_function_pointer : is_member_function_pointer_aux<remove_cv_t<T>> {};
#endif
template <class T>
constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;


#if defined(MSTL_COMPILE_CLANG__)
template <class T>
struct is_member_object_pointer : bool_constant<__is_member_object_pointer(T)> {};
#else
template <class>
struct is_member_object_pointer : false_type {};
template <class T, class C>
struct is_member_object_pointer<T C::*> : bool_constant<!is_function_v<T>> {};
#endif
template <class T>
constexpr bool is_member_object_pointer_v = is_member_object_pointer<remove_cv_t<T>>::value;


#if defined(MSTL_COMPILE_CLANG__)
template <class T>
constexpr bool is_member_pointer_v = __is_member_pointer(T);
#else
template <class T>
constexpr bool is_member_pointer_v = is_member_object_pointer_v<T> || is_member_function_pointer_v<T>;
#endif
template <class T>
struct is_member_pointer : bool_constant<is_member_pointer_v<T>> {};


// arithmetics, enums and pointers are scalar types.
template <class T>
constexpr bool is_scalar_v =
is_arithmetic_v<T> || is_enum_v<T> || is_pointer_v<T> || is_member_pointer_v<T> || is_null_pointer_v<T>;
template <class T>
struct is_scalar : bool_constant<is_scalar_v<T>> {};


// empty types don't have non-static members, virtual functions and virtual base class.
// if have a base class, that should also be an empty type.
template <class T>
struct is_empty : bool_constant<__is_empty(T)> {};
template <class T>
constexpr bool is_empty_v = is_empty<T>::value;


template <class T>
struct is_polymorphic : bool_constant<__is_polymorphic(T)> {};
template <class T>
constexpr bool is_polymorphic_v = is_polymorphic<T>::value;


template <class T>
struct is_abstract : bool_constant<__is_abstract(T)> {};
template <class T>
constexpr bool is_abstract_v = is_abstract<T>::value;


template <class T>
struct is_final : bool_constant<__is_final(T)> {};
template <class T>
constexpr bool is_final_v = is_final<T>::value;


// standard layout types have these characteristics:
// consistent access control for non-static members (all public).
// no virtual functions and virtual base classes.
// no base class or there is only one base class and the base class is also a standard layout type.
// non-static data members cannot appear in both the base class and the derived class.
template <class T>
struct is_standard_layout : bool_constant<__is_standard_layout(T)> {};
template <class T>
constexpr bool is_standard_layout_v = is_standard_layout<T>::value;


// trivial types have these characteristics:
// the constructor, copy constructor, move constructor, copy assignment operator, 
// move assignment operator, and destructor are automatically generated by the compiler (is default).
// no virtual functions and virtual base classes.
template <class T>
struct is_trivial : bool_constant<__is_trivial(T)> {};
template <class T>
constexpr bool is_trivial_v = is_trivial<T>::value;


template <class T>
struct is_trivially_copyable : bool_constant<__is_trivially_copyable(T)> {};
template <class T>
constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;


template <class T>
struct has_virtual_destructor : bool_constant<__has_virtual_destructor(T)> {};
template <class T>
constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;


// each different value of a type has a unique binary representation, 
// but padding bytes and implementation dependencies should also be taken into account.
// if a type has unique object representations,
// we can use memory comparison to easily determine the equality.
// standard layout types and trivial types match unique object representations.
template <class T>
struct has_unique_object_representations : bool_constant<__has_unique_object_representations(T)> {};
template <class T>
constexpr bool has_unique_object_representations_v = has_unique_object_representations<T>::value;


#if defined(MSTL_COMPILE_MSVC__) 
template <class T>
struct is_aggregate_aux : bool_constant<__is_aggregate(T)> {};
template <class T>
constexpr bool is_aggregate_v = disjunction_v<is_array<T>, is_aggregate_aux<T>>;
#else
template <class T>
constexpr bool is_aggregate_v = __is_aggregate(remove_cv_t<T>);
#endif

// aggregate types have these characteristics:
// is array types;
// is class types (include struct and union) which have the following characteristics:
//   no user-defined constructors;
//   all non-static data members are public; 
//   no virtual functions and virtual base classes.
template <class T>
struct is_aggregate : bool_constant<is_aggregate_v<T>> {};


template <class T, class... Args>
struct is_constructible : bool_constant<__is_constructible(T, Args...)> {};
template <class T, class... Args>
constexpr bool is_constructible_v = is_constructible<T, Args...>::value;


template <class T>
struct is_copy_constructible 
    : bool_constant<is_constructible_v<T, add_lvalue_reference_t<const T>>> {};
template <class T>
constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;


template <class T>
struct is_default_constructible : bool_constant<is_constructible_v<T>> {};
template <class T>
constexpr bool is_default_constructible_v = is_default_constructible<T>::value;


template <class T>
void implicitly_default_construct_aux(const T&) { MSTL_NO_EVALUATION__ }

template <class, class = void>
struct is_implicitly_default_constructible : false_type {};
template <class T>
struct is_implicitly_default_constructible 
    <T, void_t<decltype(implicitly_default_construct_aux<T>({}))>> : true_type {};

template <typename T>
constexpr bool is_implicitly_default_constructible_v = is_implicitly_default_constructible<T>::value;


template <class T>
struct is_move_constructible : bool_constant<is_constructible_v<T, T>> {};
template <class T>
constexpr bool is_move_constructible_v = is_move_constructible<T>::value;


template <class To, class From>
struct is_assignable : bool_constant<__is_assignable(To, From)> {};
template <class To, class From>
constexpr bool is_assignable_v = is_assignable<To, From>::value;


template <class T>
struct is_copy_assignable
    : bool_constant<is_assignable_v<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>>> {};
template <class T>
constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;


template <class T>
struct is_move_assignable : bool_constant<is_assignable_v<add_lvalue_reference_t<T>, T>> {};
template <class T>
constexpr bool is_move_assignable_v = is_move_assignable<T>::value;


template <class T>
struct is_destructible : bool_constant<__is_destructible(T)> {};
template <class T>
constexpr bool is_destructible_v = is_destructible<T>::value;


template <class T, class... Args>
struct is_trivially_constructible : bool_constant<__is_trivially_constructible(T, Args...)> {};
template <class T, class... Args>
constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;


template <class T>
struct is_trivially_copy_constructible
    : bool_constant<is_trivially_constructible_v<T, add_lvalue_reference_t<const T>>> {};
template <class T>
constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<T>::value;


template <class T>
struct is_trivially_default_constructible : bool_constant<is_trivially_constructible_v<T>> {};
template <class T>
constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;


template <class T>
struct is_trivially_move_constructible : bool_constant<is_trivially_constructible_v<T, T>> {};
template <class T>
constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;


template <class To, class From>
struct is_trivially_assignable : bool_constant<__is_trivially_assignable(To, From)> {};
template <class To, class From>
constexpr bool is_trivially_assignable_v = is_trivially_assignable<To, From>::value;


template <class T>
struct is_trivially_copy_assignable : bool_constant<is_trivially_assignable_v<
    add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>>> {};
template <class T>
constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<T>::value;


template <class T>
struct is_trivially_move_assignable 
    : bool_constant<is_trivially_assignable_v<add_lvalue_reference_t<T>, T>> {};
template <class T>
constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<T>::value;


template <class T>
struct is_trivially_destructible : bool_constant<__is_trivially_destructible(T)> {};
template <class T>
constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;


template <class T, class... Args>
struct is_nothrow_constructible : bool_constant<__is_nothrow_constructible(T, Args...)> {};
template <class T, class... Args>
constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;


template <class T>
struct is_nothrow_copy_constructible
    : bool_constant<is_nothrow_constructible_v<T, add_lvalue_reference_t<const T>>> {};
template <class T>
constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;


template <class T>
struct is_nothrow_default_constructible : bool_constant<is_nothrow_constructible_v<T>> {};
template <class T>
constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;


template <class T>
struct is_nothrow_move_constructible : bool_constant<is_nothrow_constructible_v<T, T>> {};
template <class T>
constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;


template <class To, class From>
struct is_nothrow_assignable : bool_constant<__is_nothrow_assignable(To, From)> {};
template <class To, class From>
constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<To, From>::value;


template <class T>
struct is_nothrow_copy_assignable
    : bool_constant<is_nothrow_assignable_v<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>>> {};
template <class T>
constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;


template <typename T>
struct is_nothrow_move_assignable : bool_constant<is_nothrow_assignable_v<add_lvalue_reference_t<T>, T>> {};
template <typename T>
constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;


template <typename T>
struct is_nothrow_destructible : bool_constant<__is_nothrow_destructible(T)> {};
template <typename T>
constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;


template <class T>
MSTL_NODISCARD constexpr T&& forward(remove_reference_t<T>& x) noexcept {
    return static_cast<T&&>(x);
}
template <class T>
    requires(!is_lvalue_reference_v<T>)
MSTL_NODISCARD constexpr T&& forward(remove_reference_t<T>&& x) noexcept {
    return static_cast<T&&>(x);
}
template <class T>
MSTL_NODISCARD constexpr remove_reference_t<T>&& move(T&& x) noexcept {
    return static_cast<remove_reference_t<T>&&>(x);
}
template <class T>
MSTL_NODISCARD constexpr
conditional_t<!is_nothrow_move_constructible_v<T>&& is_copy_constructible_v<T>, const T&, T&&>
move_if_noexcept(T& x) noexcept {
    return MSTL::move(x);
}
template <class T>
MSTL_NODISCARD constexpr T* addressof(T& x) noexcept {
    return __builtin_addressof(x);
}
template <class T>
const T* addressof(const T&&) = delete;


template <size_t>
struct sign_byte_aux;

TEMNULL__
struct sign_byte_aux<1> {
    template <class>
    using signed_t = signed char;
    template <class>
    using unsigned_t = unsigned char;
};
TEMNULL__
struct sign_byte_aux<2> {
    template <class>
    using signed_t = short;
    template <class>
    using unsigned_t = unsigned short;
};
TEMNULL__
struct sign_byte_aux<4> {
    template <class T>
    using signed_t = 
        conditional_t<is_same_v<T, long> || is_same_v<T, unsigned long>, long, int>;

    template <class T>
    using unsigned_t = 
        conditional_t<is_same_v<T, long> || is_same_v<T, unsigned long>, unsigned long, unsigned int>;
};
TEMNULL__
struct sign_byte_aux<8> {
    template <class>
    using signed_t = long long;
    template <class>
    using unsigned_t = unsigned long long;
};

template <class T>
using set_signed_byte = typename sign_byte_aux<sizeof(T)>::template signed_t<T>;
template <class T>
using set_unsigned_byte = typename sign_byte_aux<sizeof(T)>::template unsigned_t<T>;

template <class T>
struct make_signed {
    static_assert(is_nonbool_integral<T> || is_enum_v<T>,
        "make signed only support non bool integral types and enum types");

    using signed_type = typename remove_cv<T>::template apply_t<set_signed_byte>;
    using unsigned_type = typename remove_cv<T>::template apply_t<set_unsigned_byte>;
};
template <class T>
using make_signed_t = typename make_signed<T>::signed_type;
template <class T>
using make_unsigned_t = typename make_signed<T>::unsigned_type;

template <class T>
constexpr make_signed_t<T> signed_value(T x) {
    return static_cast<make_signed_t<T>>(x);
}
template <class T>
constexpr make_unsigned_t<T> unsigned_value(T x) {
    return static_cast<make_unsigned_t<T>>(x);
}


template <class T>
struct alignment_of : integral_constant<size_t, alignof(T)> {};
template <class T>
constexpr size_t alignment_of_v = alignment_of<T>::value;


// get the base type of enum
template <class T, bool = is_enum_v<T>>
struct underlying_type {
    using type = __underlying_type(T);
};
template <class T>
struct underlying_type<T, false> {};
template <class T>
using underlying_type_t = typename underlying_type<T>::type;


template <class>
constexpr size_t rank_v = 0;
template <class T, size_t Idx>
constexpr size_t rank_v<T[Idx]> = rank_v<T> +1;
template <class T>
constexpr size_t rank_v<T[]> = rank_v<T> +1;
template <class T>
struct rank : integral_constant<size_t, rank_v<T>> {};


template <class, unsigned int = 0>
constexpr size_t extent_v = 0;
template <class T, size_t N>
constexpr size_t extent_v<T[N], 0> = N;
template <class T, unsigned int Idx, size_t N>
constexpr size_t extent_v<T[N], Idx> = extent_v<T, Idx - 1>;
template <class T, unsigned int Idx>
constexpr size_t extent_v<T[], Idx> = extent_v<T, Idx - 1>;
template <class T, unsigned int Idx = 0>
struct extent : integral_constant<size_t, extent_v<T, Idx>> {};


template <class Base, class Derived>
struct is_base_of : bool_constant<__is_base_of(Base, Derived)> {};
template <class Base, class Derived>
constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;


// decay_t will simulate the decline in the argument passing of function:
//   ref and cv qualifier will be removed;
//   array types will be converted to pointer types;
//   function types will be converted to function pointer types.
template <class T>
struct decay {
    using rem_ref_t = remove_reference_t<T>;
    using chk_func_t = conditional_t<is_function_v<rem_ref_t>, add_pointer_t<rem_ref_t>, remove_cv_t<rem_ref_t>>;
    using type = conditional_t<is_array_v<rem_ref_t>, add_pointer_t<remove_extent_t<rem_ref_t>>, chk_func_t>;
};
template <class T>
using decay_t = typename decay<T>::type;


template <class T, template <class...> class Template>
constexpr bool is_specialization_v = false;
template <template <class...> class Template, class... Args>
constexpr bool is_specialization_v<Template<Args...>, Template> = true;
template <class T, template <class...> class Template>
struct is_specialization : bool_constant<is_specialization_v<T, Template>> {};


template <class From, class To, bool = is_convertible_v<From, To>, bool = is_void_v<To>>
constexpr bool is_nothrow_convertible_v = noexcept(MSTL::declcopy<To>(MSTL::declval<From>()));
template <class From, class To, bool IsVoid>
constexpr bool is_nothrow_convertible_v<From, To, false, IsVoid> = false;
template <class From, class To>
constexpr bool is_nothrow_convertible_v<From, To, true, true> = true;

template <class From, class To>
struct is_nothrow_convertible : bool_constant<is_nothrow_convertible_v<From, To>> {};


// layout compatible types have the same layout in memory.
// that is, their member variables are arranged in the same order and alignment.
// they have the same member variable types, number, and arrangement order.
template <class T1, class T2>
struct is_layout_compatible : bool_constant<__is_layout_compatible(T1, T2)> {};
template <class T1, class T2>
constexpr bool is_layout_compatible_v = is_layout_compatible<T1, T2>::value;


template <class Base, class Derived>
struct is_pointer_interconvertible_base_of 
    : bool_constant<__is_pointer_interconvertible_base_of(Base, Derived)> {};
template <class Base, class Derived>
constexpr bool is_pointer_interconvertible_base_of_v = is_pointer_interconvertible_base_of<Base, Derived>::value;


template <class Class1, class Class2, class Member1, class Member2>
MSTL_NODISCARD constexpr bool is_corresponding_member(Member1 Class1::* p1, Member2 Class2::* p2) noexcept {
    return __is_corresponding_member(Class1, Class2, p1, p2);
}


template <class T>
void ref_wrapper_construct_aux(identity_t<T&>) noexcept { MSTL_NO_EVALUATION__ }
template <class T>
void ref_wrapper_construct_aux(identity_t<T&&>) = delete;

template <class, class, class = void>
struct ref_wrapper_constructable_from : false_type {};
template <class T, class U>
struct ref_wrapper_constructable_from<T, U, 
    void_t<decltype(MSTL::ref_wrapper_construct_aux<T>(MSTL::declval<U>()))>> : true_type {};

template <class T>
class reference_wrapper {
public:
    static_assert(is_object_v<T> || is_function_v<T>,
        "reference_wrapper<T> requires T to be an object type or a function type.");

    using type = T;

    template <class U> requires(conjunction_v<negation<is_same<remove_cvref_t<U>,
        reference_wrapper>>, ref_wrapper_constructable_from<T, U>>)
    MSTL_CONSTEXPR reference_wrapper(U&& x) 
        noexcept(noexcept(MSTL::ref_wrapper_construct_aux<T>(MSTL::declval<U>()))) {
        T& ref = static_cast<U&&>(x);
        ptr_ = MSTL::addressof(ref);
    }
    MSTL_CONSTEXPR operator T& () const noexcept {
        return *ptr_;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR T& get() const noexcept {
        return *ptr_;
    }

private:
    T* ptr_{};

public:
    template <class... Args>
    MSTL_CONSTEXPR decltype(auto) operator()(Args&&... args) const
        noexcept(noexcept((*ptr_)(MSTL_ forward<Args>(args)...))) {
        return (*ptr_)(MSTL_ forward<Args>(args)...);
    }
};
template <class T>
MSTL_NODISCARD MSTL_CONSTEXPR reference_wrapper<T> ref(T& val) noexcept {
    return reference_wrapper<T>(val);
}
template <class T>
void ref(const T&&) = delete;

template <class T>
MSTL_NODISCARD MSTL_CONSTEXPR reference_wrapper<T> ref(reference_wrapper<T> wrapper) noexcept {
    return wrapper;
}

template <class T>
MSTL_NODISCARD MSTL_CONSTEXPR reference_wrapper<const T> cref(const T& val) noexcept {
    return reference_wrapper<const T>(val);
}
template <class T>
void cref(const T&&) = delete;

template <class T>
MSTL_NODISCARD MSTL_CONSTEXPR reference_wrapper<const T> cref(reference_wrapper<T> wrapper) noexcept {
    return wrapper;
}


template <class T>
struct unwrap_reference {
    using type = T;
};
template <class T>
struct unwrap_reference<reference_wrapper<T>> {
    using type = T&;
};
template <class T>
using unwrap_reference_t = unwrap_reference<T>::type;


template <class T>
using unwrap_ref_decay_t = unwrap_reference_t<decay_t<T>>;
template <class T>
struct unwrap_ref_decay {
    using type = unwrap_ref_decay_t<T>;
};


template <class>
struct is_swappable;
template <class>
struct is_nothrow_swappable;

template <class T>
    requires(conjunction_v<is_move_constructible<T>, is_move_assignable<T>>)
MSTL_CONSTEXPR void swap(T&, T&) 
noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>);

template <class T, size_t Size> requires(is_swappable<T>::value)
MSTL_CONSTEXPR void swap(T(&)[Size], T(&)[Size]) noexcept(is_nothrow_swappable<T>::value);

void swap() = delete;


template <class, class, class = void>
struct one_way_swappable : false_type {};

template <class T1, class T2>
struct one_way_swappable<T1, T2, void_t<decltype(
    MSTL::swap(MSTL::declval<T1>(), MSTL::declval<T2>()))>> : true_type {};
template <class T1, class T2>
struct is_swappable_with
    : bool_constant<conjunction_v<one_way_swappable<T1, T2>, one_way_swappable<T2, T1>>> {};
template <class T1, class T2>
constexpr bool is_swappable_with_v = is_swappable_with<T1, T2>::value;


template <class T>
struct is_swappable 
    : bool_constant<is_swappable_with_v<add_lvalue_reference_t<T>, add_lvalue_reference_t<T>>> {};
template <class T>
constexpr bool is_swappable_v = is_swappable<T>::value;


template <class T1, class T2>
struct nothrow_swappable_only : bool_constant<noexcept(MSTL::swap(MSTL::declval<T1>(), MSTL::declval<T2>()))
    && noexcept(MSTL::swap(MSTL::declval<T2>(), MSTL::declval<T1>()))> {};

template <class T1, class T2>
struct is_nothrow_swappable_with
    : bool_constant<conjunction_v<is_swappable_with<T1, T2>, nothrow_swappable_only<T1, T2>>> {};
template <class T1, class T2>
constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T1, T2>::value;


template <class T>
struct is_nothrow_swappable
    : bool_constant<is_nothrow_swappable_with_v<add_lvalue_reference_t<T>, add_lvalue_reference_t<T>>> {};
template <class T>
constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;


// ADL (Argument Dependent Lookup) is a function–lookup mechanism.
// specifically， when a function is called, in addition to the global and local scopes, 
// the compiler will also look up the function declaration in the namespace to which the argument type belongs.
template <class, class = void>
struct ADL_swappable : false_type {};
template <class T>
struct ADL_swappable<T, void_t<decltype(MSTL::swap(MSTL::declval<T&>(), MSTL::declval<T&>()))>> : true_type {};

template <class T>
constexpr bool is_trivially_swappable_v = conjunction_v<is_trivially_destructible<T>,
    is_trivially_move_constructible<T>, is_trivially_move_assignable<T>, negation<ADL_swappable<T>>>;
TEMNULL__
inline constexpr bool is_trivially_swappable_v<byte_t> = true;

template <class T>
struct is_trivially_swappable : bool_constant<is_trivially_swappable_v<T>> {};


#if defined(MSTL_COMPILE_CLANG__)
template <class From, class To>
concept convertible_to = is_convertible_v<From, To>&& requires { static_cast<To>(MSTL::declval<From>()); };
#elif defined(MSTL_COMPILE_GCC__)
template <typename From, typename To>
auto conversion_aux(int) -> decltype(static_cast<To>(MSTL::declval<From>()), true_type{}) { MSTL_NO_EVALUATION__ }
template <typename From, typename To>
false_type conversion_aux(...) { MSTL_NO_EVALUATION__ }

template <typename From, typename To, bool = disjunction_v<is_void<From>, is_function<To>, is_array<To>>>
struct is_convertible_aux : bool_constant<is_void_v<To>> {};
template <typename From, typename To>
struct is_convertible_aux<From, To, false> : decltype(test_conversion<From, To>(0)) {};

template <typename From, typename To>
struct convertible_to : bool_constant<is_convertible_aux<From, To>::value> {};
#elif defined(MSTL_COMPILE_MSVC__)
template <class From, class To>
concept convertible_to = __is_convertible_to(From, To) && requires { static_cast<To>(MSTL::declval<From>()); };
#endif
template <class From, class To>
struct is_convertible_to : bool_constant<convertible_to<From, To>> {};
template <class From, class To>
constexpr bool is_convertible_to_v = is_convertible_to<From, To>::value;


template <typename Key>
struct hash;

template <class, class = void>
struct is_nothrow_hashable : false_type {};
template <class Key>
struct is_nothrow_hashable<Key, void_t<decltype(MSTL::hash<Key>{}(MSTL::declval<const Key&>()))>>
    : bool_constant<noexcept(MSTL::hash<Key>{}(MSTL::declval<const Key&>()))> {};
template <typename Key>
constexpr bool is_nothrow_hashable_v = is_nothrow_hashable<Key>::value;

MSTL_END_NAMESPACE__
#endif // MSTL_TYPE_TRAITS_HPP__
