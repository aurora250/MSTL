#ifndef MSTL_ALLOCATOR_HPP__
#define MSTL_ALLOCATOR_HPP__
#include "type_traits.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T>
class standard_allocator {
    static_assert(is_allocable_v<T>, "allocator can`t alloc void, reference, function or const type.");
public:
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using self              = standard_allocator<T>;

private:
    std::allocator<T> alloc;

public:
    template <typename U>
    struct rebind {
        using other = standard_allocator<U>;
    };

    MSTL_CONSTEXPR standard_allocator() noexcept = default;
    template <typename U>
    MSTL_CONSTEXPR standard_allocator(const standard_allocator<U>&) noexcept {}
    MSTL_CONSTEXPR ~standard_allocator() noexcept = default;
    MSTL_CONSTEXPR self& operator =(const self&) noexcept = default;

    MSTL_ALLOCNODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(const size_type n) {
        return 0 == n ? 0 : alloc.allocate(n * sizeof(value_type));
    }
    MSTL_ALLOCNODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(void) {
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
class ctype_allocator {
    static_assert(is_allocable_v<T>, "allocator can`t alloc void, reference, function or const type.");
public:
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using self              = ctype_allocator<T>;

    template <typename U>
    struct rebind {
        using other = ctype_allocator<U>;
    };

    MSTL_CONSTEXPR ctype_allocator() noexcept = default;
    template <typename U>
    MSTL_CONSTEXPR ctype_allocator(const ctype_allocator<U>&) noexcept {}
    MSTL_CONSTEXPR ~ctype_allocator() noexcept = default;
    MSTL_CONSTEXPR self& operator =(const self&) noexcept = default;

    MSTL_ALLOCNODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(const size_type n) {
        return 0 == n ? 0 : static_cast<pointer>(std::malloc(n * sizeof(T)));
    }
    MSTL_ALLOCNODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(void) {
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
class new_allocator {
    static_assert(is_allocable_v<T>, "allocator can`t alloc void, reference, function or const type.");
public:
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using self              = new_allocator<T>;

    template <typename U>
    struct rebind {
        using other = new_allocator<U>;
    };

    MSTL_CONSTEXPR new_allocator() noexcept = default;
    template <typename U>
    MSTL_CONSTEXPR new_allocator(const new_allocator<U>&) noexcept {}
    MSTL_CONSTEXPR ~new_allocator() noexcept = default;
    MSTL_CONSTEXPR self& operator =(const self&) noexcept = default;

    MSTL_ALLOCNODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(size_type n) {
        return 0 == n ? 0 : static_cast<pointer>(::operator new(n * sizeof(T)));
    }
    MSTL_ALLOCNODISCARD MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(void) {
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

MSTL_END_NAMESPACE__
#endif // MSTL_ALLOCATOR_HPP__
