#ifndef MSTL_HASH_FUNCTION_HPP__
#define MSTL_HASH_FUNCTION_HPP__
#include "basiclib.h"
#include "pair.hpp"
#include "tuple.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Key>
struct hash {
    size_t operator()(const Key& data) const {
        return size_t(data);
    }
};

inline size_t __hash_string(const char* s) {
    size_t h = 0;
    for (; *s; ++s)
        h = 5 * h + *s;
    return h;
}

MSTL_TEMPLATE_NULL__ struct hash<char*> {
    size_t operator ()(const char* s) const { return __hash_string(s); }
};
MSTL_TEMPLATE_NULL__ struct hash<const char*> {
    size_t operator ()(const char* s) const { return __hash_string(s); }
};
MSTL_TEMPLATE_NULL__ struct hash<std::string> {
    size_t operator ()(const std::string& s) const { return __hash_string(s.c_str()); }
};
#define HASH_STRUCT__(OPT) \
    MSTL_TEMPLATE_NULL__ struct hash<OPT> { \
    size_t operator ()(OPT x) const { return x; } \
    };
HASH_STRUCT__(char)
HASH_STRUCT__(unsigned char)
HASH_STRUCT__(signed char)
HASH_STRUCT__(short)
HASH_STRUCT__(unsigned short)
HASH_STRUCT__(int)
HASH_STRUCT__(unsigned int)
HASH_STRUCT__(long)
HASH_STRUCT__(unsigned long)
HASH_STRUCT__(long long)
HASH_STRUCT__(size_t)

template <typename T1, typename T2>
struct hash<MSTL::pair<T1, T2>> {
    size_t operator() (const MSTL::pair<T1, T2>& pair) const {
		return hash<T1>()(pair.first) ^ hash<T2>()(pair.second);
	}
};
template <typename T1, typename T2>
struct hash<std::pair<T1, T2>> {
    size_t operator() (const std::pair<T1, T2>& pair) const {
        return hash<T1>()(pair.first) ^ hash<T2>()(pair.second);
    }
};
template <typename... T>
struct hash<MSTL::tuple<T...>> {
    template <size_t... Index>
    static size_t get_hash(const MSTL::tuple<T...>& tup, std::index_sequence<Index...>) {
        return (... ^ hash<tuple_element_t<Index, T...>>()(MSTL::get<Index>(tup)));
    }
    size_t operator() (const MSTL::tuple<T...>& tup) const {
        return get_hash(tup, std::index_sequence_for<T...>{});
    }
};
template <typename... T>
struct hash<std::tuple<T...>> {
    template <size_t... Index>
    static size_t get_hash(const std::tuple<T...> tup, std::index_sequence<Index...>) {
        return (... ^ hash<tuple_element_t<Index, T...>>()(std::get(tup)));
    }
    size_t operator() (const std::tuple<T...>& tup) const {
        return get_hash(tup, std::index_sequence_for<T...>{});
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_HASH_FUNCTION_HPP__
