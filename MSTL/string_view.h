#ifndef MSTL_BASIC_STRING_H__
#define MSTL_BASIC_STRING_H__
#include "type_traits.hpp"
#include "errorlib.h"
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
    static MSTL_CONSTEXPR char_type* copy_safe(char_type* const dest,
        const size_t dest_size, const char_type* const srcs, const size_t count) noexcept {
        MSTL_DEBUG_VERIFY__(count <= dest_size, "invalid count argument.");
        return copy(dest, srcs, count);
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

    MSTL_NODISCARD static MSTL_CONSTEXPR int compare(const char_type* str1,
        const char_type* str2, size_t count) noexcept {
        for (; 0 < count; --count, ++str1, ++str2) {
            if (*str1 != *str2) 
                return *str1 < *str2 ? -1 : +1;
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
    MSTL_NODISCARD static MSTL_CONSTEXPR bool eq_int_type(const int_type _Left, const int_type _Right) noexcept {
        return _Left == _Right;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type not_eof(const int_type _Meta) noexcept {
        return _Meta != eof() ? _Meta : !eof();
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type eof() noexcept {
        return static_cast<int_type>(-1);
    }
};

template <class CharT>
struct wide_char_traits : private base_char_traits<CharT, unsigned short> {
private:
    using base_type = base_char_traits<CharT, unsigned short>;

public:
    using char_type = CharT;
    using int_type = unsigned short;

    using base_type::copy_safe;
    using base_type::copy;
    using base_type::move;

public:
    MSTL_NODISCARD static MSTL_CONSTEXPR int compare(const char_type* const str1,
        const char_type* const str2, const size_t count) noexcept {
#if MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            if constexpr (is_same_v<char_type, wchar_t>) {
                return __builtin_wmemcmp(str1, str2, count);
            }
            else {
                return base_type::compare(str1, str2, count);
            }
        }
#endif // MSTL_VERSION_20__
        return MSTL::wmemcmp(reinterpret_cast<const wchar_t*>(str1),
            reinterpret_cast<const wchar_t*>(str2), count);
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
        const char_type* str, const size_t count, const char_type& chr) noexcept {
#if MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            if constexpr (is_same_v<char_type, wchar_t>) {
                return __builtin_wmemchr(str, chr, count);
            }
            else {
                return base_type::find(str, count, chr);
            }
        }
#endif // MSTL_VERSION_20__
        return reinterpret_cast<const char_type*>(
            MSTL::wmemchr(reinterpret_cast<const wchar_t*>(str), chr, count));
    }

    static MSTL_CONSTEXPR char_type* assign(char_type* const str, size_t count, const char_type chr) noexcept {
#if MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            return base_type::assign(str, count, chr);
        }
#endif // MSTL_VERSION_20__
        return reinterpret_cast<char_type*>(MSTL::wmemset(reinterpret_cast<wchar_t*>(str), chr, count));
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
    MSTL_NODISCARD static MSTL_CONSTEXPR char_type to_char_type(const int_type sur) noexcept {
        return sur;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type to_int_type(const char_type chr) noexcept {
        return chr;
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR bool eq_int_type(const int_type lh, const int_type rh) noexcept {
        return lh == rh;
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR int_type not_eof(const int_type sur) noexcept {
        return sur != eof() ? sur : static_cast<int_type>(!eof());
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

    using base_type::copy_safe;
    using base_type::copy;
    using base_type::move;

public:
    MSTL_NODISCARD static MSTL_CONSTEXPR int compare(const char_type* const str1,
        const char_type* const str2, const size_t count) noexcept {
#if MSTL_VERSION_17__
        return __builtin_memcmp(str1, str2, count);
#else
        return MSTL::memcmp(str1, str2, count);
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
        const size_t count, const char_type& chr) noexcept {
#ifdef MSTL_VERSION_17__
#ifdef MSTL_VERSION_20__
        if constexpr (is_same_v<char_type, char8_t>) {
#ifdef MSTL_SUPPORT_U8_INTRINSICS__
            return __builtin_u8memchr(str, chr, count);
#else
            return base_type::find(str, count, chr);
#endif // MSTL_SUPPORT_U8_INTRINSICS__
        }
        else
#endif // MSTL_VERSION_20__
        {
            return __builtin_char_memchr(str, chr, count);
        }
#else
        return static_cast<const char_type*>(MSTL::memchr(str, chr, count));
#endif // MSTL_VERSION_17__
    }

    static MSTL_CONSTEXPR char_type* assign(char_type* const str, size_t count, const char_type chr) noexcept {
#ifdef MSTL_VERSION_20__
        if (MSTL::is_constant_evaluated()) {
            return base_type::assign(str, count, chr);
        }
#endif // MSTL_VERSION_20__
        return static_cast<char_type*>(MSTL::memset(str, chr, count));
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
    MSTL_NODISCARD static MSTL_CONSTEXPR char_type to_char_type(const int_type sur) noexcept {
        return static_cast<char_type>(sur);
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type to_int_type(const char_type chr) noexcept {
        return static_cast<unsigned char>(chr);
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR bool eq_int_type(const int_type lh, const int_type rh) noexcept {
        return lh == rh;
    }

    MSTL_NODISCARD static MSTL_CONSTEXPR int_type not_eof(const int_type sur) noexcept {
        return sur != eof() ? sur : !eof();
    }
    MSTL_NODISCARD static MSTL_CONSTEXPR int_type eof() noexcept {
        return static_cast<int_type>(-1);
    }
};

template <typename CharT>
struct char_traits : base_char_traits<CharT, long> {};
template <>
struct char_traits<char> : narrow_char_traits<char, int> {};
template <>
struct char_traits<wchar_t> : wide_char_traits<wchar_t> {};
#ifdef MSTL_VERSION_20__
template <>
struct char_traits<char8_t> : narrow_char_traits<char8_t, unsigned int> {};
#endif
template <>
struct char_traits<char16_t> : wide_char_traits<char16_t> {};
template <>
struct char_traits<char32_t> : base_char_traits<char32_t, unsigned int> {};


template <typename CharT, typename Traits = char_traits<CharT>>
class basic_string_view;



MSTL_END_NAMESPACE__
#endif // MSTL_BASIC_STRING_H__
