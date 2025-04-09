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
	#elif (__WORDSIZE == 32) || (__SIZEOF_POINTER__ == 4) || defined(MSTL_PLATFORM_LINUX64__)
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


#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG) || defined(QT_QML_DEBUG)
	#define MSTL_STATE_DEBUG__			1
#else
	#define MSTL_STATE_RELEASE__		1
#endif


#if defined(MSTL_PLATFORM_WIN64__) || defined(MSTL_PLATFORM_LINUX64__) || defined(__amd64__) || defined(__x86_64__) || defined(__aarch64__)
	#define MSTL_DATA_BUS_WIDTH_64__	1
#endif
#if defined(MSTL_PLATFORM_WIN32__) || defined(MSTL_PLATFORM_LINUX32__) || defined(__i386__) || defined(MSTL_DATA_BUS_WIDTH_64__)
	#define MSTL_DATA_BUS_WIDTH_32__	1
#endif


#define __MSTL_GLOBAL_NAMESPACE__ MSTL
#define __MSTL_PROJECT_VERSION__ 101000

#define USE_MSTL using namespace __MSTL_GLOBAL_NAMESPACE__;
#define MSTL_BEGIN_NAMESPACE__ namespace __MSTL_GLOBAL_NAMESPACE__ {
#define MSTL_END_NAMESPACE__ }
#define _MSTL __MSTL_GLOBAL_NAMESPACE__::

#define MSTL_INDEPENDENT_TAG_NAMESPACE_SETTING inline


#if _HAS_CXX23 || (__cplusplus >= 202100L) || (_MSVC_LANG >= 202100L)
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
	#define MSTL_SUPPORT_ALIGNED__		1
#endif
#if defined(MSTL_VERSION_17__)
	#define MSTL_SUPPORT_ALWAYS_INLINE__	1
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
#if defined(MSTL_COMPILE_MSVC__) || defined(MSTL_COMPILE_CLANG__)
	#define MSTL_SUPPORT_MAKE_INTEGER_SEQ__	1
#endif
#if defined(MSTL_VERSION_20__) && !defined(MSTL_COMPILE_CLANG__) && !defined(MSTL_COMPILE_WITH_EDG__)
	#define MSTL_SUPPORT_U8_INTRINSICS__	1
#endif


#define TO_STRING(VALUE) #VALUE

#define FOR_EACH(VALUE, CONTAINER) \
	for(auto VALUE = CONTAINER.begin(); VALUE != CONTAINER.end(); ++VALUE)

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
		#define MSTL_INLINE17 inline
	#else
		#define MSTL_CONSTEXPR17 inline
		#define MSTL_INLINE17
	#endif // MSTL_VERSION_17__
	#ifdef MSTL_VERSION_14__
		#define MSTL_CONSTEXPR14 MSTL_CONSTEXPR
	#else
		#define MSTL_CONSTEXPR14 inline
	#endif // MSTL_VERSION_14__
#else
	#define MSTL_CONSTEXPR inline
	#define MSTL_CONSTEXPR23 inline
	#define MSTL_CONSTEXPR20 inline
	#define MSTL_CONSTEXPR17 inline
	#define MSTL_CONSTEXPR14 inline
	#define MSTL_INLINE17 inline
#endif // MSTL_SUPPORT_CONSTEXPR__


#ifdef MSTL_SUPPORT_IF_CONSTEXPR__
	// this macro will be used with caution, as it will break static overload.
	#define MSTL_IF_CONSTEXPR if constexpr
#else
	#define MSTL_IF_CONSTEXPR if
#endif


#ifdef MSTL_SUPPORT_NODISCARD__
	#define MSTL_NODISCARD [[nodiscard]]
	#define MSTL_ALLOC_NODISCARD \
		[[nodiscard("discard the return of allocators will cause memory leaks.")]]
#else
	#define MSTL_NODISCARD
	#define MSTL_ALLOC_NODISCARD
#endif


#ifdef MSTL_SUPPORT_ALIGNED__
	#ifdef MSTL_COMPILE_GNUC__
		#define MSTL_ALIGNOF_DEFAULT() __attribute__((__aligned__))
		#define MSTL_ALIGNOF(ALIGN) __attribute__((__aligned__((ALIGN))))
	#elif defined(MSTL_COMPILE_MSVC__)
		#define MSTL_ALIGNOF_DEFAULT() [[aligned]]
		#define MSTL_ALIGNOF(ALIGN) [[aligned(ALIGN)]]
	#else
		#define MSTL_ALIGNOF_DEFAULT()
		#define MSTL_ALIGNOF(ALIGN)
	#endif
#else
	#define MSTL_ALIGNOF_DEFAULT()
	#define MSTL_ALIGNOF(ALIGN)
#endif


#ifdef MSTL_SUPPORT_ALWAYS_INLINE__
	#ifdef MSTL_COMPILE_GNUC__
		#define MSTL_ALWAYS_INLINE __attribute__((always_inline))
	#elif defined(MSTL_COMPILE_MSVC__)
		#define MSTL_ALWAYS_INLINE [[always_inline]]
	#else
		#define MSTL_ALWAYS_INLINE
	#endif
#else
	#define MSTL_ALWAYS_INLINE
#endif


#ifdef MSTL_SUPPORT_UNLIKELY__
	#define MSTL_UNLIKELY [[unlikely]]
#else 
	#define MSTL_UNLIKELY
#endif


#ifdef MSTL_SUPPORT_NORETURN__
	#ifdef MSTL_COMPILE_GNUC__
		#define MSTL_NORETURN __attribute__((noreturn))
	#elif defined(MSTL_COMPILE_MSVC__)
		#define MSTL_NORETURN [[noreturn]]
	#else
		#define MSTL_NORETURN
	#endif
#else 
	#define MSTL_NORETURN 
#endif


#ifdef MSTL_SUPPORT_DEPRECATED__
	#define MSTL_DEPRECATED [[deprecated]]
	// after C++ 11, we can use lambda expressions to quickly build closures 
	// instead of using functor adapters.
	#define MSTL_FUNCADP_DEPRE \
		[[deprecated("C++ 11 and later versions no longer use functor adapters.")]]
	#define MSTL_TRAITS_DEPRE \
		[[deprecated("C++ 11 and later versions no longer use iterator traits functions.")]]
#else 
	#define MSTL_DEPRECATED
	#define MSTL_FUNCADP_DEPRE
	#define MSTL_TRAITS_DEPRE
#endif


#ifdef MSTL_COMPILE_MSVC__
	#define MSTL_DECLALLOC __declspec(allocator)
	#define MSTL_DECLNOVTB __declspec(novtable)
#elif defined(MSTL_COMPILE_GNUC__)
	#define MSTL_DECLALLOC __attribute__((__always_inline__))
	#define MSTL_DECLNOVTB __attribute__((novtable))
#else
	#define MSTL_DECLALLOC
	#define MSTL_DECLNOVTB
#endif


#ifdef MSTL_SUPPORT_NO_UNIQUE_ADS__
	#define MSTL_NO_UNIADS [[no_unique_address]]
#else
	#define MSTL_NO_UNIADS
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
	MAC(long long)

#define MSTL_MACRO_RANGE_USINT(MAC) \
	MAC(unsigned short) \
	MAC(unsigned int) \
	MAC(unsigned long) \
	MAC(unsigned long long)

#define MSTL_MACRO_RANGE_INT(MAC) \
	MSTL_MACRO_RANGE_SINT(MAC) \
	MSTL_MACRO_RANGE_USINT(MAC)

#define MSTL_MACRO_RANGE_FLOAT(MAC) \
	MAC(float) \
	MAC(double) \
	MAC(long double)


#define MSTL_BUILD_TYPE_ALIAS(TYPE) \
	using value_type        = TYPE; \
	using pointer           = TYPE*; \
	using reference         = TYPE&; \
	using const_pointer     = const TYPE*; \
	using const_reference   = const TYPE&; \
	using size_type         = size_t; \
	using difference_type   = ptrdiff_t;


MSTL_BEGIN_NAMESPACE__

using nullptr_t	= decltype(nullptr);
using byte_t    = unsigned char;

#if defined(MSTL_VERSION_17__) && defined(MSTL_COMPILE_CLANG__)
using max_align_t	= std::max_align_t;
#else
using max_align_t	= double;
#endif

using int8_t	= signed char;
using int16_t	= short;
using int32_t	= int;
using int64_t	= long long;
using uint8_t	= unsigned char;
using uint16_t	= unsigned short;
using uint32_t	= unsigned int;
using uint64_t	= unsigned long long;

using float32_t	= float;
using float64_t	= double;
using float_ex_t = long double;

#if defined(MSTL_PLATFORM_LINUX__)
using uintptr_t = unsigned long;
using size_t	= unsigned long;
using ptrdiff_t = long;
using intptr_t	= long;
#elif defined(MSTL_PLATFORM_WIN64__)
using uintptr_t = unsigned long long;
using size_t	= unsigned long long;
using ptrdiff_t = long long;
using intptr_t	= long long;
#elif defined(MSTL_PLATFORM_WIN32__)
using uintptr_t = unsigned int;
using size_t	= unsigned int;
using ptrdiff_t = int;
using intptr_t	= int;
#endif


MSTL_INLINE17 constexpr int8_t INT8_MIN_SIZE   = -128;
MSTL_INLINE17 constexpr int16_t INT16_MIN_SIZE = -32768;
MSTL_INLINE17 constexpr int32_t INT32_MIN_SIZE = -2147483647 - 1;
MSTL_INLINE17 constexpr int64_t INT64_MIN_SIZE = -9223372036854775807LL - 1;

MSTL_INLINE17 constexpr int8_t INT8_MAX_SIZE   = 127;
MSTL_INLINE17 constexpr int16_t INT16_MAX_SIZE = 32767;
MSTL_INLINE17 constexpr int32_t INT32_MAX_SIZE = 2147483647;
MSTL_INLINE17 constexpr int64_t INT64_MAX_SIZE = 9223372036854775807LL;

MSTL_INLINE17 constexpr uint8_t UINT8_MAX_SIZE   = 255;
MSTL_INLINE17 constexpr uint16_t UINT16_MAX_SIZE = 65535;
MSTL_INLINE17 constexpr uint32_t UINT32_MAX_SIZE = 0xffffffffU;
MSTL_INLINE17 constexpr uint64_t UINT64_MAX_SIZE = 0xffffffffffffffffULL;


MSTL_INLINE17 constexpr size_t MEMORY_ALIGN_THRESHHOLD = 16ULL;
MSTL_INLINE17 constexpr size_t MEMORY_BIG_ALLOC_ALIGN = 32ULL;
MSTL_INLINE17 constexpr size_t MEMORY_BIG_ALLOC_THRESHHOLD = 4096ULL;

#ifdef MSTL_STATE_DEBUG__
MSTL_INLINE17 constexpr size_t MEMORY_NO_USER_SIZE = 2 * sizeof(void*) + MEMORY_BIG_ALLOC_ALIGN - 1;
#else
MSTL_INLINE17 constexpr size_t MEMORY_NO_USER_SIZE = sizeof(void*) + MEMORY_BIG_ALLOC_ALIGN - 1;
#endif

#ifdef MSTL_DATA_BUS_WIDTH_64__
MSTL_INLINE17 constexpr size_t MEMORY_BIG_ALLOC_SENTINEL = 0xFAFAFAFAFAFAFAFAULL;
#else
MSTL_INLINE17 constexpr size_t MEMORY_BIG_ALLOC_SENTINEL = 0xFAFAFAFAUL;
#endif


constexpr void* memcpy(void* dest, const void* src, size_t byte) noexcept {
	if(dest == nullptr || src == nullptr) return nullptr;
	void* res = dest;
	while (byte--) {
		*static_cast<char*>(dest) = *static_cast<const char*>(src);
		dest = static_cast<char*>(dest) + 1;
		src = static_cast<const char*>(src) + 1;
	}
	return res;
}

constexpr wchar_t* wmemcpy(wchar_t* dest, const wchar_t* src, size_t count) noexcept {
	if(dest == nullptr || src == nullptr) return nullptr;
	wchar_t* res = dest;
	while (count--) {
		*dest = *src;
		++dest;
		++src;
	}
	return res;
}

constexpr int memcmp(const void* dest, const void* src, size_t byte) noexcept {
	assert(dest && src);
	while (byte--) {
		if (*static_cast<const char*>(dest) != *static_cast<const char*>(src))
			return *static_cast<const char*>(dest) - *static_cast<const char*>(src);
		dest = static_cast<const char*>(dest) + 1;
		src = static_cast<const char*>(src) + 1;
	}
	return 0;
}

constexpr int wmemcmp(const wchar_t* dest, const wchar_t* src, size_t count) noexcept {
	assert(dest && src);
	while (count--) {
		if (*dest != *src) return *dest - *src;
		++dest;
		++src;
	}
	return 0;
}

constexpr void* memchr(const void* dest, const int value, size_t byte) noexcept {
	if(dest == nullptr) return nullptr;
	auto p = static_cast<const char *>(dest);
	while (byte--) {
		if (*p == static_cast<char>(value))
			return const_cast<char*>(p);
		p++;
	}
	return nullptr;
}

constexpr wchar_t* wmemchr(const wchar_t* dest, const wchar_t value, size_t count) noexcept {
	if(dest == nullptr) return nullptr;
	const wchar_t* p = dest;
	while (count--) {
		if (*p == value)
			return const_cast<wchar_t*>(p);
		p++;
	}
	return nullptr;
}

constexpr void* memmove(void* dest, const void* src, size_t byte) noexcept {
	if(dest == nullptr || src == nullptr) return nullptr;
	void* res = dest;
	if (dest < src) {
		while (byte--) {
			*static_cast<char*>(dest) = *static_cast<const char*>(src);
			dest = static_cast<char *>(dest) + 1;
			src = static_cast<const char*>(src) + 1;
		}
	}
	else if (dest > src) {
		while (byte--) {
			*(static_cast<char *>(dest) + byte) = *(static_cast<const char*>(src) + byte);
		}
	}
	return res;
}

constexpr wchar_t* wmemmove(wchar_t* dest, const wchar_t* src, const size_t count) noexcept {
	if(dest == nullptr || src == nullptr) return nullptr;
	return static_cast<wchar_t *>(memmove(dest, src, count * sizeof(wchar_t)));
}

constexpr void* memset(void* dest, const int value, size_t byte) noexcept {
	if(dest == nullptr) return nullptr;
	void* ret = static_cast<char *>(dest);
	while (byte--) {
		*static_cast<char *>(dest) = static_cast<char>(value);
		dest = static_cast<char *>(dest) + 1;
	}
	return ret;
}

constexpr wchar_t* wmemset(wchar_t* dest, const wchar_t value, size_t count) noexcept {
	if(dest == nullptr) return nullptr;
	wchar_t* ret = dest;
	while (count--) {
		*dest = value;
		dest++;
	}
	return ret;
}


constexpr int strlen(const char* str) noexcept {
	if (*str != '\0') return strlen(str + 1) + 1;
	return 0;
}

constexpr int wcslen(const wchar_t* str) noexcept {
	if (*str != L'\0') return wcslen(str + 1) + 1;
	return 0;
}
#ifdef MSTL_VERSION_20__
constexpr int u8cslen(const char8_t* str) noexcept {
	if (*str != u8'\0') return u8cslen(str + 1) + 1;
	return 0;
}
#endif

constexpr char* strcpy(char* dest, const char* src) noexcept {
	if(dest == nullptr || src == nullptr) return nullptr;
	char* _ret = dest;
	while (*src != '\0') {
		*dest = *src;
		dest++;
		src++;
	}
	*dest = *src;
	return _ret;
}

constexpr int strcmp(const char* dest, const char* src) noexcept {
	assert(dest && src);
	while (*dest == *src) {
		if (*dest == '\0') return 0;
		dest++;
		src++;
	}
	if (*dest > *src) return 1;
	return -1;
}

constexpr const char* strstr(const char* dest, const char* src) noexcept {
	if(dest == nullptr || src == nullptr) return nullptr;
	const char* cur = dest;
	while (*cur) {
		const char *str1 = cur;
		const char *str2 = src;
		while (*str1 && *str2 && *str1 == *str2) {
			str1++;
			str2++;
		}
		if (*str2 == '\0') return cur;
		cur++;
	}
	return nullptr;
}

constexpr char* memstr(char* data, const int len, const char* sub) noexcept {
	if (data == nullptr || len <= 0 || sub == nullptr) return nullptr;
	if (*sub == '\0') return nullptr;
	const int str_len = strlen(sub);
	char* cur = data;
	const int last = len - str_len + 1;
	for (int i = 0; i < last; i++) {
		if (*cur == *sub)
			if (memcmp(cur, sub, str_len) == 0) return cur;
		cur++;
	}
	return nullptr;
}

MSTL_END_NAMESPACE__
#endif // MSTL_BASICLIB_H__
