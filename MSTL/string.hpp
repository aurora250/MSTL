#ifndef MSTL_STRING_HPP__
#define MSTL_STRING_HPP__
#include "basic_string.hpp"
MSTL_BEGIN_NAMESPACE__

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
#ifdef MSTL_VERSION_20__
using u8string = basic_string<char8_t>;
#endif
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

TEMNULL__ struct hash<MSTL::string> {
    MSTL_NODISCARD size_t operator ()(const MSTL::string& s) const noexcept {
        return string_hash(s.c_str(), MSTL::strlen(s.c_str()), 0);
    }
};
template <typename CharT, typename Traits, typename Alloc>
struct hash<basic_string<CharT, Traits, Alloc>> {
    MSTL_NODISCARD size_t operator ()(const basic_string<CharT, Traits, Alloc>& str) const noexcept {
        return FNV_hash(reinterpret_cast<const byte_t*>(str.c_str()), sizeof(CharT) * str.size());
    }
};
TEMNULL__ struct hash<std::string> {
    MSTL_NODISCARD size_t operator ()(const std::string& s) const noexcept {
        return string_hash(s.c_str(), MSTL::strlen(s.c_str()), 0);
    }
};


#ifdef MSTL_VERSION_17__
#pragma warning(push)
#pragma warning(disable: 4455)
inline namespace string_operator {
    inline MSTL_NODISCARD string operator ""s(const char* str, size_t len) noexcept {
        return string(str, len);
    }
    inline MSTL_NODISCARD wstring operator ""s(const wchar_t* str, size_t len) noexcept {
        return wstring(str, len);
    }
#ifdef MSTL_VERSION_20__
    inline MSTL_NODISCARD u8string operator ""s(const char8_t* str, size_t len) noexcept {
        return u8string(str, len);
    }
#endif // MSTL_VERSION_20__
    inline MSTL_NODISCARD u16string operator ""s(const char16_t* str, size_t len) noexcept {
        return u16string(str, len);
    }
    inline MSTL_NODISCARD u32string operator ""s(const char32_t* str, size_t len) noexcept {
        return u32string(str, len);
    }
}
#pragma warning(pop)
#endif // MSTL_VERSION_17__


template <typename CharT, typename UT>
MSTL_NODISCARD CharT* uint_to_buff(CharT* riter, UT ux) {
    static_assert(is_unsigned_v<UT>, "UT must be unsigned types.");
#ifdef MSTL_DATA_BUS_WIDTH_64__
    auto holder = ux;
#else
    if constexpr (sizeof(UT) > 4) {
        while (ux > 0xFFFFFFFFU) {
            auto chunk = static_cast<unsigned long>(ux % 1000000000);
            ux /= 1000000000;
            for (int idx = 0; idx != 9; ++idx) {
                *--riter = static_cast<CharT>('0' + chunk % 10);
                chunk /= 10;
            }
        }
    }
    auto holder = static_cast<unsigned long>(ux);
#endif
    do {
        *--riter = static_cast<CharT>('0' + holder % 10);
        holder /= 10;
    } while (holder != 0);
    return riter;
}

template <typename CharT, typename T>
MSTL_NODISCARD basic_string<CharT> int_to_string(const T x) {
    static_assert(is_integral_v<T>, "T must be integral types.");
    CharT buffer[21];
    CharT* const buffer_end = buffer + 21;
    CharT* rnext = buffer_end;
    const auto unsigned_x = static_cast<make_unsigned_t<T>>(x);
    if (x < 0) {
        rnext = (uint_to_buff)(rnext, 0 - unsigned_x);
        *--rnext = '-';
    }
    else 
        rnext = (uint_to_buff)(rnext, unsigned_x);
    return basic_string<CharT>(rnext, buffer_end);
}

template <typename CharT, typename T>
MSTL_NODISCARD basic_string<CharT> uint_to_string(const T x) {
    static_assert(is_integral_v<T> && is_unsigned_v<T>, "T must be unsigned integral types.");
    CharT buffer[21];
    CharT* const buffer_end = buffer + 21;
    CharT* const rnext = (uint_to_buff)(buffer_end, x);
    return basic_string<CharT>(rnext, buffer_end);
}


MSTL_NODISCARD inline string to_string(int x) {
    return int_to_string<char>(x);
}
MSTL_NODISCARD inline string to_string(unsigned int x) {
    return uint_to_string<char>(x);
}
MSTL_NODISCARD inline string to_string(long x) {
    return int_to_string<char>(x);
}
MSTL_NODISCARD inline string to_string(unsigned long x) {
    return uint_to_string<char>(x);
}
MSTL_NODISCARD inline string to_string(MSTL_LLT x) {
    return int_to_string<char>(x);
}
MSTL_NODISCARD inline string to_string(unsigned MSTL_LLT x) {
    return uint_to_string<char>(x);
}

MSTL_NODISCARD inline string to_string(double x) {
    const auto len = static_cast<size_t>(::_scprintf("%f", x));
    string str(len, '\0');
    ::sprintf_s(&str[0], len + 1, "%f", x);
    return str;
}
MSTL_NODISCARD inline string to_string(float x) {
    return to_string(static_cast<double>(x));
}
MSTL_NODISCARD inline string to_string(long double x) {
    return to_string(static_cast<double>(x));
}


MSTL_NODISCARD inline wstring to_wstring(int x) {
    return int_to_string<wchar_t>(x);
}
MSTL_NODISCARD inline wstring to_wstring(unsigned int x) {
    return uint_to_string<wchar_t>(x);
}
MSTL_NODISCARD inline wstring to_wstring(long x) {
    return int_to_string<wchar_t>(x);
}
MSTL_NODISCARD inline wstring to_wstring(unsigned long x) {
    return uint_to_string<wchar_t>(x);
}
MSTL_NODISCARD inline wstring to_wstring(long long x) {
    return int_to_string<wchar_t>(x);
}
MSTL_NODISCARD inline wstring to_wstring(unsigned long long x) {
    return uint_to_string<wchar_t>(x);
}
MSTL_NODISCARD inline wstring to_wstring(double x) {
    const auto len = static_cast<size_t>(::_scwprintf(L"%f", x));
    wstring str(len, L'\0');
    ::swprintf_s(&str[0], len + 1, L"%f", x);
    return str;
}
MSTL_NODISCARD inline wstring to_wstring(float x) {
    return to_wstring(static_cast<double>(x));
}
MSTL_NODISCARD inline wstring to_wstring(long double x) {
    return to_wstring(static_cast<double>(x));
}


template <typename CharT, typename T>
MSTL_NODISCARD inline basic_string<CharT> __stream_to_string(T x) {
    if constexpr (is_same_v<CharT, char>)
        return to_string(x);
    else if constexpr (is_same_v<CharT, wchar_t>)
        return to_wstring(x);
    else static_assert(false, "to_string unsupport char type.");
}

MSTL_END_NAMESPACE__
#endif // MSTL_STRING_HPP__
