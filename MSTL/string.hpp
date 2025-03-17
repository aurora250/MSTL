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
    MSTL_NODISCARD MSTL_CONSTEXPR size_t operator ()(
        const basic_string<CharT, Traits, Alloc>& str) const noexcept {
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
    MSTL_NODISCARD MSTL_CONSTEXPR20 string operator ""_s(const char* str, size_t len) noexcept {
        return {str, len};
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 wstring operator ""_s(const wchar_t* str, size_t len) noexcept {
        return {str, len};
    }
#ifdef MSTL_VERSION_20__
    MSTL_NODISCARD MSTL_CONSTEXPR20 u8string operator ""_s(const char8_t* str, size_t len) noexcept {
        return {str, len};
    }
#endif // MSTL_VERSION_20__
    MSTL_NODISCARD MSTL_CONSTEXPR20 u16string operator ""_s(const char16_t* str, size_t len) noexcept {
        return {str, len};
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 u32string operator ""_s(const char32_t* str, size_t len) noexcept {
        return {str, len};
    }
}
#pragma warning(pop)
#endif // MSTL_VERSION_17__


MSTL_NODISCARD inline int stoi(const string& str, size_t* idx = nullptr, const int base = 10) {
    const char* raw = str.c_str();
    char* err;
    const long num = ::strtol(raw, &err, base);
    MSTL_DEBUG_VERIFY__(raw != err, "invalid argument.");
    if (idx) 
        *idx = static_cast<size_t>(err - raw);
    return static_cast<int>(num);
}


#ifndef MSTL_DATA_BUS_WIDTH_64__
template <typename CharT, typename UT, enable_if_t<(sizeof(UT) > 4), int> = 0>
inline void __uint_to_buff_aux(CharT* riter, UT& ux) {
    while (ux > 0xFFFFFFFFU) {
        auto chunk = static_cast<unsigned long>(ux % 1000000000);
        ux /= 1000000000;
        for (int idx = 0; idx != 9; ++idx) {
            *--riter = static_cast<CharT>('0' + chunk % 10);
            chunk /= 10;
        }
    }
}
template <typename CharT, typename UT, enable_if_t<!(sizeof(UT) > 4), int> = 0>
MSTL_NODISCARD inline void __uint_to_buff_aux(CharT* riter, UT& ux) {}
#endif // MSTL_DATA_BUS_WIDTH_64__

template <typename CharT, typename UT>
MSTL_NODISCARD CharT* uint_to_buff(CharT* riter, UT ux) {
    static_assert(is_unsigned_v<UT>, "UT must be unsigned types.");
#ifdef MSTL_DATA_BUS_WIDTH_64__
    auto holder = ux;
#else
    MSTL::__uint_to_buff_aux(riter, ux);
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
#ifdef MSTL_PLATFORM_LINUX__
    const int len = ::snprintf(nullptr, 0, "%f", x);
    string str(len, '\0');
    ::snprintf(&str[0], len + 1, "%f", x);
    return str;
#else
    const auto len = static_cast<size_t>(::_scprintf("%f", x));
    string str(len, '\0');
    ::sprintf_s(&str[0], len + 1, "%f", x);
    return str;
#endif
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
#ifdef MSTL_PLATFORM_LINUX__
    std::setlocale(LC_ALL, "en_US.UTF-8");
    const int len = ::swprintf(nullptr, 0, L"%f", x);
    wstring str(len, L'\0');
    ::swprintf(&str[0], len + 1, L"%f", x);
    return str;
#else
    const auto len = static_cast<size_t>(::_scwprintf(L"%f", x));
    wstring str(len, L'\0');
    ::swprintf_s(&str[0], len + 1, L"%f", x);
    return str;
#endif
}
MSTL_NODISCARD inline wstring to_wstring(float x) {
    return to_wstring(static_cast<double>(x));
}
MSTL_NODISCARD inline wstring to_wstring(long double x) {
    return to_wstring(static_cast<double>(x));
}


template <typename CharT, typename T, enable_if_t<is_same_v<CharT, char>, int> = 0>
MSTL_NODISCARD basic_string<CharT> __stream_to_string(T x) {
    return MSTL::to_string(x);
}
template <typename CharT, typename T, enable_if_t<is_same_v<CharT, wchar_t>, int> = 0>
MSTL_NODISCARD basic_string<CharT> __stream_to_string(T x) {
    return MSTL::to_wstring(x);
}

MSTL_END_NAMESPACE__
#endif // MSTL_STRING_HPP__
