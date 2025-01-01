#ifndef MSTL_ALLOC_H__
#define MSTL_ALLOC_H__
#include "basiclib.h"
#include "errorlib.h"
MSTL_BEGIN_NAMESPACE__

template <int inst>
class __malloc_alloc_template {
private:
    static void* oom_malloc(size_t n) {
        void (*my_malloc_handler)();
        void* result;
        for (;;) {
            my_malloc_handler = malloc_alloc_oom_handler__;
            if (my_malloc_handler == 0) { MSTL_EXEC_MEMORY__; }
            (*my_malloc_handler)();
            result = malloc(n);
            if (result) return(result);
        }
    }

    static void* oom_realloc(void* p, size_t n) {
        void (*my_malloc_handler)();
        void* result;
        for (;;) {
            my_malloc_handler = malloc_alloc_oom_handler__;
            if (my_malloc_handler == 0) { MSTL_EXEC_MEMORY__; }
            (*my_malloc_handler)();
            result = realloc(p, n);
            if (result) return(result);
        }
    }

    static void (*malloc_alloc_oom_handler__)();
public:
    static void* allocate(size_t n) {
        void* result = malloc(n);
        if (result == 0) result = oom_malloc(n);
        return result;
    }
    static void deallocate(void* p, size_t) {
        free(p);
    }
    static void* reallocate(void* p, size_t, size_t new_sz) {
        void* result = realloc(p, new_sz);
        if (result == 0) result = oom_realloc(p, new_sz);
        return result;
    }
    static void (*set_malloc_handler(void (*f)()))() {
        void (*old)() = malloc_alloc_oom_handler__;
        malloc_alloc_oom_handler__ = f;
        return(old);
    }

};
template <int inst>
void (*__malloc_alloc_template<inst>::malloc_alloc_oom_handler__)() = 0;

typedef __malloc_alloc_template<0> malloc_alloc;


template <typename T, typename Alloc = std::allocator<T>>
class default_standard_alloc {
private:
    typedef Alloc   data_allocator;
    data_allocator alloc;
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    pointer allocate(const size_type n) {
        return 0 == n ? 0 : (pointer)alloc.allocate(n * sizeof(value_type));
    }
    pointer allocate(void) {
        return (pointer)alloc.allocate(sizeof(value_type));
    }
    void deallocate(pointer const p) {
        alloc.deallocate(p, sizeof(value_type));
    }
    void deallocate(pointer const p, const size_type n) {
        if (n != 0)alloc.deallocate(p, n * sizeof(value_type));
    }
};


enum { __ALIGN = 8 };
enum { __MAX_BYTES = 128 };
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };

template <int inst>
class __default_alloc_template {
private:
    union obj {
        union obj* free_list_link_;
        char client_data_[1];
    };
    static obj* volatile free_list_[__NFREELISTS];
    static char* start_free_;
    static char* end_free_;
    static size_t heap_size_;

    static size_t ROUND_UP(size_t bytes) {
        return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
    }
    static size_t FREELIST_INDEX(size_t bytes) {
        return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
    }
    static void* refill(size_t n) {
        int nobjs = 20;
        char* chunk = chunk_alloc(n, nobjs);
        obj* volatile* my_free_list;
        obj* result;
        obj* current_obj, * next_obj;
        int i;
        if (1 == nobjs) return(chunk);
        my_free_list = free_list_ + FREELIST_INDEX(n);
        result = (obj*)chunk;
        *my_free_list = next_obj = (obj*)(chunk + n);
        for (i = 1; ; i++) {
            current_obj = next_obj;
            next_obj = (obj*)((char*)next_obj + n);
            if (nobjs - 1 == i) {
                current_obj->free_list_link = 0;
                break;
            }
            else 
                current_obj->free_list_link = next_obj;
        }
        return (result);
    }

    static char* chunk_alloc(size_t size, int& nobjs) {
        char* result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free_ - start_free_;
        if (bytes_left >= total_bytes) {
            result = start_free_;
            start_free_ += total_bytes;
            return(result);
        }
        else if (bytes_left >= size) {
            nobjs = bytes_left / size;
            total_bytes = size * nobjs;
            result = start_free_;
            start_free_ += total_bytes;
            return(result);
        }
        else {
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size_ >> 4);
            if (bytes_left > 0) {
                obj* volatile* my_free_list = free_list_ + FREELIST_INDEX(bytes_left);
                ((obj*)start_free_)->free_list_link = *my_free_list;
                *my_free_list = (obj*)start_free_;
            }
            start_free_ = (char*)malloc(bytes_to_get);
            if (0 == start_free_) {
                int i;
                obj* volatile* my_free_list, * p;
                for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
                    my_free_list = free_list_ + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (0 != p) {
                        *my_free_list = p->free_list_link;
                        start_free_ = (char*)p;
                        end_free_ = start_free_ + i;
                        return(chunk_alloc(size, nobjs));
                    }
                }
                end_free_ = 0;
                start_free_ = (char*)malloc_alloc::allocate(bytes_to_get);
            }
            heap_size_ += bytes_to_get;
            end_free_ = start_free_ + bytes_to_get;
            return(chunk_alloc(size, nobjs));
        }
    }
public:
    static void* allocate(size_t n) {
        obj* volatile* my_free_list;
        obj* result;

        if (n > (size_t)__MAX_BYTES) {
            return(malloc_alloc::allocate(n));
        }
        my_free_list = free_list_ + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == 0) {
            void* r = refill(ROUND_UP(n));
            return r;
        }
        *my_free_list = result->free_list_link;
        return (result);
    };

    static void deallocate(void* p, size_t n) {
        obj* q = (obj*)p;
        obj* volatile* my_free_list;
        if (n > (size_t)__MAX_BYTES) {
            malloc_alloc::deallocate(p, n);
            return;
        }
        my_free_list = free_list_ + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }

    static void* reallocate(void* p, size_t old_sz, size_t new_sz) {
        void* result;
        size_t copy_sz;
        if (old_sz > (size_t)__MAX_BYTES && new_sz > (size_t)__MAX_BYTES) 
            return(realloc(p, new_sz));
        if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return(p);
        result = allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz);
        return(result);
    }
};
template <int inst>
char* __default_alloc_template<inst>::start_free_ = 0;
template <int inst>
char* __default_alloc_template<inst>::end_free_ = 0;
template <int inst>
size_t __default_alloc_template<inst>::heap_size_ = 0;
template <int inst>
__default_alloc_template<inst>::obj* volatile __default_alloc_template<inst>::
free_list_[__NFREELISTS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

typedef __default_alloc_template<0> alloc;


template<class T, class Alloc = alloc>
struct simple_alloc {
    static T* allocate(size_t n) {
        return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
    }
    static T* allocate(void) {
        return (T*)Alloc::allocate(sizeof(T));
    }
    static void deallocate(T* p, size_t n) {
        if (0 != n) Alloc::deallocate(p, n * sizeof(T));
    }
    static void deallocate(T* p) {
        Alloc::deallocate(p, sizeof(T));
    }
};


MSTL_END_NAMESPACE__
#endif // MSTL_ALLOC_H__
