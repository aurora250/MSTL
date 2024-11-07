#ifndef ALLOC_H
#define ALLOC_H
#include "iterator_adapter.h"

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


}

#endif // ALLOC_H
