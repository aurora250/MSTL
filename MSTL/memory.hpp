#ifndef MSTL_MEMORY_HPP__
#define MSTL_MEMORY_HPP__
#include "allocator.hpp"
#include "algobase.hpp"
#include "errorlib.h"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__

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


template <typename Iterator, typename T = typename std::iterator_traits<Iterator>::value_type>
    requires(ForwardIterator<Iterator>)
class temporary_buffer {
private:
    ptrdiff_t original_len_;
    ptrdiff_t len_;
    T* buffer_;

    void allocate_buffer() {
        original_len_ = len_;
        buffer_ = 0;
        if (len_ > (ptrdiff_t)(INT_MAX / sizeof(T))) len_ = INT_MAX / sizeof(T);
        while (len_ > 0) {
            buffer_ = (T*)std::malloc(len_ * sizeof(T));
            if (buffer_) break;
            len_ /= 2;
        }
    }
    void initialize_buffer(const T&) requires(TrivialCopyAssignable<T>) {}
    void initialize_buffer(const T& val) requires(!TrivialCopyAssignable<T>) {
        MSTL::uninitialized_fill_n(buffer_, len_, val);
    }

public:
    ptrdiff_t size() const { return len_; }
    ptrdiff_t requested_size() const { return original_len_; }
    T* begin() { return buffer_; }
    T* end() { return buffer_ + len_; }

    temporary_buffer(const temporary_buffer&) = delete;
    void operator =(const temporary_buffer&) = delete;
    temporary_buffer(Iterator first, Iterator last) {
        MSTL_TRY__{
            len_ = MSTL::distance(first, last);
            allocate_buffer();
            if (len_ > 0) initialize_buffer(*first);
        }
        MSTL_CATCH_UNWIND_THROW_M__(std::free(buffer_); buffer_ = 0; len_ = 0);
    }
    ~temporary_buffer() {
        MSTL::destroy(buffer_, buffer_ + len_);
        std::free(buffer_);
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_MEMORY_HPP__
