#ifndef MSTL_STRING_HPP__
#define MSTL_STRING_HPP__
#include "basic_string.h"
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
template <typename CharT, typename Alloc>
struct hash<basic_string<CharT, char_traits<CharT>, Alloc>> {
    MSTL_NODISCARD size_t operator ()(const basic_string<CharT, char_traits<CharT>, Alloc>& str) const noexcept {
        return FNV_hash(reinterpret_cast<const byte_t*>(str.c_str()), sizeof(CharT) * str.size());
    }
};
TEMNULL__ struct hash<std::string> {
    MSTL_NODISCARD size_t operator ()(const std::string& s) const noexcept {
        return string_hash(s.c_str(), MSTL::strlen(s.c_str()), 0);
    }
};


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

MSTL_END_NAMESPACE__
#endif // MSTL_STRING_HPP__
