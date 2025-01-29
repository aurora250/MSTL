#ifndef MSTL_BASICLIB_H__
#define MSTL_BASICLIB_H__
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_)
#define MSTL_PLATFORM_WIN32__	1
#if defined(WIN64) || defined(_WIN64) || defined(_WIN64_)
#define MSTL_PLATFORM_WIN64__	1
#endif // win64
#define MSTL_PLATFORM_WINDOWS__	1
#elif defined(__linux__) // not windows
#define MSTL_PLATFORM_LINUX__	1
#if (__WORDSIZE == 64) || (__SIZEOF_POINTER__ == 8)
#define MSTL_PLATFORM_LINUX64__ 1
#elif (__WORDSIZE == 32) || (__SIZEOF_POINTER__ == 4)
#define MSTL_PLATFORM_LINUX32__ 1
#endif
#elif defined(_POSIX_) // not linux
#define MSTL_PLATFORM_POSIX__	1
#else
#define MSTL_PLATFORM_UNKNOWN__	1
#endif

#if defined(__GNUC__)
#define MSTL_COMPILE_GNUC__		1
#if defined(__clang__)
#define MSTL_COMPILE_CLANG__	1
#else
#define MSTL_COMPILE_GCC__		1
#endif
#elif defined(_MSC_VER)
#define MSTL_COMPILE_MSVC__		1
#else
#define MSTL_COMPILE_UNKNOW__	1
#endif

#if defined(_M_CEE)
#define MSTL_COMPILE_WITH_CRL__ 1
#elif defined(__EDG__)
#define MSTL_COMPILE_WITH_EDG__ 1
#else
#define MSTL_COMPILE_WITH_OS__  1
#endif

#if defined(DEBUG) || defined(_DEBUG) || defined(QT_QML_DEBUG)
#define MSTL_STATE_DEBUG__		1
#else
#define MSTL_STATE_RELEASE__	1
#endif

#define MSTL_NAMESPACE__ using namespace MSTL;
#define MSTL_BEGIN_NAMESPACE__ namespace MSTL {
#define MSTL_END_NAMESPACE__ }
#define MSTL_ MSTL::

#define MSTL_CONCEPTS__ using namespace MSTL::concepts;
#define MSTL_SET_CONCEPTS__ namespace concepts {
#define MSTL_END_CONCEPTS__ }
#define CONCEPTS_ MSTL::concepts:: 

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
#define MSTL_SUPPORT_CONCEPTS__		1
#endif
#if defined(MSTL_VERSION_17__)
#define MSTL_SUPPORT_NODISCARD__	1
#endif
#if defined(MSTL_VERSION_17__)
#define MSTL_SUPPORT_NORETURN__		1
#endif
#if defined(MSTL_VERSION_11__)
#define MSTL_SUPPORT_CONSTEXPR__	1
#endif
#if defined(MSTL_COMPILE_MSVC__)
#define MSTL_SUPPORT_DECLALLOC__	1
#endif

#define MSTL_DLL_LINK__				1

#define TO_STRING(VALUE) #VALUE
#define FOR_EACH(VALUE, CONTAINER) for(auto VALUE = CONTAINER.begin(); VALUE != CONTAINER.end(); ++VALUE)

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
#else
#define MSTL_NODISCARD__
#endif

#ifdef MSTL_SUPPORT_NORETURN__
#define MSTL_NORETURN [[noreturn]]
#else 
#define MSTL_NORETURN 
#endif

#ifdef MSTL_SUPPORT_DECLALLOC__
#define MSTL_DECLALLOC __declspec(allocator)
#else
#define MSTL_DECLALLOC 
#endif

#if defined(MSTL_COMPILE_MSVC__)
#define MSTL_LONG_LONG_TYPE__ __int64
#else
#define MSTL_LONG_LONG_TYPE__ long long
#endif

#define TEMNULL__ template<>
#define MSTL_NO_EVALUATION__ \
	static_assert(false, "this function will only be used in no evaluation context.");

MSTL_BEGIN_NAMESPACE__

typedef unsigned char byte_t;
typedef unsigned MSTL_LONG_LONG_TYPE__ size_t;
typedef MSTL_LONG_LONG_TYPE__ ptrdiff_t;
typedef MSTL_LONG_LONG_TYPE__ intptr_t;

typedef const char* cstring_t;
typedef const char* const ccstring_t;

MSTL_CONSTEXPR size_t MSTL_SPLIT_LENGTH = 15;

extern void* memcpy(void* _dest, void* _rsc, int _byte);
extern int memcmp(const void* _dest, const void* _rsc, int _byte);
extern void* memmove(void* _dest, const void* _rsc, int _byte);
extern void* memset(void* _dest, int _val, int _byte);

extern int strlen(const char* _str);
extern char* strcpy(char* _dest, const char* _sou);
extern int strcmp(const char* _des, const char* _sou);
extern const char* strstr(const char* _des, const char* _sou);
extern char* memstr(char* _data, int _len, char* _sub);

extern void split_line(std::ostream & out = std::cout, size_t size = MSTL_SPLIT_LENGTH, char split_type = '-');

MSTL_END_NAMESPACE__
#endif // MSTL_BASICLIB_H__
