#ifndef MSTL_BASICLIB_H__
#define MSTL_BASICLIB_H__
#include <initializer_list>
#include <type_traits>
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

#define MSTL_USE_SPACE__ using namespace MSTL;
#define MSTL__ MSTL
#define MSTL_BEGIN_NAMESPACE__ namespace MSTL {
#define MSTL_END_NAMESPACE__ }

#if defined(_GLIBCXX_ITERATOR) || defined(_GLIBCXX_PARALLEL_ITERATOR_H) || defined(_ITERATOR_)
#define MSTL_NEED_ITERATOR_TAG__ 0
#else
#define MSTL_NEED_ITERATOR_TAG__ 1
#endif // _ITERATOR_

#if defined(_EXT_TYPE_TRAITS) || defined(__TYPE_TRAITS_H)
#define MSTL_NEED_SGI_TYPE_TRAITS__ 0
#else
#define MSTL_NEED_SGI_TYPE_TRAITS__ 1
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

extern void repect_ostm_str(const char* _str, std::ostream& _out, size_t _size);
extern void repect_ostm_chr(char _chr, std::ostream& _out, size_t _size);
extern void split_line(std::ostream& _out = std::cout, size_t _size = 15);

extern void* memcpy(void* _dest, void* _rsc, int _byte);
extern int memcmp(const void* _dest, const void* _rsc, int _byte);
extern void* memmove(void* _dest, const void* _rsc, int _byte);
extern void* memset(void* _dest, int _val, size_t _byte);
extern size_t deque_buf_size(size_t n, size_t sz);

template <typename T>
void ostream_set(const std::initializer_list<T>& _vals,
	bool _enter = false, std::ostream& _out = std::cout,
	const char* _join = nullptr) {
	auto _beg = _vals.begin();
	size_t _size = _vals.size();
	if (not _size == 0) {
		if (_join) {
			for (; _beg != _vals.end(); _beg++) {
				_out << (*_beg) << std::flush;
				if (--_size)
					_out << _join << std::flush;
			}
			// or:
			// #include <iterator>  #include <algorithm>
			// std::copy(_vals.begin(), _vals.end(), std::ostream_iterator(std::cout, " "))
		}
		else {
			for (; _beg != _vals.end(); _beg++)
				_out << (*_beg) << std::flush;
		}
	}
	if (_enter) _out << std::endl;
}

MSTL_END_NAMESPACE__

#endif // MSTL_BASICLIB_H__