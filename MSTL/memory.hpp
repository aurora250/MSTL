#ifndef MSTL_MEMORY_HPP__
#define MSTL_MEMORY_HPP__
#include "algobase.hpp"
#include "errorlib.h"
#include "alloc.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

template <typename T1, typename... T2>
    requires(ConstructibleFrom<T1, T2...>)
MSTL_CONSTEXPR void construct(T1* p, T2&&... value) {
    new (p) T1(std::forward<T2>(value)...);
}
template <typename T>
MSTL_CONSTEXPR void destroy(T* pointer) {
    pointer->~T();
}
template <typename Iterator>
    requires(ForwardIterator<Iterator> && (!TrivialDestructible<
        typename std::iterator_traits<Iterator>::value_type>))
MSTL_CONSTEXPR void destroy(Iterator first, Iterator last) {
    for (; first < last; ++first) MSTL::destroy(&*first);
}
template <typename Iterator>
    requires(ForwardIterator<Iterator> && TrivialDestructible<
        typename std::iterator_traits<Iterator>::value_type>)
MSTL_CONSTEXPR void destroy(Iterator, Iterator) {}

inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}

template <typename Iterator1, typename Iterator2>
    requires(InputIterator<Iterator1> && ForwardIterator<Iterator2> && TrivialCopyAssignable<
        typename std::iterator_traits<Iterator1>::value_type>)
MSTL_CONSTEXPR Iterator2 uninitialized_copy(Iterator1 first, Iterator1 last, Iterator2 result) {
    return MSTL::copy(first, last, result);
}
template <typename Iterator1, typename Iterator2>
    requires(InputIterator<Iterator1>&& ForwardIterator<Iterator2> && (!TrivialCopyAssignable<
        typename std::iterator_traits<Iterator1>::value_type>))
MSTL_CONSTEXPR Iterator2 uninitialized_copy(Iterator1 first, Iterator1 last, Iterator2 result) {
    Iterator2 cur = result;
    for (; first != last; ++first, ++cur) 
        MSTL::construct(&*cur, *first);
    return cur;
}
inline char* uninitialized_copy(const char* first, const char* last, char* result) {
    memmove(result, first, (int)(last - first));
    return result + (last - first);
}
inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    memmove(result, first, (int)(sizeof(wchar_t) * (last - first)));
    return result + (last - first);
}

template <typename Iterator1, typename Iterator2>
    requires(InputIterator<Iterator1> && ForwardIterator<Iterator2>)
pair<Iterator1, Iterator2> 
MSTL_CONSTEXPR uninitialized_copy_n(Iterator1 first, size_t count, Iterator2 result) {
    Iterator2 cur = result;
    for (; count > 0; --count, ++first, ++cur)
        MSTL::construct(&*cur, *first);
    return pair<Iterator1, Iterator2>(first, cur);
}
template <typename Iterator1, typename Iterator2>
    requires(RandomAccessIterator<Iterator1>&& ForwardIterator<Iterator2>)
inline pair<Iterator1, Iterator2> 
MSTL_CONSTEXPR uninitialized_copy_n(Iterator1 first, size_t count, Iterator2 result) {
    Iterator1 last = first + count;
    return MSTL::make_pair(last, MSTL::uninitialized_copy(first, last, result));
}

template <typename Iterator, typename T>
    requires(ForwardIterator<Iterator> && TrivialAssignable<typename std::iterator_traits<Iterator>::value_type>)
MSTL_CONSTEXPR void uninitialized_fill(Iterator first, Iterator last, T&& x) {
    MSTL::fill(first, last, std::forward<T>(x));
}
template <typename Iterator, typename T>
    requires(ForwardIterator<Iterator> && (!TrivialAssignable<typename std::iterator_traits<Iterator>::value_type>))
MSTL_CONSTEXPR void uninitialized_fill(Iterator first, Iterator last, T&& x) {
    Iterator cur = first;
    for (; cur != last; ++cur)
        MSTL::construct(&*cur, std::forward<T>(x));
}

template <typename Iterator, typename T>
    requires(ForwardIterator<Iterator> && TrivialAssignable<typename std::iterator_traits<Iterator>::value_type>)
MSTL_CONSTEXPR Iterator uninitialized_fill_n(Iterator first, size_t n, T&& x) {
    return MSTL::fill_n(first, n, std::forward<T>(x));
}
template <typename Iterator, typename T>
    requires(ForwardIterator<Iterator> && (!TrivialAssignable<typename std::iterator_traits<Iterator>::value_type>))
MSTL_CONSTEXPR Iterator uninitialized_fill_n(Iterator first, size_t n, T&& x) {
    Iterator cur = first;
    for (; n > 0; --n, ++cur) 
        MSTL::construct(&*cur, std::forward<T>(x));
    return cur;
}
MSTL_END_NAMESPACE__

#endif // MSTL_MEMORY_HPP__
