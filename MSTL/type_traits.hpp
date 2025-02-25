#ifndef MSTL_TYPE_TRAITS_HPP__
#define MSTL_TYPE_TRAITS_HPP__
#include "basiclib.h"
MSTL_BEGIN_NAMESPACE__

template <typename T, T Value>
struct integral_constant {
    static constexpr T value = Value;

    using value_type    = T;
    using type          = integral_constant;

    constexpr operator value_type() const noexcept {
        return value;
    }
    MSTL_NODISCARD constexpr value_type operator()() const noexcept {
        return value;
    }
};


template <bool Value>
using bool_constant = integral_constant<bool, Value>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;


// Test is false, SFINAF
template <bool Test, typename T = void>
struct enable_if {};
// Test is true
template <typename T>
struct enable_if<true, T> {
    using type = T;
};
template <bool Test, typename T = void>
using enable_if_t = typename enable_if<Test, T>::type;


// Test is true, using type = T1
template <bool Test, typename T1, typename T2>
struct conditional {
    using type = T1;
};
// Test is false, using type = T2
template <typename T1, typename T2>
struct conditional<false, T1, T2> {
    using type = T2;
};
template <bool Test, typename T1, typename T2>
using conditional_t = typename conditional<Test, T1, T2>::type;


template <typename T>
struct negation : bool_constant<!static_cast<bool>(T::value)> {};
template <typename T>
constexpr bool negation_v = negation<T>::value;


template <typename, typename>
constexpr bool is_same_v = false;
template <typename T>
constexpr bool is_same_v<T, T> = true;

template <typename T1, typename T2>
struct is_same : bool_constant<is_same_v<T1, T2>> {};


template <typename T>
struct type_identity {
    using type = T;
};
template <typename T>
using type_identity_t = typename type_identity<T>::type;


#ifdef MSTL_VERSION_17__
template <typename T, typename... Types>
constexpr bool is_any_of_v = (is_same_v<T, Types> || ...);
template <typename T, typename... Types>
struct is_any_of : bool_constant<is_any_of_v<T, Types...>> {};
#else
template <typename T, typename... Types>
struct is_any_of : false_type {};
template <typename T, typename U>
struct is_any_of<T, U> : is_same<T, U> {};

template <typename T, typename U, typename... Types>
struct is_any_of<T, U, Types...> 
    : conditional<is_same_v<T, U>, true_type, is_any_of<T, Types...>>::type {};
template <typename T, typename... Types>
constexpr bool is_any_of_v = is_any_of<T, Types...>::value;
#endif // MSTL_VERSION_17__


// Test is true, stop recursion and set type
template <bool, typename first, typename...>
struct disjunction_aux {
    using type = first;
};
// Curr`s Test is false, continue checking next
template <typename Curr, typename Next, typename... Rest>
struct disjunction_aux<false, Curr, Next, Rest...> {
    using type = typename disjunction_aux<static_cast<bool>(Next::value), Next, Rest...>::type;
};

// recursion end
template <typename...>
struct disjunction : false_type {};
// start recursion
template <typename First, typename... Rest>
struct disjunction<First, Rest...>
    : disjunction_aux<static_cast<bool>(First::value), First, Rest...>::type {
};

template <typename... Args>
constexpr bool disjunction_v = disjunction<Args...>::value;


// Test is false, stop recursion and set type
template <bool, typename First, typename...>
struct conjunction_aux {
    using type = First;
};
// Curr`s Test is true, continue checking next
template <typename Curr, typename Next, typename... Rest>
struct conjunction_aux<true, Curr, Next, Rest...> {
    using type = typename conjunction_aux<static_cast<bool>(Next::value), Next, Rest...>::type;
};

// recursion finished
template <typename...>
struct conjunction : true_type {};
// start recursion
template <typename First, typename... Rest>
struct conjunction<First, Rest...>
    : conjunction_aux<static_cast<bool>(First::value), First, Rest...>::type {
};

template <typename... Args>
constexpr bool conjunction_v = conjunction<Args...>::value;


template <typename T>
struct remove_const {
    using type = T;
};
template <typename T>
struct remove_const<const T> {
    using type = T;
};
template <typename T>
using remove_const_t = typename remove_const<T>::type;


template <typename T>
struct remove_volatile {
    using type = T;
};
template <typename T>
struct remove_volatile<volatile T> {
    using type = T;
};
template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;


template <typename T>
struct remove_cv {
    using type = T;

    template <typename wrapper>
    using bind_cv_t = wrapper;
};
template <typename T>
struct remove_cv<const T> {
    using type = T;

    template <typename wrapper>
    using bind_cv_t = const wrapper;
};
template <typename T>
struct remove_cv<volatile T> {
    using type = T;

    template <typename wrapper>
    using bind_cv_t = volatile wrapper;
};
template <typename T>
struct remove_cv<const volatile T> {
    using type = T;

    template <typename wrapper>
    using bind_cv_t = const volatile wrapper;
};
template <typename T>
using remove_cv_t = typename remove_cv<T>::type;
template <typename From, typename To>
using copy_cv_t = typename remove_cv<From>::template bind_cv_t<To>;


template <typename T>
struct remove_reference {
    using type = T;

    template <typename wrapper>
    using bind_ref_t = wrapper;
};
template <typename T>
struct remove_reference<T&> {
    using type = T;

    template <typename wrapper>
    using bind_ref_t = wrapper&;
};
template <typename T>
struct remove_reference<T&&> {
    using type = T;

    template <typename wrapper>
    using bind_ref_t = wrapper&&;
};
template <typename T>
using remove_reference_t = typename remove_reference<T>::type;
template <typename From, typename To>
using copy_ref_t = typename remove_reference<From>::template bind_ref_t<To>;

template <typename From, typename To>
using copy_cvref_t = copy_ref_t<From, copy_cv_t<From, To>>;


template <typename T>
using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;
template <typename T>
struct remove_cvref {
    using type = remove_cvref_t<T>;
};


template <typename T>
struct remove_extent {
    using type = T;
};
template <typename T, size_t Idx>
struct remove_extent<T[Idx]> {
    using type = T;
};
template <typename T>
struct remove_extent<T[]> {
    using type = T;
};

template <typename T>
using remove_extent_t = typename remove_extent<T>::type;


template <typename T>
struct remove_all_extents {
    using type = T;
};
template <typename T, size_t Idx>
struct remove_all_extents<T[Idx]> {
    using type = typename remove_all_extents<T>::type;
};
template <typename T>
struct remove_all_extents<T[]> {
    using type = typename remove_all_extents<T>::type;
};

template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;


template <typename T>
struct remove_pointer {
    using type = T;

    template <typename wrapper>
    using bind_pointer_t = wrapper;
};
template <typename T>
struct remove_pointer<T*> {
    using type = T;

    template <typename wrapper>
    using bind_pointer_t = wrapper*;
};
template <typename T>
struct remove_pointer<T* const> {
    using type = T;

    template <typename wrapper>
    using bind_pointer_t = const wrapper*;
};
template <typename T>
struct remove_pointer<T* volatile> {
    using type = T;

    template <typename wrapper>
    using bind_pointer_t = volatile wrapper*;
};
template <typename T>
struct remove_pointer<T* const volatile> {
    using type = T;

    template <typename wrapper>
    using bind_pointer_t = volatile const wrapper*;
};
template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;
template <typename From, typename To>
using copy_pointer_t = typename remove_pointer<From>::template bind_pointer_t<To>;


template <typename T>
constexpr bool is_void_v = is_same_v<remove_cv_t<T>, void>;
template <typename T>
struct is_void : bool_constant<is_void_v<T>> {};

template <typename...>
using void_t = void;


template <typename T>
constexpr bool is_character_v = is_any_of_v<remove_cv_t<T>,
    char, signed char, unsigned char, wchar_t,
#ifdef MSTL_VERSION_20__
    char8_t,
#endif
    char16_t, char32_t
>;
template <typename T>
struct is_character : bool_constant<is_character_v<T>> {};


template <typename T>
constexpr bool is_integral_v = is_any_of_v<remove_cv_t<T>, short, unsigned short, int, unsigned int, long,
    unsigned long, long long, unsigned long long, unsigned char, char>;
template <typename T>
struct is_integral : bool_constant<is_integral_v<T>> {};


template <typename T>
constexpr bool is_standard_integer_v = is_any_of_v<remove_cv_t<T>, signed char, short, int, long, long long,
    unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;
template <typename T>
struct is_standard_integral : bool_constant<is_standard_integer_v<T>> {};


template <typename T>
constexpr bool is_nonbool_integral = is_integral_v<T> && !is_same_v<remove_cv_t<T>, bool>;


template <typename T>
constexpr bool is_floating_point_v = is_any_of_v<remove_cv_t<T>, float, double, long double>;
template <typename T>
struct is_floating_point : bool_constant<is_floating_point_v<T>> {};


template <typename T>
constexpr bool is_arithmetic_v = is_integral_v<T> || is_floating_point_v<T>;
template <typename T>
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

template <typename T>
struct is_unsigned : bool_constant<sign_aux<T>::is_unsigned> {};
template <typename T>
constexpr bool is_unsigned_v = is_unsigned<T>::value;


template <typename T>
struct add_const {
    using type = const T;
};
template <typename T>
using add_const_t = typename add_const<T>::type;

template <typename T>
MSTL_NODISCARD MSTL_CONSTEXPR add_const_t<T>& as_const(T& val) noexcept {
    return val;
}
template <typename T>
void as_const(const T&&) = delete;


template <typename T>
struct add_volatile {
    using type = volatile T;
};
template <typename T>
using add_volatile_t = typename add_volatile<T>::type;


template <typename T>
struct add_cv {
    using type = const volatile T;
};
template <typename T>
using add_cv_t = typename add_cv<T>::type;


template <typename T, typename = void>
struct add_reference {
    using lvalue = T;
    using rvalue = T;
};
template <typename T>
struct add_reference<T, void_t<T&>> {
    using lvalue = T&;
    using rvalue = T&&;
};

template <typename T>
struct add_lvalue_reference {
    using type = typename add_reference<T>::lvalue;
};
template <typename T>
using add_lvalue_reference_t = typename add_reference<T>::lvalue;

template <typename T>
struct add_rvalue_reference {
    using type = typename add_reference<T>::rvalue;
};
template <typename T>
using add_rvalue_reference_t = typename add_reference<T>::rvalue;


template <typename T, typename = void>
struct add_pointer {
    using type = T;
};
template <typename T>
struct add_pointer<T, void> {
    using type = remove_reference_t<T>*;
};

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;


// declval will only work in no evaluation context (decltype or sizeof) to quickly get rvalue types. 
template <typename T>
add_rvalue_reference_t<T> declval() noexcept { MSTL_NO_EVALUATION }
// try to copy type T
template <typename T>
T declcopy(T) noexcept { MSTL_NO_EVALUATION }
// work with is_void_t
template <typename T>
void declvoid(T) noexcept { MSTL_NO_EVALUATION }


template <typename>
constexpr bool is_array_v = false;
template <typename T, size_t Idx>
constexpr bool is_array_v<T[Idx]> = true;
template <typename T>
constexpr bool is_array_v<T[]> = true;

template <typename T>
struct is_array : bool_constant<is_array_v<T>> {};


template <typename>
constexpr bool is_bounded_array_v = false;
template <typename T, size_t Idx>
constexpr bool is_bounded_array_v<T[Idx]> = true;

template <typename T>
struct is_bounded_array : bool_constant<is_bounded_array_v<T>> {};


template <typename>
constexpr bool is_unbounded_array_v = false;
template <typename T>
constexpr bool is_unbounded_array_v<T[]> = true;

template <typename T>
struct is_unbounded_array : bool_constant<is_unbounded_array_v<T>> {};


template <typename>
constexpr bool is_lvalue_reference_v = false;
template <typename T>
constexpr bool is_lvalue_reference_v<T&> = true;

template <typename T>
struct is_lvalue_reference : bool_constant<is_lvalue_reference_v<T>> {};


template <typename>
constexpr bool is_rvalue_reference_v = false;
template <typename T>
constexpr bool is_rvalue_reference_v<T&&> = true;

template <typename T>
struct is_rvalue_reference : bool_constant<is_rvalue_reference_v<T>> {};


template <typename>
constexpr bool is_reference_v = false;
template <typename T>
constexpr bool is_reference_v<T&> = true;
template <typename T>
constexpr bool is_reference_v<T&&> = true;

template <typename T>
struct is_reference : bool_constant<is_reference_v<T>> {};


template <typename>
constexpr bool is_pointer_v = false;
template <typename T>
constexpr bool is_pointer_v<T*> = true;
template <typename T>
constexpr bool is_pointer_v<T* const> = true;
template <typename T>
constexpr bool is_pointer_v<T* volatile> = true;
template <typename T>
constexpr bool is_pointer_v<T* const volatile> = true;

template <typename T>
struct is_pointer : bool_constant<is_pointer_v<T>> {};


template <typename T>
constexpr bool is_null_pointer_v = is_same_v<remove_cv_t<T>, std::nullptr_t>;
template <typename T>
struct is_null_pointer : bool_constant<is_null_pointer_v<T>> {};


template<typename T>
struct is_enum : bool_constant<__is_enum(T)> {};
template <typename T>
constexpr bool is_enum_v = is_enum<T>::value;

template <typename T>
struct is_union : bool_constant<__is_union(T)> {};
template <typename T>
constexpr bool is_union_v = is_union<T>::value;

template <typename T>
struct is_class : bool_constant<__is_class(T)> {};
template <typename T>
constexpr bool is_class_v = is_class<T>::value;


// literal type
template <typename T>
constexpr bool is_fundamental_v = is_arithmetic_v<T> || is_void_v<T> || is_null_pointer_v<T>;
template <typename T>
struct is_fundamental : bool_constant<is_fundamental_v<T>> {};

template <typename T>
struct is_compound : bool_constant<!is_fundamental_v<T>> {};
template <typename T>
constexpr bool is_compound_v = is_compound<T>::value;


template <typename From, typename To>
struct is_convertible : bool_constant<__is_convertible_to(From, To)> {};
template <typename From, typename To>
constexpr bool is_convertible_v = is_convertible<From, To>::value;


template <typename>
constexpr bool is_const_v = false;
template <typename T>
constexpr bool is_const_v<const T> = true;
template <typename T>
struct is_const : bool_constant<is_const_v<T>> {};


template <typename>
constexpr bool is_volatile_v = false;
template <typename T>
constexpr bool is_volatile_v<volatile T> = true;
template <typename T>
struct is_volatile : bool_constant<is_volatile_v<T>> {};


// function cannot be const qualified.
template <typename T>
constexpr bool is_function_v = !is_const_v<const T> && !is_reference_v<T>;
template <typename T>
struct is_function : bool_constant<is_function_v<T>> {};


template <typename T>
constexpr bool is_allocable_v = !(is_void_v<T> || is_reference_v<T> || is_function_v<T> || is_const_v<T>);
template <typename T>
struct is_allocable : bool_constant<is_allocable_v<T>> {};


// object types are types without function, reference or void types. 
//   especially, function and reference types cannot be const–qualified.
template <typename T>
constexpr bool is_object_v = is_const_v<const T> && !is_void_v<T>;
template <typename T>
struct is_object : bool_constant<is_object_v<T>> {};


#if defined(MSTL_COMPILE_CLANG__)
template <typename T>
struct is_member_function_pointer : bool_constant<__is_member_function_pointer(T)> {};
#else
template <typename>
struct is_member_function_pointer_aux : false_type {};
template <typename T, typename C>
struct is_member_function_pointer_aux<T C::*> : is_function<T> {};
template<typename T>
struct is_member_function_pointer : is_member_function_pointer_aux<remove_cv_t<T>> {};
#endif
template <typename T>
constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;


#if defined(MSTL_COMPILE_CLANG__)
template <typename T>
struct is_member_object_pointer : bool_constant<__is_member_object_pointer(T)> {};
#else
template <typename>
struct is_member_object_pointer : false_type {};
template <typename T, typename C>
struct is_member_object_pointer<T C::*> : bool_constant<!is_function_v<T>> {};
#endif
template <typename T>
constexpr bool is_member_object_pointer_v = is_member_object_pointer<remove_cv_t<T>>::value;


#if defined(MSTL_COMPILE_CLANG__)
template <typename T>
constexpr bool is_member_pointer_v = __is_member_pointer(T);
#else
template <typename T>
constexpr bool is_member_pointer_v = is_member_object_pointer_v<T> || is_member_function_pointer_v<T>;
#endif
template <typename T>
struct is_member_pointer : bool_constant<is_member_pointer_v<T>> {};


// arithmetic, enum and pointer types are scalar types.
template <typename T>
constexpr bool is_scalar_v =
is_arithmetic_v<T> || is_enum_v<T> || is_pointer_v<T> || is_member_pointer_v<T> || is_null_pointer_v<T>;
template <typename T>
struct is_scalar : bool_constant<is_scalar_v<T>> {};


// empty types don't have non-static members, virtual functions and virtual base class.
// if it has a base class, that should also be an empty type.
template <typename T>
struct is_empty : bool_constant<__is_empty(T)> {};
template <typename T>
constexpr bool is_empty_v = is_empty<T>::value;


template <typename T>
struct is_polymorphic : bool_constant<__is_polymorphic(T)> {};
template <typename T>
constexpr bool is_polymorphic_v = is_polymorphic<T>::value;


template <typename T>
struct is_abstract : bool_constant<__is_abstract(T)> {};
template <typename T>
constexpr bool is_abstract_v = is_abstract<T>::value;


template <typename T>
struct is_final : bool_constant<__is_final(T)> {};
template <typename T>
constexpr bool is_final_v = is_final<T>::value;


// standard layout types have these characteristics:
//   consistent access control for non-static members (all public).
//   no virtual functions and virtual base classes.
//   no base class or there is only one base class and the base class is also a standard layout type.
//   non-static data members cannot appear in both the base class and the derived class.
template <typename T>
struct is_standard_layout : bool_constant<__is_standard_layout(T)> {};
template <typename T>
constexpr bool is_standard_layout_v = is_standard_layout<T>::value;


// trivial types have these characteristics:
//   the constructor, copy constructor, move constructor, copy assignment operator, 
//   move assignment operator, and destructor are automatically generated by the compiler (is default).
//   no virtual functions and virtual base classes.
template <typename T>
struct is_trivial : bool_constant<__is_trivial(T)> {};
template <typename T>
constexpr bool is_trivial_v = is_trivial<T>::value;


template <typename T>
struct is_trivially_copyable : bool_constant<__is_trivially_copyable(T)> {};
template <typename T>
constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;


template <typename T>
struct has_virtual_destructor : bool_constant<__has_virtual_destructor(T)> {};
template <typename T>
constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;


// each different value of a type has a unique binary representation, 
// but padding bytes and implementation dependencies should also be taken into account.
// if a type has unique object representations,
// we can use memory comparison to easily determine the equality.
// standard layout types and trivial types match unique object representations.
template <typename T>
struct has_unique_object_representations : bool_constant<__has_unique_object_representations(T)> {};
template <typename T>
constexpr bool has_unique_object_representations_v = has_unique_object_representations<T>::value;


#if defined(MSTL_COMPILE_MSVC__) 
template <typename T>
struct is_aggregate_aux : bool_constant<__is_aggregate(T)> {};
template <typename T>
constexpr bool is_aggregate_v = disjunction_v<is_array<T>, is_aggregate_aux<T>>;
#else
template <typename T>
constexpr bool is_aggregate_v = __is_aggregate(remove_cv_t<T>);
#endif

// aggregate types have these characteristics:
//   is array types;
//   is class types (include struct and union) which have the following characteristics:
//     no user-defined constructors;
//     all non-static data members are public; 
//     no virtual functions and virtual base classes.
template <typename T>
struct is_aggregate : bool_constant<is_aggregate_v<T>> {};


template <typename T, typename... Args>
struct is_constructible : bool_constant<__is_constructible(T, Args...)> {};
template <typename T, typename... Args>
constexpr bool is_constructible_v = is_constructible<T, Args...>::value;


template <typename T>
struct is_copy_constructible 
    : bool_constant<is_constructible_v<T, add_lvalue_reference_t<const T>>> {};
template <typename T>
constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;


template <typename T>
struct is_default_constructible : bool_constant<is_constructible_v<T>> {};
template <typename T>
constexpr bool is_default_constructible_v = is_default_constructible<T>::value;


template <typename T>
void implicitly_default_construct_aux(const T&) noexcept { MSTL_NO_EVALUATION }

template <typename, typename = void>
struct is_implicitly_default_constructible : false_type {};
template <typename T>
struct is_implicitly_default_constructible 
    <T, void_t<decltype(implicitly_default_construct_aux<T>({}))>> : true_type {};

template <typename T>
constexpr bool is_implicitly_default_constructible_v = is_implicitly_default_constructible<T>::value;


template <typename T>
struct is_move_constructible : bool_constant<is_constructible_v<T, T>> {};
template <typename T>
constexpr bool is_move_constructible_v = is_move_constructible<T>::value;


template <typename To, typename From>
struct is_assignable : bool_constant<__is_assignable(To, From)> {};
template <typename To, typename From>
constexpr bool is_assignable_v = is_assignable<To, From>::value;


template <typename T>
struct is_copy_assignable
    : bool_constant<is_assignable_v<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>>> {};
template <typename T>
constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;


template <typename T>
struct is_move_assignable : bool_constant<is_assignable_v<add_lvalue_reference_t<T>, T>> {};
template <typename T>
constexpr bool is_move_assignable_v = is_move_assignable<T>::value;


template <typename T>
struct is_destructible : bool_constant<__is_destructible(T)> {};
template <typename T>
constexpr bool is_destructible_v = is_destructible<T>::value;


template <typename T, typename... Args>
struct is_trivially_constructible : bool_constant<__is_trivially_constructible(T, Args...)> {};
template <typename T, typename... Args>
constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;


template <typename T>
struct is_trivially_copy_constructible
    : bool_constant<is_trivially_constructible_v<T, add_lvalue_reference_t<const T>>> {};
template <typename T>
constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<T>::value;


template <typename T>
struct is_trivially_default_constructible : bool_constant<is_trivially_constructible_v<T>> {};
template <typename T>
constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;


template <typename T>
struct is_trivially_move_constructible : bool_constant<is_trivially_constructible_v<T, T>> {};
template <typename T>
constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;


template <typename To, typename From>
struct is_trivially_assignable : bool_constant<__is_trivially_assignable(To, From)> {};
template <typename To, typename From>
constexpr bool is_trivially_assignable_v = is_trivially_assignable<To, From>::value;


template <typename T>
struct is_trivially_copy_assignable : bool_constant<is_trivially_assignable_v<
    add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>>> {};
template <typename T>
constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<T>::value;


template <typename T>
struct is_trivially_move_assignable 
    : bool_constant<is_trivially_assignable_v<add_lvalue_reference_t<T>, T>> {};
template <typename T>
constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<T>::value;


template <typename T>
struct is_trivially_destructible : bool_constant<__is_trivially_destructible(T)> {};
template <typename T>
constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;


template <typename T, typename... Args>
struct is_nothrow_constructible : bool_constant<__is_nothrow_constructible(T, Args...)> {};
template <typename T, typename... Args>
constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;


template <typename T>
struct is_nothrow_copy_constructible
    : bool_constant<is_nothrow_constructible_v<T, add_lvalue_reference_t<const T>>> {};
template <typename T>
constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;


template <typename T>
struct is_nothrow_default_constructible : bool_constant<is_nothrow_constructible_v<T>> {};
template <typename T>
constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;


template <typename T>
struct is_nothrow_move_constructible : bool_constant<is_nothrow_constructible_v<T, T>> {};
template <typename T>
constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;


template <typename To, typename From>
struct is_nothrow_assignable : bool_constant<__is_nothrow_assignable(To, From)> {};
template <typename To, typename From>
constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<To, From>::value;


template <typename T>
struct is_nothrow_copy_assignable
    : bool_constant<is_nothrow_assignable_v<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>>> {};
template <typename T>
constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;


template <typename T>
struct is_nothrow_move_assignable : bool_constant<is_nothrow_assignable_v<add_lvalue_reference_t<T>, T>> {};
template <typename T>
constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;


template <typename T>
struct is_nothrow_destructible : bool_constant<__is_nothrow_destructible(T)> {};
template <typename T>
constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;


template <typename T>
MSTL_NODISCARD constexpr T&& forward(remove_reference_t<T>& x) noexcept {
    return static_cast<T&&>(x);
}
template <typename T>
MSTL_NODISCARD constexpr T&& forward(remove_reference_t<T>&& x) noexcept {
    static_assert(!is_lvalue_reference_v<T>, "forward failed.");
    return static_cast<T&&>(x);
}
template <typename T>
MSTL_NODISCARD constexpr remove_reference_t<T>&& move(T&& x) noexcept {
    return static_cast<remove_reference_t<T>&&>(x);
}
template <typename T>
MSTL_NODISCARD constexpr
conditional_t<!is_nothrow_move_constructible_v<T>&& is_copy_constructible_v<T>, const T&, T&&>
move_if_noexcept(T& x) noexcept {
    return MSTL::move(x);
}
template <typename T>
MSTL_NODISCARD constexpr T* addressof(T& x) noexcept {
    return __builtin_addressof(x);
}
template <typename T>
const T* addressof(const T&&) = delete;

MSTL_NODISCARD constexpr bool is_constant_evaluated() noexcept {
    return __builtin_is_constant_evaluated();
}


template <size_t>
struct sign_byte_aux;

TEMNULL__
struct sign_byte_aux<1> {
    template <typename>
    using signed_t = signed char;
    template <typename>
    using unsigned_t = unsigned char;
};
TEMNULL__
struct sign_byte_aux<2> {
    template <typename>
    using signed_t = signed short;
    template <typename>
    using unsigned_t = unsigned short;
};
TEMNULL__
struct sign_byte_aux<4> {
    template <typename T>
    using signed_t = 
        conditional_t<is_same_v<T, signed long> || is_same_v<T, unsigned long>, signed long, signed int>;

    template <typename T>
    using unsigned_t = 
        conditional_t<is_same_v<T, signed long> || is_same_v<T, unsigned long>, unsigned long, unsigned int>;
};
TEMNULL__
struct sign_byte_aux<8> {
    template <typename>
    using signed_t = signed MSTL_LLT;
    template <typename>
    using unsigned_t = unsigned MSTL_LLT;
};

template <typename T>
using set_signed_byte = typename sign_byte_aux<sizeof(T)>::template signed_t<T>;
template <typename T>
using set_unsigned_byte = typename sign_byte_aux<sizeof(T)>::template unsigned_t<T>;

template <typename T>
struct set_signed {
    static_assert(is_nonbool_integral<T> || is_enum_v<T>,
        "make signed only support non bool integral types and enum types");

    using signed_type   = copy_cv_t<T, set_signed_byte<T>>;
    using unsigned_type = copy_cv_t<T, set_unsigned_byte<T>>;
};
template <typename T>
using make_signed_t = typename set_signed<T>::signed_type;
template <typename T>
using make_unsigned_t = typename set_signed<T>::unsigned_type;

template <typename T>
struct make_signed {
    using type = make_signed_t<T>;
};
template <typename T>
struct make_unsigned {
    using type = make_unsigned_t<T>;
};


template <typename T>
constexpr make_signed_t<T> signed_value(T x) {
    return static_cast<make_signed_t<T>>(x);
}
template <typename T>
constexpr make_unsigned_t<T> unsigned_value(T x) {
    return static_cast<make_unsigned_t<T>>(x);
}


template <typename T>
struct alignment_of : integral_constant<size_t, alignof(T)> {};
template <typename T>
constexpr size_t alignment_of_v = alignment_of<T>::value;


// get the base type of enum
template <typename T, bool = is_enum_v<T>>
struct underlying_type {
    using type = __underlying_type(T);
};
template <typename T>
struct underlying_type<T, false> {};
template <typename T>
using underlying_type_t = typename underlying_type<T>::type;


template <typename>
constexpr size_t rank_v = 0;
template <typename T, size_t Idx>
constexpr size_t rank_v<T[Idx]> = rank_v<T> +1;
template <typename T>
constexpr size_t rank_v<T[]> = rank_v<T> +1;

template <typename T>
struct rank : integral_constant<size_t, rank_v<T>> {};


template <typename, unsigned int = 0>
constexpr size_t extent_v = 0;
template <typename T, size_t N>
constexpr size_t extent_v<T[N], 0> = N;
template <typename T, unsigned int Idx, size_t N>
constexpr size_t extent_v<T[N], Idx> = extent_v<T, Idx - 1>;
template <typename T, unsigned int Idx>
constexpr size_t extent_v<T[], Idx> = extent_v<T, Idx - 1>;

template <typename T, unsigned int Idx = 0>
struct extent : integral_constant<size_t, extent_v<T, Idx>> {};


template <typename Base, typename Derived>
struct is_base_of : bool_constant<__is_base_of(Base, Derived)> {};
template <typename Base, typename Derived>
constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;


// decay_t will simulate the decline in the argument passing of function:
//   ref and cv qualifier will be removed;
//   array and function types will be converted to pointer types unless they are used to iitialize referece.
template <typename T>
struct decay {
    using rem_ref_t = remove_reference_t<T>;
    using chk_func_t = conditional_t<is_function_v<rem_ref_t>, add_pointer_t<rem_ref_t>, remove_cv_t<rem_ref_t>>;
    using type = conditional_t<is_array_v<rem_ref_t>, add_pointer_t<remove_extent_t<rem_ref_t>>, chk_func_t>;
};
template <typename T>
using decay_t = typename decay<T>::type;


// ternary operator (expr1 ? expr2 : expr3) will try to find a suitable common type,
// so that both expr2 and expr3 can implicitly convert to this type:
//   if expr2 and expr3 are of the same type, then the type of the entire expression is that type.
//   if expr2 and expr3 are different types, the compiler will attempt to type a common type.
//   conversion rules are typically based on standard implicit type conversion rules.
template <typename T1, typename T2>
using common_ternary_operator_t = decltype(false ? MSTL::declval<T1>() : MSTL::declval<T2>());

template <typename, typename, typename = void>
struct oper_decay_aux {};
template <typename T1, typename T2>
struct oper_decay_aux<T1, T2, void_t<common_ternary_operator_t<T1, T2>>> {
    using type = decay_t<common_ternary_operator_t<T1, T2>>;
};

template <typename...>
struct common_type;
template <typename... T>
using common_type_t = typename common_type<T...>::type;

template <>
struct common_type<> {};
template <typename T1>
struct common_type<T1> : common_type<T1, T1> {};
template <typename T1, typename T2>
struct common_type<T1, T2> : oper_decay_aux<T1, T2> {};
template <typename T1, typename T2, typename... Rest>
struct common_type<T1, T2, Rest...> : common_type<common_type_t<T1, T2>, Rest...> {};


#ifdef MSTL_VERSION_20__
template <typename, typename, template <typename> typename, template <typename> typename>
struct basic_common_reference {};
template <typename T1>
struct add_qualifier_aux {
    template <typename T2>
    using apply_t = copy_ref_t<T1, copy_cv_t<T1, T2>>;
};

template <typename...>
struct common_reference;
template <typename... Types>
using common_reference_t = common_reference<Types...>::type;

template <>
struct common_reference<> {};
template <typename T>
struct common_reference<T> {
    using type = T;
};

template <typename T1, typename T2>
struct common_reference_base_aux : common_type<T1, T2> {};

template <typename T1, typename T2>
    requires requires { typename common_ternary_operator_t<T1, T2>; }
struct common_reference_base_aux<T1, T2> {
    using type = common_ternary_operator_t<T1, T2>;
};

template <typename T1, typename T2>
using qualifier_extract = basic_common_reference<remove_cvref_t<T1>, remove_cvref_t<T2>,
    add_qualifier_aux<T1>::template apply_t, add_qualifier_aux<T2>::template apply_t>::type;

template <typename T1, typename T2>
struct common_ref_qualify_aux : common_reference_base_aux<T1, T2> {};

template <typename T1, typename T2>
    requires requires { typename qualifier_extract<T1, T2>; }
struct common_ref_qualify_aux<T1, T2> {
    using type = qualifier_extract<T1, T2>;
};

template <typename T1, typename T2>
struct common_reference_ptr_aux : common_ref_qualify_aux<T1, T2> {};

template <typename T1, typename T2>
    requires is_lvalue_reference_v<common_ternary_operator_t<copy_cv_t<T1, T2>&, copy_cv_t<T2, T1>&>>
using common_lvalue_aux = common_ternary_operator_t<copy_cv_t<T1, T2>&, copy_cv_t<T2, T1>&>;

template <typename, typename>
struct common_reference_aux {};

template <typename T1, typename T2>
    requires requires { typename common_lvalue_aux<T1, T2>; }
struct common_reference_aux<T1&, T2&> {
    using type = common_lvalue_aux<T1, T2>;
};

template <typename T1, typename T2>
    requires is_convertible_v<T1&&, common_lvalue_aux<const T1, T2>>
struct common_reference_aux<T1&&, T2&> {
    using type = common_lvalue_aux<const T1, T2>;
};

template <typename T1, typename T2>
    requires is_convertible_v<T2&&, common_lvalue_aux<const T2, T1>>
struct common_reference_aux<T1&, T2&&> {
    using type = common_lvalue_aux<const T2, T1>;
};

template <typename T1, typename T2>
using common_rvalue_aux = remove_reference_t<common_lvalue_aux<T1, T2>>&&;

template <typename T1, typename T2>
    requires is_convertible_v<T1&&, common_rvalue_aux<T1, T2>>
&& is_convertible_v<T2&&, common_rvalue_aux<T1, T2>>
struct common_reference_aux<T1&&, T2&&> {
    using type = common_rvalue_aux<T1, T2>;
};

template <typename T1, typename T2>
using common_reference_aux_t = common_reference_aux<T1, T2>::type;

template <typename T1, typename T2>
    requires is_convertible_v<add_pointer_t<T1>, add_pointer_t<common_reference_aux_t<T1, T2>>>
&& is_convertible_v<add_pointer_t<T2>, add_pointer_t<common_reference_aux_t<T1, T2>>>
struct common_reference_ptr_aux<T1, T2> {
    using type = common_reference_aux_t<T1, T2>;
};

template <typename T1, typename T2>
struct common_reference<T1, T2> : common_reference_ptr_aux<T1, T2> {};

template <typename T1, typename T2, typename T3, typename... Rest>
struct common_reference<T1, T2, T3, Rest...> {};

template <typename T1, typename T2, typename T3, typename... Rest>
    requires requires { typename common_reference_t<T1, T2>; }
struct common_reference<T1, T2, T3, Rest...> : common_reference<common_reference_t<T1, T2>, T3, Rest...> {};
#endif // MSTL_VERSION_20__


template <typename T, template <typename...> typename Template>
constexpr bool is_specialization_v = false;
template <template <typename...> typename Template, typename... Args>
constexpr bool is_specialization_v<Template<Args...>, Template> = true;

template <typename T, template <typename...> typename Template>
struct is_specialization : bool_constant<is_specialization_v<T, Template>> {};


template <typename From, typename To, bool = is_convertible_v<From, To>, bool = is_void_v<To>>
constexpr bool is_nothrow_convertible_v = noexcept(MSTL::declcopy<To>(MSTL::declval<From>()));
template <typename From, typename To, bool IsVoid>
constexpr bool is_nothrow_convertible_v<From, To, false, IsVoid> = false;
template <typename From, typename To>
constexpr bool is_nothrow_convertible_v<From, To, true, true> = true;

template <typename From, typename To>
struct is_nothrow_convertible : bool_constant<is_nothrow_convertible_v<From, To>> {};


// layout compatible types have the same layout in memory.
// that is, their member variables are arranged in the same order and alignment.
// they have the same member variable types, number, and arrangement order.
template <typename T1, typename T2>
struct is_layout_compatible : bool_constant<__is_layout_compatible(T1, T2)> {};
template <typename T1, typename T2>
constexpr bool is_layout_compatible_v = is_layout_compatible<T1, T2>::value;


template <typename Base, typename Derived>
struct is_pointer_interconvertible_base_of 
    : bool_constant<__is_pointer_interconvertible_base_of(Base, Derived)> {};
template <typename Base, typename Derived>
constexpr bool is_pointer_interconvertible_base_of_v = is_pointer_interconvertible_base_of<Base, Derived>::value;


template <typename Class1, typename Class2, typename Member1, typename Member2>
MSTL_NODISCARD constexpr bool is_corresponding_member(Member1 Class1::* p1, Member2 Class2::* p2) noexcept {
    return __is_corresponding_member(Class1, Class2, p1, p2);
}


template <typename T>
void ref_wrapper_construct_aux(type_identity_t<T&>) noexcept { MSTL_NO_EVALUATION }
template <typename T>
void ref_wrapper_construct_aux(type_identity_t<T&&>) = delete;

template <typename, typename, typename = void>
struct ref_wrapper_constructable_from : false_type {};
template <typename T, typename U>
struct ref_wrapper_constructable_from<T, U, 
    void_t<decltype(MSTL::ref_wrapper_construct_aux<T>(MSTL::declval<U>()))>> : true_type {};
template <typename T, typename U>
constexpr bool ref_wrapper_constructable_from_v = ref_wrapper_constructable_from<T, U>::value;

template <typename T>
class reference_wrapper {
public:
    static_assert(is_object_v<T> || is_function_v<T>,
        "reference_wrapper<T> requires T to be an object type or a function type.");

    using type = T;

    template <typename U, enable_if_t<conjunction_v<negation<is_same<remove_cvref_t<U>,
        reference_wrapper>>, ref_wrapper_constructable_from<T, U>>, int> = 0>
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
    template <typename... Args>
    MSTL_CONSTEXPR decltype(auto) operator()(Args&&... args) const
        noexcept(noexcept((*ptr_)(MSTL::forward<Args>(args)...))) {
        return (*ptr_)(MSTL::forward<Args>(args)...);
    }
};
#if MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename T>
reference_wrapper(T&) -> reference_wrapper<T>;
#endif

template <typename T>
MSTL_NODISCARD MSTL_CONSTEXPR reference_wrapper<T> ref(T& val) noexcept {
    return reference_wrapper<T>(val);
}
template <typename T>
void ref(const T&&) = delete;

template <typename T>
MSTL_NODISCARD MSTL_CONSTEXPR reference_wrapper<T> ref(reference_wrapper<T> wrapper) noexcept {
    return wrapper;
}

template <typename T>
MSTL_NODISCARD MSTL_CONSTEXPR reference_wrapper<const T> cref(const T& val) noexcept {
    return reference_wrapper<const T>(val);
}
template <typename T>
void cref(const T&&) = delete;

template <typename T>
MSTL_NODISCARD MSTL_CONSTEXPR reference_wrapper<const T> cref(reference_wrapper<T> wrapper) noexcept {
    return wrapper;
}


template <typename T>
struct unwrap_reference {
    using type = T;
};
template <typename T>
struct unwrap_reference<reference_wrapper<T>> {
    using type = T&;
};
template <typename T>
using unwrap_reference_t = typename unwrap_reference<T>::type;


template <typename T>
using unwrap_ref_decay_t = unwrap_reference_t<decay_t<T>>;
template <typename T>
struct unwrap_ref_decay {
    using type = unwrap_ref_decay_t<T>;
};


template <typename>
struct get_first_parameter;
template <template <typename, typename...> typename T, typename First, typename... Rest>
struct get_first_parameter<T<First, Rest...>> {
    using type = First;
};


template <typename, typename = void>
struct get_ptr_difference_type {
    using type = ptrdiff_t;
};
template <typename T>
struct get_ptr_difference_type<T, void_t<typename T::difference_type>> {
    using type = typename T::difference_type;
};
template <typename T>
using get_ptr_difference_type_t = typename get_ptr_difference_type<T>::type;


template <typename, typename>
struct replace_first_parameter;
template <typename NewFirst, template <typename, typename...> typename T, typename First, typename... Rest>
struct replace_first_parameter<NewFirst, T<First, Rest...>> {
    using type = T<NewFirst, Rest...>;
};


template <typename T, typename U, typename = void>
struct get_rebind_type {
    using type = typename replace_first_parameter<U, T>::type;
};
template <typename T, typename U>
struct get_rebind_type<T, U, void_t<typename T::template rebind<U>>> {
    using type = typename T::template rebind<U>;
};
template <typename T, typename U>
using get_rebind_type_t = typename get_rebind_type<T, U>::type;


#ifdef MSTL_VERSION_20__
template <typename T>
concept is_allocator_v = requires(T& alloc) {
    alloc.deallocate(alloc.allocate(size_t{ 1 }), size_t{ 1 });
};
template <typename T>
struct is_allocator : bool_constant<is_allocator_v<T>> {};
#endif // MSTL_VERSION_20__


template <typename Alloc1, typename Alloc2>
constexpr bool allocable_from_v = is_same_v<
    decltype(MSTL::declval<Alloc1>().allocate(1)), decltype(MSTL::declval<Alloc1>().allocate(1))>;

template <typename Alloc1, typename Alloc2>
constexpr bool allocable_with = allocable_from_v<Alloc1, Alloc2>&& allocable_from_v<Alloc2, Alloc1>;


template <typename>
struct is_swappable;
template <typename>
struct is_nothrow_swappable;

template <typename T, enable_if_t<conjunction_v<is_move_constructible<T>, is_move_assignable<T>>, int> = 0>
MSTL_CONSTEXPR void swap(T&, T&) 
noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>);

template <typename T, size_t Size, enable_if_t<is_swappable<T>::value, int> = 0>
MSTL_CONSTEXPR void swap(T(&)[Size], T(&)[Size]) noexcept(is_nothrow_swappable<T>::value);

void swap() = delete;

template <typename T, typename U = T>
MSTL_CONSTEXPR T exchange(T&, U&&) noexcept(conjunction_v<
    is_nothrow_move_constructible<T>, is_nothrow_assignable<T&, U>>);


template <typename, typename, typename = void>
struct one_way_swappable : false_type {};

template <typename T1, typename T2>
struct one_way_swappable<T1, T2, void_t<decltype(
    MSTL::swap(MSTL::declval<T1>(), MSTL::declval<T2>()))>> : true_type {};
template <typename T1, typename T2>
struct is_swappable_with
    : bool_constant<conjunction_v<one_way_swappable<T1, T2>, one_way_swappable<T2, T1>>> {};
template <typename T1, typename T2>
constexpr bool is_swappable_with_v = is_swappable_with<T1, T2>::value;


template <typename T>
struct is_swappable 
    : bool_constant<is_swappable_with_v<add_lvalue_reference_t<T>, add_lvalue_reference_t<T>>> {};
template <typename T>
constexpr bool is_swappable_v = is_swappable<T>::value;


template <typename T1, typename T2>
struct nothrow_swappable_only : bool_constant<noexcept(MSTL::swap(MSTL::declval<T1>(), MSTL::declval<T2>()))
    && noexcept(MSTL::swap(MSTL::declval<T2>(), MSTL::declval<T1>()))> {};

template <typename T1, typename T2>
struct is_nothrow_swappable_with
    : bool_constant<conjunction_v<is_swappable_with<T1, T2>, nothrow_swappable_only<T1, T2>>> {};
template <typename T1, typename T2>
constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T1, T2>::value;


template <typename T>
struct is_nothrow_swappable
    : bool_constant<is_nothrow_swappable_with_v<add_lvalue_reference_t<T>, add_lvalue_reference_t<T>>> {};
template <typename T>
constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;


// ADL (Argument Dependent Lookup) is a function–lookup mechanism.
// specifically, when a function is called, in addition to the global and local scopes, 
// the compiler will also look up the function declaration in the namespace to which the argument type belongs.
template <typename, typename = void>
struct ADL_swappable : false_type {};
template <typename T>
struct ADL_swappable<T, void_t<decltype(MSTL::swap(MSTL::declval<T&>(), MSTL::declval<T&>()))>> : true_type {};

template <typename T>
constexpr bool is_trivially_swappable_v = conjunction_v<is_trivially_destructible<T>,
    is_trivially_move_constructible<T>, is_trivially_move_assignable<T>, negation<ADL_swappable<T>>>;
TEMNULL__
MSTL_INLINECSP constexpr bool is_trivially_swappable_v<byte_t> = true;

template <typename T>
struct is_trivially_swappable : bool_constant<is_trivially_swappable_v<T>> {};


#ifdef MSTL_VERSION_20__
#if defined(MSTL_COMPILE_CLANG__)
template <typename From, typename To>
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
template <typename From, typename To>
concept convertible_to = __is_convertible_to(From, To) && requires { static_cast<To>(MSTL::declval<From>()); };
#endif
template <typename From, typename To>
struct is_convertible_to : bool_constant<convertible_to<From, To>> {};
template <typename From, typename To>
constexpr bool is_convertible_to_v = is_convertible_to<From, To>::value;
#endif // MSTL_VERSION_20__


template <typename Iterator, typename Ptr, bool = is_pointer_v<remove_cvref_t<Iterator>>>
constexpr bool is_nothrow_arrow = is_nothrow_convertible_v<Iterator, Ptr>;
template <typename Iterator, typename Ptr>
constexpr bool is_nothrow_arrow<Iterator, Ptr, false> =
noexcept(MSTL::declcopy<Ptr>(MSTL::declval<Iterator>().operator->()));


template <typename Key>
struct hash {};

template <typename T>
struct hash<T*> {
    MSTL_NODISCARD constexpr size_t operator()(T* ptr) const noexcept {
        return reinterpret_cast<size_t>(ptr);
    }
};

#define INT_HASH_STRUCT__(OPT) \
    TEMNULL__ struct hash<OPT> { \
        MSTL_NODISCARD constexpr size_t operator ()(OPT x) const noexcept { return static_cast<size_t>(x); } \
    };
MSTL_MACRO_RANGE_CHARS(INT_HASH_STRUCT__)
MSTL_MACRO_RANGE_INT(INT_HASH_STRUCT__)


#if defined(MSTL_PLATFORM_WIN64__) || defined(MSTL_PLATFORM_LINUX64__)
static constexpr size_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
static constexpr size_t FNV_PRIME = 1099511628211ULL;
#elif defined(MSTL_PLATFORM_WIN32__) || defined(MSTL_PLATFORM_LINUX32__)
static constexpr size_t FNV_OFFSET_BASIS = 2166136261U;
static constexpr size_t FNV_PRIME = 16777619U;
#endif

// the FNV (Fowler-Noll-Vo) is a non-cryptographic hash algorithm
// with a good avalanche effect and a low collision rate.
// FNV_hash function is FNV-1a version.
constexpr size_t FNV_hash(const byte_t* first, size_t count) noexcept {
    size_t result = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < count; i++) {
        result ^= (size_t)first[i];
        result *= FNV_PRIME;
    }
    return result;
}

#define FLOAT_HASH_STRUCT__(OPT) \
    TEMNULL__ struct hash<OPT> { \
        MSTL_NODISCARD constexpr size_t operator ()(const OPT& x) const noexcept { \
            return x == 0.0f ? 0 : FNV_hash((const byte_t*)&x, sizeof(OPT)); \
        } \
    };
MSTL_MACRO_RANGE_FLOAT(FLOAT_HASH_STRUCT__)


template <typename, typename = void>
struct is_nothrow_hashable : false_type {};
template <typename Key>
struct is_nothrow_hashable<Key, void_t<decltype(MSTL::hash<Key>{}(MSTL::declval<const Key&>()))>>
    : bool_constant<noexcept(MSTL::hash<Key>{}(MSTL::declval<const Key&>()))> {};
template <typename Key>
constexpr bool is_nothrow_hashable_v = is_nothrow_hashable<Key>::value;


#ifdef MSTL_VERSION_20__
template <typename Func, typename Arg>
concept is_hash_v = requires(Func f, Arg a) {
    { f(a) } -> convertible_to<size_t>;
};

template <typename T>
concept is_pair_v = requires(T p) {
    typename T::first_type;
    typename T::second_type;
    p.first;
    p.second;
};
#endif // MSTL_VERSION_20__


template <typename, typename = void>
struct iterator_traits_base {};

template <typename Iterator>
struct iterator_traits_base<Iterator,
    void_t<typename Iterator::iterator_category, typename Iterator::value_type,
    typename Iterator::difference_type, typename Iterator::pointer, typename Iterator::reference>>
{
    using iterator_category = typename Iterator::iterator_category;
    using value_type        = typename Iterator::value_type;
    using difference_type   = typename Iterator::difference_type;
    using pointer           = typename Iterator::pointer;
    using reference         = typename Iterator::reference;
};

template <typename Iterator>
struct iterator_traits : public iterator_traits_base<Iterator> {};

template <typename T>
struct iterator_traits<T*> {
    static_assert(is_object_v<T>, "iterator traits requires object types.");

#ifdef MSTL_VERSION_20__
    using iterator_category = std::contiguous_iterator_tag;
#else
    using iterator_category = std::random_access_iterator_tag;
#endif // MSTL_VERSION_20__
    using value_type        = remove_cv_t<T>;
    using difference_type   = ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;
};

template <typename T>
struct iterator_traits<const T*> {
    static_assert(is_object_v<T>, "iterator traits requires object types.");

#ifdef MSTL_VERSION_20__
    using iterator_category = std::contiguous_iterator_tag;
#else
    using iterator_category = std::random_access_iterator_tag;
#endif // MSTL_VERSION_20__
    using value_type        = remove_cv_t<T>;
    using difference_type   = ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;
};

template <typename Iterator>
using iter_cat_t = typename iterator_traits<Iterator>::iterator_category;
template <typename Iterator>
using iter_val_t = typename iterator_traits<Iterator>::value_type;
template <typename Iterator>
using iter_dif_t = typename iterator_traits<Iterator>::difference_type;
template <typename Iterator>
using iter_ptr_t = typename iterator_traits<Iterator>::pointer;
template <typename Iterator>
using iter_ref_t = typename iterator_traits<Iterator>::reference;

MSTL_END_NAMESPACE__
#endif // MSTL_TYPE_TRAITS_HPP__
