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

template <>
struct hash<_MSTL string> {
    MSTL_NODISCARD size_t operator ()(const _MSTL string& s) const noexcept {
        return string_hash(s.c_str(), _MSTL string_length(s.c_str()), 0);
    }
};

template <typename CharT, typename Traits, typename Alloc>
struct hash<basic_string<CharT, Traits, Alloc>> {
    MSTL_NODISCARD size_t operator ()(
        const basic_string<CharT, Traits, Alloc>& str) const noexcept {
        return FNV_hash(reinterpret_cast<const byte_t*>(str.c_str()), sizeof(CharT) * str.size());
    }
};

template <>
struct hash<std::string> {
    MSTL_NODISCARD size_t operator ()(const std::string& s) const noexcept {
        return string_hash(s.c_str(), _MSTL string_length(s.c_str()), 0);
    }
};


#ifdef MSTL_VERSION_17__
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
#endif // MSTL_VERSION_17__


inline string wstring_to_utf8(const wchar_t* str) {
    string utf8_str;
    if (!str) return utf8_str;

    size_t len = char_traits<wchar_t>::length(str);
#ifdef MSTL_PLATFORM_WINDOWS__
    const int size_needed = WideCharToMultiByte(CP_UTF8, 0, str,
        static_cast<int>(len), nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0) return utf8_str;

    utf8_str.resize(size_needed);
    WideCharToMultiByte(CP_UTF8, 0, str,
        static_cast<int>(len), &utf8_str[0], size_needed, nullptr, nullptr);
#elif defined(MSTL_PLATFORM_LINUX__)
    for (size_t i = 0; i < len; ++i) {
        const auto cp = static_cast<uint32_t>(str[i]);

        if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
            utf8_str += "\xEF\xBF\xBD";
            continue;
        }
        if (cp <= 0x7F) {
            utf8_str += static_cast<char>(cp);
        } else if (cp <= 0x7FF) {
            utf8_str += static_cast<char>(0xC0 | (cp >> 6));
            utf8_str += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp <= 0xFFFF) {
            utf8_str += static_cast<char>(0xE0 | (cp >> 12));
            utf8_str += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            utf8_str += static_cast<char>(0x80 | (cp & 0x3F));
        } else {
            utf8_str += static_cast<char>(0xF0 | (cp >> 18));
            utf8_str += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            utf8_str += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            utf8_str += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }
#endif
    return _MSTL move(utf8_str);
}

#ifdef MSTL_PLATFORM_WINDOWS__
inline string u16string_to_utf8(const char16_t* str) {
    return wstring_to_utf8(reinterpret_cast<const wchar_t*>(str));
}
#elif defined(MSTL_PLATFORM_LINUX__)
constexpr bool is_high_surrogate(const char16_t c) {
    return (c >= 0xD800 && c <= 0xDBFF);
}
constexpr bool is_low_surrogate(const char16_t c) {
    return (c >= 0xDC00 && c <= 0xDFFF);
}

constexpr uint32_t combine_surrogates(const char16_t high, const char16_t low) {
    return 0x10000 + ((static_cast<uint32_t>(high) - 0xD800) << 10) +
           (static_cast<uint32_t>(low) - 0xDC00);
}

inline string u16string_to_utf8(const char16_t* str) {
    string utf8_str;
    if (!str) return utf8_str;

    for (size_t i = 0; str[i] != u'\0'; ++i) {
        const char16_t c1 = str[i];
        if (i == 0 && c1 == 0xFEFF) {
            continue;
        }
        if (!is_high_surrogate(c1) && !is_low_surrogate(c1)) {
            const auto cp = static_cast<uint32_t>(c1);

            if (cp > 0x10FFFF) {
                utf8_str += "\xEF\xBF\xBD";
                continue;
            }
            if (cp <= 0x7F) {
                utf8_str += static_cast<char>(cp);
            } else if (cp <= 0x7FF) {
                utf8_str += static_cast<char>(0xC0 | (cp >> 6));
                utf8_str += static_cast<char>(0x80 | (cp & 0x3F));
            } else {
                utf8_str += static_cast<char>(0xE0 | (cp >> 12));
                utf8_str += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                utf8_str += static_cast<char>(0x80 | (cp & 0x3F));
            }
        }
        else if (is_high_surrogate(c1)) {
            if (str[i+1] == u'\0' || !is_low_surrogate(str[i+1])) {
                utf8_str += "\xEF\xBF\xBD";
                continue;
            }
            const char16_t c2 = str[++i];
            const uint32_t cp = combine_surrogates(c1, c2);

            utf8_str += static_cast<char>(0xF0 | (cp >> 18));
            utf8_str += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            utf8_str += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            utf8_str += static_cast<char>(0x80 | (cp & 0x3F));
        }
        else {
            utf8_str += "\xEF\xBF\xBD";
        }
    }
    return _MSTL move(utf8_str);
}
#endif

inline string u32string_to_utf8(const char32_t* str) {
    string utf8_str;
    if (!str) return utf8_str;

    size_t len = char_traits<char32_t>::length(str);
#ifdef MSTL_PLATFORM_WINDOWS__
    wstring utf16_buf;
    // In the worst case, each UTF-32 character needs to be split into 2 UTF-16 surrogate pairs.
    utf16_buf.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        char32_t cp = str[i];
        if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
            utf16_buf.push_back(0xFFFD);
            continue;
        }
        if (cp <= 0xFFFF) {  // BMP
            utf16_buf.push_back(static_cast<wchar_t>(cp));
        } else {
            cp -= 0x10000;
            utf16_buf.push_back(static_cast<wchar_t>((cp >> 10) + 0xD800));
            utf16_buf.push_back(static_cast<wchar_t>((cp & 0x3FF) + 0xDC00));
        }
    }
    const int size_needed = WideCharToMultiByte(CP_UTF8, 0,
        utf16_buf.data(), static_cast<int>(utf16_buf.size()),
        nullptr, 0, nullptr, nullptr
    );
    if (size_needed <= 0) return utf8_str;

    utf8_str.resize(size_needed);
    const int written = WideCharToMultiByte(CP_UTF8, 0,
        utf16_buf.data(), static_cast<int>(utf16_buf.size()),
        &utf8_str[0], size_needed, nullptr, nullptr
    );
    if (written != size_needed) {
        utf8_str.resize(written > 0 ? written : 0);
    }
#elif defined(MSTL_PLATFORM_LINUX__)
    for (size_t i = 0; i < len; ++i) {
        auto cp = static_cast<uint32_t>(str[i]);

        if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
            utf8_str += "\xEF\xBF\xBD";
            continue;
        }
        if (cp <= 0x7F) {
            utf8_str += static_cast<char>(cp);
        } else if (cp <= 0x7FF) {
            utf8_str += static_cast<char>(0xC0 | (cp >> 6));
            utf8_str += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp <= 0xFFFF) {
            utf8_str += static_cast<char>(0xE0 | (cp >> 12));
            utf8_str += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            utf8_str += static_cast<char>(0x80 | (cp & 0x3F));
        } else {
            utf8_str += static_cast<char>(0xF0 | (cp >> 18));
            utf8_str += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            utf8_str += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            utf8_str += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }
#endif
    return _MSTL move(utf8_str);
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
    MSTL_ __uint_to_buff_aux(riter, ux);
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
MSTL_NODISCARD inline string to_string(long long x) {
    return int_to_string<char>(x);
}
MSTL_NODISCARD inline string to_string(unsigned long long x) {
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
#else
    const auto len = static_cast<size_t>(::_scwprintf(L"%f", x));
#endif
    wstring str(len, L'\0');
    ::swprintf(&str[0], len + 1, L"%f", x);
    return str;
}
MSTL_NODISCARD inline wstring to_wstring(float x) {
    return to_wstring(static_cast<double>(x));
}
MSTL_NODISCARD inline wstring to_wstring(long double x) {
    return to_wstring(static_cast<double>(x));
}


template <typename CharT, typename T, enable_if_t<is_same_v<CharT, char>, int> = 0>
MSTL_NODISCARD basic_string<CharT> __stream_to_string(T x) {
    return _MSTL to_string(x);
}
template <typename CharT, typename T, enable_if_t<is_same_v<CharT, wchar_t>, int> = 0>
MSTL_NODISCARD basic_string<CharT> __stream_to_string(T x) {
    return _MSTL to_wstring(x);
}

MSTL_END_NAMESPACE__
#endif // MSTL_STRING_HPP__
