#ifndef MSTL_HASH_FUNCTION_HPP__
#define MSTL_HASH_FUNCTION_HPP__
#include "pair.hpp"
#include "tuple.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Key>
struct hash {};

template <typename T>
struct hash<T*> {
    MSTL_NODISCARD size_t operator()(T* ptr) const noexcept {
        return reinterpret_cast<size_t>(ptr);
    }
};

inline size_t __hash_string(const char* s) noexcept {
    size_t h = 0;
    for (; *s; ++s)
        h = 5 * h + *s;
    return h;
}

TEMNULL__ struct hash<char*> {
    MSTL_NODISCARD size_t operator ()(const char* s) const noexcept { return __hash_string(s); }
};
TEMNULL__ struct hash<const char*> {
    MSTL_NODISCARD size_t operator ()(const char* s) const noexcept { return __hash_string(s); }
};
TEMNULL__ struct hash<std::string> {
    MSTL_NODISCARD size_t operator ()(const std::string& s) const noexcept { return __hash_string(s.c_str()); }
};
#define INT_HASH_STRUCT__(OPT) \
    TEMNULL__ struct hash<OPT> { \
        MSTL_NODISCARD size_t operator ()(OPT x) const noexcept { return x; } \
    };
INT_HASH_STRUCT__(bool)
INT_HASH_STRUCT__(char)
INT_HASH_STRUCT__(unsigned char)
INT_HASH_STRUCT__(signed char)
INT_HASH_STRUCT__(wchar_t)
#ifdef MSTL_VERSION_20__
INT_HASH_STRUCT__(char8_t)
#endif
INT_HASH_STRUCT__(char16_t)
INT_HASH_STRUCT__(char32_t)
INT_HASH_STRUCT__(short)
INT_HASH_STRUCT__(unsigned short)
INT_HASH_STRUCT__(int)
INT_HASH_STRUCT__(unsigned int)
INT_HASH_STRUCT__(long)
INT_HASH_STRUCT__(unsigned long)
INT_HASH_STRUCT__(long long)
INT_HASH_STRUCT__(size_t)

#if defined(MSTL_PLATFORM_WIN64__) || defined(MSTL_PLATFORM_LINUX64__)
static constexpr size_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
static constexpr size_t FNV_PRIME = 1099511628211ULL;
#elif defined(MSTL_PLATFORM_WIN32__) || defined(MSTL_PLATFORM_LINUX32__)
static constexpr size_t FNV_OFFSET_BASIS = 2166136261U;
static constexpr size_t FNV_PRIME = 16777619U;
#endif

// the FNV (Fowler-Noll-Vo) hash algorithm is a non-encrypted hash function
// with a good avalanche effect and a low collision rate.
// FNV_hash function is FNV-1a version.
inline size_t FNV_hash(const unsigned char* first, size_t count) noexcept {
    size_t result = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < count; i++) {
        result ^= (size_t)first[i];
        result *= FNV_PRIME;
    }
    return result;
}

template <typename... T, size_t... Index>
inline size_t tuple_hash(const MSTL::tuple<T...>& tup, std::index_sequence<Index...>) noexcept {
    return (hash<tuple_element_t<Index, T...>>()(MSTL::get<Index>(tup)) ^ ...);
}

#define FLOAT_HASH_STRUCT__(OPT) \
    TEMNULL__ struct hash<OPT> { \
        MSTL_NODISCARD size_t operator ()(const OPT & x) const noexcept { \
            return x == 0.0f ? 0 : FNV_hash((const unsigned char*)&x, sizeof(OPT)); \
        } \
    };
FLOAT_HASH_STRUCT__(float);
FLOAT_HASH_STRUCT__(double);
FLOAT_HASH_STRUCT__(long double);

template <typename T1, typename T2>
struct hash<MSTL::pair<T1, T2>> {
    MSTL_NODISCARD size_t operator() (const MSTL::pair<T1, T2>& pair) const noexcept {
		return hash<T1>()(pair.first) ^ hash<T2>()(pair.second);
	}
};

template <typename... T>
struct hash<MSTL::tuple<T...>> {
    MSTL_NODISCARD size_t operator() (const MSTL::tuple<T...>& tup) const noexcept {
        return tuple_hash(tup, std::index_sequence_for<T...>{});
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_HASH_FUNCTION_HPP__
