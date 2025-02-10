#ifndef MSTL_BASICLIB_H__
#define MSTL_BASICLIB_H__
#include <iostream>
#include <assert.h>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_)
	#define MSTL_PLATFORM_WINDOWS__		1
	#define MSTL_PLATFORM_WIN32__		1
	#if defined(WIN64) || defined(_WIN64) || defined(_WIN64_)
		#define MSTL_PLATFORM_WIN64__	1
	#endif // win64
#elif defined(__linux__) // not windows
	#define MSTL_PLATFORM_LINUX__		1
	#if (__WORDSIZE == 64) || (__SIZEOF_POINTER__ == 8)
		#define MSTL_PLATFORM_LINUX64__ 1
	#elif (__WORDSIZE == 32) || (__SIZEOF_POINTER__ == 4)
		#define MSTL_PLATFORM_LINUX32__ 1
	#endif
#elif defined(__APPLE__) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_MAC)
	#define MSTL_PLATFORM_IOS__			1
#else
	#define MSTL_PLATFORM_UNSUPPORT__	1
#endif

#if defined(__GNUC__)
	#define MSTL_COMPILE_GNUC__			1
	#if defined(__clang__)
		#define MSTL_COMPILE_CLANG__	1
	#else
		#define MSTL_COMPILE_GCC__		1
	#endif
#elif defined(_MSC_VER)
	#define MSTL_COMPILE_MSVC__			1
#else
	#define MSTL_COMPILE_UNSUPPORT__	1
#endif


#if defined(_M_CEE)
	#define MSTL_COMPILE_WITH_CRL__		1
#elif defined(__EDG__)
	#define MSTL_COMPILE_WITH_EDG__		1
#else
	#define MSTL_COMPILE_WITH_OS__		1
#endif


#if defined(DEBUG) || defined(_DEBUG) || defined(QT_QML_DEBUG)
	#define MSTL_STATE_DEBUG__			1
#else
	#define MSTL_STATE_RELEASE__		1
#endif


#if defined(MSTL_PLATFORM_WIN64__) || defined(MSTL_PLATFORM_LINUX64__)
	#define MSTL_DATA_BUS_WIDTH_64__	1
#endif
#if defined(MSTL_PLATFORM_WIN32__) || defined(MSTL_PLATFORM_LINUX32__)
	#define MSTL_DATA_BUS_WIDTH_32__	1
#endif


#define MSTL_NAMESPACE__ using namespace MSTL;
#define MSTL_BEGIN_NAMESPACE__ namespace MSTL {
#define MSTL_END_NAMESPACE__ }
#define MSTL_ MSTL::


#if defined(_HAS_CXX20) || (__cplusplus >= 202002L)
	#define MSTL_VERSION_20__	1
#endif
#if defined(_HAS_CXX17) || (__cplusplus >= 201703L) || defined(MSTL_VERSION_20__)
	#define MSTL_VERSION_17__	1
#endif
#if (__cplusplus >= 201402L) || defined(MSTL_VERSION_17__)
	#define MSTL_VERSION_14__	1
#endif
#if (__cplusplus >= 201103L) || defined(MSTL_VERSION_14__)
	#define MSTL_VERSION_11__	1
#endif
#if (__cplusplus >= 199711L) || defined(MSTL_VERSION_11__)
	#define MSTL_VERSION_98__	1
#endif


#if defined(MSTL_VERSION_20__)
	#define MSTL_SUPPORT_CONCEPTS__			1
#endif
#if defined(MSTL_VERSION_17__)
	#define MSTL_SUPPORT_NODISCARD__		1
#endif
#if defined(MSTL_VERSION_11__)
	#define MSTL_SUPPORT_NORETURN__			1
#endif
#if defined(MSTL_VERSION_14__)
	#define MSTL_SUPPORT_DEPRECATED__		1
#endif
#if defined(MSTL_VERSION_17__)
	#define MSTL_SUPPORT_DEDUCTION_GUIDES__ 1
#endif
#if defined(MSTL_VERSION_11__)
	#define MSTL_SUPPORT_CONSTEXPR__		1
#endif
#if defined(MSTL_VERSION_11__)
	#define MSTL_SUPPORT_STATIC_ASSERT__	1
#endif
#if defined(MSTL_COMPILE_MSVC__)
	#define MSTL_SUPPORT_DECLALLOC__		1
#endif
#if defined(MSTL_COMPILE_MSVC__)
	#define MSTL_SUPPORT_MAKE_INTEGER_SEQ__	1
#endif
#if defined(MSTL_COMPILE_CLANG__)
	#define MSTL_SUPPORT_MEM_INTRINSICS__	1
#endif
#if defined(MSTL_VERSION_20__) && !defined(MSTL_COMPILE_CLANG__) && !defined(MSTL_COMPILE_WITH_EDG__)
	#define MSTL_SUPPORT_U8_INTRINSICS__	1
#endif 


// to libraries : boost / mysql
#define MSTL_DLL_LINK__	1


#define TO_STRING(VALUE) #VALUE

#define FOR_EACH(VALUE, CONTAINER) \
	for(auto VALUE = CONTAINER.begin(); VALUE != CONTAINER.end(); ++VALUE)

#define TEMNULL__ template<>

#ifdef MSTL_STATE_DEBUG__
	#define SIMPLE_LOG(MESG) \
		std::cout << __FILE__ << ":" << __LINE__ << " " << __TIMESTAMP__ << " : " << MESG << std::endl;
#else
	#define SIMPLE_LOG(MESG) 
#endif


#ifdef MSTL_SUPPORT_CONSTEXPR__
	#define MSTL_CONSTEXPR constexpr
#else
	#define MSTL_CONSTEXPR
#endif

#ifdef MSTL_SUPPORT_NODISCARD__
	#define MSTL_NODISCARD [[nodiscard]]
	#define MSTL_ALLOCNODISCARD \
		[[nodiscard("discard the return of allocators will cause memory leaks.")]]
#else
	#define MSTL_NODISCARD
	#define MSTL_ALLOCNODISCARD
#endif

#ifdef MSTL_SUPPORT_NORETURN__
	#define MSTL_NORETURN [[noreturn]]
#else 
	#define MSTL_NORETURN 
#endif

#ifdef MSTL_SUPPORT_DEPRECATED__
	#define MSTL_DEPRECATED [[deprecated]]
	// after C++ 11, we can use lambda expressions to quickly build closures 
	// instead of using functor adapters.
	#define MSTL_FADEPRECATED [[deprecated("C++ 11 and later versions no longer use functor adapters.")]]
#else 
	#define MSTL_DEPRECATED
	#define MSTL_FADEPRECATED
#endif

#ifdef MSTL_SUPPORT_DECLALLOC__
	#define MSTL_DECLALLOC __declspec(allocator)
#else
	#define MSTL_DECLALLOC 
#endif

#if defined(MSTL_COMPILE_MSVC__)
	#define MSTL_LONG_LONG_TYPE __int64
#else
	#define MSTL_LONG_LONG_TYPE long long
#endif

#if defined(MSTL_SUPPORT_STATIC_ASSERT__)
	#define MSTL_NO_EVALUATION \
		static_assert(false, "this function will only be used in no evaluation context.");
#else
	#define MSTL_NO_EVALUATION assert(false)
#endif


MSTL_BEGIN_NAMESPACE__

using byte_t	= unsigned char;
using size_t	= unsigned MSTL_LONG_LONG_TYPE;
using ptrdiff_t = MSTL_LONG_LONG_TYPE;
using intptr_t	= MSTL_LONG_LONG_TYPE;

using int8_t	= signed char;
using int16_t	= short;
using int32_t	= int;
using int64_t	= MSTL_LONG_LONG_TYPE;
using uint8_t	= unsigned char;
using uint16_t	= unsigned short;
using uint32_t	= unsigned int;
using uint64_t	= unsigned MSTL_LONG_LONG_TYPE;

using cstring_t = const char*;
using ccstring_t = const char* const;

MSTL_CONSTEXPR size_t MSTL_SPLIT_LENGTH = 15;

void* memcpy(void*, const void*, size_t);
wchar_t* wmemcpy(wchar_t*, const wchar_t*, size_t);

int memcmp(const void*, const void*, size_t);
int wmemcmp(const wchar_t*, const wchar_t*, size_t);

void* memchr(const void*, int, size_t);
wchar_t* wmemchr(const wchar_t*, wchar_t, size_t);

void* memmove(void*, const void*, size_t);
wchar_t* wmemmove(wchar_t*, const wchar_t*, size_t);

void* memset(void*, int, size_t);
wchar_t* wmemset(wchar_t*, wchar_t, size_t);

int strlen(const char*);
int wcslen(const wchar_t*);

char* strcpy(char*, const char*);
int strcmp(const char*, const char*);
const char* strstr(const char*, const char*);
char* memstr(char*, int, char*);

void split_line(std::ostream& = std::cout, size_t = MSTL_SPLIT_LENGTH, char = '-');

MSTL_END_NAMESPACE__
#endif // MSTL_BASICLIB_H__
