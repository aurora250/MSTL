#ifndef MSTL_BASICLIB_H__
#define MSTL_BASICLIB_H__
#include <iostream>
#include <cassert>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_) || defined(_M_X86)
	#define MSTL_PLATFORM_WINDOWS__		1
	#define MSTL_PLATFORM_WIN32__		1
	#if defined(WIN64) || defined(_WIN64) || defined(_WIN64_) || defined(_M_X64)
		#define MSTL_PLATFORM_WIN64__	1
	#endif // win64
#elif defined(__linux__) // not windows
	#define MSTL_PLATFORM_LINUX__		1
	#if (__WORDSIZE == 64) || (__SIZEOF_POINTER__ == 8)
		#define MSTL_PLATFORM_LINUX64__ 1
	#elif (__WORDSIZE == 32) || (__SIZEOF_POINTER__ == 4)
		#define MSTL_PLATFORM_LINUX32__ 1
	#endif
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


#if defined(MSTL_PLATFORM_WIN64__) || defined(MSTL_PLATFORM_LINUX64__) || defined(__amd64__) || defined(__x86_64__) || defined(__aarch64__)
	#define MSTL_DATA_BUS_WIDTH_64__	1
#endif
#if defined(MSTL_PLATFORM_WIN32__) || defined(MSTL_PLATFORM_LINUX32__) || defined(__i386__)
	#define MSTL_DATA_BUS_WIDTH_32__	1
#endif


#define USE_MSTL using namespace MSTL;
#define MSTL_BEGIN_NAMESPACE__ namespace MSTL {
#define MSTL_END_NAMESPACE__ }
#define MSTL_ MSTL::


#if (__cplusplus >= 202100L) || (_MSVC_LANG >= 202100L)
	#define MSTL_VERSION_23__	1
#endif
#if _HAS_CXX20 || (__cplusplus >= 202002L) || (_MSVC_LANG >= 202002L)
	#define MSTL_VERSION_20__	1
#endif
#if _HAS_CXX17 || (__cplusplus >= 201703L) || defined(MSTL_VERSION_20__) || (_MSVC_LANG >= 201703L)
	#define MSTL_VERSION_17__	1
#endif
#if (__cplusplus >= 201402L) || defined(MSTL_VERSION_17__) || (_MSVC_LANG >= 201402L)
	#define MSTL_VERSION_14__	1
#endif
#if (__cplusplus >= 201103L) || defined(MSTL_VERSION_14__) || (_MSVC_LANG >= 201103L)
	#define MSTL_VERSION_11__	1
#endif
#if (__cplusplus >= 199711L) || defined(MSTL_VERSION_11__) || (_MSVC_LANG >= 199711L)
	#define MSTL_VERSION_98__	1
#endif


#if defined(MSTL_VERSION_11__)
	#define MSTL_SUPPORT_CONSTEXPR__		1
#endif
#if defined(MSTL_VERSION_11__)
	#define MSTL_SUPPORT_STATIC_ASSERT__	1
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
#if defined(MSTL_VERSION_17__)
	#define MSTL_SUPPORT_NODISCARD__		1
#endif
#if defined(MSTL_VERSION_17__)
	#define MSTL_SUPPORT_IF_CONSTEXPR__		1
#endif
#if defined(MSTL_VERSION_20__)
	#define MSTL_SUPPORT_NO_UNIQUE_ADS__	1
#endif
#if defined(MSTL_VERSION_20__)
	#define MSTL_SUPPORT_CONCEPTS__			1
#endif

#if defined(MSTL_COMPILE_GNUC__) && defined(MSTL_VERSION_17__)
	#define MSTL_SUPPORT_UNLIKELY__			1
#endif
#if defined(MSTL_COMPILE_MSVC__)
	#define MSTL_SUPPORT_DECLSPEC__			1
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
#define MSTL_DLL_LINK__	0


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
#ifdef MSTL_VERSION_23__
		#define MSTL_CONSTEXPR23 MSTL_CONSTEXPR
#else
		#define MSTL_CONSTEXPR23 inline
#endif // MSTL_VERSION_23__
#ifdef MSTL_VERSION_20__
		#define MSTL_CONSTEXPR20 MSTL_CONSTEXPR
#else
		#define MSTL_CONSTEXPR20 inline
#endif // MSTL_VERSION_20__
#ifdef MSTL_VERSION_17__
	#define MSTL_CONSTEXPR17 MSTL_CONSTEXPR
	#define MSTL_INLINECSP inline 
#else
	#define MSTL_CONSTEXPR17 inline
	#define MSTL_INLINECSP
#endif // MSTL_VERSION_17__
#else
	#define MSTL_CONSTEXPR inline
	#define MSTL_CONSTEXPR23 inline
	#define MSTL_CONSTEXPR20 inline
	#define MSTL_CONSTEXPR17 inline
	#define MSTL_INLINECSP inline
#endif // MSTL_SUPPORT_CONSTEXPR__


#ifdef MSTL_SUPPORT_IF_CONSTEXPR__
	// this macro will be used with caution, as it will break static overload.
	#define MSTL_IF_CONSTEXPR if constexpr
#else
	#define MSTL_IF_CONSTEXPR if
#endif


#ifdef MSTL_SUPPORT_NODISCARD__
	#define MSTL_NODISCARD [[nodiscard]]
	#if (defined(MSTL_VERSION_20__) && defined(MSTL_COMPILE_GCC__)) || defined(MSTL_COMPILE_MSVC__)
		#define MSTL_ALLOCNODISCARD \
			[[nodiscard("discard the return of allocators will cause memory leaks.")]]
	#else
		#define MSTL_ALLOCNODISCARD MSTL_NODISCARD
	#endif
#else
	#define MSTL_NODISCARD
	#define MSTL_ALLOCNODISCARD
#endif

#ifdef MSTL_SUPPORT_UNLIKELY__
	#define MSTL_UNLIKELY [[unlikely]]
#else 
	#define MSTL_UNLIKELY
#endif

#ifdef MSTL_SUPPORT_NORETURN__
	#define MSTL_NORETURN [[noreturn]]
#else 
	#define MSTL_NORETURN 
#endif

#ifdef MSTL_SUPPORT_DEPRECATED__
	#define MSTL_DEPRECATED [[deprecated]]
	#if (defined(MSTL_VERSION_20__) && defined(MSTL_COMPILE_GCC__)) || defined(MSTL_COMPILE_MSVC__)
	// after C++ 11, we can use lambda expressions to quickly build closures 
	// instead of using functor adapters.
		#define MSTL_FUNCADP_DEPRE \
			[[deprecated("C++ 11 and later versions no longer use functor adapters.")]]
		#define MSTL_TRAITS_DEPRE \
			[[deprecated("C++ 11 and later versions no longer use iterator traits functions.")]]
	#else
		#define MSTL_FUNCADP_DEPRE MSTL_DEPRECATED
		#define MSTL_TRAITS_DEPRE MSTL_DEPRECATED
	#endif
#else 
	#define MSTL_DEPRECATED
	#define MSTL_FUNCADP_DEPRE
	#define MSTL_TRAITS_DEPRE
#endif

#ifdef MSTL_SUPPORT_DECLSPEC__
	#define MSTL_DECLALLOC __declspec(allocator)
	#define MSTL_DECLNOVTB __declspec(novtable)
#else
	#define MSTL_DECLALLOC
	#define MSTL_DECLNOVTB
#endif

#ifdef MSTL_SUPPORT_NO_UNIQUE_ADS__
	#define MSTL_NO_UNIADS [[no_unique_address]]
#else
	#define MSTL_NO_UNIADS
#endif

#if defined(MSTL_COMPILE_MSVC__)
	#define MSTL_LLT __int64
#else
	#define MSTL_LLT long long
#endif

#if defined(MSTL_SUPPORT_STATIC_ASSERT__)
	#define MSTL_NO_EVALUATION \
		static_assert(false, "this function will only be used in no evaluation context.");
#else
	#define MSTL_NO_EVALUATION assert(false)
#endif


#define MSTL_MACRO_RANGE_BASIC_CHARS(MAC) \
	MAC(char) \
	MAC(signed char) \
	MAC(unsigned char) \


#ifdef MSTL_VERSION_20__
#define MSTL_MACRO_RANGES_UNICODE_CHARS(MAC) \
	MAC(char8_t) \
	MAC(char16_t) \
	MAC(char32_t) 
#else
#define MSTL_MACRO_RANGES_UNICODE_CHARS(MAC) \
	MAC(char16_t) \
	MAC(char32_t) 
#endif

#define MSTL_MACRO_RANGE_CHARS(MAC) \
	MSTL_MACRO_RANGE_BASIC_CHARS(MAC) \
	MAC(wchar_t) \
	MSTL_MACRO_RANGES_UNICODE_CHARS(MAC)

#define MSTL_MACRO_RANGE_SINT(MAC) \
	MAC(short) \
	MAC(int) \
	MAC(long) \
	MAC(MSTL_LLT) 

#define MSTL_MACRO_RANGE_USINT(MAC) \
	MAC(unsigned short) \
	MAC(unsigned int) \
	MAC(unsigned long) \
	MAC(unsigned MSTL_LLT) 

#define MSTL_MACRO_RANGE_INT(MAC) \
	MSTL_MACRO_RANGE_SINT(MAC) \
	MSTL_MACRO_RANGE_USINT(MAC)

#define MSTL_MACRO_RANGE_FLOAT(MAC) \
	MAC(float) \
	MAC(double) \
	MAC(long double)


#ifdef MSTL_COMPILE_CLANG__
	#define MSTL_PUSH_WARNING__ clang diagnostic push
#elif defined(MSTL_COMPILE_GCC__)
	#define MSTL_PUSH_WARNING__ GCC diagnostic push
#elif defined(MSTL_COMPILE_MSVC__)
	#define MSTL_PUSH_WARNING__ warning(push)
#endif

#ifdef MSTL_COMPILE_CLANG__
	#define MSTL_IGNORE_WARNING__(WARN) clang diagnostic ignored WARN
#elif defined(MSTL_COMPILE_GCC__)
	#define MSTL_IGNORE_WARNING__(WARN) GCC diagnostic ignored WARN
#elif defined(MSTL_COMPILE_MSVC__)
	#define MSTL_IGNORE_WARNING__(WARN) warning(disable : WARN)
#endif

#ifdef MSTL_COMPILE_CLANG__
	#define MSTL_POP_WARNING__ clang diagnostic pop
#elif defined(MSTL_COMPILE_GCC__)
	#define MSTL_POP_WARNING__ GCC diagnostic pop
#elif defined(MSTL_COMPILE_MSVC__)
	#define MSTL_POP_WARNING__ warning(pop)
#endif


MSTL_BEGIN_NAMESPACE__

using nullptr_t = decltype(nullptr);
using byte_t	= unsigned char;

using int8_t	= signed char;
using int16_t	= short;
using int32_t	= int;
using int64_t	= MSTL_LLT;
using uint8_t	= unsigned char;
using uint16_t	= unsigned short;
using uint32_t	= unsigned int;
using uint64_t	= unsigned MSTL_LLT;

#ifdef MSTL_DATA_BUS_WIDTH_64__
using uintptr_t = unsigned MSTL_LLT;
using size_t	= unsigned MSTL_LLT;
using ptrdiff_t = MSTL_LLT;
using intptr_t	= MSTL_LLT;
#else
using uintptr_t = unsigned int;
using size_t	= unsigned int;
using ptrdiff_t = int;
using intptr_t	= int;
#endif

using cstring_t = const char*;
using ccstring_t = const char* const;


MSTL_INLINECSP constexpr size_t INT_MAX_SIZE = static_cast<size_t>(-1);
MSTL_INLINECSP constexpr uint32_t MSTL_SPLIT_LENGTH = 15U;


MSTL_INLINECSP constexpr size_t MEMORY_ALIGN_THRESHHOLD = 16ULL;
MSTL_INLINECSP constexpr size_t MEMORY_BIG_ALLOC_ALIGN = 32ULL;
MSTL_INLINECSP constexpr size_t MEMORY_BIG_ALLOC_THRESHHOLD = 4096ULL;
#ifdef MSTL_STATE_DEBUG__
MSTL_INLINECSP constexpr size_t MEMORY_NO_USER_SIZE = 2 * sizeof(void*) + MEMORY_BIG_ALLOC_ALIGN - 1;
#else
MSTL_INLINECSP constexpr size_t MEMORY_NO_USER_SIZE = sizeof(void*) + MEMORY_BIG_ALLOC_ALIGN - 1;
#endif
#ifdef MSTL_DATA_BUS_WIDTH_64__
MSTL_INLINECSP constexpr size_t MEMORY_BIG_ALLOC_SENTINEL = 0xFAFAFAFAFAFAFAFAULL;
#else
MSTL_INLINECSP constexpr size_t MEMORY_BIG_ALLOC_SENTINEL = 0xFAFAFAFAUL;
#endif


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
#ifdef MSTL_VERSION_20__
int u8cslen(const char8_t*);
#endif

char* strcpy(char*, const char*);
int strcmp(const char*, const char*);
const char* strstr(const char*, const char*);
char* memstr(char*, int, char*);

void split_line(std::ostream& = std::cout, uint32_t = MSTL_SPLIT_LENGTH, char = '-');

MSTL_END_NAMESPACE__
#endif // MSTL_BASICLIB_H__
