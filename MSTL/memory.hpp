#ifndef MSTL_MEMORY_HPP__
#define MSTL_MEMORY_HPP__
#include "algobase.hpp"
#include <atomic>
MSTL_BEGIN_NAMESPACE__

template <typename Iterator1, typename Iterator2, enable_if_t<
    is_trivially_copy_assignable_v<iter_val_t<Iterator1>>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 __uninitialized_copy_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
    return MSTL::copy(first, last, result);
}

template <typename Iterator1, typename Iterator2, enable_if_t<
    !is_trivially_copy_assignable_v<iter_val_t<Iterator1>>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 __uninitialized_copy_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
    Iterator2 cur = result;
    MSTL_TRY__{
    for (; first != last; ++first, ++cur)
        MSTL::construct(&*cur, *first);
    }
    MSTL_CATCH_UNWIND_THROW_M__(for (; result != cur; --cur) MSTL::destroy(&*cur); )
    return cur;
}

template <typename Iterator1, typename Iterator2, enable_if_t<
    is_input_iter_v<Iterator1> && is_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 uninitialized_copy(Iterator1 first, Iterator1 last, Iterator2 result) {
    return MSTL::__uninitialized_copy_aux(first, last, result);
}


template <typename Iterator1, typename Iterator2, enable_if_t<!is_rnd_iter_v<Iterator1>, int> = 0>
MSTL_CONSTEXPR20 pair<Iterator1, Iterator2> __uninitialized_copy_n_aux(
    Iterator1 first, size_t count, Iterator2 result) {
    Iterator2 cur = result;
    MSTL_TRY__{
        for (; count > 0; --count, ++first, ++cur)
            MSTL::construct(&*cur, *first); 
    }
    MSTL_CATCH_UNWIND_THROW_M__(for (; result != cur; --cur) MSTL::destroy(&*cur); )
    return pair<Iterator1, Iterator2>(first, cur);
}

template <typename Iterator1, typename Iterator2, enable_if_t<is_rnd_iter_v<Iterator1>, int> = 0>
MSTL_CONSTEXPR20 pair<Iterator1, Iterator2> __uninitialized_copy_n_aux(
    Iterator1 first, size_t count, Iterator2 result) {
    Iterator1 last = first + count;
    return MSTL::make_pair(last, MSTL::uninitialized_copy(first, last, result));
}

template <typename Iterator1, typename Iterator2, enable_if_t<
    is_input_iter_v<Iterator1> && is_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 pair<Iterator1, Iterator2> uninitialized_copy_n(
    Iterator1 first, size_t count, Iterator2 result) {
    return MSTL::__uninitialized_copy_n_aux(first, count, result);
}


template <typename Iterator, typename T, enable_if_t<
    is_trivially_copy_assignable_v<iter_val_t<Iterator>>, int> = 0>
MSTL_CONSTEXPR20 void __uninitialized_fill_aux(Iterator first, Iterator last, const T& x) {
    MSTL::fill(first, last, x);
}

template <typename Iterator, typename T, enable_if_t<
    !is_trivially_copy_assignable_v<iter_val_t<Iterator>>, int> = 0>
MSTL_CONSTEXPR20 void __uninitialized_fill_aux(Iterator first, Iterator last, const T& x) {
    Iterator cur = first;
    MSTL_TRY__{
        for (; cur != last; ++cur)
            MSTL::construct(&*cur, x);
    }
    MSTL_CATCH_UNWIND_THROW_M__(for (; cur != first; --cur) MSTL::destroy(&*cur); )
}

template <typename Iterator, typename T, enable_if_t<is_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 void uninitialized_fill(Iterator first, Iterator last, const T& x) {
    MSTL::__uninitialized_fill_aux(first, last, x);
}


template <typename Iterator, typename T, enable_if_t<
    is_trivially_copy_assignable_v<iter_val_t<Iterator>>, int> = 0>
MSTL_CONSTEXPR20 Iterator __uninitialized_fill_n_aux(Iterator first, size_t n, const T& x) {
    return MSTL::fill_n(first, n, x);
}

template <typename Iterator, typename T, enable_if_t<
    !is_trivially_copy_assignable_v<iter_val_t<Iterator>>, int> = 0>
MSTL_CONSTEXPR20 Iterator __uninitialized_fill_n_aux(Iterator first, size_t n, const T& x) {
    Iterator cur = first;
    MSTL_TRY__{
        for (; n > 0; --n, ++cur)
            MSTL::construct(&*cur, x);
    }
    MSTL_CATCH_UNWIND_THROW_M__(for (; cur != first; --cur) MSTL::destroy(&*cur); )
    return cur;
}

template <typename Iterator, typename T, enable_if_t<is_fwd_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR20 Iterator uninitialized_fill_n(Iterator first, size_t n, const T& x) {
    return MSTL::__uninitialized_fill_n_aux(first, n, x);
}


template <typename Iterator1, typename Iterator2, enable_if_t<
    is_trivially_copy_assignable_v<iter_val_t<Iterator1>>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 __uninitialized_move_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
    return MSTL::move(first, last, result);
}

template <typename Iterator1, typename Iterator2, enable_if_t<
    !is_trivially_copy_assignable_v<iter_val_t<Iterator1>>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 __uninitialized_move_aux(Iterator1 first, Iterator1 last, Iterator2 result) {
    Iterator2 cur = result;
    MSTL_TRY__{
        for (; first != last; ++first, ++cur)
            MSTL::construct(&*cur, MSTL::move(*first));
    }
    MSTL_CATCH_UNWIND_THROW_M__(for (; result != cur; --cur) MSTL::destroy(&*cur); )
    return cur;
}

template <typename Iterator1, typename Iterator2, enable_if_t<
    is_input_iter_v<Iterator1> && is_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 Iterator2 uninitialized_move(Iterator1 first, Iterator1 last, Iterator2 result) {
    return MSTL::__uninitialized_move_aux(first, last, result);
}


template <typename Iterator1, typename Iterator2, enable_if_t<!is_rnd_iter_v<Iterator1>, int> = 0>
MSTL_CONSTEXPR20 pair<Iterator1, Iterator2> __uninitialized_move_n_aux(
    Iterator1 first, size_t count, Iterator2 result) {
    Iterator2 cur = result;
    MSTL_TRY__{
        for (; count > 0; --count, ++first, ++cur)
            MSTL::construct(&*cur, MSTL::move(*first));
    }
    MSTL_CATCH_UNWIND_THROW_M__(for (; result != cur; --cur) MSTL::destroy(&*cur); )
    return pair<Iterator1, Iterator2>(first, cur);
}

template <typename Iterator1, typename Iterator2, enable_if_t<is_rnd_iter_v<Iterator1>, int> = 0>
MSTL_CONSTEXPR20 pair<Iterator1, Iterator2> __uninitialized_move_n_aux(
    Iterator1 first, size_t count, Iterator2 result) {
    Iterator1 last = first + count;
    return MSTL::make_pair(last, MSTL::uninitialized_move(first, last, result));
}

template <typename Iterator1, typename Iterator2, enable_if_t<
    is_input_iter_v<Iterator1> && is_fwd_iter_v<Iterator2>, int> = 0>
MSTL_CONSTEXPR20 pair<Iterator1, Iterator2> uninitialized_move_n(
    Iterator1 first, size_t count, Iterator2 result) {
    return MSTL::__uninitialized_move_n_aux(first, count, result);
}


template <typename Iterator, typename T = iter_val_t<Iterator>>
struct temporary_buffer {
    static_assert(is_fwd_iter_v<Iterator>, "temporary buffer requires forward iterator types.");

private:
    ptrdiff_t original_len_ = 0;
    ptrdiff_t len_ = 0;
    T* buffer_ = nullptr;

    MSTL_CONSTEXPR20 void allocate_buffer() {
        original_len_ = len_;
        buffer_ = 0;
        if (len_ > static_cast<ptrdiff_t>((INT_MAX / sizeof(T)))) len_ = INT_MAX / sizeof(T);
        while (len_ > 0) {
            buffer_ = static_cast<T *>(std::malloc(len_ * sizeof(T)));
            if (buffer_) break;
            len_ /= 2;
        }
    }

    template <typename U = T, enable_if_t<is_trivially_copy_assignable_v<U>, int> = 0>
    MSTL_CONSTEXPR20 void initialize_buffer(const U&) {}
    template <typename U = T, enable_if_t<!is_trivially_copy_assignable_v<U>, int> = 0>
    MSTL_CONSTEXPR20 void initialize_buffer(const U& val) {
        MSTL::uninitialized_fill_n(buffer_, len_, val);
    }

public:
    MSTL_NODISCARD MSTL_CONSTEXPR20 ptrdiff_t size() const { return len_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 ptrdiff_t requested_size() const { return original_len_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 T* begin() { return buffer_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 T* end() { return buffer_ + len_; }

    temporary_buffer(const temporary_buffer&) = delete;
    void operator =(const temporary_buffer&) = delete;
    MSTL_CONSTEXPR20 temporary_buffer(Iterator first, Iterator last) {
        MSTL_TRY__{
            len_ = MSTL::distance(first, last);
            allocate_buffer();
            if (len_ > 0) initialize_buffer(*first);
        }
        MSTL_CATCH_UNWIND_THROW_M__(std::free(buffer_); buffer_ = 0; len_ = 0);
    }
    MSTL_CONSTEXPR20 ~temporary_buffer() {
        MSTL::destroy(buffer_, buffer_ + len_);
        std::free(buffer_);
    }
};


template <typename Ptr, typename Elem>
struct ptr_traits_base {
    using pointer = Ptr;
    using element_type = Elem;
    using difference_type = get_ptr_difference_type_t<Ptr>;
    using reference = conditional_t<is_void_v<Elem>, char, Elem>&;

    template <typename U>
    using rebind = typename get_rebind_type<Ptr, U>::type;

    MSTL_NODISCARD static MSTL_CONSTEXPR pointer pointer_to(reference x)
        noexcept(noexcept(Ptr::pointer_to(x))) {
        return Ptr::pointer_to(x);
    }
};

template <typename, typename = void, typename = void>
struct ptr_traits_extract {};

template <typename T, typename U>
struct ptr_traits_extract<T, U, void_t<typename get_first_parameter<T>::type>>
    : ptr_traits_base<T, typename get_first_parameter<T>::type> {
};

template <typename T>
struct ptr_traits_extract<T, void_t<typename T::element_type>, void>
    : ptr_traits_base<T, typename T::element_type> {
};

template <typename T>
struct pointer_traits : ptr_traits_extract<T> {};

template <typename T>
struct pointer_traits<T*> {
    using pointer = T*;
    using element_type = T;
    using difference_type = ptrdiff_t;
    using reference = conditional_t<is_void_v<T>, char, T>&;

    template <typename U>
    using rebind = U*;

    MSTL_NODISCARD static MSTL_CONSTEXPR pointer pointer_to(reference x) noexcept {
        return MSTL::addressof(x);
    }
};

template <typename Ptr>
MSTL_CONSTEXPR decltype(auto) ptr_const_cast(Ptr ptr) noexcept {
    using T = typename pointer_traits<Ptr>::element_type;
    using NonConst = remove_const_t<T>;
    using Dest = typename pointer_traits<Ptr>::template rebind<NonConst>;

    return pointer_traits<Dest>::pointer_to(const_cast<NonConst&>(*ptr));
}
template <typename T>
MSTL_CONSTEXPR decltype(auto) ptr_const_cast(T* ptr) noexcept {
    return const_cast<remove_const_t<T>*>(ptr);
}


template <typename T, typename = void>
struct get_pointer_type {
    using type = typename T::value_type*;
};
template <typename T>
struct get_pointer_type<T, void_t<typename T::pointer>> {
    using type = typename T::pointer;
};

template <typename T, typename = void>
struct get_difference_type {
    using pointer = typename get_pointer_type<T>::type;
    using type = typename pointer_traits<pointer>::difference_type;
};
template <typename T>
struct get_difference_type<T, void_t<typename T::difference_type>> {
    using type = typename T::difference_type;
};

template <typename T, typename = void>
struct get_size_type {
    using type = make_unsigned_t<typename get_difference_type<T>::type>;
};
template <typename T>
struct get_size_type<T, void_t<typename T::size_type>> {
    using type = typename T::size_type;
};

template <typename T, typename U, typename = void>
struct get_alloc_rebind_type {
    using type = typename replace_first_parameter<U, T>::type;
};
template <typename T, typename U>
struct get_alloc_rebind_type<T, U, void_t<typename T::template rebind<U>::other>> {
    using type = typename T::template rebind<U>::other;
};

template <typename Alloc>
struct allocator_traits {
    using allocator_type    = Alloc;
    using value_type        = typename Alloc::value_type;
    using pointer           = typename get_pointer_type<Alloc>::type;
    using size_type         = typename get_size_type<Alloc>::type;
    using difference_type   = typename get_difference_type<Alloc>::type;

    template <class U>
    using rebind_alloc  = typename get_alloc_rebind_type<Alloc, U>::type;
    template <class U>
    using rebind_traits = allocator_traits<rebind_alloc<U>>;

    MSTL_ALLOCNODISCARD static MSTL_CONSTEXPR20 MSTL_DECLALLOC pointer allocate(
        Alloc& alloc, const size_type n) {
        return alloc.allocate(n);
    }
    static MSTL_CONSTEXPR20 void deallocate(Alloc& alloc, pointer ptr, size_type n) {
        alloc.deallocate(ptr, n);
    }
};


template <size_t Align>
MSTL_DECLALLOC MSTL_CONSTEXPR20 void* __allocate_aux(const size_t bytes) {
    if (bytes >= MEMORY_BIG_ALLOC_THRESHHOLD) {
        const void* raw_ptr = ::operator new(MEMORY_NO_USER_SIZE + bytes);
        const auto holder = reinterpret_cast<uintptr_t>(raw_ptr);
        MSTL_DEBUG_VERIFY__(holder != 0, "invalid argument");
        const auto ptr = reinterpret_cast<void*>(
            (holder + MEMORY_NO_USER_SIZE) & ~(MEMORY_BIG_ALLOC_ALIGN - 1)); // align the memory address
        static_cast<uintptr_t*>(ptr)[-1] = holder;
#ifdef MSTL_STATE_DEBUG__
        static_cast<uintptr_t*>(ptr)[-2] = MEMORY_BIG_ALLOC_SENTINEL;
#endif
        return ptr;
    }
    return ::operator new(bytes);
}
#ifdef MSTL_VERSION_17__
template <size_t Align, enable_if_t<(Align > MEMORY_ALIGN_THRESHHOLD) ,int> = 0>
MSTL_DECLALLOC MSTL_CONSTEXPR20 void* __allocate_dispatch(const size_t bytes) {
    size_t align = MSTL::max(Align, MEMORY_BIG_ALLOC_ALIGN);
#if defined(MSTL_COMPILE_CLANG__) && defined(MSTL_VERSION_20__)
    if (MSTL::is_constant_evaluated()) {
        return ::operator new(bytes);
    }
    else
#endif
    {
        return ::operator new(bytes, std::align_val_t{ align });
    }
}
template <size_t Align, enable_if_t<Align <= MEMORY_ALIGN_THRESHHOLD, int> = 0>
MSTL_DECLALLOC MSTL_CONSTEXPR20 void* __allocate_dispatch(const size_t bytes) {
    return MSTL::__allocate_aux<Align>(bytes);
}
#endif

template <size_t Align>
MSTL_DECLALLOC MSTL_CONSTEXPR20 void* allocate(const size_t bytes) {
    if (bytes == 0) return nullptr;
#if MSTL_VERSION_20__
    if (MSTL::is_constant_evaluated()) {
        return ::operator new(bytes);
    }
#endif // MSTL_VERSION_20__

#ifdef MSTL_VERSION_17__
    return MSTL::__allocate_dispatch<Align>(bytes);
#else
    return MSTL::__allocate_aux<Align>(bytes);
#endif // MSTL_VERSION_17__
}


template <size_t Align>
MSTL_CONSTEXPR20 void __deallocate_aux(void* ptr, size_t bytes) noexcept {
    if (bytes >= MEMORY_BIG_ALLOC_THRESHHOLD) {
        bytes += MEMORY_NO_USER_SIZE;
        const uintptr_t* const user_ptr = static_cast<uintptr_t*>(ptr);
        const uintptr_t holder = user_ptr[-1];
        MSTL_DEBUG_VERIFY__(user_ptr[-2] == MEMORY_BIG_ALLOC_SENTINEL, "invalid sentinel.");
#ifdef MSTL_STATE_DEBUG__
        constexpr uintptr_t min_shift = 2 * sizeof(void*);
#else
        constexpr uintptr_t min_shift = sizeof(void*);
#endif // MSTL_STATE_DEBUG__
        const uintptr_t shift = reinterpret_cast<uintptr_t>(ptr) - holder;
        MSTL_DEBUG_VERIFY__(shift >= min_shift && shift <= MEMORY_NO_USER_SIZE, "invalid argument.");
        ptr = reinterpret_cast<void*>(holder);
    }
    ::operator delete(ptr, bytes);
}
#ifdef MSTL_VERSION_17__
template <size_t Align, enable_if_t<(Align > MEMORY_ALIGN_THRESHHOLD), int> = 0>
MSTL_CONSTEXPR20 void __deallocate_dispatch(void* ptr, const size_t bytes) noexcept {
    size_t align = MSTL::max(Align, MEMORY_BIG_ALLOC_ALIGN);
    ::operator delete(ptr, bytes, std::align_val_t{ align });
}
template <size_t Align, enable_if_t<Align <= MEMORY_ALIGN_THRESHHOLD, int> = 0>
MSTL_CONSTEXPR20 void __deallocate_dispatch(void* ptr, const size_t bytes) noexcept {
    MSTL::__deallocate_aux<Align>(ptr, bytes);
}
#endif // MSTL_VERSION_17__

template <size_t Align>
MSTL_CONSTEXPR20 void deallocate(void* ptr, size_t bytes) noexcept {
#if MSTL_VERSION_20__
    if (MSTL::is_constant_evaluated()) {
        ::operator delete(ptr);
        return;
    }
#endif // MSTL_VERSION_20__

#ifdef MSTL_VERSION_17__
    MSTL::__deallocate_dispatch<Align>(ptr, bytes);
#else
    MSTL::__deallocate_aux<Align>(ptr, bytes);
#endif // MSTL_VERSION_17__
}


template <typename T>
constexpr size_t FINAL_ALIGN_SIZE = MSTL::max(alignof(T), MEMORY_ALIGN_THRESHHOLD);


template <typename T>
class standard_allocator {
    static_assert(is_allocable_v<T>, "allocator can`t alloc void, reference, function or const type.");

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

    MSTL_CONSTEXPR20 standard_allocator() noexcept = default;
    template <typename U>
    MSTL_CONSTEXPR20 standard_allocator(const standard_allocator<U>&) noexcept {}
    MSTL_CONSTEXPR20 ~standard_allocator() noexcept = default;
    MSTL_CONSTEXPR20 self& operator =(const self&) noexcept = default;

    MSTL_ALLOCNODISCARD static MSTL_CONSTEXPR20 MSTL_DECLALLOC pointer allocate(const size_type n) {
        constexpr size_t value_size = sizeof(value_type);
        static_assert(value_size > 0, "value type must be complete before allocation called.");
        const size_t alloc_size = value_size * n;
        MSTL_DEBUG_VERIFY__(alloc_size <= INT_MAX_SIZE, "allocation will cause memory overflow.");
        return static_cast<T*>(MSTL::allocate<FINAL_ALIGN_SIZE<T>>(alloc_size));
    }
    MSTL_ALLOCNODISCARD static MSTL_CONSTEXPR20 MSTL_DECLALLOC pointer allocate() {
        return allocate(1);
    }
    static MSTL_CONSTEXPR20 void deallocate(pointer p, const size_type n) noexcept {
        constexpr size_t value_size = sizeof(value_type);
        MSTL_DEBUG_VERIFY__(p != nullptr || n == 0, "pointer invalid.");
        MSTL::deallocate<FINAL_ALIGN_SIZE<T>>(p, n * value_size);
    }
    static MSTL_CONSTEXPR20 void deallocate(pointer p) noexcept {
        deallocate(p, 1);
    }
};
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator ==(
    const standard_allocator<T>& lh, const standard_allocator<U>& rh) noexcept {
    return true;
}
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator !=(
    const standard_allocator<T>& lh, const standard_allocator<U>& rh) noexcept {
    return false;
}

template <typename T>
class ctype_allocator {
    static_assert(is_allocable_v<T>, "allocator can`t alloc void, reference, function or const type.");

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

    MSTL_CONSTEXPR20 ctype_allocator() noexcept = default;
    template <typename U>
    MSTL_CONSTEXPR20 ctype_allocator(const ctype_allocator<U>&) noexcept {}
    MSTL_CONSTEXPR20 ~ctype_allocator() noexcept = default;
    MSTL_CONSTEXPR20 self& operator =(const self&) noexcept = default;

    MSTL_ALLOCNODISCARD static MSTL_CONSTEXPR20 MSTL_DECLALLOC pointer allocate(const size_type n) {
        return 0 == n ? nullptr : static_cast<pointer>(std::malloc(n * sizeof(T)));
    }
    MSTL_ALLOCNODISCARD static MSTL_CONSTEXPR20 MSTL_DECLALLOC pointer allocate() {
        return static_cast<pointer>(std::malloc(sizeof(T)));
    }
    static MSTL_CONSTEXPR20 void deallocate(pointer p) noexcept {
        std::free(p);
    }
    static MSTL_CONSTEXPR20 void deallocate(pointer p, const size_type) noexcept {
        std::free(p);
    }
};
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator ==(const ctype_allocator<T>&, const ctype_allocator<U>&) noexcept {
    return true;
}
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator !=(const ctype_allocator<T>&, const ctype_allocator<U>&) noexcept {
    return false;
}

template <typename T>
class new_allocator {
    static_assert(is_allocable_v<T>, "allocator can`t alloc void, reference, function or const type.");

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

    MSTL_CONSTEXPR20 new_allocator() noexcept = default;
    template <typename U>
    MSTL_CONSTEXPR20 new_allocator(const new_allocator<U>&) noexcept {}
    MSTL_CONSTEXPR20 ~new_allocator() noexcept = default;
    MSTL_CONSTEXPR20 self& operator =(const self&) noexcept = default;

    MSTL_ALLOCNODISCARD static MSTL_CONSTEXPR20 MSTL_DECLALLOC pointer allocate(size_type n) {
        return 0 == n ? nullptr : static_cast<pointer>(::operator new(n * sizeof(T)));
    }
    MSTL_ALLOCNODISCARD static MSTL_CONSTEXPR20 MSTL_DECLALLOC pointer allocate(void) {
        return static_cast<pointer>(::operator new(sizeof(T)));
    }
    static MSTL_CONSTEXPR20 void deallocate(pointer p) noexcept {
        ::operator delete(p);
    }
    static MSTL_CONSTEXPR20 void deallocate(pointer p, const size_type) noexcept {
        ::operator delete(p);
    }
};
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator ==(const new_allocator<T>&, const new_allocator<U>&) noexcept {
    return true;
}
template <typename T, typename U>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator !=(const new_allocator<T>&, const new_allocator<U>&) noexcept {
    return false;
}


template <typename Alloc>
using alloc_ptr_t = typename allocator_traits<Alloc>::pointer;
template <typename Alloc>
using alloc_size_t = typename allocator_traits<Alloc>::size_type;


#ifndef MSTL_VERSION_17__
template <typename Alloc, enable_if_t<
    is_same_v<alloc_ptr_t<Alloc>, typename Alloc::value_type*>, int> = 0>
MSTL_CONSTEXPR20 void __delete_internal_aux(Alloc& alloc, typename Alloc::value_type* const ptr) noexcept {
    allocator_traits<Alloc>::deallocate(alloc, ptr, 1);
}
template <typename Alloc, enable_if_t<
    !is_same_v<alloc_ptr_t<Alloc>, typename Alloc::value_type*>, int> = 0>
MSTL_CONSTEXPR20 void __delete_internal_aux(Alloc& alloc, typename Alloc::value_type* const ptr) noexcept {
    using pointer = pointer_traits<alloc_ptr_t<Alloc>>;
    allocator_traits<Alloc>::deallocate(alloc, pointer::pointer_to(*ptr), 1);
}
template <typename Alloc>
MSTL_CONSTEXPR20 void delete_internal(Alloc& alloc, typename Alloc::value_type* const ptr) noexcept {
    ptr->~T();
    __delete_internal_aux(alloc, ptr);
}
#else
template <typename Alloc>
MSTL_CONSTEXPR20 void delete_internal(Alloc& alloc, typename Alloc::value_type* const ptr) noexcept {
    ptr->~T();
    if constexpr (is_same_v<alloc_ptr_t<Alloc>, typename Alloc::value_type*>) {
        allocator_traits<Alloc>::deallocate(alloc, ptr, 1);
    }
    else {
        using pointer = pointer_traits<alloc_ptr_t<Alloc>>;
        allocator_traits<Alloc>::deallocate(alloc, pointer::pointer_to(*ptr), 1);
    }
}
#endif // MSTL_VERSION_17__


template <class T>
struct default_deleter {
    void operator()(T* p) const noexcept { delete p; }
};
template <class T>
struct default_deleter<T[]> {
    void operator()(T* p) const { delete[] p; }
};


template <class T, class Deleter = default_deleter<T>>
class unique_ptr {
private:
    T* ptr_ = nullptr;
    MSTL_NO_UNIADS Deleter deleter_{};

public:
    using element_type  = T;
    using pointer       = T*;
    using deleter_type  = Deleter;
    using self          = unique_ptr<T, Deleter>;

    unique_ptr() noexcept = default;
    explicit unique_ptr(T* p) noexcept : ptr_(p) {}

    template <class U, enable_if_t<is_convertible_v<U*, T*>, int> = 0>
    explicit unique_ptr(unique_ptr<U, Deleter>&& x) noexcept : ptr_(x.ptr_) {
        x.ptr_ = nullptr;
    }

    unique_ptr(self const&) = delete;
    self& operator =(self const&) = delete;

    unique_ptr(self&& x) noexcept : ptr_(x.ptr_) {
        x.ptr_ = nullptr;
    }
    self& operator =(self&& x) noexcept {
        if (this != &x) MSTL_UNLIKELY {
            if (ptr_) deleter_(ptr_);
            ptr_ = MSTL::exchange(x.ptr_, nullptr);
        }
        return *this;
    }

    ~unique_ptr() noexcept {
        if (ptr_) deleter_(ptr_);
    }

    MSTL_NODISCARD T* get() const noexcept {
        return ptr_;
    }
    MSTL_NODISCARD T* operator ->() const noexcept {
        return ptr_;
    }
    MSTL_NODISCARD add_lvalue_reference_t<T> operator *() const noexcept {
        return *ptr_;
    }
    MSTL_NODISCARD Deleter get_deleter() const noexcept {
        return deleter_;
    }

    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }
    void reset(T* ptr = nullptr) noexcept {
        if (ptr_) deleter_(ptr_);
        ptr_ = ptr;
    }

    void swap(unique_ptr& x) noexcept {
        MSTL::swap(ptr_, x.ptr_);
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
    bool operator ==(const self& rh) const noexcept {
        return ptr_ == rh.ptr_;
    }
    bool operator !=(const self& rh) const noexcept {
        return ptr_ != rh.ptr_;
    }
    bool operator <(const self& rh) const noexcept {
        return ptr_ < rh.ptr_;
    }
    bool operator >(const self& rh) const noexcept {
        return ptr_ > rh.ptr_;
    }
    bool operator <=(const self& rh) const noexcept {
        return ptr_ <= rh.ptr_;
    }
    bool operator >=(const self& rh) const noexcept {
        return ptr_ >= rh.ptr_;
    }
};

template <class T, class Deleter>
class unique_ptr<T[], Deleter> : public unique_ptr<T, Deleter> {
    using unique_ptr<T, Deleter>::unique_ptr;

    add_lvalue_reference_t<T> operator [](size_t idx) {
        return this->get()[idx];
    }
};

template <class T, class... Args, enable_if_t<!is_unbounded_array_v<T>, int> = 0>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(MSTL::forward<Args>(args)...));
}
template <class T, enable_if_t<is_unbounded_array_v<T>, int> = 0>
unique_ptr<T> make_unique(size_t len) {
    return unique_ptr<T>(new remove_extent_t<T>[len]());
}

template <class T, enable_if_t<!is_unbounded_array_v<T>, int> = 0>
unique_ptr<T> make_unique_for_overwrite() {
    return unique_ptr<T>(new T());
}
template <class T, enable_if_t<is_unbounded_array_v<T>, int> = 0>
unique_ptr<T> make_unique_for_overwrite(size_t len) {
    return unique_ptr<T>(new remove_extent_t<T>[len]());
}


struct smart_ptr_counter {
    std::atomic_ulong count_;

    smart_ptr_counter() noexcept : count_(1) {}
    smart_ptr_counter(smart_ptr_counter&&) = delete;
    virtual ~smart_ptr_counter() = default;

    void incref() noexcept {
        count_.fetch_add(1, std::memory_order_relaxed);
    }
    void decref() noexcept {
        if (count_.fetch_sub(1, std::memory_order_relaxed) == 1) {
            delete this;
        }
    }
    MSTL_NODISCARD uint64_t countref() const noexcept {
        return count_.load(std::memory_order_relaxed);
    }
};

template <class T, class Deleter>
struct smart_ptr_counter_impl final : smart_ptr_counter {
    T* ptr_;
    MSTL_NO_UNIADS Deleter deleter_;

    explicit smart_ptr_counter_impl(T* ptr) noexcept : ptr_(ptr) {}

    explicit smart_ptr_counter_impl(T* ptr, Deleter deleter) noexcept
        : ptr_(ptr), deleter_(MSTL::move(deleter)) {}

    ~smart_ptr_counter_impl() noexcept override {
        deleter_(ptr_);
    }
};

template <class T, class Deleter>
struct smart_ptr_counter_impl_fused final : smart_ptr_counter {
    T* ptr_;
    void* mem_;
    MSTL_NO_UNIADS Deleter deleter_;

    explicit smart_ptr_counter_impl_fused(T* ptr, void* mem, Deleter deleter) noexcept
        : ptr_(ptr), mem_(mem), deleter_(MSTL::move(deleter)) {}

    ~smart_ptr_counter_impl_fused() noexcept override {
        deleter_(ptr_);
    }

    void operator delete(void* mem) noexcept {
#if MSTL_VERSION_17__
        ::operator delete(mem, static_cast<std::align_val_t>(MSTL::max(alignof(T), alignof(smart_ptr_counter_impl_fused))));
#else
        ::operator delete(mem);
#endif
    }
};

template <typename T>
struct enable_shared_from;
template <class T>
class shared_ptr;

template <class T, enable_if_t<is_base_of_v<enable_shared_from<T>, T>, int>>
void __setup_enable_shared_from(T* ptr, smart_ptr_counter* owner);
template <class T>
shared_ptr<T> __make_shared_fused(T* ptr, smart_ptr_counter* owner) noexcept {
    return shared_ptr<T>(ptr, owner);
}

template <class T>
class shared_ptr {
private:
    T* ptr_ = nullptr;
    smart_ptr_counter* owner_ = nullptr;

    explicit shared_ptr(T* ptr, smart_ptr_counter* owner) noexcept : ptr_(ptr), owner_(owner) {}

    template <class>
    friend class shared_ptr;

    template <typename U>
    friend shared_ptr<U> __make_shared_fused(U*, smart_ptr_counter*) noexcept;

public:
    using element_type  = T;
    using pointer       = T*;
    using self          = shared_ptr<T>;

    explicit shared_ptr(nullptr_t) noexcept {}

    template <class U, enable_if_t<is_convertible_v<U*, T*>, int> = 0>
    explicit shared_ptr(U* ptr) 
        : ptr_(ptr), owner_(new smart_ptr_counter_impl<U, default_deleter<U>>(ptr)) {
        MSTL::__setup_enable_shared_from<T>(ptr_, owner_);
    }

    template <class U, class Deleter, enable_if_t<is_convertible_v<U*, T*>, int> = 0>
    explicit shared_ptr(U* ptr, Deleter deleter)
        : ptr_(ptr), owner_(new smart_ptr_counter_impl<U, Deleter>(ptr, MSTL::move(deleter))) {
        MSTL::__setup_enable_shared_from<T>(ptr_, owner_);
    }

    template <class U, class Deleter, enable_if_t<is_convertible_v<U*, T*>, int> = 0>
    explicit shared_ptr(unique_ptr<U, Deleter>&& ptr)
        : shared_ptr(ptr.release(), ptr.get_deleter()) {}

    shared_ptr(const self& x) noexcept : ptr_(x.ptr_), owner_(x.owner_) {
        if (owner_) owner_->incref();
    }
    self& operator =(const self& x) noexcept {
        if (MSTL::addressof(x) == this) return *this;
        if (owner_) owner_->decref();
        ptr_ = x.ptr_;
        owner_ = x.owner_;
        if (owner_) owner_->incref();
        return *this;
    }
    template <class U, enable_if_t<is_convertible_v<U*, T*>, int> = 0>
    explicit shared_ptr(const shared_ptr<U>& x) noexcept : ptr_(x.ptr_), owner_(x.owner_) {
        if (owner_) owner_->incref();
    }

    shared_ptr(self&& x) noexcept : ptr_(x.ptr_), owner_(x.owner_) {
        x.ptr_ = nullptr;
        x.owner_ = nullptr;
    }
    self& operator =(self&& x) noexcept {
        if (MSTL::addressof(x) == this) return *this;
        if (owner_) owner_->decref();
        ptr_ = x.ptr_;
        owner_ = x.owner_;
        x.ptr_ = nullptr;
        x.owner_ = nullptr;
        return *this;
    }
    template <class U, enable_if_t<is_convertible_v<U*, T*>, int> = 0>
    explicit shared_ptr(shared_ptr<U>&& x) noexcept : ptr_(x.ptr_), owner_(x.owner_) {
        x.ptr_ = nullptr;
        x.owner_ = nullptr;
    }

    template <class U>
    shared_ptr(const shared_ptr<U>& x, T* ptr) noexcept : ptr_(ptr), owner_(x.owner_) {
        if (owner_) owner_->incref();
    }
    template <class U>
    shared_ptr(shared_ptr<U>&& x, T* ptr) noexcept : ptr_(ptr), owner_(x.owner_) {
        x.ptr_ = nullptr;
        x.owner_ = nullptr;
    }

    template <class U, enable_if_t<is_convertible_v<U*, T*>, int> = 0>
    shared_ptr& operator =(const shared_ptr<U>& x) noexcept {
        if (MSTL::addressof(x) == this) return *this;
        if (owner_) owner_->decref();
        ptr_ = x.ptr_;
        owner_ = x.owner_;
        if (owner_) owner_->incref();
        return *this;
    }
    template <class U, enable_if_t<is_convertible_v<U*, T*>, int> = 0>
    shared_ptr& operator =(shared_ptr<U>&& x) noexcept {
        if (MSTL::addressof(x) == this) return *this;
        if (owner_) owner_->decref();
        ptr_ = x.ptr_;
        owner_ = x.owner_;
        x.ptr_ = nullptr;
        x.owner_ = nullptr;
        return *this;
    }

    ~shared_ptr() noexcept {
        if (owner_) owner_->decref();
    }

    void reset() noexcept {
        if (owner_) owner_->decref();
        owner_ = nullptr;
        ptr_ = nullptr;
    }
    template <class U>
    void reset(U* ptr) {
        if (owner_) owner_->decref();
        ptr_ = nullptr;
        owner_ = nullptr;
        ptr_ = ptr;
        owner_ = new smart_ptr_counter_impl<U, default_deleter<U>>(ptr);
        MSTL::__setup_enable_shared_from<T>(ptr_, owner_);
    }
    template <class U, class Deleter>
    void reset(U* ptr, Deleter deleter) {
        if (owner_) owner_->decref();
        ptr_ = nullptr;
        owner_ = nullptr;
        ptr_ = ptr;
        owner_ = new smart_ptr_counter_impl<U, Deleter>(ptr, MSTL::move(deleter));
        MSTL::__setup_enable_shared_from<T>(ptr_, owner_);
    }

    long use_count() noexcept {
        return owner_ ? owner_->countref() : 0;
    }
    bool unique() noexcept {
        return owner_ ? owner_->countref() == 1 : true;
    }

    void swap(shared_ptr& x) noexcept {
        if (MSTL::addressof(x) == this) return;
        MSTL::swap(ptr_, x.ptr_);
        MSTL::swap(owner_, x.owner_);
    }

    MSTL_NODISCARD T* get() const noexcept {
        return ptr_;
    }
    MSTL_NODISCARD T* operator ->() const noexcept {
        return ptr_;
    }
    MSTL_NODISCARD add_lvalue_reference_t<T> operator *() const noexcept {
        return *ptr_;
    }

    MSTL_NODISCARD explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
    template <class U>
    MSTL_NODISCARD bool owner_equal(shared_ptr<U> const& rh) const noexcept {
        return owner_ == rh.owner_;
    }
    template <class U>
    MSTL_NODISCARD bool owner_before(shared_ptr<U> const& rh) const noexcept {
        return owner_ < rh.owner_;
    }
};
template <typename T, typename U>
MSTL_NODISCARD bool operator ==(const shared_ptr<T>& lh, const shared_ptr<U>& rh) noexcept {
    return lh.owner_equal(rh);
}
template <typename T, typename U>
MSTL_NODISCARD bool operator !=(const shared_ptr<T>& lh, const shared_ptr<U>& rh) noexcept {
    return !(lh == rh);
}
template <typename T, typename U>
MSTL_NODISCARD bool operator <(const shared_ptr<T>& lh, const shared_ptr<U>& rh) noexcept {
    return lh.owner_before(rh);
}
template <typename T, typename U>
MSTL_NODISCARD bool operator >(const shared_ptr<T>& lh, const shared_ptr<U>& rh) noexcept {
    return rh < lh;
}
template <typename T, typename U>
MSTL_NODISCARD bool operator <=(const shared_ptr<T>& lh, const shared_ptr<U>& rh) noexcept {
    return !(lh > rh);
}
template <typename T, typename U>
MSTL_NODISCARD bool operator >=(const shared_ptr<T>& lh, const shared_ptr<U>& rh) noexcept {
    return !(lh < rh);
}


template <class T>
class shared_ptr<T[]> : shared_ptr<T> {
public:
    using shared_ptr<T>::shared_ptr;

    add_lvalue_reference_t<T> operator [](size_t idx) {
        return this->get()[idx];
    }
};


template <class T>
struct enable_shared_from {
private:
    smart_ptr_counter* owner_;

protected:
    enable_shared_from() noexcept : owner_(nullptr) {}

    shared_ptr<T> shared_from_this() {
        static_assert(is_base_of_v<enable_shared_from, T>, "shared from T requires derived class");
        if (!owner_) Exception(MemoryError("smart pointer share failed."));
        owner_->incref();
        return (__make_shared_fused)(static_cast<T*>(this), owner_);
    }

    shared_ptr<T const> shared_from_this() const {
        static_assert(is_base_of_v<enable_shared_from, T>, "shared from T requires derived class");
        if (!owner_) Exception(MemoryError("smart pointer share failed."));
        owner_->incref();
        return (__make_shared_fused)(static_cast<T const*>(this), owner_);
    }

    template <class U>
    friend void __set_enable_shared_from(enable_shared_from<U>*, smart_ptr_counter*);
};

template <class T>
void __set_enable_shared_from(enable_shared_from<T>* ptr, smart_ptr_counter* owner) {
    ptr->owner_ = owner;
}

template <class T, enable_if_t<is_base_of_v<enable_shared_from<T>, T>, int> = 0>
void __setup_enable_shared_from(T* ptr, smart_ptr_counter* owner) {
    (__set_enable_shared_from)(static_cast<enable_shared_from<T>*>(ptr), owner);
}

template <class T, enable_if_t<!is_base_of_v<enable_shared_from<T>, T>, int> = 0>
void __setup_enable_shared_from(T*, smart_ptr_counter*) {}


template <class T, class... Args, enable_if_t<!is_unbounded_array_v<T>, int> = 0>
shared_ptr<T> make_shared(Args&&... args) {
    auto const deleter = [](T* ptr) noexcept { ptr->~T(); };
    using Counter = smart_ptr_counter_impl_fused<T, decltype(deleter)>;
    constexpr size_t offset = MSTL::max(alignof(T), sizeof(Counter));
    constexpr size_t align = MSTL::max(alignof(T), alignof(Counter));
    constexpr size_t size = offset + sizeof(T);
#if MSTL_VERSION_17__
    void* mem = ::operator new(size, static_cast<std::align_val_t>(align));
    auto* counter = static_cast<Counter*>(mem);
#else
    void* mem = ::operator new(size + align);
    Counter* counter = reinterpret_cast<Counter*>(reinterpret_cast<size_t>(mem) & align);
#endif
    T* object = reinterpret_cast<T*>(reinterpret_cast<char*>(counter) + offset);
    MSTL_TRY__{
        new (object) T(MSTL::forward<Args>(args)...);
    }
    MSTL_CATCH_UNWIND__{
#if MSTL_VERSION_17__
        ::operator delete(mem, static_cast<std::align_val_t>(align));
#else
        ::operator delete(mem);
#endif
        MSTL_EXEC_MEMORY__;
    }
    new (counter) Counter(object, mem, deleter);
    __setup_enable_shared_from(object, counter);
    return (__make_shared_fused)(object, counter);
}

template <class T, enable_if_t<!is_unbounded_array_v<T>, int> = 0>
shared_ptr<T> make_shared_for_overwrite() {
    auto const deleter = [](T* ptr) noexcept { ptr->~T(); };
    using Counter = smart_ptr_counter_impl_fused<T, decltype(deleter)>;
    constexpr size_t offset = MSTL::max(alignof(T), sizeof(Counter));
    constexpr size_t align = MSTL::max(alignof(T), alignof(Counter));
    constexpr size_t size = offset + sizeof(T);
#if MSTL_VERSION_17__
    void* mem = ::operator new(size, static_cast<std::align_val_t>(align));
    auto* counter = static_cast<Counter*>(mem);
#else
    void* mem = ::operator new(size + align);
    Counter* counter = reinterpret_cast<Counter*>(reinterpret_cast<size_t>(mem) & align);
#endif
    T* object = reinterpret_cast<T*>(reinterpret_cast<char*>(counter) + offset);
    MSTL_TRY__{
        new (object) T;
    }
    MSTL_CATCH_UNWIND__{
#if MSTL_VERSION_17__
        ::operator delete(mem, static_cast<std::align_val_t>(align));
#else
        ::operator delete(mem);
#endif
        MSTL_EXEC_MEMORY__;
    }
    new (counter) Counter(object, mem, deleter);
    __setup_enable_shared_from(object, counter);
    return (__make_shared_fused)(object, counter);
}

template <class T, class... Args, enable_if_t<is_unbounded_array_v<T>, int> = 0>
shared_ptr<T> make_shared(const size_t len) {
    using value = remove_extent_t<T>;
    auto* tmp = new value[len];
    MSTL_TRY__{ return shared_ptr<T>(tmp); }
    MSTL_CATCH_UNWIND_THROW_M__(delete[] tmp)
}
template <class T, enable_if_t<is_unbounded_array_v<T>, int> = 0>
shared_ptr<T> make_shared_for_overwrite(const size_t len) {
    using value = remove_extent_t<T>;
    auto* tmp = new value[len];
    MSTL_TRY__{ return shared_ptr<T>(tmp); }
    MSTL_CATCH_UNWIND_THROW_M__(delete[] tmp)
}


template <typename T, typename U>
shared_ptr<T> static_pointer_cast(const shared_ptr<U>& ptr) {
    return shared_ptr<T>(ptr, static_cast<T*>(ptr.get()));
}
template <typename T, typename U>
shared_ptr<T> const_pointer_cast(const shared_ptr<U>& ptr) {
    return shared_ptr<T>(ptr, const_cast<T*>(ptr.get()));
}
template <typename T, typename U>
shared_ptr<T> reinterpret_pointer_cast(const shared_ptr<U>& ptr) {
    return shared_ptr<T>(ptr, reinterpret_cast<T*>(ptr.get()));
}
template <typename T, typename U>
shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U>& ptr) {
    T* tmp = dynamic_cast<T*>(ptr.get());
    if (tmp != nullptr) return shared_ptr<T>(ptr, tmp);
    else return nullptr;
}

MSTL_END_NAMESPACE__
#endif // MSTL_MEMORY_HPP__
