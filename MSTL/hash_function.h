#ifndef MSTL_HASH_FUNCTION_HPP__
#define MSTL_HASH_FUNCTION_HPP__
#include "utility.hpp"
#include "macro_ranges.h"
MSTL_BEGIN_NAMESPACE__

template <typename Key>
struct hash {};

template <typename T>
struct hash<T*> {
    MSTL_NODISCARD size_t operator()(T* ptr) const noexcept {
        return reinterpret_cast<size_t>(ptr);
    }
};


// DJB2 is a non-cryptographic hash algorithm
// with simple implement, fast speed and evenly distributed.
// but in some special cases, there will still occur hash conflicts.
size_t DJB2_hash(const char* str);

#ifdef MSTL_DATA_BUS_WIDTH_32__

constexpr uint32_t BLOCK_MULTIPLIER32_1 = 0xcc9e2d51;
constexpr uint32_t BLOCK_MULTIPLIER32_2 = 0x1b873593;
constexpr uint32_t HASH_UPDATE_CONSTANT32 = 0xe6546b64;
constexpr uint32_t FINAL_MIX_MULTIPLIER32_1 = 0x85ebca6b;
constexpr uint32_t FINAL_MIX_MULTIPLIER32_2 = 0xc2b2ae35;

constexpr uint32_t hash_rotate_x32(uint32_t x, int r) {
    return (x << r) | (x >> (32 - r));
}

uint32_t MurmurHash_x32(const char* key, size_t len, uint32_t seed);

#endif

#ifdef MSTL_DATA_BUS_WIDTH_64__

constexpr uint64_t hash_rotate_x64(uint64_t x, int r) {
    return (x << r) | (x >> (64 - r));
}

constexpr uint64_t MULTIPLIER64_1 = 0x87c37b91114253d5ULL;
constexpr uint64_t MULTIPLIER64_2 = 0x4cf5ad432745937fULL;
constexpr uint64_t FINAL_MIX_MULTIPLIER64_1 = 0xff51afd7ed558ccdULL;
constexpr uint64_t FINAL_MIX_MULTIPLIER64_2 = 0xc4ceb9fe1a85ec53ULL;
constexpr uint64_t HASH_UPDATE_CONSTANT64_1 = 0x52dce729;
constexpr uint64_t HASH_UPDATE_CONSTANT64_2 = 0x38495ab5;

constexpr uint64_t hash_mix_x64(uint64_t k) {
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
pair<size_t, size_t> MurmurHash_x64(const char* key, size_t len, uint32_t seed);

#endif

inline size_t string_hash(const char* s) noexcept {
#ifdef MSTL_DATA_BUS_WIDTH_64__
    pair<size_t, size_t> p = MurmurHash_x64(s, MSTL::strlen(s), std::rand());
    return p.first ^ p.second;
#elif MSTL_DATA_BUS_WIDTH_32__
    return (size_t)MurmurHash_x32(s, MSTL::strlen(s), std::rand());
#else 
    return DJB2_hash(s);
#endif
} 

TEMNULL__ struct hash<char*> { \
    MSTL_NODISCARD size_t operator ()(const char* s) const noexcept { return string_hash(s); } \
}; \
TEMNULL__ struct hash<const char*> { \
    MSTL_NODISCARD size_t operator ()(const char* s) const noexcept { return string_hash(s); } \
};
TEMNULL__ struct hash<std::string> {
    MSTL_NODISCARD size_t operator ()(const std::string& s) const noexcept { return string_hash(s.c_str()); }
};


#define INT_HASH_STRUCT__(OPT) \
    TEMNULL__ struct hash<OPT> { \
        MSTL_NODISCARD size_t operator ()(OPT x) const noexcept { return size_t(x); } \
    };
MSTL_MACRO_RANGE_CHARS(INT_HASH_STRUCT__)
MSTL_MACRO_RANGE_INTEGRAL(INT_HASH_STRUCT__)


#if defined(MSTL_PLATFORM_WIN64__) || defined(MSTL_PLATFORM_LINUX64__)
static constexpr size_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
static constexpr size_t FNV_PRIME = 1099511628211ULL;
#elif defined(MSTL_PLATFORM_WIN32__) || defined(MSTL_PLATFORM_LINUX32__)
static constexpr size_t FNV_OFFSET_BASIS = 2166136261U;
static constexpr size_t FNV_PRIME = 16777619U;
#endif

// the FNV (Fowler-Noll-Vo) is a non-cryptographic hash algorithm
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

#define FLOAT_HASH_STRUCT__(OPT) \
    TEMNULL__ struct hash<OPT> { \
        MSTL_NODISCARD size_t operator ()(const OPT& x) const noexcept { \
            return x == 0.0f ? 0 : FNV_hash((const unsigned char*)&x, sizeof(OPT)); \
        } \
    };
MSTL_MACRO_RANGE_FLOAT(FLOAT_HASH_STRUCT__)


template <typename T1, typename T2>
struct hash<MSTL::pair<T1, T2>> {
    MSTL_NODISCARD size_t operator() (const MSTL::pair<T1, T2>& pair) const noexcept {
		return hash<T1>()(pair.first) ^ hash<T2>()(pair.second);
	}
};

MSTL_END_NAMESPACE__
#endif // MSTL_HASH_FUNCTION_HPP__
