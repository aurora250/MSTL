#ifndef MSTL_MEMORY_HPP__
#define MSTL_MEMORY_HPP__
#include "algobase.hpp"
#include "errorlib.h"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

template <typename T>
class default_standard_alloc {
private:
    typedef std::allocator<T> alloc_type;
    alloc_type alloc;
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef size_t      size_type;

    pointer allocate(const size_type n) {
        return 0 == n ? 0 : (pointer)alloc.allocate(n * sizeof(value_type));
    }
    pointer allocate(void) {
        return (pointer)alloc.allocate(sizeof(value_type));
    }
    void deallocate(pointer const p) noexcept {
        alloc.deallocate(p, sizeof(value_type));
    }
    void deallocate(pointer const p, const size_type n) noexcept {
        if (n != 0)alloc.deallocate(p, n * sizeof(value_type));
    }
};

template <typename T1, typename... T2>
    requires(ConstructibleFrom<T1, T2...>)
MSTL_CONSTEXPR void construct(T1* p, T2&&... value) { // placement new
    new (p) T1(std::forward<T2>(value)...);
}
template <typename T>
MSTL_CONSTEXPR void destroy(T* pointer) noexcept(NothrowDestructable<T>) {
    pointer->~T();
}
template <typename Iterator>
    requires(ForwardIterator<Iterator> && (!TrivialDestructible<
        typename std::iterator_traits<Iterator>::value_type>))
MSTL_CONSTEXPR void destroy(Iterator first, Iterator last) 
noexcept(NothrowDestructable<typename std::iterator_traits<Iterator>::value_type>) {
    for (; first < last; ++first) MSTL::destroy(&*first);
}
template <typename Iterator>
    requires(ForwardIterator<Iterator> && TrivialDestructible<
        typename std::iterator_traits<Iterator>::value_type>)
MSTL_CONSTEXPR void destroy(Iterator, Iterator) noexcept {}

inline void destroy(char*, char*) noexcept {}
inline void destroy(wchar_t*, wchar_t*) noexcept {}

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
