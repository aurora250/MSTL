#ifndef MSTL_STRING_VIEW_HPP__
#define MSTL_STRING_VIEW_HPP__
#include "algobase.hpp"
#include "bitmap.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename CharT, typename IntT>
struct base_char_traits {
    using char_type = CharT;
    using int_type  = IntT;

    static MSTL_CONSTEXPR char_type* copy(char_type* const dest,
        const char_type* const srcs, const size_t count) noexcept {
#if MSTL_SUPPORT_MEM_INTRINSICS__
        __builtin_memcpy(dest, srcs, count * sizeof(char_type));
#else
#ifdef MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            for (size_t i = 0; i != count; ++i) 
                dest[i] = srcs[i];
            return dest;
        }
#endif // MSTL_VERSION_20__
        MSTL::memcpy(dest, srcs, count * sizeof(char_type));
#endif // MSTL_SUPPORT_MEM_INTRINSICS__
        return dest;
    }

    static MSTL_CONSTEXPR char_type* move(char_type* const dest,
        const char_type* const srcs, const size_t count) noexcept {
#ifdef MSTL_SUPPORT_MEM_INTRINSICS__
        __builtin_memmove(dest, srcs, count * sizeof(char_type));
#else 
#if MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            bool not_same = true;
            for (const char_type* src = srcs; src != srcs + count; ++src) {
                if (dest == src) {
                    not_same = false;
                    break;
                }
            }
            if (not_same) {
                for (size_t i = 0; i != count; ++i) 
                    dest[i] = srcs[i];
            }
            else {
                for (size_t i = count; i != 0; --i) 
                    dest[i - 1] = srcs[i - 1];
            }
            return dest;
        }
#endif // MSTL_VERSION_20__
        MSTL::memmove(dest, srcs, count * sizeof(char_type));
#endif // MSTL_SUPPORT_MEM_INTRINSICS__
        return dest;
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR int compare(const char_type* lh,
        const char_type* str2, size_t count) noexcept {
        for (; 0 < count; --count, ++lh, ++str2) {
            if (*lh != *str2) 
                return *lh < *str2 ? -1 : +1;
        }
        return 0;
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR size_t length(const char_type* str) noexcept {
        size_t count = 0;
        while (*str != char_type()) {
            ++count;
            ++str;
        }
        return count;
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR const char_type* find(const char_type* str,
        size_t count, const char_type& target) noexcept {
        for (; 0 < count; --count, ++str) {
            if (*str == target) return str;
        }
        return nullptr;
    }

    static MSTL_CONSTEXPR char_type* assign(char_type* const str, size_t count, const char_type target) noexcept {
        for (char_type* next = str; count > 0; --count, ++next) {
            *next = target;
        }
        return str;
    }
    static MSTL_CONSTEXPR void assign(char_type& lh, const char_type& rh) noexcept {
        lh = rh;
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR bool eq(const char_type lh, const char_type rh) noexcept {
        return lh == rh;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR bool lt(const char_type lh, const char_type rh) noexcept {
        return lh < rh;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR char_type to_char_type(const int_type str) noexcept {
        return static_cast<char_type>(str);
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type to_int_type(const char_type chr) noexcept {
        return static_cast<int_type>(chr);
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR bool eq_int_type(const int_type lh, const int_type rh) noexcept {
        return lh == rh;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type not_eof(const int_type rsc) noexcept {
        return rsc != eof() ? rsc : !eof();
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type eof() noexcept {
        return static_cast<int_type>(-1);
    }
};

template <class CharT>
struct wide_char_traits : private base_char_traits<CharT, uint16_t> {
private:
    using base_type = base_char_traits<CharT, uint16_t>;

public:
    using char_type = CharT;
    using int_type  = uint16_t;

    using base_type::copy;
    using base_type::move;

public:
    MSTL_NODISCARD static MSTL_CONSTEXPR int compare(const char_type* const lh,
        const char_type* const rh, const size_t n) noexcept {
#if MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            if constexpr (is_same_v<char_type, wchar_t>) {
                return __builtin_wmemcmp(lh, rh, n);
            }
            else {
                return base_type::compare(lh, rh, n);
            }
        }
#endif // MSTL_VERSION_20__
        return MSTL::wmemcmp(reinterpret_cast<const wchar_t*>(lh),
            reinterpret_cast<const wchar_t*>(rh), n);
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR size_t length(const char_type* str) noexcept {
#if MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            if constexpr (is_same_v<char_type, wchar_t>) {
                return __builtin_wcslen(str);
            }
            else {
                return base_type::length(str);
            }
        }
#endif // MSTL_VERSION_20__
        return MSTL::wcslen(reinterpret_cast<const wchar_t*>(str));
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR const char_type* find(
        const char_type* str, const size_t n, const char_type& chr) noexcept {
#if MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            if constexpr (is_same_v<char_type, wchar_t>) {
                return __builtin_wmemchr(str, chr, n);
            }
            else {
                return base_type::find(str, n, chr);
            }
        }
#endif // MSTL_VERSION_20__
        return reinterpret_cast<const char_type*>(
            MSTL::wmemchr(reinterpret_cast<const wchar_t*>(str), chr, n));
    }

    static MSTL_CONSTEXPR char_type* assign(char_type* const str, size_t n, const char_type chr) noexcept {
#if MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            return base_type::assign(str, n, chr);
        }
#endif // MSTL_VERSION_20__
        return reinterpret_cast<char_type*>(MSTL::wmemset(reinterpret_cast<wchar_t*>(str), chr, n));
    }
    static MSTL_CONSTEXPR void assign(char_type& lh, const char_type& rh) noexcept {
#if MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            return base_type::assign(lh, rh);
        }
#endif // MSTL_VERSION_20__
        lh = rh;
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR bool eq(const char_type lh, const char_type rh) noexcept {
        return lh == rh;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR bool lt(const char_type lh, const char_type rh) noexcept {
        return lh < rh;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR char_type to_char_type(const int_type rsc) noexcept {
        return rsc;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type to_int_type(const char_type chr) noexcept {
        return chr;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR bool eq_int_type(const int_type lh, const int_type rh) noexcept {
        return lh == rh;
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR int_type not_eof(const int_type rsc) noexcept {
        return rsc != eof() ? rsc : static_cast<int_type>(!eof());
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type eof() noexcept {
        return uint16_t(0xffff);
    }
};

template <typename CharT, typename IntT>
struct narrow_char_traits : private base_char_traits<CharT, IntT> {
private:
    using base_type = base_char_traits<CharT, IntT>;

public:
    using char_type = CharT;
    using int_type  = IntT;

    using base_type::copy;
    using base_type::move;

public:
    MSTL_NODISCARD static MSTL_CONSTEXPR int compare(const char_type* const lh,
        const char_type* const rh, const size_t n) noexcept {
#if MSTL_VERSION_17__
        return __builtin_memcmp(lh, rh, n);
#else
        return MSTL::memcmp(lh, rh, n);
#endif
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR size_t length(const char_type* const str) noexcept {
#ifdef MSTL_VERSION_17__
#ifdef MSTL_VERSION_20__
        if constexpr (is_same_v<char_type, char8_t>) {
#ifdef MSTL_SUPPORT_U8_INTRINSICS__
            return __builtin_u8strlen(str);
#else
            return base_type::length(str);
#endif // MSTL_SUPPORT_U8_INTRINSICS__
        }
        else
#endif // MSTL_VERSION_20__
        {
            return __builtin_strlen(str);
        }
#else
        return MSTL::strlen(reinterpret_cast<const char*>(str));
#endif // MSTL_VERSION_17__
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR const char_type* find(const char_type* const str,
        const size_t n, const char_type& chr) noexcept {
#ifdef MSTL_VERSION_17__
#ifdef MSTL_VERSION_20__
        if constexpr (is_same_v<char_type, char8_t>) {
#ifdef MSTL_SUPPORT_U8_INTRINSICS__
            return __builtin_u8memchr(str, chr, n);
#else
            return base_type::find(str, n, chr);
#endif // MSTL_SUPPORT_U8_INTRINSICS__
        }
        else
#endif // MSTL_VERSION_20__
        {
            return __builtin_char_memchr(str, chr, n);
        }
#else
        return static_cast<const char_type*>(MSTL::memchr(str, chr, n));
#endif // MSTL_VERSION_17__
    }

    static MSTL_CONSTEXPR char_type* assign(char_type* const str, size_t n, const char_type chr) noexcept {
#ifdef MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            return base_type::assign(str, n, chr);
        }
#endif // MSTL_VERSION_20__
        return static_cast<char_type*>(MSTL::memset(str, chr, n));
    }
    static MSTL_CONSTEXPR void assign(char_type& lh, const char_type& rh) noexcept {
#ifdef MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            return base_type::assign(lh, rh);
        }
#endif // MSTL_VERSION_20__
        lh = rh;
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR bool eq(const char_type lh, const char_type rh) noexcept {
        return lh == rh;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR bool lt(const char_type lh, const char_type rh) noexcept {
        return static_cast<unsigned char>(lh) < static_cast<unsigned char>(rh);
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR char_type to_char_type(const int_type rsc) noexcept {
        return static_cast<char_type>(rsc);
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type to_int_type(const char_type chr) noexcept {
        return static_cast<unsigned char>(chr);
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR bool eq_int_type(const int_type lh, const int_type rh) noexcept {
        return lh == rh;
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR int_type not_eof(const int_type rsc) noexcept {
        return rsc != eof() ? rsc : !eof();
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type eof() noexcept {
        return static_cast<int_type>(-1);
    }
};

template <typename CharT>
struct char_traits : public base_char_traits<CharT, int64_t> {};
TEMNULL__ struct char_traits<char> : public narrow_char_traits<char, int32_t> {};
TEMNULL__ struct char_traits<wchar_t> : public wide_char_traits<wchar_t> {};
#ifdef MSTL_VERSION_20__
TEMNULL__ struct char_traits<char8_t> : public narrow_char_traits<char8_t, uint32_t> {};
#endif
TEMNULL__ struct char_traits<char16_t> : public wide_char_traits<char16_t> {};
TEMNULL__ struct char_traits<char32_t> : public base_char_traits<char32_t, uint32_t> {};


template <typename Traits>
using char_traits_char_t = typename Traits::char_type;
template <typename Traits>
using char_traits_ptr_t = const typename Traits::char_type*;


template <typename Traits>
constexpr bool char_traits_equal(const char_traits_ptr_t<Traits> lh, const size_t lh_size,
    const char_traits_ptr_t<Traits> rh, const size_t rh_size) noexcept {
    if (lh_size != rh_size) return false;
    if (lh_size == 0u) return true;

    return Traits::compare(lh, rh, lh_size) == 0;
}

template <typename Traits>
constexpr int char_traits_compare(const char_traits_ptr_t<Traits> lh, const size_t lh_size,
    const char_traits_ptr_t<Traits> rh, const size_t rh_size) noexcept {
    const int state = Traits::compare(lh, rh, MSTL::min(lh_size, rh_size));
    if (state != 0) return state;

    if (lh_size < rh_size) return -1;
    if (lh_size > rh_size) return 1;
    return 0;
}

template <typename Traits>
constexpr size_t char_traits_find(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (rsc_size > dest_size || start > dest_size - rsc_size) return static_cast<size_t>(-1);
    if (rsc_size == 0)  return start;

    const auto may_match_end = dest + (dest_size - rsc_size) + 1;
    for (auto if_match = dest + start; ; ++if_match) {
        if_match = Traits::find(if_match, static_cast<size_t>(may_match_end - if_match), *rsc);
        if (!if_match) return static_cast<size_t>(-1);

        if (Traits::compare(if_match, rsc, rsc_size) == 0)
            return static_cast<size_t>(if_match - dest);
    }
}

template <typename Traits>
constexpr size_t char_traits_find_char(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_char_t<Traits> chr) noexcept {
    if (start < dest_size) {
        const auto found = Traits::find(dest + start, dest_size - start, chr);
        if (found) 
            return static_cast<size_t>(found - dest);
    }
    return static_cast<size_t>(-1);
}

template <typename Traits>
constexpr size_t char_traits_rfind(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (rsc_size == 0) return MSTL::min(start, dest_size);

    if (rsc_size <= dest_size) {
        for (auto if_match = dest + MSTL::min(start, dest_size - rsc_size);; --if_match) {
            if (Traits::eq(*if_match, *rsc) && Traits::compare(if_match, rsc, rsc_size) == 0) 
                return static_cast<size_t>(if_match - dest);
            
            if (if_match == dest) break;
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits>
constexpr size_t char_traits_rfind_char(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_char_t<Traits> chr) noexcept {
    if (dest_size != 0) {
        for (auto if_match = dest + MSTL::min(start, dest_size - 1);; --if_match) {
            if (Traits::eq(*if_match, chr)) 
                return static_cast<size_t>(if_match - dest);
            
            if (if_match == dest) break;
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits, bool Special = is_specialization_v<Traits, char_traits>>
constexpr size_t char_traits_find_first_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size, 
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (rsc_size != 0 && start < dest_size) {
        if constexpr (Special) {
            MSTL::string_bitmap<char_traits_char_t<Traits>> match;
            if (!match.mark(rsc, rsc + rsc_size)) {
                return (char_traits_find_first_of<Traits, false>)
                    (dest, dest_size, start, rsc, rsc_size);
            }
            const auto end = dest + dest_size;
            for (auto if_match = dest + start; if_match < end; ++if_match) {
                if (match.match(*if_match))
                    return static_cast<size_t>(if_match - dest);
            }
        }
        else {
            const auto end = dest + dest_size;
            for (auto if_match = dest + start; if_match < end; ++if_match) {
                if (Traits::find(rsc, rsc_size, *if_match)) 
                    return static_cast<size_t>(if_match - dest);
            }
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits, bool Special = is_specialization_v<Traits, char_traits>>
constexpr size_t char_traits_find_last_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size, 
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (rsc_size != 0 && dest_size != 0) {
        if constexpr (Special) {
            MSTL::string_bitmap<char_traits_char_t<Traits>> match;
            if (!match.mark(rsc, rsc + rsc_size)) 
                return (char_traits_find_last_of<Traits, false>)
                    (dest, dest_size, start, rsc, rsc_size);
            
            for (auto if_match = dest + MSTL::min(start, dest_size - 1);; --if_match) {
                if (match.match(*if_match)) 
                    return static_cast<size_t>(if_match - dest);
                
                if (if_match == dest) break;
            }
        }
        else {
            for (auto if_match = dest + MSTL::min(start, dest_size - 1);; --if_match) {
                if (Traits::find(rsc, rsc_size, *if_match)) 
                    return static_cast<size_t>(if_match - dest);

                if (if_match == dest) break;
            }
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits, bool Special = is_specialization_v<Traits, char_traits>>
constexpr size_t char_traits_find_first_not_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (start < dest_size) {
        if constexpr (Special) {
            MSTL::string_bitmap<char_traits_char_t<Traits>> match;
            if (!match.mark(rsc, rsc + rsc_size)) 
                return (char_traits_find_first_not_of<Traits, false>)
                    (dest, dest_size, start, rsc, rsc_size);
            
            const auto end = dest + dest_size;
            for (auto if_match = dest + start; if_match < end; ++if_match) {
                if (!match.match(*if_match)) 
                    return static_cast<size_t>(if_match - dest);
            }
        }
        else {
            const auto end = dest + dest_size;
            for (auto if_match = dest + start; if_match < end; ++if_match) {
                if (!Traits::find(rsc, rsc_size, *if_match)) 
                    return static_cast<size_t>(if_match - dest);
            }
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits>
constexpr size_t char_traits_find_not_char(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_char_t<Traits> chr) noexcept {
    if (start < dest_size) {
        const auto end = dest + dest_size;
        for (auto if_match = dest + start; if_match < end; ++if_match) {
            if (!Traits::eq(*if_match, chr)) 
                return static_cast<size_t>(if_match - dest);
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits, bool Special = is_specialization_v<Traits, char_traits>>
constexpr size_t char_traits_find_last_not_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size, 
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (dest_size != 0) {
        if constexpr (Special) {
            MSTL::string_bitmap<char_traits_char_t<Traits>> match;
            if (!match.mark(rsc, rsc + rsc_size)) 
                return (char_traits_find_last_not_of<Traits, false>)
                    (dest, dest_size, start, rsc, rsc_size);
            
            for (auto if_match = dest + MSTL::min(start, dest_size - 1);; --if_match) {
                if (!match.match(*if_match)) 
                    return static_cast<size_t>(if_match - dest);
                
                if (if_match == dest) break;
            }
        }
        else {
            for (auto if_match = dest + MSTL::min(start, dest_size - 1);; --if_match) {
                if (!Traits::find(rsc, rsc_size, *if_match)) 
                    return static_cast<size_t>(if_match - dest);
                
                if (if_match == dest) break;
            }
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits>
constexpr size_t char_traits_rfind_not_char(const char_traits_ptr_t<Traits> dest, const size_t dest_size, 
    const size_t start, const char_traits_char_t<Traits> chr) noexcept {
    if (dest_size != 0) {
        for (auto if_match = dest + MSTL::min(start, dest_size - 1);; --if_match) {
            if (!Traits::eq(*if_match, chr)) 
                return static_cast<size_t>(if_match - dest);

            if (if_match == dest) break;
        }
    }
    return static_cast<size_t>(-1);
}


template <typename CharT, typename Traits = char_traits<CharT>>
class basic_string_view;


template <typename Traits>
class string_view_iterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = typename Traits::char_type;
    using difference_type   = ptrdiff_t;
    using pointer           = const value_type*;
    using reference         = const value_type&;

private:
    pointer data_ = nullptr;
    size_t size_ = 0;
    size_t idx_ = 0;

    friend basic_string_view<value_type, Traits>;

public:
    constexpr string_view_iterator() noexcept = default;

    MSTL_CONSTEXPR string_view_iterator(const pointer data, const size_t size, const size_t off) noexcept
        : data_(data), size_(size), idx_(off) {
    }

    MSTL_NODISCARD MSTL_CONSTEXPR reference operator *() const noexcept {
        MSTL_DEBUG_VERIFY__(data_, "cannot dereference value-initialized string view iterator");
        MSTL_DEBUG_VERIFY__(idx_ < size_, "cannot dereference end string view iterator");
        return data_[idx_];
    }

    MSTL_NODISCARD MSTL_CONSTEXPR pointer operator ->() const noexcept {
        return &(operator*());
    }

    MSTL_CONSTEXPR string_view_iterator& operator ++() noexcept {
        MSTL_DEBUG_VERIFY__(data_, "cannot increment value-initialized string_view iterator");
        MSTL_DEBUG_VERIFY__(idx_ < size_, "cannot increment string_view iterator past end");
        ++idx_;
        return *this;
    }

    MSTL_CONSTEXPR string_view_iterator operator ++(int) noexcept {
        string_view_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    MSTL_CONSTEXPR string_view_iterator& operator --() noexcept {
        MSTL_DEBUG_VERIFY__(data_, "cannot decrement value-initialized string_view iterator");
        MSTL_DEBUG_VERIFY__(idx_ != 0, "cannot decrement string_view iterator before begin");
        --idx_;
        return *this;
    }

    MSTL_CONSTEXPR string_view_iterator operator --(int) noexcept {
        string_view_iterator tmp(*this);
        --*this;
        return tmp;
    }

    MSTL_CONSTEXPR string_view_iterator& operator +=(const difference_type n) noexcept {
        if (n != 0)
            MSTL_DEBUG_VERIFY__(data_, "cannot seek value-initialized string_view iterator");
        if (n < 0) {
            MSTL_DEBUG_VERIFY__(
                idx_ >= size_t(0) - static_cast<size_t>(n), "cannot seek string_view iterator before begin");
        }
        if (n > 0)
            MSTL_DEBUG_VERIFY__(size_ - idx_ >= static_cast<size_t>(n), "cannot seek string_view iterator after end");
        idx_ += static_cast<size_t>(n);
        return *this;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR string_view_iterator operator +(const difference_type n) const noexcept {
        string_view_iterator tmp(*this);
        tmp += n;
        return tmp;
    }

    friend MSTL_NODISCARD MSTL_CONSTEXPR string_view_iterator operator +(
        const difference_type n, string_view_iterator iter) noexcept {
        iter += n;
        return iter;
    }

    MSTL_CONSTEXPR string_view_iterator& operator -=(const difference_type n) noexcept {
        if (n != 0) {
            MSTL_DEBUG_VERIFY__(data_, "cannot seek value-initialized string_view iterator");
        }

        if (n > 0) {
            MSTL_DEBUG_VERIFY__(idx_ >= static_cast<size_t>(n), "cannot seek string_view iterator before begin");
        }

        if (n < 0) {
            MSTL_DEBUG_VERIFY__(size_ - idx_ >= size_t(0) - static_cast<size_t>(n),
                "cannot seek string_view iterator after end");
        }
        idx_ -= static_cast<size_t>(n);
        return *this;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR string_view_iterator operator -(const difference_type n) const noexcept {
        string_view_iterator tmp(*this);
        tmp -= n;
        return tmp;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR difference_type operator -(const string_view_iterator& iter) const noexcept {
        MSTL_DEBUG_VERIFY__(data_ == iter.data_ && size_ == iter.size_,
            "cannot subtract incompatible string_view iterators");
        return static_cast<difference_type>(idx_ - iter.idx_);
    }

    MSTL_NODISCARD MSTL_CONSTEXPR reference operator [](const difference_type n) const noexcept {
        return *(*this + n);
    }

    MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(const string_view_iterator& iter) const noexcept {
        MSTL_DEBUG_VERIFY__(data_ == iter.data_ && size_ == iter.size_,
            "cannot compare incompatible string_view iterators for equality");
        return idx_ == iter.idx_;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(const string_view_iterator& iter) const noexcept {
        return !(*this == iter);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(const string_view_iterator& iter) const noexcept {
        MSTL_DEBUG_VERIFY__(data_ == iter.data_ && size_ == iter.size_,
            "cannot compare incompatible string_view iterators");
        return idx_ < iter.idx_;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(const string_view_iterator& iter) const noexcept {
        return iter < *this;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(const string_view_iterator& iter) const noexcept {
        return !(iter < *this);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(const string_view_iterator& iter) const noexcept {
        return !(*this < iter);
    }
};


template <typename CharT, typename Traits>
class basic_string_view {
public:
    static_assert(is_same_v<CharT, typename Traits::char_type>, 
        "char type of basic string view should be same with char traits.");

    static_assert(!is_array_v<CharT> && is_trivial_v<CharT> && is_standard_layout_v<CharT>,
        "basic string view only contains non-array trivial standard-layout types.");

    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using traits_type       = Traits;
    using value_type        = CharT;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using const_iterator    = string_view_iterator<Traits>;
    using iterator          = const_iterator;
    using const_reverse_iterator = MSTL::reverse_iterator<const_iterator>;
    using reverse_iterator  = const_reverse_iterator;
    using self              = basic_string_view<CharT, Traits>;

    static constexpr auto npos = static_cast<size_type>(-1);

private:
    const_pointer data_;
    size_type size_;

    inline constexpr void range_check(const size_type n) const {
        MSTL_DEBUG_VERIFY__(size_ < n, "basic string view index out of ranges.");
    }

    constexpr size_type clamp_size(const size_type position, const size_type size) const noexcept {
        return MSTL::min(size, size_ - position);
    }

public:
    constexpr basic_string_view() noexcept : data_(), size_(0) {}

    constexpr basic_string_view(const self&) noexcept = default;
    constexpr basic_string_view& operator =(const self&) noexcept = default;

    constexpr basic_string_view(const const_pointer str) noexcept
        : data_(str), size_(Traits::length(str)) {
    }
    constexpr basic_string_view(const const_pointer str, const size_type n) noexcept
        : data_(str), size_(n) {}

    MSTL_NODISCARD constexpr const_iterator begin() const noexcept { return const_iterator(data_, size_, 0); }
    MSTL_NODISCARD constexpr const_iterator end() const noexcept { return const_iterator(data_, size_, size_); }
    MSTL_NODISCARD constexpr const_iterator cbegin() const noexcept { return begin(); }
    MSTL_NODISCARD constexpr const_iterator cend() const noexcept { return end(); }
    MSTL_NODISCARD constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    MSTL_NODISCARD constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    MSTL_NODISCARD constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    MSTL_NODISCARD constexpr const_reverse_iterator crend() const noexcept { return rend(); }

    MSTL_NODISCARD constexpr size_type size() const noexcept { return size_; }
    MSTL_NODISCARD constexpr size_type max_size() const noexcept {
        return MSTL::min(static_cast<size_t>(INT64_MAX), static_cast<size_t>(-1) / sizeof(CharT));
    }
    MSTL_NODISCARD constexpr size_type length() const noexcept { return size_; }
    MSTL_NODISCARD constexpr bool empty() const noexcept { return size_ == 0; }

    MSTL_NODISCARD constexpr const_pointer data() const noexcept { return data_; }
    MSTL_NODISCARD constexpr const_reference front() const noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "cannot call front on empty string_view");
        return data_[0];
    }
    MSTL_NODISCARD constexpr const_reference back() const noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "cannot call back on empty string_view");
        return data_[size_ - 1];
    }

    MSTL_NODISCARD constexpr const_reference operator [](const size_type n) const noexcept {
        range_check(n);
        return data_[n];
    }
    MSTL_NODISCARD constexpr const_reference at(const size_type n) const {
        range_check(n);
        return data_[n];
    }

    constexpr void remove_prefix(const size_type n) noexcept {
        MSTL_DEBUG_VERIFY__(size_ >= n, "cannot remove prefix longer than total size");
        data_ += n;
        size_ -= n;
    }
    constexpr void remove_suffix(const size_type n) noexcept {
        MSTL_DEBUG_VERIFY__(size_ >= n, "cannot remove suffix longer than total size");
        size_ -= n;
    }

    constexpr void swap(self& view) noexcept {
        const self tmp(view);
        view = *this;
        *this = tmp;
    }

    MSTL_CONSTEXPR size_type copy(CharT* const str, size_type count, const size_type off = 0) const {
        range_check(off);
        count = clamp_size(off, count);
        Traits::copy(str, data_ + off, count);
        return count;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR self substr(const size_type off = 0, size_type count = npos) const {
        range_check(off);
        count = clamp_size(off, count);
        return self(data_ + off, count);
    }

    MSTL_CONSTEXPR bool equal_to(const self view) const noexcept {
        return (char_traits_equal<Traits>)(data_, size_, view.data_, view.size_);
    }

    MSTL_NODISCARD constexpr int compare(const self view) const noexcept {
        return (char_traits_compare<Traits>)(data_, size_, view.data_, view.size_);
    }
    MSTL_NODISCARD constexpr int compare(const size_type off, const size_type n, const self view) const {
        return substr(off, n).compare(view);
    }
    MSTL_NODISCARD constexpr int compare(const size_type off, const size_type n, const self view,
        const size_type roff, const size_type count) const {
        return substr(off, n).compare(view.substr(roff, count));
    }
    MSTL_NODISCARD constexpr int compare(const CharT* const str) const noexcept {
        return compare(self(str));
    }
    MSTL_NODISCARD constexpr int compare(const size_type off, const size_type n, const CharT* const str) const {
        return substr(off, n).compare(self(str));
    }
    MSTL_NODISCARD constexpr int compare(const size_type off, const size_type n,
        const CharT* const str, const size_type count) const {
        return substr(off, n).compare(self(str, count));
    }

    MSTL_NODISCARD constexpr size_type find(const self view, const size_type n = 0) const noexcept {
        return (char_traits_find<Traits>)(data_, size_, n, view.data_, view.size_);
    }
    MSTL_NODISCARD constexpr size_type find(const CharT chr, const size_type n = 0) const noexcept {
        return (char_traits_find_char<Traits>)(data_, size_, n, chr);
    }
    MSTL_NODISCARD constexpr size_type find(const CharT* const str,
        const size_type off, const size_type count) const noexcept {
        return (char_traits_find<Traits>)(data_, size_, off, str, count);
    }
    MSTL_NODISCARD constexpr size_type find(const CharT* const str, const size_type off = 0) const noexcept {
        return (char_traits_find<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD constexpr size_type rfind(const self view, const size_type off = npos) const noexcept {
        return (char_traits_rfind<Traits>)(data_, size_, off, view.data_, view.size_);
    }
    MSTL_NODISCARD constexpr size_type rfind(const CharT chr, const size_type n = npos) const noexcept {
        return (char_traits_rfind_char<Traits>)(data_, size_, n, chr);
    }
    MSTL_NODISCARD constexpr size_type rfind(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_rfind<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD constexpr size_type rfind(const CharT* const str, const size_type off = npos) const noexcept {
        return (char_traits_rfind<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD constexpr size_type find_first_of(const self view, const size_type off = 0) const noexcept {
        return (char_traits_find_first_of<Traits>)(data_, size_, off, view.data_, view.size_);
    }
    MSTL_NODISCARD constexpr size_type find_first_of(const CharT chr, const size_type off = 0) const noexcept {
        return (char_traits_find_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD constexpr size_type find_first_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_first_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD constexpr size_type find_first_of(const CharT* const str, const size_type off = 0) const noexcept {
        return (char_traits_find_first_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD constexpr size_type find_last_of(const self view, const size_type off = npos) const noexcept {
        return (char_traits_find_last_of<Traits>)(data_, size_, off, view.data_, view.size_);
    }
    MSTL_NODISCARD constexpr size_type find_last_of(const CharT chr, const size_type off = npos) const noexcept {
        return (char_traits_rfind_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD constexpr size_type find_last_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_last_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD constexpr size_type find_last_of(const CharT* const str, const size_type off = npos) const noexcept {
        return (char_traits_find_last_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD constexpr size_type find_first_not_of(const self view,
        const size_type off = 0) const noexcept {
        return (char_traits_find_first_not_of<Traits>)(data_, size_, off, view.data_, view.size_);
    }
    MSTL_NODISCARD constexpr size_type find_first_not_of(const CharT chr, const size_type off = 0) const noexcept {
        return (char_traits_find_not_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD constexpr size_type find_first_not_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_first_not_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD constexpr size_type find_first_not_of(const CharT* const str, const size_type off = 0) const noexcept {
        return (char_traits_find_first_not_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD constexpr size_type find_last_not_of(const self view,
        const size_type off = npos) const noexcept {
        return (char_traits_find_last_not_of<Traits>)(data_, size_, off, view.data_, view.size_);
    }
    MSTL_NODISCARD constexpr size_type find_last_not_of(const CharT chr, const size_type off = npos) const noexcept {
        return (char_traits_rfind_not_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD constexpr size_type find_last_not_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_last_not_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD constexpr size_type find_last_not_of(const CharT* const str,
        const size_type off = npos) const noexcept {
        return (char_traits_find_last_not_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }
};
template <typename CharT, typename Traits>
MSTL_NODISCARD constexpr bool operator ==(
    const basic_string_view<CharT, Traits> lh, const basic_string_view<CharT, Traits> rh) noexcept {
    return lh.equal_to(rh);
}
template <typename CharT, typename Traits>
MSTL_NODISCARD constexpr bool operator !=(
    const basic_string_view<CharT, Traits> lh, const basic_string_view<CharT, Traits> rh) noexcept {
    return !lh.equal_to(rh);
}
template <typename CharT, typename Traits>
MSTL_NODISCARD constexpr bool operator <(
    const basic_string_view<CharT, Traits> lh, const basic_string_view<CharT, Traits> rh) noexcept {
    return lh.compare(rh) < 0;
}
template <typename CharT, typename Traits>
MSTL_NODISCARD constexpr bool operator >(
    const basic_string_view<CharT, Traits> lh, const basic_string_view<CharT, Traits> rh) noexcept {
    return lh.compare(rh) > 0;
}
template <typename CharT, typename Traits>
MSTL_NODISCARD constexpr bool operator <=(
    const basic_string_view<CharT, Traits> lh, const basic_string_view<CharT, Traits> rh) noexcept {
    return lh.compare(rh) <= 0;
}
template <typename CharT, typename Traits>
MSTL_NODISCARD constexpr bool operator >=(
    const basic_string_view<CharT, Traits> lh, const basic_string_view<CharT, Traits> rh) noexcept {
    return lh.compare(rh) >= 0;
}

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
#ifdef MSTL_VERSION_20__
using u8string_view = basic_string_view<char8_t>;
#endif // MSTL_VERSION_20__
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;


inline namespace string_operator {
    MSTL_NODISCARD constexpr string_view operator ""sv(const char* str, size_t len) noexcept {
        return string_view(str, len);
    }
    MSTL_NODISCARD constexpr wstring_view operator ""sv(const wchar_t* str, size_t len) noexcept {
        return wstring_view(str, len);
    }
#ifdef MSTL_VERSION_20__
    MSTL_NODISCARD constexpr u8string_view operator ""sv(const char8_t* str, size_t len) noexcept {
        return u8string_view(str, len);
    }
#endif // MSTL_VERSION_20__
    MSTL_NODISCARD constexpr u16string_view operator ""sv(const char16_t* str, size_t len) noexcept {
        return u16string_view(str, len);
    }
    MSTL_NODISCARD constexpr u32string_view operator ""sv(const char32_t* str, size_t len) noexcept {
        return u32string_view(str, len);
    }
}


// DJB2 is a non-cryptographic hash algorithm
// with simple implement, fast speed and evenly distributed.
// but in some special cases, there will still occur hash conflicts.
constexpr size_t DJB2_hash(const char* str, size_t len) noexcept {
    size_t hash = 5381;
    for (size_t i = 0; i < len; ++i) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(str[i]);
    }
    return hash;
}

// use switch penetrate
#pragma warning(push)
#pragma warning(disable: 26819)

#ifdef MSTL_DATA_BUS_WIDTH_32__

constexpr uint32_t BLOCK_MULTIPLIER32_1 = 0xcc9e2d51;
constexpr uint32_t BLOCK_MULTIPLIER32_2 = 0x1b873593;
constexpr uint32_t HASH_UPDATE_CONSTANT32 = 0xe6546b64;
constexpr uint32_t FINAL_MIX_MULTIPLIER32_1 = 0x85ebca6b;
constexpr uint32_t FINAL_MIX_MULTIPLIER32_2 = 0xc2b2ae35;

constexpr uint32_t hash_rotate_x32(uint32_t x, int r) noexcept {
    return (x << r) | (x >> (32 - r));
}

inline uint32_t MurmurHash_x32(const char* key, size_t len, uint32_t seed) noexcept {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(key);
    const size_t nblocks = len / 4;

    uint32_t h1 = seed;

    const uint32_t* blocks = reinterpret_cast<const uint32_t*>(data);
    for (size_t i = 0; i < nblocks; ++i) {
        uint32_t k1 = blocks[i];

        k1 *= BLOCK_MULTIPLIER32_1;
        k1 = hash_rotate_x32(k1, 15);
        k1 *= BLOCK_MULTIPLIER32_2;

        h1 ^= k1;
        h1 = hash_rotate_x32(h1, 13);
        h1 = h1 * 5 + HASH_UPDATE_CONSTANT32;
    }

    const uint8_t* tail = data + nblocks * 4;
    uint32_t k1 = 0;

    switch (len & 3) {
    case 3:
        k1 ^= static_cast<uint32_t>(tail[2]) << 16;
    case 2:
        k1 ^= static_cast<uint32_t>(tail[1]) << 8;
    case 1:
        k1 ^= static_cast<uint32_t>(tail[0]);
        k1 *= BLOCK_MULTIPLIER32_1;
        k1 = hash_rotate_x32(k1, 15);
        k1 *= BLOCK_MULTIPLIER32_2;
        h1 ^= k1;
    }

    h1 ^= static_cast<uint32_t>(len);

    h1 ^= h1 >> 16;
    h1 *= FINAL_MIX_MULTIPLIER32_1;
    h1 ^= h1 >> 13;
    h1 *= FINAL_MIX_MULTIPLIER32_2;
    h1 ^= h1 >> 16;

    return h1;
}

#endif

#ifdef MSTL_DATA_BUS_WIDTH_64__

constexpr uint64_t hash_rotate_x64(uint64_t x, int r) noexcept {
    return (x << r) | (x >> (64 - r));
}

constexpr uint64_t MULTIPLIER64_1 = 0x87c37b91114253d5ULL;
constexpr uint64_t MULTIPLIER64_2 = 0x4cf5ad432745937fULL;
constexpr uint64_t FINAL_MIX_MULTIPLIER64_1 = 0xff51afd7ed558ccdULL;
constexpr uint64_t FINAL_MIX_MULTIPLIER64_2 = 0xc4ceb9fe1a85ec53ULL;
constexpr uint64_t HASH_UPDATE_CONSTANT64_1 = 0x52dce729;
constexpr uint64_t HASH_UPDATE_CONSTANT64_2 = 0x38495ab5;

constexpr uint64_t hash_mix_x64(uint64_t k) noexcept {
    k ^= k >> 33;
    k *= FINAL_MIX_MULTIPLIER64_1;
    k ^= k >> 33;
    k *= FINAL_MIX_MULTIPLIER64_2;
    k ^= k >> 33;
    return k;
}

// MurmurHash is a non-cryptographic hash algorithm 
// with fast speed, low collision rate and customizable seed.
// MurmurHash_x64 is MurmurHash3 version.
inline pair<size_t, size_t> MurmurHash_x64(const char* key, size_t len, uint32_t seed) noexcept {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(key);
    const size_t nblocks = len / 16;

    uint64_t h1 = seed;
    uint64_t h2 = seed;

    const uint64_t* blocks = reinterpret_cast<const uint64_t*>(data);
    for (size_t i = 0; i < nblocks; i++) {
        uint64_t k1 = blocks[i * 2];
        uint64_t k2 = blocks[i * 2 + 1];

        k1 *= MULTIPLIER64_1;
        k1 = hash_rotate_x64(k1, 31);
        k1 *= MULTIPLIER64_2;
        h1 ^= k1;

        h1 = hash_rotate_x64(h1, 27);
        h1 += h2;
        h1 = h1 * 5 + HASH_UPDATE_CONSTANT64_1;

        k2 *= MULTIPLIER64_2;
        k2 = hash_rotate_x64(k2, 33);
        k2 *= MULTIPLIER64_1;
        h2 ^= k2;

        h2 = hash_rotate_x64(h2, 31);
        h2 += h1;
        h2 = h2 * 5 + HASH_UPDATE_CONSTANT64_2;
    }

    const uint8_t* tail = data + nblocks * 16;
    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch (len & 15) {
    case 15: k2 ^= static_cast<uint64_t>(tail[14]) << 48;
    case 14: k2 ^= static_cast<uint64_t>(tail[13]) << 40;
    case 13: k2 ^= static_cast<uint64_t>(tail[12]) << 32;
    case 12: k2 ^= static_cast<uint64_t>(tail[11]) << 24;
    case 11: k2 ^= static_cast<uint64_t>(tail[10]) << 16;
    case 10: k2 ^= static_cast<uint64_t>(tail[9]) << 8;
    case 9:  k2 ^= static_cast<uint64_t>(tail[8]) << 0;
        k2 *= MULTIPLIER64_2;
        k2 = hash_rotate_x64(k2, 33);
        k2 *= MULTIPLIER64_1;
        h2 ^= k2;
    case 8:  k1 ^= static_cast<uint64_t>(tail[7]) << 56;
    case 7:  k1 ^= static_cast<uint64_t>(tail[6]) << 48;
    case 6:  k1 ^= static_cast<uint64_t>(tail[5]) << 40;
    case 5:  k1 ^= static_cast<uint64_t>(tail[4]) << 32;
    case 4:  k1 ^= static_cast<uint64_t>(tail[3]) << 24;
    case 3:  k1 ^= static_cast<uint64_t>(tail[2]) << 16;
    case 2:  k1 ^= static_cast<uint64_t>(tail[1]) << 8;
    case 1:  k1 ^= static_cast<uint64_t>(tail[0]) << 0;
        k1 *= MULTIPLIER64_1;
        k1 = hash_rotate_x64(k1, 31);
        k1 *= MULTIPLIER64_2;
        h1 ^= k1;
    };

    h1 ^= len;
    h2 ^= len;
    h1 += h2;
    h2 += h1;
    h1 = hash_mix_x64(h1);
    h2 = hash_mix_x64(h2);
    h1 += h2;
    h2 += h1;
    return MSTL::make_pair(h1, h2);
}

#endif
#pragma warning(pop)


inline size_t string_hash(const char* s, size_t len, uint32_t seed) noexcept {
#if defined(MSTL_DATA_BUS_WIDTH_64__)
    pair<size_t, size_t> p = MurmurHash_x64(s, len, seed);
    return p.first ^ p.second;
#elif defined(MSTL_DATA_BUS_WIDTH_32__)
    return (size_t)MurmurHash_x32(s, len, seed);
#else 
    return DJB2_hash(s, len);
#endif
}

TEMNULL__ struct hash<char*> {
    MSTL_NODISCARD size_t operator ()(const char* str) const noexcept {
        return string_hash(str, MSTL::strlen(str), 0);
    }
}; 
TEMNULL__ struct hash<const char*> {
    MSTL_NODISCARD size_t operator ()(const char* str) const noexcept {
        return string_hash(str, MSTL::strlen(str), 0);
    }
};
TEMNULL__ struct hash<wchar_t*> {
    MSTL_NODISCARD size_t operator ()(const wchar_t* str) const noexcept {
        return FNV_hash(reinterpret_cast<const byte_t*>(str), sizeof(wchar_t) * char_traits<wchar_t>::length(str));
    }
};
TEMNULL__ struct hash<const wchar_t*> {
    MSTL_NODISCARD size_t operator ()(const wchar_t* str) const noexcept {
        return FNV_hash(reinterpret_cast<const byte_t*>(str), sizeof(wchar_t) * char_traits<wchar_t>::length(str));
    }
};
#define CHAR_PTR_HASH_STRUCTS__(OPT) \
TEMNULL__ struct hash<OPT*> { \
    MSTL_NODISCARD size_t operator ()(const OPT* str) const noexcept { \
        return FNV_hash(reinterpret_cast<const byte_t*>(str), sizeof(OPT) * char_traits<OPT>::length(str)); \
    } \
}; \
TEMNULL__ struct hash<const OPT*> { \
    MSTL_NODISCARD size_t operator ()(const OPT* str) const noexcept { \
        return FNV_hash(reinterpret_cast<const byte_t*>(str), sizeof(OPT) * char_traits<OPT>::length(str)); \
    } \
};
MSTL_MACRO_RANGES_UNICODE_CHARS(CHAR_PTR_HASH_STRUCTS__)

TEMNULL__ struct hash<MSTL::string_view> {
    MSTL_NODISCARD size_t operator ()(const MSTL::string_view str) const noexcept {
        return string_hash(str.data(), str.size(), 0);
    }
};
template <typename CharT, typename Traits>
struct hash<basic_string_view<CharT, Traits>> {
    MSTL_NODISCARD size_t operator ()(const basic_string_view<CharT, Traits> str) const noexcept {
        return FNV_hash(reinterpret_cast<const byte_t*>(str.data()), sizeof(CharT) * str.size());
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_STRING_VIEW_HPP__
