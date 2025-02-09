#ifndef MSTL_MEMORY_HPP__
#define MSTL_MEMORY_HPP__
#include "algobase.hpp"
MSTL_BEGIN_NAMESPACE__

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

template <class Ptr>
decltype(auto) ptr_const_cast(Ptr ptr) noexcept {
    using T = typename pointer_traits<Ptr>::element_type;
    using NonConst = remove_const_t<T>;
    using Dest = typename pointer_traits<Ptr>::template rebind<NonConst>;

    return pointer_traits<Dest>::pointer_to(const_cast<NonConst&>(*ptr));
}
template <class T>
decltype(auto) ptr_const_cast(T* ptr) noexcept {
    return const_cast<remove_const_t<T>*>(ptr);
}


template <typename T>
concept support_ptr_address = requires(const T & x) {
    typename pointer_traits<T>;
    pointer_traits<T>::to_address(x);
};

template <typename T>
MSTL_NODISCARD constexpr T* to_address(T* const x) noexcept {
    static_assert(!is_function_v<T>, "to address don`t support function types.");
    return x;
}

template <typename Ptr>
MSTL_NODISCARD constexpr decltype(auto) to_address(const Ptr& x) noexcept {
    if constexpr (support_ptr_address<Ptr>)
        return pointer_traits<Ptr>::to_address(x);
    else
        return MSTL::to_address(x.operator->());
}



template <class Ptr>
MSTL_NODISCARD constexpr auto unfancy(Ptr p) noexcept {
    return MSTL::addressof(*p);
}
template <class T>
MSTL_NODISCARD constexpr T* unfancy(T* p) noexcept {
    return p;
}
template <class Ptr>
constexpr decltype(auto) unfancy_maybe_null(Ptr p) noexcept {
    return p ? MSTL::addressof(*p) : nullptr;
}
template <class T>
constexpr T* unfancy_maybe_null(T* p) noexcept {
    return p;
}


template <class Ptr, class T>
using rebind_pointer_t = typename pointer_traits<Ptr>::template rebind<T>;

template <class Ptr, enable_if_t<!is_pointer_v<Ptr>, int> = 0>
MSTL_CONSTEXPR Ptr refancy(typename pointer_traits<Ptr>::element_type* _Ptr) noexcept {
    return pointer_traits<Ptr>::pointer_to(*_Ptr);
}
template <class Ptr, enable_if_t<is_pointer_v<Ptr>, int> = 0>
MSTL_CONSTEXPR Ptr refancy(Ptr _Ptr) noexcept {
    return _Ptr;
}
template <class Ptr, enable_if_t<!is_pointer_v<Ptr>, int> = 0>
MSTL_CONSTEXPR Ptr refancy_maybe_null(typename pointer_traits<Ptr>::element_type* _Ptr) noexcept {
    return _Ptr == nullptr ? Ptr() : pointer_traits<Ptr>::pointer_to(*_Ptr);
}
template <class Ptr, enable_if_t<is_pointer_v<Ptr>, int> = 0>
MSTL_CONSTEXPR Ptr refancy_maybe_null(Ptr p) noexcept {
    return p;
}


template <class T, class = void>
struct get_pointer_type {
    using type = typename T::value_type*;
};
template <class T>
struct get_pointer_type<T, void_t<typename T::pointer>> {
    using type = typename T::pointer;
};

template <class T, class = void>
struct get_difference_type {
    using pointer = typename get_pointer_type<T>::type;
    using type = typename pointer_traits<pointer>::difference_type;
};
template <class T>
struct get_difference_type<T, void_t<typename T::difference_type>> {
    using type = typename T::difference_type;
};

template <class T, class = void>
struct get_size_type {
    using type = make_unsigned_t<typename get_difference_type<T>::type>;
};
template <class T>
struct get_size_type<T, void_t<typename T::size_type>> {
    using type = typename T::size_type;
};

template <class T, class U, class = void>
struct get_alloc_rebind_type {
    using type = typename replace_first_parameter<U, T>::type;
};
template <class T, class U>
struct get_alloc_rebind_type<T, U, void_t<typename T::template rebind<U>::other>> {
    using type = typename T::template rebind<U>::other;
};

template <class Alloc>
struct allocator_traits {
    using allocator_type = Alloc;
    using value_type = typename Alloc::value_type;
    using pointer = typename get_pointer_type<Alloc>::type;
    using size_type = typename get_size_type<Alloc>::type;
    using difference_type = typename get_difference_type<Alloc>::type;

    template <class U>
    using rebind_alloc = typename get_alloc_rebind_type<Alloc, U>::type;
    template <class U>
    using rebind_traits = allocator_traits<rebind_alloc<U>>;

    MSTL_ALLOCNODISCARD static MSTL_CONSTEXPR MSTL_DECLALLOC pointer allocate(Alloc& alloc, const size_type n) {
        return alloc.allocate(n);
    }
    static _CONSTEXPR20 void deallocate(Alloc& alloc, pointer ptr, size_type n) {
        alloc.deallocate(ptr, n);
    }
};


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


template <class Alloc>
using alloc_ptr_t = typename allocator_traits<Alloc>::pointer;
template <class Alloc>
using alloc_size_t = typename allocator_traits<Alloc>::size_type;

template <class Alloc>
MSTL_CONSTEXPR void delete_internal(Alloc& alloc, typename Alloc::value_type* const ptr) noexcept {
    ptr->~T();
    if constexpr (is_same_v<alloc_ptr_t<Alloc>, typename Alloc::value_type*>) {
        allocator_traits<Alloc>::deallocate(alloc, ptr, 1);
    }
    else {
        using pointer = pointer_traits<alloc_ptr_t<Alloc>>;
        allocator_traits<Alloc>::deallocate(alloc, pointer::pointer_to(*ptr), 1);
    }
}


struct iterator_base;
struct container_base;

struct container_proxy {
    MSTL_CONSTEXPR container_proxy() noexcept = default;
    MSTL_CONSTEXPR container_proxy(container_base* _Mycont_) noexcept : cont_(_Mycont_) {}

    const container_base* cont_ = nullptr;
    mutable iterator_base* first_iter_ = nullptr;
};

struct container_base {
public:
    container_proxy* proxy_ = nullptr;

public:
    MSTL_CONSTEXPR container_base() noexcept = default;

    container_base(const container_base&) = delete;
    container_base& operator =(const container_base&) = delete;

    MSTL_CONSTEXPR void swap_proxy_and_iterators(container_base& cont) noexcept {
        container_proxy* tmp = proxy_;
        proxy_ = cont.proxy_;
        cont.proxy_ = tmp;

        if (proxy_) proxy_->cont_ = this;
        if (cont.proxy_)  cont.proxy_->cont_ = &cont;
    }

    template <class Alloc>
    MSTL_CONSTEXPR void alloc_proxy(Alloc&& _Al) {
        container_proxy* const proxy = MSTL::unfancy(_Al.allocate(1));
        MSTL::construct(proxy, this);
        proxy_ = proxy;
        proxy->cont_ = this;
    }

    template <class Alloc>
    MSTL_CONSTEXPR void reset_proxy(Alloc&& old_alloc, Alloc&& new_alloc) {
        container_proxy* const proxy = MSTL::unfancy(new_alloc.allocate(1));
        MSTL::construct(proxy, this);
        proxy->cont_ = this;
        MSTL::delete_internal(old_alloc, MSTL::exchange(proxy_, proxy));
    }

private:
    MSTL_CONSTEXPR void orphan_all() noexcept;
};

struct iterator_base {
public:
    MSTL_CONSTEXPR iterator_base() noexcept = default;
    MSTL_CONSTEXPR iterator_base(const iterator_base& iter) noexcept {
        *this = iter;
    }
    MSTL_CONSTEXPR iterator_base& operator =(const iterator_base& iter) noexcept {
        if (proxy_ == iter.proxy_) return *this;

        if (iter.proxy_) adopt_aux(iter.proxy_->cont_);
        else orphan_this();
        return *this;
    }

    MSTL_CONSTEXPR ~iterator_base() noexcept {
        orphan_this();
    }

    MSTL_CONSTEXPR void adopt(const container_base* cont) noexcept {
        adopt_aux(cont);
    }
    MSTL_CONSTEXPR const container_base* get_container() const noexcept {
        return proxy_ ? proxy_->cont_ : nullptr;
    }

    mutable container_proxy* proxy_ = nullptr;
    mutable iterator_base* next_iter_ = nullptr;

private:
    MSTL_CONSTEXPR void adopt_aux(const container_base* cont) noexcept {
        if (!cont) {
            orphan_this();
            return;
        }

        container_proxy* proxy = cont->proxy_;
        if (proxy_ != proxy) {
            if (proxy_) {
                orphan_this();
            }
            next_iter_ = proxy->first_iter_;
            proxy->first_iter_ = this;
            proxy_ = proxy;
        }
    }

    MSTL_CONSTEXPR void orphan_this() noexcept {
        if (!proxy_) return;

        iterator_base** next = &proxy_->first_iter_;
        while (*next && *next != this) {
            next = &(*next)->next_iter_;
        }
        MSTL_DEBUG_VERIFY__(*next, "iterator proxy invalid!");
        *next = next_iter_;
        proxy_ = nullptr;
    }
};

MSTL_CONSTEXPR void container_base::orphan_all() noexcept {
    if (!proxy_)  return;

    for (iterator_base* next = MSTL::exchange(proxy_->first_iter_, nullptr); next; next = next->next_iter_)
        next->proxy_ = nullptr;
}


template <typename, typename...>
struct map_key_extract {
    static constexpr bool extractable = false;
};
template <typename Key, typename Next>
struct map_key_extract<Key, Key, Next> {
    static constexpr bool extractable = true;
    static const Key& extract(const Key& key, const Next&) noexcept {
        return key;
    }
};
template <typename Key, typename First, typename Second>
struct map_key_extract<Key, pair<First, Second>> {
    static constexpr bool extractable = is_same_v<Key, remove_cvref_t<First>>;
    static const Key& extract(const pair<First, Second>& pir) {
        return pir.first;
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_MEMORY_HPP__
