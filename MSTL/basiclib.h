#ifndef MSTL_BASICLIB_H__
#define MSTL_BASICLIB_H__
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_) || defined(WIN64) || defined(_WIN64) || defined(_WIN64_)
#define MSTL_PLATFORM_WINDOWS__ 1
#elif defined(ANDROID) || defined(_ANDROID_)
#define MSTL_PLATFORM_ANDROID__ 1
#elif defined(__linux__)
#define MSTL_PLATFORM_LINUX__	1
#elif defined(__APPLE__) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_MAC)
#define MSTL_PLATFORM_IOS__     1
#else
#define MSTL_PLATFORM_UNKNOWN__ 1
#endif

#if defined(__GNUC__)
#define MSTL_COMPILE_GNUC__	  1
#if defined(__clang__)
#define MSTL_COMPILE_CLANG__  1
#else
#define MSTL_COMPILE_GCC__    1
#endif
#elif defined(_MSC_VER)
#define MSTL_COMPILE_MSVC__   1
#else
#define MSTL_COMPILE_UNKNOW__ 1
#endif

#ifndef NULL
#ifdef __cplusplus
#ifdef _WIN64
#define NULL 0LL
#else // not define _WIN64
#define NULL 0
#endif // _WIN64
#else // not define __cplusplus
#define NULL ((void *)0)
#endif // __cplusplus
#endif // NULL

#define MSTL_NAMESPACE__ using namespace MSTL;
#define MSTL_BEGIN_NAMESPACE__ namespace MSTL {
#define MSTL_END_NAMESPACE__ }

#if defined(_HAS_CXX20)
#define MSTL_SUPPORT_CONCEPTS__ 1
#endif

#if !(defined(_GLIBCXX_ITERATOR) || defined(_GLIBCXX_PARALLEL_ITERATOR_H) || defined(_ITERATOR_))
#define MSTL_NEED_ITERATOR_TAG__ 1
#endif

#if !defined(__TYPE_TRAITS_H)
#define MSTL_NEED_SGI_TYPE_TRAITS__ 1
#endif

#if defined(MSTL_COMPILE_MSVC__)
#define MSTL_DLL_LINK__ 1
#endif // __GNUC__

#if defined(MSTL_COMPILE_GNUC__)
#define MSTL_ITERATOR_TRATIS_FROM__ std::
#else 
#define MSTL_ITERATOR_TRATIS_FROM__ 
#endif

#define TO_STRING(VALUE) #VALUE
#define FOR_EACH(VALUE, CONTAINER) for(auto VALUE = CONTAINER.begin(); VALUE != CONTAINER.end(); ++VALUE)
#if defined(__FILE__)
#define FILE_PATH TO_STRING(__FILE__)
#endif

// #if defined(_HAS_CXX17)
// typedef decltype(nullptr) nullptr_t;
// #endif

#if defined(MSTL_PLATFORM_WINDOWS__) && defined(MSTL_COMPILE_MSVC__)
#define MSTL_LONG_LONG_TYPE__ __int64
#else
#define MSTL_LONG_LONG_TYPE__ long long
#endif

#define MSTL_TEMPLATE_NULL__ template<>

MSTL_BEGIN_NAMESPACE__

typedef unsigned MSTL_LONG_LONG_TYPE__ size_t;
typedef MSTL_LONG_LONG_TYPE__ ptrdiff_t;
typedef MSTL_LONG_LONG_TYPE__ intptr_t;

typedef const char* cstring;
typedef const char* const const_cstring;

extern void* memcpy(void* _dest, void* _rsc, int _byte);
extern int memcmp(const void* _dest, const void* _rsc, int _byte);
extern void* memmove(void* _dest, const void* _rsc, int _byte);
extern void* memset(void* _dest, int _val, int _byte);

extern size_t strlen(const char* _str);
extern char* strcpy(char* _dest, const char* _sou);
extern int strcmp(const char* _des, const char* _sou);
extern const char* strstr(const char* _des, const char* _sou);
extern char* memstr(char* _data, int _len, char* _sub);

extern void split_line(std::ostream & _out = std::cout, size_t _size = 15);
extern size_t deque_buf_size(size_t n, size_t sz);

MSTL_END_NAMESPACE__
#endif // MSTL_BASICLIB_H__
