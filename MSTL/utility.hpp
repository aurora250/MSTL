#ifndef MSTL_UTILITY_HPP__
#define MSTL_UTILITY_HPP__
#include "type_traits.hpp"
MSTL_BEGIN_NAMESPACE__

template <class T, T... Values>
struct integer_sequence {
    static_assert(is_integral_v<T>, "integer sequence requires integral types.");

    using value_type = T;

    MSTL_NODISCARD static MSTL_CONSTEXPR size_t size() noexcept {
        return sizeof...(Values);
    }
};
template <class T, T Size>
using make_integer_sequence = __make_integer_seq<integer_sequence, T, Size>;
template <size_t... Values>
using index_sequence = integer_sequence<size_t, Values...>;
template <size_t Size>
using make_index_sequence = make_integer_sequence<size_t, Size>;
template <class... Types>
using index_sequence_for = make_index_sequence<sizeof...(Types)>;


template <class T, size_t Size> requires(is_swappable_v<T>)
MSTL_CONSTEXPR void swap(T(& lh)[Size], T(& rh)[Size]) noexcept(is_nothrow_swappable_v<T>) {
    if (&lh != &rh) {
        T* first1 = lh;
        T* last1 = first1 + Size;
        T* first2 = rh;
        for (; first1 != last1; ++first1, ++first2) {
            MSTL::swap(*first1, *first2);
        }
    }
}

template <typename T>
    requires(conjunction_v<is_move_constructible<T>, is_move_assignable<T>>)
MSTL_CONSTEXPR void swap(T& lh, T& rh)
noexcept(is_nothrow_move_constructible_v<T>&& is_nothrow_move_assignable_v<T>) {
    T tmp = std::move(lh);
    lh = std::move(rh);
    rh = std::move(tmp);
}



MSTL_END_NAMESPACE__
#endif // MSTL_UTILITY_HPP__
