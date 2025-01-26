#ifndef MSTL_MEMORY_HPP__
#define MSTL_MEMORY_HPP__
#include "algobase.hpp"
#include "errorlib.h"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

template <typename T>
    requires(NVoidT<T>)
class standard_allocator {
private:
    typedef std::allocator<T> alloc_type;
    alloc_type alloc;
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using self = standard_allocator<T>;

    template <typename U>
    struct rebind {
        using other = standard_allocator<U>;
    };

    MSTL_CONSTEXPR standard_allocator() noexcept = default;
    template <typename U>
    MSTL_CONSTEXPR standard_allocator(const standard_allocator<U>&) noexcept {}
    MSTL_CONSTEXPR ~standard_allocator() noexcept = default;
    MSTL_CONSTEXPR self& operator =(const self&) noexcept = default;

    MSTL_NODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(const size_type n) {
        return 0 == n ? 0 : alloc.allocate(n * sizeof(value_type));
    }
    MSTL_NODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(void) {
        return alloc.allocate(sizeof(value_type));
    }
    MSTL_CONSTEXPR void deallocate(pointer p) noexcept {
        alloc.deallocate(p, sizeof(value_type));
    }
    MSTL_CONSTEXPR void deallocate(pointer p, const size_type n) noexcept {
        alloc.deallocate(p, n * sizeof(value_type));
    }
};
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(
    const standard_allocator<T>& lh, const standard_allocator<U>& rh) noexcept {
    return lh.alloc == rh.alloc;
}
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(
    const standard_allocator<T>& lh, const standard_allocator<U>& rh) noexcept {
    return lh.alloc != rh.alloc;
}

template <typename T>
    requires(NVoidT<T>)
class ctype_allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using self = ctype_allocator<T>;

    template <typename U>
    struct rebind {
        using other = ctype_allocator<U>;
    };

    MSTL_CONSTEXPR ctype_allocator() noexcept = default;
    template <typename U>
    MSTL_CONSTEXPR ctype_allocator(const ctype_allocator<U>&) noexcept {}
    MSTL_CONSTEXPR ~ctype_allocator() noexcept = default;
    MSTL_CONSTEXPR self& operator =(const self&) noexcept = default;

    MSTL_NODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(const size_type n) {
        return 0 == n ? 0 : static_cast<pointer>(std::malloc(n * sizeof(T)));
    }
    MSTL_NODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(void) {
        return static_cast<pointer>(std::malloc(sizeof(T)));
    }
    MSTL_CONSTEXPR void deallocate(pointer p) noexcept {
        std::free(p);
    }
    MSTL_CONSTEXPR void deallocate(pointer p, const size_type) noexcept {
        std::free(p);
    }
};
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(const ctype_allocator<T>&, const ctype_allocator<U>&) noexcept {
    return true;
}
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(const ctype_allocator<T>&, const ctype_allocator<U>&) noexcept {
    return false;
}

template <typename T>
    requires(NVoidT<T>)
class new_allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using self = new_allocator<T>;

    template <typename U>
    struct rebind {
        using other = new_allocator<U>;
    };

    MSTL_CONSTEXPR new_allocator() noexcept = default;
    template <typename U>
    MSTL_CONSTEXPR new_allocator(const new_allocator<U>&) noexcept {}
    MSTL_CONSTEXPR ~new_allocator() noexcept = default;
    MSTL_CONSTEXPR self& operator =(const self&) noexcept = default;

    MSTL_NODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(size_type n) {
        return 0 == n ? 0 : static_cast<pointer>(::operator new(n * sizeof(T)));
    }
    MSTL_NODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(void) {
        return static_cast<pointer>(::operator new(sizeof(T)));
    }
    MSTL_CONSTEXPR void deallocate(pointer p) noexcept {
        ::operator delete(p);
    }
    MSTL_CONSTEXPR void deallocate(pointer p, const size_type) noexcept {
        ::operator delete(p);
    }
};
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(const new_allocator<T>&, const new_allocator<U>&) noexcept {
    return true;
}
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(const new_allocator<T>&, const new_allocator<U>&) noexcept {
    return false;
}

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
