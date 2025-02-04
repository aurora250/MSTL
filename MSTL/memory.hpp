#ifndef MSTL_MEMORY_HPP__
#define MSTL_MEMORY_HPP__
#include "allocator.hpp"
#include "algobase.hpp"
#include "errorlib.h"
#include "concepts.hpp"
#include "iterator_traits.hpp"
#include "macro_ranges.h"
MSTL_BEGIN_NAMESPACE__

// placement new
template <typename T1, typename... T2>
    requires(constructible_from<T1, T2...>)
MSTL_CONSTEXPR void construct(T1* p, T2&&... value) 
noexcept(noexcept(new (p) T1(MSTL::forward<T2>(value)...))) {
    new (p) T1(MSTL::forward<T2>(value)...);
}


template <typename T>
MSTL_CONSTEXPR void destroy(T* pointer) noexcept(is_nothrow_destructible_v<T>) {
    pointer->~T();
}

template <typename Iterator>
    requires(forward_iterator<Iterator> && (!trivially_destructible<typename iterator_traits<Iterator>::value_type>))
MSTL_CONSTEXPR void destroy(Iterator first, Iterator last) 
noexcept(is_nothrow_destructible_v<typename iterator_traits<Iterator>::value_type>) {
    for (; first < last; ++first) MSTL::destroy(&*first);
}

template <typename Iterator>
    requires(forward_iterator<Iterator> && trivially_destructible<typename iterator_traits<Iterator>::value_type>)
MSTL_CONSTEXPR void destroy(Iterator, Iterator) noexcept {}

#define DESTORY_CHAR_FUNCTION__(OPT) \
    inline void destroy(OPT*, OPT*) noexcept {}
MSTL_MACRO_RANGE_CHARS(DESTORY_CHAR_FUNCTION__)


template <typename Iterator1, typename Iterator2>
    requires(input_iterator<Iterator1> && forward_iterator<Iterator2> && trivially_copy_assignable<
        typename iterator_traits<Iterator1>::value_type>)
MSTL_CONSTEXPR Iterator2 uninitialized_copy(Iterator1 first, Iterator1 last, Iterator2 result) {
    return MSTL::copy(first, last, result);
}

template <typename Iterator1, typename Iterator2>
    requires(input_iterator<Iterator1>&& forward_iterator<Iterator2> && (!trivially_copy_assignable<
        typename iterator_traits<Iterator1>::value_type>))
MSTL_CONSTEXPR Iterator2 uninitialized_copy(Iterator1 first, Iterator1 last, Iterator2 result) {
    Iterator2 cur = result;
    MSTL_TRY__{
    for (; first != last; ++first, ++cur)
        MSTL::construct(&*cur, *first); 
    }
    MSTL_CATCH_UNWIND_THROW_M__(for (; result != cur; --cur) MSTL::destroy(&*cur); )
    return cur;
}

#define UNINITCOPY_CHAR_FUNCTION__(OPT) \
inline OPT* uninitialized_copy(const OPT* first, const OPT* last, OPT* result) { \
    memmove(result, first, (int)(sizeof(OPT) * (last - first))); \
    return result + (last - first); \
}
MSTL_MACRO_RANGE_CHARS(UNINITCOPY_CHAR_FUNCTION__)


template <typename Iterator1, typename Iterator2>
    requires(input_iterator<Iterator1> && forward_iterator<Iterator2>)
pair<Iterator1, Iterator2> 
MSTL_CONSTEXPR uninitialized_copy_n(Iterator1 first, size_t count, Iterator2 result) {
    Iterator2 cur = result;
    MSTL_TRY__{
    for (; count > 0; --count, ++first, ++cur)
        MSTL::construct(&*cur, *first); 
    }
    MSTL_CATCH_UNWIND_THROW_M__(for (; result != cur; --cur) MSTL::destroy(&*cur); )
    return pair<Iterator1, Iterator2>(first, cur);
}

template <typename Iterator1, typename Iterator2>
    requires(random_access_iterator<Iterator1>&& forward_iterator<Iterator2>)
inline pair<Iterator1, Iterator2>
MSTL_CONSTEXPR uninitialized_copy_n(Iterator1 first, size_t count, Iterator2 result) {
    Iterator1 last = first + count;
    return MSTL::make_pair(last, MSTL::uninitialized_copy(first, last, result));
}


template <typename Iterator, typename T>
    requires(forward_iterator<Iterator> && trivially_assignable<typename iterator_traits<Iterator>::value_type>)
MSTL_CONSTEXPR void uninitialized_fill(Iterator first, Iterator last, const T& x) {
    MSTL::fill(first, last, x);
}

template <typename Iterator, typename T>
    requires(forward_iterator<Iterator> && (!trivially_assignable<typename iterator_traits<Iterator>::value_type>))
MSTL_CONSTEXPR void uninitialized_fill(Iterator first, Iterator last, const T& x) {
    Iterator cur = first;
    for (; cur != last; ++cur)
        MSTL::construct(&*cur, x);
}


template <typename Iterator, typename T>
    requires(forward_iterator<Iterator> && trivially_assignable<typename iterator_traits<Iterator>::value_type>)
MSTL_CONSTEXPR Iterator uninitialized_fill_n(Iterator first, size_t n, const T& x) {
    return MSTL::fill_n(first, n, x);
}

template <typename Iterator, typename T>
    requires(forward_iterator<Iterator> && (!trivially_assignable<typename iterator_traits<Iterator>::value_type>))
MSTL_CONSTEXPR Iterator uninitialized_fill_n(Iterator first, size_t n, const T& x) {
    Iterator cur = first;
    for (; n > 0; --n, ++cur) 
        MSTL::construct(&*cur, x);
    return cur;
}


template <typename Iterator1, typename Iterator2>
    requires input_iterator<Iterator1>&& forward_iterator<Iterator2>&&
is_trivially_move_assignable_v<typename iterator_traits<Iterator1>::value_type>
MSTL_CONSTEXPR Iterator2 uninitialized_move(Iterator1 first, Iterator1 last, Iterator2 result) {
    return MSTL::move(first, last, result);
}

template <typename Iterator1, typename Iterator2>
    requires input_iterator<Iterator1>&& forward_iterator<Iterator2> &&
(!is_trivially_move_assignable_v<typename iterator_traits<Iterator1>::value_type>)
MSTL_CONSTEXPR Iterator2 uninitialized_move(Iterator1 first, Iterator1 last, Iterator2 result) {
    Iterator2 cur = result;
    MSTL_TRY__{
        for (; first != last; ++first, ++cur) 
            MSTL::construct(&*cur, MSTL::move(*first));
    }
    MSTL_CATCH_UNWIND_THROW_M__(for (; result != cur; --cur) MSTL::destroy(&*cur);)
    return cur;
}

#define UNINITMOVE_CHAR_FUNCTION__(OPT) \
inline OPT* uninitialized_move(const OPT* first, const OPT* last, OPT* result) { \
    MSTL::memmove(result, first, (int)(sizeof(OPT) * (last - first))); \
    return result + (last - first); \
}
MSTL_MACRO_RANGE_CHARS(UNINITMOVE_CHAR_FUNCTION__)


template <typename Iterator1, typename Iterator2>
    requires input_iterator<Iterator1>&& forward_iterator<Iterator2>
pair<Iterator1, Iterator2>
MSTL_CONSTEXPR uninitialized_move_n(Iterator1 first, size_t count, Iterator2 result) {
    Iterator2 cur = result;
    MSTL_TRY__{
        for (; count > 0; --count, ++first, ++cur) 
            MSTL::construct(&*cur, MSTL::move(*first));
    }
    MSTL_CATCH_UNWIND_THROW_M__(for (; result != cur; --cur) MSTL::destroy(&*cur); )
    return pair<Iterator1, Iterator2>(first, cur);
}

template <typename Iterator1, typename Iterator2>
    requires random_access_iterator<Iterator1>&& forward_iterator<Iterator2>
inline pair<Iterator1, Iterator2>
MSTL_CONSTEXPR uninitialized_move_n(Iterator1 first, size_t count, Iterator2 result) {
    Iterator1 last = first + count;
    return MSTL::make_pair(last, MSTL::uninitialized_move(first, last, result));
}


template <typename Iterator, typename T = typename iterator_traits<Iterator>::value_type>
    requires(forward_iterator<Iterator>)
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
    void initialize_buffer(const T&) requires(is_trivially_copy_assignable_v<T>) {}
    void initialize_buffer(const T& val) requires(!is_trivially_copy_assignable_v<T>) {
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
