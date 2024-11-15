#ifndef MSTL_BASICLIB_H__
#define MSTL_BASICLIB_H__
#include <iostream>

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
#define MSTL_SUPPORT_CONCEPTS__
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

#if defined(__GNUC__) || defined(__clang__)
#define MSTL_ITERATOR_TRATIS_FROM__ std::
#else 
#define MSTL_ITERATOR_TRATIS_FROM__ 
#endif

MSTL_BEGIN_NAMESPACE__

#if defined(_WIN64) || defined(_WIN32)
#ifdef _MSC_VER
typedef unsigned __int64 size_t;
typedef __int64 ptrdiff_t;
typedef __int64 intptr_t;
#else  // not define _MSC_VER   // FOR __GNUC__
typedef unsigned long long size_t;
typedef long long ptrdiff_t;
typedef long long intptr_t;
#endif // _MSC_VER
#elif defined(__linux__)  // not define _WIN64 || _WIN32   // FOR __linux__
typedef unsigned long long size_t;
typedef long long ptrdiff_t;
typedef long long intptr_t;
#else
typedef unsigned int size_t;
typedef int ptrdiff_t;
typedef int intptr_t;
#endif // __linux__

extern void split_line(std::ostream& _out = std::cout, size_t _size = 15);

extern void* memcpy(void* _dest, void* _rsc, int _byte);
extern int memcmp(const void* _dest, const void* _rsc, int _byte);
extern void* memmove(void* _dest, const void* _rsc, int _byte);
extern void* memset(void* _dest, int _val, size_t _byte);
extern size_t deque_buf_size(size_t n, size_t sz);

MSTL_END_NAMESPACE__

#endif // MSTL_BASICLIB_H__
