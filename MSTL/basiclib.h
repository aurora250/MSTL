#ifndef MSTL_BASICLIB_H__
#define MSTL_BASICLIB_H__
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_) || defined(WIN64) || defined(_WIN64) || defined(_WIN64_)
#define PLATFORM_WINDOWS 1
#elif defined(ANDROID) || defined(_ANDROID_)
#define PLATFORM_ANDROID 1
#elif defined(__linux__)
#define PLATFORM_LINUX	 1
#elif defined(__APPLE__) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_MAC)
#define PLATFORM_IOS	 1
#else
#define PLATFORM_UNKNOWN 1
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

#if defined(_GLIBCXX_ITERATOR) || defined(_GLIBCXX_PARALLEL_ITERATOR_H) || defined(_ITERATOR_)
#define MSTL_NEED_ITERATOR_TAG__ 0
#else
#define MSTL_NEED_ITERATOR_TAG__ 1
#endif // _ITERATOR_

#if defined(__TYPE_TRAITS_H)
#define MSTL_NEED_SGI_TYPE_TRAITS__ 0
#else
#define MSTL_NEED_SGI_TYPE_TRAITS__ 1
#endif

#if defined(__GNUC__) || defined(__clang__)
#define MSTL_DLL_LINK__ 0 
#else
#define MSTL_DLL_LINK__ 1
#endif // __GNUC__

#if defined(__GNUC__)
#define MSTL_ITERATOR_TRATIS_FROM__ std::
#else 
#define MSTL_ITERATOR_TRATIS_FROM__ 
#endif

MSTL_BEGIN_NAMESPACE__

#if defined(PLATFORM_WINDOWS)
#ifdef _MSC_VER
typedef unsigned __int64 size_t;
typedef __int64 ptrdiff_t;
typedef __int64 intptr_t;
#else  // not define _MSC_VER   // FOR __GNUC__
typedef unsigned long long size_t;
typedef long long ptrdiff_t;
typedef long long intptr_t;
#endif // _MSC_VER
#elif defined(PLATFORM_LINUX)  // not define _WIN64 || _WIN32   // FOR __linux__
typedef unsigned long long size_t;
typedef long long ptrdiff_t;
typedef long long intptr_t;
#else
typedef unsigned int size_t;
typedef int ptrdiff_t;
typedef int intptr_t;
#endif // __linux__

#define TO_STRING(VALUE) #VALUE
#define FOR(VALUE, CONTAINER) for(auto VALUE = CONTAINER.begin(); VALUE != CONTAINER.end(); ++VALUE)

extern void* memcpy(void* _dest, void* _rsc, int _byte);
extern int memcmp(const void* _dest, const void* _rsc, int _byte);
extern void* memmove(void* _dest, const void* _rsc, int _byte);
extern void* memset(void* _dest, int _val, size_t _byte);

size_t strlen(const char* _str);
char* strcpy(char* _dest, const char* _sou);
int strcmp(const char* _des, const char* _sou);
const char* strstr(const char* _des, const char* _sou);
char* memstr(char* _data, size_t _len, char* _sub);

extern void split_line(std::ostream & _out = std::cout, size_t _size = 15);
extern size_t deque_buf_size(size_t n, size_t sz);

MSTL_END_NAMESPACE__

#endif // MSTL_BASICLIB_H__
