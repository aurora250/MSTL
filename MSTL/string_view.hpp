#ifndef MSTL_STRING_VIEW_HPP__
#define MSTL_STRING_VIEW_HPP__
#include "algobase.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename CharT, typename IntT>
struct base_char_traits {
    using char_type = CharT;
    using int_type  = IntT;

    static constexpr char_type* copy(char_type* const dest,
        const char_type* const srcs, const size_t count) noexcept {
#ifdef MSTL_COMPILER_CLANG__
        ::__builtin_memcpy(dest, srcs, count * sizeof(char_type));
#else
#ifdef MSTL_VERSION_20__
        if (_MSTL is_constant_evaluated()) {
            for (size_t i = 0; i != count; ++i) 
                dest[i] = srcs[i];
            return dest;
        }
#endif // MSTL_VERSION_20__
        _MSTL memory_copy(dest, srcs, count * sizeof(char_type));
#endif // MSTL_COMPILER_CLANG__
        return dest;
    }

    static constexpr char_type* move(char_type* const dest,
        const char_type* const srcs, const size_t count) noexcept {
#if defined(MSTL_COMPILER_CLANG__) && __has_builtin(__builtin_memmove)
        ::__builtin_memmove(dest, srcs, count * sizeof(char_type));
#else 
#if MSTL_VERSION_20__
        if (_MSTL is_constant_evaluated()) {
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
        _MSTL memory_move(dest, srcs, count * sizeof(char_type));
#endif // MSTL_SUPPORT_MEM_INTRINSICS__
        return dest;
    }

    MSTL_NODISCARD static constexpr int compare(const char_type* lh,
        const char_type* str2, size_t count) noexcept {
        for (; 0 < count; --count, ++lh, ++str2) {
            if (*lh != *str2) 
                return *lh < *str2 ? -1 : +1;
        }
        return 0;
    }

    MSTL_NODISCARD static constexpr size_t length(const char_type* str) noexcept {
        size_t count = 0;
        while (*str != static_cast<char_type>(0)) {
            ++count;
            ++str;
        }
        return count;
    }

    MSTL_NODISCARD static constexpr const char_type* find(const char_type* str,
        size_t count, const char_type& target) noexcept {
        for (; 0 < count; --count, ++str) {
            if (*str == target) return str;
        }
        return nullptr;
    }

    static constexpr char_type* assign(char_type* const str, size_t count, const char_type chr) noexcept {
        for (char_type* next = str; count > 0; --count, ++next) {
            *next = chr;
        }
        return str;
    }
    static constexpr void assign(char_type& lh, const char_type& rh) noexcept {
        lh = rh;
    }

    MSTL_NODISCARD static constexpr bool eq(const char_type lh, const char_type rh) noexcept {
        return lh == rh;
    }
    MSTL_NODISCARD static constexpr bool lt(const char_type lh, const char_type rh) noexcept {
        return lh < rh;
    }
    MSTL_NODISCARD static constexpr char_type to_char_type(const int_type str) noexcept {
        return static_cast<char_type>(str);
    }
    MSTL_NODISCARD static constexpr int_type to_int_type(const char_type chr) noexcept {
        return static_cast<int_type>(chr);
    }
    MSTL_NODISCARD static constexpr bool eq_int_type(const int_type lh, const int_type rh) noexcept {
        return lh == rh;
    }
    MSTL_NODISCARD static constexpr int_type not_eof(const int_type rsc) noexcept {
        return rsc != eof() ? rsc : !eof();
    }
    MSTL_NODISCARD static constexpr int_type eof() noexcept {
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
    MSTL_NODISCARD static constexpr int compare(const char_type* const lh,
        const char_type* const rh, const size_t n) noexcept {
#if MSTL_VERSION_20__
        if (_MSTL is_constant_evaluated()) {
            MSTL_IF_CONSTEXPR (is_same_v<char_type, wchar_t>) {
                return __builtin_wmemcmp(lh, rh, n);
            }
            else {
                return base_type::compare(lh, rh, n);
            }
        }
#endif // MSTL_VERSION_20__
        return _MSTL wchar_memory_compare(reinterpret_cast<const wchar_t*>(lh),
            reinterpret_cast<const wchar_t*>(rh), n);
    }

    MSTL_NODISCARD static constexpr size_t length(const char_type* str) noexcept {
#if MSTL_VERSION_20__
        if (_MSTL is_constant_evaluated()) {
            MSTL_IF_CONSTEXPR (is_same_v<char_type, wchar_t>) {
#if defined(MSTL_COMPILER_MSVC__) || defined(MSTL_COMPILER_CLANG__)
                return __builtin_wcslen(str);
#else
                return _MSTL wstring_length(str);
#endif
            }
            else {
                return base_type::length(str);
            }
        }
#endif // MSTL_VERSION_20__
        return _MSTL wstring_length(reinterpret_cast<const wchar_t*>(str));
    }

    MSTL_NODISCARD static constexpr const char_type* find(
        const char_type* str, const size_t n, const char_type& chr) noexcept {
#if MSTL_VERSION_20__
        if (_MSTL is_constant_evaluated()) {
            MSTL_IF_CONSTEXPR (is_same_v<char_type, wchar_t>) {
                return __builtin_wmemchr(str, chr, n);
            }
            else {
                return base_type::find(str, n, chr);
            }
        }
#endif // MSTL_VERSION_20__
        return reinterpret_cast<const char_type*>(
            _MSTL wchar_memory_char(reinterpret_cast<const wchar_t*>(str), chr, n));
    }

    static constexpr char_type* assign(char_type* const str, size_t n, const char_type chr) noexcept {
#if MSTL_VERSION_20__
        if (_MSTL is_constant_evaluated()) {
            return base_type::assign(str, n, chr);
        }
#endif // MSTL_VERSION_20__
        return reinterpret_cast<char_type*>(_MSTL wchar_memory_set(reinterpret_cast<wchar_t*>(str), chr, n));
    }

    static constexpr void assign(char_type& lh, const char_type& rh) noexcept {
#if MSTL_VERSION_20__
        if (_MSTL is_constant_evaluated()) {
            return base_type::assign(lh, rh);
        }
#endif // MSTL_VERSION_20__
        lh = rh;
    }

    MSTL_NODISCARD static constexpr bool eq(const char_type lh, const char_type rh) noexcept {
        return lh == rh;
    }
    MSTL_NODISCARD static constexpr bool lt(const char_type lh, const char_type rh) noexcept {
        return lh < rh;
    }
    MSTL_NODISCARD static constexpr char_type to_char_type(const int_type rsc) noexcept {
        return rsc;
    }
    MSTL_NODISCARD static constexpr int_type to_int_type(const char_type chr) noexcept {
        return chr;
    }
    MSTL_NODISCARD static constexpr bool eq_int_type(const int_type lh, const int_type rh) noexcept {
        return lh == rh;
    }

    MSTL_NODISCARD static constexpr int_type not_eof(const int_type rsc) noexcept {
        return rsc != eof() ? rsc : static_cast<int_type>(!eof());
    }
    MSTL_NODISCARD static constexpr int_type eof() noexcept {
        return 0xffff;
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
    MSTL_NODISCARD static constexpr int compare(const char_type* const lh,
        const char_type* const rh, const size_t n) noexcept {
#ifdef MSTL_VERSION_17__
        return ::__builtin_memcmp(lh, rh, n);
#else
        return _MSTL memory_compare(lh, rh, n);
#endif
    }

    MSTL_NODISCARD static constexpr size_t length(const char_type* const str) noexcept {
#ifdef MSTL_VERSION_17__
#ifdef MSTL_VERSION_20__
        if constexpr (is_same_v<char_type, char8_t>) {
#if defined(MSTL_VERSION_20__) && !defined(MSTL_COMPILER_CLANG__) && !defined(MSTL_COMPILE_WITH_EDG__)
#ifdef MSTL_COMPILER_MSVC__
            return __builtin_u8strlen(str);
#else
            return _MSTL u8string_length(str);
#endif
#else
            return base_type::length(str);
#endif // MSTL_SUPPORT_U8_INTRINSICS__
        }
        else
#endif // MSTL_VERSION_20__
        {
            return ::__builtin_strlen(str);
        }
#else
        return _MSTL string_length(reinterpret_cast<const char*>(str));
#endif // MSTL_VERSION_17__
    }

    MSTL_NODISCARD static constexpr const char_type* find(const char_type* const str,
        const size_t n, const char_type& chr) noexcept {
#ifdef MSTL_VERSION_17__
#ifdef MSTL_VERSION_20__
        if constexpr (is_same_v<char_type, char8_t>) {
#if defined(MSTL_VERSION_20__) && !defined(MSTL_COMPILER_CLANG__) && !defined(MSTL_COMPILE_WITH_EDG__)
            return __builtin_u8memchr(str, chr, n);
#else
            return base_type::find(str, n, chr);
#endif // MSTL_SUPPORT_U8_INTRINSICS__
        }
        else
#endif // MSTL_VERSION_20__
        {
#ifdef MSTL_COMPILER_MSVC__
            return ::__builtin_char_memchr(str, chr, n);
#else
            return base_type::find(str, n, chr);
#endif
        }
#else
        return static_cast<const char_type*>(_MSTL memory_char(str, chr, n));
#endif // MSTL_VERSION_17__
    }

    static constexpr char_type* assign(char_type* const str, size_t n, const char_type chr) noexcept {
#ifdef MSTL_VERSION_20__
        if (_MSTL is_constant_evaluated()) {
            return base_type::assign(str, n, chr);
        }
#endif // MSTL_VERSION_20__
        return static_cast<char_type*>(_MSTL memory_set(str, chr, n));
    }

    static constexpr void assign(char_type& lh, const char_type& rh) noexcept {
#ifdef MSTL_VERSION_20__
        if (_MSTL is_constant_evaluated()) {
            return base_type::assign(lh, rh);
        }
#endif // MSTL_VERSION_20__
        lh = rh;
    }

    MSTL_NODISCARD static constexpr bool eq(const char_type lh, const char_type rh) noexcept {
        return lh == rh;
    }
    MSTL_NODISCARD static constexpr bool lt(const char_type lh, const char_type rh) noexcept {
        return static_cast<byte_t>(lh) < static_cast<byte_t>(rh);
    }
    MSTL_NODISCARD static constexpr char_type to_char_type(const int_type rsc) noexcept {
        return static_cast<char_type>(rsc);
    }
    MSTL_NODISCARD static constexpr int_type to_int_type(const char_type chr) noexcept {
        return static_cast<byte_t>(chr);
    }
    MSTL_NODISCARD static constexpr bool eq_int_type(const int_type lh, const int_type rh) noexcept {
        return lh == rh;
    }

    MSTL_NODISCARD static constexpr int_type not_eof(const int_type rsc) noexcept {
        return rsc != eof() ? rsc : !eof();
    }
    MSTL_NODISCARD static constexpr int_type eof() noexcept {
        return static_cast<int_type>(-1);
    }
};

template <typename CharT>
struct char_traits : public base_char_traits<CharT, int64_t> {};
template <> struct char_traits<char> : public narrow_char_traits<char, int32_t> {};
template <> struct char_traits<wchar_t> : public wide_char_traits<wchar_t> {};
#ifdef MSTL_VERSION_20__
template <> struct char_traits<char8_t> : public narrow_char_traits<char8_t, uint32_t> {};
#endif
template <> struct char_traits<char16_t> : public wide_char_traits<char16_t> {};
template <> struct char_traits<char32_t> : public base_char_traits<char32_t, uint32_t> {};


template <typename Traits>
using char_traits_char_t = typename Traits::char_type;
template <typename Traits>
using char_traits_ptr_t = const typename Traits::char_type*;


template <typename CharT, bool = is_character_v<CharT>>
class __string_bitmap {
private:
    bool matches_[256] = {};

public:
    constexpr __string_bitmap() = default;

    constexpr bool mark(const CharT* first, const CharT* const last) noexcept {
        for (; first != last; ++first)
            matches_[static_cast<byte_t>(*first)] = true;
        return true;
    }
    constexpr bool match(const CharT chr) const noexcept {
        return matches_[static_cast<byte_t>(chr)];
    }
};

template <typename CharT>
class __string_bitmap<CharT, false> {};


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
    const int state = Traits::compare(lh, rh, _MSTL min(lh_size, rh_size));
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
    if (rsc_size == 0) return _MSTL min(start, dest_size);

    if (rsc_size <= dest_size) {
        for (auto if_match = dest + _MSTL min(start, dest_size - rsc_size);; --if_match) {
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
        for (auto if_match = dest + _MSTL min(start, dest_size - 1);; --if_match) {
            if (Traits::eq(*if_match, chr)) 
                return static_cast<size_t>(if_match - dest);
            
            if (if_match == dest) break;
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits, enable_if_t<
#ifdef MSTL_VERSION_17__
    is_specialization_v<Traits, char_traits>
#else
    is_specialization_v<Traits, char_traits>()
#endif
    , int> = 0>
constexpr size_t char_traits_find_first_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (rsc_size != 0 && start < dest_size) {
        _MSTL __string_bitmap<char_traits_char_t<Traits>> match;
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
    return static_cast<size_t>(-1);
}

template <typename Traits, enable_if_t<
#ifdef MSTL_VERSION_17__
    !is_specialization_v<Traits, char_traits>
#else
    !is_specialization_v<Traits, char_traits>()
#endif
    , int> = 0>
constexpr size_t char_traits_find_first_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size, 
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (rsc_size != 0 && start < dest_size) {
        const auto end = dest + dest_size;
        for (auto if_match = dest + start; if_match < end; ++if_match) {
            if (Traits::find(rsc, rsc_size, *if_match))
                return static_cast<size_t>(if_match - dest);
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits, enable_if_t<
#ifdef MSTL_VERSION_17__
    is_specialization_v<Traits, char_traits>
#else
    is_specialization_v<Traits, char_traits>()
#endif
    , int> = 0>
constexpr size_t char_traits_find_last_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (rsc_size != 0 && dest_size != 0) {
        _MSTL __string_bitmap<char_traits_char_t<Traits>> match;
        if (!match.mark(rsc, rsc + rsc_size))
            return (char_traits_find_last_of<Traits, false>)
            (dest, dest_size, start, rsc, rsc_size);

        for (auto if_match = dest + _MSTL min(start, dest_size - 1);; --if_match) {
            if (match.match(*if_match))
                return static_cast<size_t>(if_match - dest);

            if (if_match == dest) break;
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits, enable_if_t<
#ifdef MSTL_VERSION_17__
    !is_specialization_v<Traits, char_traits>
#else
    !is_specialization_v<Traits, char_traits>()
#endif
    , int> = 0>
constexpr size_t char_traits_find_last_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size, 
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (rsc_size != 0 && dest_size != 0) {
        for (auto if_match = dest + _MSTL min(start, dest_size - 1);; --if_match) {
            if (Traits::find(rsc, rsc_size, *if_match))
                return static_cast<size_t>(if_match - dest);

            if (if_match == dest) break;
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits, enable_if_t<
#ifdef MSTL_VERSION_17__
    is_specialization_v<Traits, char_traits>
#else
    is_specialization_v<Traits, char_traits>()
#endif
    , int> = 0>
constexpr size_t char_traits_find_first_not_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (start < dest_size) {
        _MSTL __string_bitmap<char_traits_char_t<Traits>> match;
        if (!match.mark(rsc, rsc + rsc_size))
            return (char_traits_find_first_not_of<Traits, false>)
            (dest, dest_size, start, rsc, rsc_size);

        const auto end = dest + dest_size;
        for (auto if_match = dest + start; if_match < end; ++if_match) {
            if (!match.match(*if_match))
                return static_cast<size_t>(if_match - dest);
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits, enable_if_t<
#ifdef MSTL_VERSION_17__
    !is_specialization_v<Traits, char_traits>
#else
    !is_specialization_v<Traits, char_traits>()
#endif
    , int> = 0>
constexpr size_t char_traits_find_first_not_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (start < dest_size) {
        const auto end = dest + dest_size;
        for (auto if_match = dest + start; if_match < end; ++if_match) {
            if (!Traits::find(rsc, rsc_size, *if_match))
                return static_cast<size_t>(if_match - dest);
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

template <typename Traits, enable_if_t<
#ifdef MSTL_VERSION_17__
    is_specialization_v<Traits, char_traits>
#else
    is_specialization_v<Traits, char_traits>()
#endif
    , int> = 0>
constexpr size_t char_traits_find_last_not_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size,
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (dest_size != 0) {
        _MSTL __string_bitmap<char_traits_char_t<Traits>> match;
        if (!match.mark(rsc, rsc + rsc_size))
            return (char_traits_find_last_not_of<Traits, false>)
            (dest, dest_size, start, rsc, rsc_size);

        for (auto if_match = dest + _MSTL min(start, dest_size - 1);; --if_match) {
            if (!match.match(*if_match))
                return static_cast<size_t>(if_match - dest);

            if (if_match == dest) break;
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits, enable_if_t<
#ifdef MSTL_VERSION_17__
    !is_specialization_v<Traits, char_traits>
#else
    !is_specialization_v<Traits, char_traits>()
#endif
    , int> = 0>
constexpr size_t char_traits_find_last_not_of(const char_traits_ptr_t<Traits> dest, const size_t dest_size, 
    const size_t start, const char_traits_ptr_t<Traits> rsc, const size_t rsc_size) noexcept {
    if (dest_size != 0) {
        for (auto if_match = dest + _MSTL min(start, dest_size - 1);; --if_match) {
            if (!Traits::find(rsc, rsc_size, *if_match))
                return static_cast<size_t>(if_match - dest);

            if (if_match == dest) break;
        }
    }
    return static_cast<size_t>(-1);
}

template <typename Traits>
constexpr size_t char_traits_rfind_not_char(const char_traits_ptr_t<Traits> dest, const size_t dest_size, 
    const size_t start, const char_traits_char_t<Traits> chr) noexcept {
    if (dest_size != 0) {
        for (auto if_match = dest + _MSTL min(start, dest_size - 1);; --if_match) {
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
private:
    using container_type	= basic_string_view<typename Traits::char_type, Traits>;
    using iterator			= string_view_iterator<Traits>;
    using const_iterator	= string_view_iterator<Traits>;

public:
#ifdef MSTL_VERSION_20__
    using iterator_category = contiguous_iterator_tag;
#else
    using iterator_category = random_access_iterator_tag;
#endif // MSTL_VERSION_20__
    using value_type		= typename container_type::value_type;
    using reference			= typename container_type::const_reference;
    using pointer			= typename container_type::const_pointer;
    using difference_type	= typename container_type::difference_type;
    using size_type			= typename container_type::size_type;

    using self				= string_view_iterator<Traits>;

private:
    pointer data_ = nullptr;
    size_t size_ = 0;
    size_t idx_ = 0;

    friend basic_string_view<value_type, Traits>;

public:
    constexpr string_view_iterator() noexcept = default;

    constexpr string_view_iterator(const pointer data, const size_t size, const size_t off) noexcept
        : data_(data), size_(size), idx_(off) {}

    MSTL_NODISCARD constexpr reference operator *() const noexcept {
        MSTL_DEBUG_VERIFY(data_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(string_view_iterator, __MSTL_DEBUG_TAG_DEREFERENCE));
        MSTL_DEBUG_VERIFY(idx_ < size_, __MSTL_DEBUG_MESG_OUT_OF_RANGE(string_view_iterator, __MSTL_DEBUG_TAG_DEREFERENCE));
        return data_[idx_];
    }

    MSTL_NODISCARD constexpr pointer operator ->() const noexcept {
        return &operator*();
    }

    constexpr self& operator ++() noexcept {
        MSTL_DEBUG_VERIFY(data_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(string_view_iterator, __MSTL_DEBUG_TAG_INCREMENT));
        MSTL_DEBUG_VERIFY(idx_ < size_, __MSTL_DEBUG_MESG_OUT_OF_RANGE(string_view_iterator, __MSTL_DEBUG_TAG_INCREMENT));
        ++idx_;
        return *this;
    }

    constexpr self operator ++(int) noexcept {
        self tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr self& operator --() noexcept {
        MSTL_DEBUG_VERIFY(data_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(string_view_iterator, __MSTL_DEBUG_TAG_DECREMENT));
        MSTL_DEBUG_VERIFY(idx_ != 0, __MSTL_DEBUG_MESG_OUT_OF_RANGE(string_view_iterator, __MSTL_DEBUG_TAG_DECREMENT));
        --idx_;
        return *this;
    }

    constexpr self operator --(int) noexcept {
        self tmp(*this);
        --*this;
        return tmp;
    }

    constexpr self& operator +=(const difference_type n) noexcept {
        if (n < 0) {
            MSTL_DEBUG_VERIFY(data_ || n == 0, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(vector_iterator, __MSTL_DEBUG_TAG_DECREMENT));
            MSTL_DEBUG_VERIFY(idx_ >= static_cast<size_t>(0) - static_cast<size_t>(n),
                __MSTL_DEBUG_MESG_OUT_OF_RANGE(vector_iterator, __MSTL_DEBUG_TAG_DECREMENT));
        }
        else if (n > 0) {
            MSTL_DEBUG_VERIFY(data_ || n == 0, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(vector_iterator, __MSTL_DEBUG_TAG_INCREMENT));
            MSTL_DEBUG_VERIFY(size_ - idx_ >= static_cast<size_t>(n),
                __MSTL_DEBUG_MESG_OUT_OF_RANGE(vector_iterator, __MSTL_DEBUG_TAG_INCREMENT));
        }
        idx_ += n;
        return *this;
    }
    MSTL_NODISCARD constexpr self operator +(const difference_type n) const noexcept {
        self tmp = *this;
        tmp += n;
        return tmp;
    }
    MSTL_NODISCARD friend constexpr self operator +(const difference_type n, const self& iter) noexcept {
        return iter + n;
    }

    constexpr self& operator -=(const difference_type n) noexcept {
        idx_ += -n;
        return *this;
    }
    MSTL_NODISCARD constexpr self operator -(const difference_type n) const noexcept {
        self tmp = *this;
        tmp -= n;
        return tmp;
    }
    MSTL_NODISCARD constexpr difference_type operator -(const self& iter) const noexcept {
        MSTL_DEBUG_VERIFY(data_ == iter.data_ && size_ == iter.size_,
            __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(string_view_iterator));
        return static_cast<difference_type>(idx_ - iter.idx_);
    }

    MSTL_NODISCARD constexpr reference operator [](const difference_type n) const noexcept {
        return *(*this + n);
    }

    MSTL_NODISCARD constexpr bool operator ==(const self& iter) const noexcept {
        MSTL_DEBUG_VERIFY(data_ == iter.data_ && size_ == iter.size_,
            __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(string_view_iterator));
        return idx_ == iter.idx_;
    }
    MSTL_NODISCARD constexpr bool operator !=(const self& iter) const noexcept {
        return !(*this == iter);
    }
    MSTL_NODISCARD constexpr bool operator <(const self& iter) const noexcept {
        MSTL_DEBUG_VERIFY(data_ == iter.data_ && size_ == iter.size_,
            __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(string_view_iterator));
        return idx_ < iter.idx_;
    }
    MSTL_NODISCARD constexpr bool operator >(const self& iter) const noexcept {
        return iter < *this;
    }
    MSTL_NODISCARD constexpr bool operator <=(const self& iter) const noexcept {
        return !(iter < *this);
    }
    MSTL_NODISCARD constexpr bool operator >=(const self& iter) const noexcept {
        return !(*this < iter);
    }
};


template <typename CharT, typename Traits>
class basic_string_view {
    static_assert(is_same_v<CharT, typename Traits::char_type>, 
        "char type of basic string view should be same with char traits.");
    static_assert(!is_array_v<CharT> && is_trivial_v<CharT> && is_standard_layout_v<CharT>,
        "basic string view only contains non-array trivial standard-layout types.");

public:
    MSTL_BUILD_TYPE_ALIAS(CharT)
    using traits_type               = Traits;
    using self                      = basic_string_view<CharT, Traits>;

    using const_iterator            = string_view_iterator<Traits>;
    using iterator                  = const_iterator;
    using const_reverse_iterator    = _MSTL reverse_iterator<const_iterator>;
    using reverse_iterator          = const_reverse_iterator;

    static constexpr auto npos = static_cast<size_type>(-1);

private:
    const_pointer data_;
    size_type size_;

    constexpr void range_check(const size_type n) const {
        MSTL_DEBUG_VERIFY(size_ < n, "basic string view index out of ranges.");
    }

    MSTL_NODISCARD constexpr size_type clamp_size(const size_type position, const size_type size) const noexcept {
        return _MSTL min(size, size_ - position);
    }

public:
    constexpr basic_string_view() noexcept : data_(), size_(0) {}

    constexpr basic_string_view(const self&) noexcept = default;
    constexpr basic_string_view& operator =(const self&) noexcept = default;

    constexpr basic_string_view(const_pointer str) noexcept
        : data_(str), size_(Traits::length(str)) {}
    constexpr basic_string_view(const_pointer str, const size_type n) noexcept
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
        return (npos - sizeof(size_type) - POINTER_SIZE) / sizeof(value_type) / 4;
    }
    MSTL_NODISCARD constexpr size_type length() const noexcept { return size_; }
    MSTL_NODISCARD constexpr bool empty() const noexcept { return size_ == 0; }

    MSTL_NODISCARD constexpr const_pointer data() const noexcept { return data_; }
    MSTL_NODISCARD constexpr const_reference front() const noexcept {
        MSTL_DEBUG_VERIFY(!empty(), "cannot call front on empty string_view");
        return data_[0];
    }
    MSTL_NODISCARD constexpr const_reference back() const noexcept {
        MSTL_DEBUG_VERIFY(!empty(), "cannot call back on empty string_view");
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
        MSTL_DEBUG_VERIFY(size_ >= n, "cannot remove prefix longer than total size");
        data_ += n;
        size_ -= n;
    }
    constexpr void remove_suffix(const size_type n) noexcept {
        MSTL_DEBUG_VERIFY(size_ >= n, "cannot remove suffix longer than total size");
        size_ -= n;
    }

    constexpr void swap(self& view) noexcept {
        const self tmp(view);
        view = *this;
        *this = tmp;
    }

    constexpr size_type copy(CharT* const str, size_type count, const size_type off = 0) const {
        range_check(off);
        count = clamp_size(off, count);
        Traits::copy(str, data_ + off, count);
        return count;
    }

    MSTL_NODISCARD constexpr self substr(const size_type off = 0, size_type count = npos) const {
        range_check(off);
        count = clamp_size(off, count);
        return self(data_ + off, count);
    }

    MSTL_NODISCARD constexpr bool equal_to(const self view) const noexcept {
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


#ifdef MSTL_VERSION_17__

inline namespace string_operator {
    MSTL_NODISCARD constexpr string_view operator ""_sv(const char* str, size_t len) noexcept {
        return {str, len};
    }
    MSTL_NODISCARD constexpr wstring_view operator ""_sv(const wchar_t* str, size_t len) noexcept {
        return {str, len};
    }
#ifdef MSTL_VERSION_20__
    MSTL_NODISCARD constexpr u8string_view operator ""_sv(const char8_t* str, size_t len) noexcept {
        return {str, len};
    }
#endif // MSTL_VERSION_20__
    MSTL_NODISCARD constexpr u16string_view operator ""_sv(const char16_t* str, size_t len) noexcept {
        return {str, len};
    }
    MSTL_NODISCARD constexpr u32string_view operator ""_sv(const char32_t* str, size_t len) noexcept {
        return {str, len};
    }
}

#endif // MSTL_VERSION_17__


inline size_t string_hash(const char* s, size_t len, uint32_t seed) noexcept {
#if defined(MSTL_DATA_BUS_WIDTH_64__)
    const pair<size_t, size_t> p = MurmurHash_x64(s, len, seed);
    return p.first ^ p.second;
#elif defined(MSTL_DATA_BUS_WIDTH_32__)
    return (size_t)MurmurHash_x32(s, len, seed);
#else 
    return DJB2_hash(s, len);
#endif
}

template <>
struct hash<char*> {
    MSTL_NODISCARD size_t operator ()(const char* str) const noexcept {
        return string_hash(str, _MSTL string_length(str), 0);
    }
}; 
template <>
struct hash<const char*> {
    MSTL_NODISCARD size_t operator ()(const char* str) const noexcept {
        return string_hash(str, _MSTL string_length(str), 0);
    }
};

#define CHAR_PTR_HASH_STRUCTS__(OPT) \
    template <> \
    struct hash<OPT*> { \
        MSTL_NODISCARD size_t operator ()(const OPT* str) const noexcept { \
            return FNV_hash(reinterpret_cast<const byte_t*>(str), sizeof(OPT) * char_traits<OPT>::length(str)); \
        } \
    }; \
    template <> \
    struct hash<const OPT*> { \
        MSTL_NODISCARD size_t operator ()(const OPT* str) const noexcept { \
            return FNV_hash(reinterpret_cast<const byte_t*>(str), sizeof(OPT) * char_traits<OPT>::length(str)); \
        } \
    };

CHAR_PTR_HASH_STRUCTS__(wchar_t)
MSTL_MACRO_RANGES_UNICODE_CHARS(CHAR_PTR_HASH_STRUCTS__)
#undef CHAR_PTR_HASH_STRUCTS__

template <>
struct hash<_MSTL string_view> {
    MSTL_NODISCARD size_t operator ()(const _MSTL string_view str) const noexcept {
        return string_hash(str.data(), str.size(), 0);
    }
};
template <typename CharT, typename Traits>
struct hash<basic_string_view<CharT, Traits>> {
    MSTL_NODISCARD constexpr size_t operator ()(
        const basic_string_view<CharT, Traits> str) const noexcept {
        return FNV_hash(reinterpret_cast<const byte_t*>(str.data()), sizeof(CharT) * str.size());
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_STRING_VIEW_HPP__
