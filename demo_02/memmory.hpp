#ifndef ALLOC_H
#define ALLOC_H
#include "iterator.hpp"
#include "algobase.hpp"

namespace MSTL {
    template <class T>
    inline void destroy(T* pointer) {
        pointer->~T();
    }
    template <class T1, class T2>
    inline void construct(T1* p, const T2& value) {
        new (p) T1(value);
    }
    template <class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
        for (; first < last; ++first) destroy(&*first);
    }
    template <class ForwardIterator>
    inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}
    template <class ForwardIterator, class T>
    inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
        typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
        __destroy_aux(first, last, trivial_destructor());
    }
    template <class ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last) {
        __destroy(first, last, value_type(first));
    }
    inline void destroy(char*, char*) {}
    inline void destroy(wchar_t*, wchar_t*) {}

    template <typename T>
    class simple_alloc {
    public:
        std::allocator<T> alloc;
        ~simple_alloc() = default;
        T* allocate(const size_t n) {
            return 0 == n ? 0 : (T*)alloc.allocate(n * sizeof(T));
        }
        T* allocate(void) {
            return (T*)alloc.allocate(sizeof(T));
        }
        void deallocate(T* const p, const size_t n) {
            if (0 != n)alloc.deallocate(p, n * sizeof(T));
        }
        void deallocate(T* const p) {
            alloc.deallocate(p, sizeof(T));
        }
    };

    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
                                                    ForwardIterator result,  __true_type) {
        return copy(first, last, result);
    }
    template <class InputIterator, class ForwardIterator>
    ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
                                             ForwardIterator result, __false_type) {
        ForwardIterator cur = result;
        try {
            for (; first != last; ++first, ++cur)
                construct(&*cur, *first);
            return cur;
        }
        catch (...) { 
            destroy(result, cur);
        }
    }
    template <class InputIterator, class ForwardIterator, class T>
    inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last,
                                                ForwardIterator result, T*) {
        using is_POD = __type_traits<T>::is_POD_type;
        return __uninitialized_copy_aux(first, last, result, is_POD());
    }
    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                              ForwardIterator result) {
        return __uninitialized_copy(first, last, result, value_type(result));
    }
    inline char* uninitialized_copy(const char* first, const char* last, char* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }

    template <class InputIterator, class Size, class ForwardIterator>
    pair<InputIterator, ForwardIterator> 
        __uninitialized_copy_n(InputIterator first, Size count,
                               ForwardIterator result, std::input_iterator_tag) {
        ForwardIterator cur = result;
        try {
            for (; count > 0; --count, ++first, ++cur)
                construct(&*cur, *first);
            return pair<InputIterator, ForwardIterator>(first, cur);
        }
        catch (...) {
            destroy(result, cur);
        }
    }
    template <class RandomAccessIterator, class Size, class ForwardIterator>
    inline pair<RandomAccessIterator, ForwardIterator>
        __uninitialized_copy_n(RandomAccessIterator first, Size count,
                               ForwardIterator result, std::random_access_iterator_tag) {
        RandomAccessIterator last = first + count;
        return make_pair(last, uninitialized_copy(first, last, result));
    }

    template <class InputIterator, class Size, class ForwardIterator>
    inline pair<InputIterator, ForwardIterator>
        uninitialized_copy_n(InputIterator first, Size count, ForwardIterator result) {
        return __uninitialized_copy_n(first, count, result, iterator_category(first));
    }

    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                                         const T& x, __true_type) {
        fill(first, last, x);
    }
    template <class ForwardIterator, class T>
    void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                                  const T& x, __false_type) {
        ForwardIterator cur = first;
        try {
            for (; cur != last; ++cur)
                construct(&*cur, x);
        }
        catch (...) {
            destroy(first, cur); 
        }
    }
    template <class ForwardIterator, class T, class V>
    inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last,
                                     const T& x, V*) {
        using is_POD = __type_traits<V>::is_POD_type;
        __uninitialized_fill_aux(first, last, x, is_POD());
    }
    template <class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
        __uninitialized_fill(first, last, x, value_type(first));
    }

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                                      const T& x, __true_type) {
        return fill_n(first, n, x);
    }
    template <class ForwardIterator, class Size, class T>
    ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                               const T& x, __false_type) {
        ForwardIterator cur = first;
        try {
            for (; n > 0; --n, ++cur)
                construct(&*cur, x);
            return cur;
        }
        catch (...) {
            destroy(first, cur);
        }
    }
    template <class ForwardIterator, class Size, class T, class V>
    inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, V*) {
        using is_POD = __type_traits<T>::is_POD_type;
        return __uninitialized_fill_n_aux(first, n, x, is_POD());
    }

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
        return __uninitialized_fill_n(first, n, x, value_type(first));
    }

    template <class InputIterator1, class InputIterator2, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_copy(InputIterator1 first1, InputIterator1 last1,
                                                     InputIterator2 first2, InputIterator2 last2,
                                                     ForwardIterator result) {
        ForwardIterator mid = uninitialized_copy(first1, last1, result);
        try {
            return uninitialized_copy(first2, last2, mid);
        }
        catch (...) {
            destroy(result, mid);
        }
    }

    template <class ForwardIterator, class T, class InputIterator>
    inline ForwardIterator __uninitialized_fill_copy(ForwardIterator result, ForwardIterator mid, const T& x,
                                                     InputIterator first, InputIterator last) {
        uninitialized_fill(result, mid, x);
        try {
          return uninitialized_copy(first, last, mid);
        }
        catch (...) {
            destroy(result, mid);
        }
    }

    template <class InputIterator, class ForwardIterator, class T>
    inline void __uninitialized_copy_fill(InputIterator first1, InputIterator last1,
                                          ForwardIterator first2, ForwardIterator last2, const T& x) {
        ForwardIterator mid2 = uninitialized_copy(first1, last1, first2);
        try {
            uninitialized_fill(mid2, last2, x);
        }
        catch (...) {
            destroy(first2, mid2);
        }
    }
}

#endif // ALLOC_H
