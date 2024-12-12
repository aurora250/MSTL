#ifndef MSTL_TEMPBUF_HPP__
#define MSTL_TEMPBUF_HPP__
#include "basiclib.h"
#include "errorlib.h"
#include "pair.hpp"
#include "type_traits.hpp"
#include "memory.hpp"
MSTL_BEGIN_NAMESPACE__

template <class T> 
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*) {
    if (len > ptrdiff_t(INT_MAX / sizeof(T))) len = INT_MAX / sizeof(T);
    while (len > 0) {
        T* tmp = (T*)malloc((size_t)len * sizeof(T));
        if (tmp != 0) return pair<T*, ptrdiff_t>(tmp, len);
        len /= 2;
    }
    return pair<T*, ptrdiff_t>((T*)0, 0);
}
template <class T>
void return_temporary_buffer(T* p) {
    free(p);
}

template <class ForwardIterator, class T = iterator_traits<ForwardIterator>::value_type>
class temporary_buffer {
private:
    ptrdiff_t original_len;
    ptrdiff_t len;
    T* buffer;

    void allocate_buffer() {
        original_len = len;
        buffer = 0;
        if (len > (ptrdiff_t)(INT_MAX / sizeof(T))) len = INT_MAX / sizeof(T);
        while (len > 0) {
            buffer = (T*)malloc(len * sizeof(T));
            if (buffer) break;
            len /= 2;
        }
    }
    void initialize_buffer(const T&, __true_type) {}
    void initialize_buffer(const T& val, __false_type) {
        (uninitialized_fill_n)(buffer, len, val);
    }

public:
    ptrdiff_t size() const { return len; }
    ptrdiff_t requested_size() const { return original_len; }
    T* begin() { return buffer; }
    T* end() { return buffer + len; }

    temporary_buffer(ForwardIterator first, ForwardIterator last) {
        MSTL_TRY__{
            len = 0;
            distance(first, last, len);
            allocate_buffer();
            if (len > 0)
                initialize_buffer(*first, typename __type_traits<T>::has_trivial_default_constructor());
        }
        MSTL_CATCH_UNWIND_THROW_M__(free(buffer); buffer = 0; len = 0);
    }
    ~temporary_buffer() {
        destroy(buffer, buffer + len);
        free(buffer);
    }
private:
    temporary_buffer(const temporary_buffer&) {}
    void operator =(const temporary_buffer&) {}
};

MSTL_END_NAMESPACE__
#endif // MSTL_TEMPBUF_HPP__
