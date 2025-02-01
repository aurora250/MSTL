#ifndef MSTL_ITERATOR_TRAITS_HPP__
#define MSTL_ITERATOR_TRAITS_HPP__
#include "type_traits.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Ptr, typename Elem>
struct ptr_traits_base {
    using pointer = Ptr;
    using element_type = Elem;
    using difference_type = get_ptr_difference_type_t<Ptr>;
    using reference = conditional_t<is_void_v<Elem>, char, Elem>&;

    template <typename U>
    using rebind = typename get_rebind_type<Ptr, U>::type;

    MSTL_NODISCARD static MSTL_CONSTEXPR pointer pointer_to(reference x) 
        noexcept(noexcept(Ptr::pointer_to(x))) {
        return Ptr::pointer_to(x);
    }
};

template <typename, typename = void, typename = void>
struct ptr_traits_extract {};

template <typename T, typename U>
struct ptr_traits_extract<T, U, void_t<typename get_first_parameter<T>::type>>
    : ptr_traits_base<T, typename get_first_parameter<T>::type> {};

template <typename T>
struct ptr_traits_extract<T, void_t<typename T::element_type>, void>
    : ptr_traits_base<T, typename T::element_type> {};

template <typename T>
struct pointer_traits : ptr_traits_extract<T> {};

template <typename T>
struct pointer_traits<T*> {
    using pointer = T*;
    using element_type = T;
    using difference_type = ptrdiff_t;
    using reference = conditional_t<is_void_v<T>, char, T>&;

    template <typename U>
    using rebind = U*;

    MSTL_NODISCARD static MSTL_CONSTEXPR pointer pointer_to(reference x) noexcept {
        return MSTL::addressof(x);
    }
};


template <typename T>
concept support_ptr_address = requires(const T & x) {
    typename pointer_traits<T>;
    pointer_traits<T>::to_address(x);
};


template <typename T>
MSTL_NODISCARD constexpr T* to_address(T* const x) noexcept {
    static_assert(!is_function_v<T>, "to address don`t support function types.");
    return x;
}
template <typename Ptr>
MSTL_NODISCARD constexpr decltype(auto) to_address(const Ptr& x) noexcept {
    if constexpr (support_ptr_address<Ptr>) {
        return pointer_traits<Ptr>::to_address(x);
    }
    else {
        return MSTL::to_address(x.operator->());
    }
}


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
    requires(is_object_v<T>)
struct iterator_traits<T*> {
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = remove_cv_t<T>;
    using difference_type   = ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;
};

template <typename Iterator>
using iterator_traits_cate_t = typename iterator_traits<Iterator>::iterator_category;
template <typename Iterator>
using iterator_traits_val_t = typename iterator_traits<Iterator>::value_type;
template <typename Iterator>
using iterator_traits_diff_t = typename iterator_traits<Iterator>::difference_type;
template <typename Iterator>
using iterator_traits_ptr_t = typename iterator_traits<Iterator>::pointer;
template <typename Iterator>
using iterator_traits_ref_t = typename iterator_traits<Iterator>::reference;


MSTL_END_NAMESPACE__
#endif // MSTL_ITERATOR_TRAITS_HPP__