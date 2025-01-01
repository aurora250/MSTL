#ifndef MSTL_MEMORY_HPP__
#define MSTL_MEMORY_HPP__
#include "type_traits.hpp"
#include "algobase.hpp"
#include "errorlib.h"
#include "alloc.hpp"

MSTL_BEGIN_NAMESPACE__

template <typename T>
inline void destroy(T* pointer) {
    pointer->~T();
}
template <typename T1, typename... T2>
inline void construct(T1* p, T2&&... value) {
    new (p) T1(std::forward<T2>(value)...);
}
template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
    for (; first < last; ++first) destroy(&*first);
}
template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}
template <typename ForwardIterator, typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}
template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    __destroy(first, last, value_type(first));
}
inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
                                                ForwardIterator result, __true_type) {
    return MSTL::copy(first, last, result);
}
template <typename InputIterator, typename ForwardIterator>
ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
                                         ForwardIterator result, __false_type) {
    ForwardIterator cur = result;
    MSTL_TRY__{
        for (; first != last; ++first, ++cur) construct(&*cur, *first);
        return cur;
    }
    MSTL_CATCH_UNWIND_THROW_U__(destroy(result, cur));
}
template <typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last,
                                            ForwardIterator result, T*) {
    using is_POD = typename __type_traits<T>::is_POD_type;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}
template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                          ForwardIterator result) {
    return __uninitialized_copy(first, last, result, value_type(result));
}
inline char* uninitialized_copy(const char* first, const char* last, char* result) {
    memmove(result, first, (int)(last - first));
    return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    memmove(result, first, (int)(sizeof(wchar_t) * (last - first)));
    return result + (last - first);
}

template <typename InputIterator, typename Size, typename ForwardIterator>
pair<InputIterator, ForwardIterator>
__uninitialized_copy_n(InputIterator first, Size count, ForwardIterator result, std::input_iterator_tag) {
    ForwardIterator cur = result;
    MSTL_TRY__{
        for (; count > 0; --count, ++first, ++cur)
            construct(&*cur, *first);
        return pair<InputIterator, ForwardIterator>(first, cur);
    }
    MSTL_CATCH_UNWIND_THROW_U__(destroy(result, cur));
}
template <typename RandomAccessIterator, typename Size, typename ForwardIterator>
inline pair<RandomAccessIterator, ForwardIterator>
__uninitialized_copy_n(RandomAccessIterator first, Size count,
    ForwardIterator result, std::random_access_iterator_tag) {
    RandomAccessIterator last = first + count;
    return make_pair(last, uninitialized_copy(first, last, result));
}

template <typename InputIterator, typename Size, typename ForwardIterator>
inline pair<InputIterator, ForwardIterator>
uninitialized_copy_n(InputIterator first, Size count, ForwardIterator result) {
    return __uninitialized_copy_n(first, count, result, iterator_category(first));
}

template <typename ForwardIterator, typename T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, T&& x, __true_type) {
    fill(first, last, std::forward<T>(x));
}
template <typename ForwardIterator, typename T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, T&& x, __false_type) {
    ForwardIterator cur = first;
    MSTL_TRY__{
        for (; cur != last; ++cur) construct(&*cur, std::forward<T>(x));
    }
    MSTL_CATCH_UNWIND_THROW_U__(destroy(first, cur));
}
template <typename ForwardIterator, typename T, typename V>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, T&& x, V*) {
    using is_POD = typename __type_traits<V>::is_POD_type;
    __uninitialized_fill_aux(first, last, std::forward<T>(x), is_POD());
}
template <typename ForwardIterator, typename T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, T&& x) {
    __uninitialized_fill(first, last, std::forward<T>(x), value_type(first));
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, T&& x, __true_type) {
    return MSTL::fill_n(first, n, std::forward<T>(x));
}
template <typename ForwardIterator, typename Size, typename T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, T&& x, __false_type) {
    ForwardIterator cur = first;
    MSTL_TRY__{
        for (; n > 0; --n, ++cur) MSTL::construct(&*cur, std::forward<T>(x));
        return cur;
    }
    MSTL_CATCH_UNWIND_THROW_U__(MSTL::destroy(first, cur));
}
template <typename ForwardIterator, typename Size, typename T, typename V>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, T&& x, V*) {
    using is_POD = typename __type_traits<T>::is_POD_type;
    return __uninitialized_fill_n_aux(first, n, std::forward<T>(x), is_POD());
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, T&& x) {
    return __uninitialized_fill_n(first, n, std::forward<T>(x), value_type(first));
}

template <typename InputIterator1, typename InputIterator2, typename ForwardIterator>
inline ForwardIterator __uninitialized_copy_copy(InputIterator1 first1, InputIterator1 last1,
    InputIterator2 first2, InputIterator2 last2,
    ForwardIterator result) {
    ForwardIterator mid = uninitialized_copy(first1, last1, result);
    MSTL_TRY__{
        return uninitialized_copy(first2, last2, mid);
    }
    MSTL_CATCH_UNWIND_THROW_U__(MSTL::destroy(result, mid));
}

template <typename ForwardIterator, typename T, typename InputIterator>
inline ForwardIterator __uninitialized_fill_copy(ForwardIterator result, ForwardIterator mid, const T& x,
    InputIterator first, InputIterator last) {
    uninitialized_fill(result, mid, x);
    MSTL_TRY__{
        return uninitialized_copy(first, last, mid);
    }
    MSTL_CATCH_UNWIND_THROW_U__(MSTL::destroy(result, mid));
}

template <typename InputIterator, typename ForwardIterator, typename T>
inline void __uninitialized_copy_fill(InputIterator first1, InputIterator last1,
    ForwardIterator first2, ForwardIterator last2, const T& x) {
    ForwardIterator mid2 = uninitialized_copy(first1, last1, first2);
    MSTL_TRY__{
        uninitialized_fill(mid2, last2, x);
    }
    MSTL_CATCH_UNWIND_THROW_U__(destroy(first2, mid2));
}
MSTL_END_NAMESPACE__

#endif // MSTL_MEMORY_HPP__
