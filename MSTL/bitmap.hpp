#ifndef MSTL_BITMAP_HPP__
#define MSTL_BITMAP_HPP__
#include "type_traits.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename CharT, bool = is_character_v<CharT>>
class __string_bitmap {
private:
    bool matches_[256] = {};

public:
    MSTL_CONSTEXPR __string_bitmap() = default;

    MSTL_CONSTEXPR bool mark(const CharT* first, const CharT* const last) noexcept {
        for (; first != last; ++first) 
            matches_[static_cast<unsigned char>(*first)] = true;
        return true;
    }
    MSTL_CONSTEXPR bool match(const CharT chr) const noexcept {
        return matches_[static_cast<unsigned char>(chr)];
    }
};
template <typename CharT>
class __string_bitmap<CharT, false> {};

MSTL_END_NAMESPACE__
#endif // MSTL_BITMAP_HPP__
