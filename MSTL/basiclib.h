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
#elif defined(_POSIX_)
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

#if defined(DEBUG) || defined(_DEBUG) || defined(QT_QML_DEBUG)
#define MSTL_STATE_DEBUG__		1
#else
#define MSTL_STATE_RELEASE__	1
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else // not define __cplusplus
#define NULL ((void *)0)
#endif // __cplusplus
#endif // NULL

#define MSTL_NAMESPACE__ using namespace MSTL;
#define MSTL_BEGIN_NAMESPACE__ namespace MSTL {
#define MSTL_END_NAMESPACE__ }
#define MSTL_ MSTL::
#define MSTL_CONCEPTS__ using namespace MSTL::concepts;
#define MSTL_SET_CONCEPTS__ namespace concepts {
#define MSTL_END_CONCEPTS__ }
#define CONCEPTS_ MSTL::concepts:: 

#if defined(_HAS_CXX20) || (__cplusplus >= 202002L)
#define MSTL_SUPPORT_CONCEPTS__		1
#endif
#if defined(_HAS_CXX17) || (__cplusplus >= 201703L)
#define MSTL_SUPPORT_NODISCARD__	1
#endif
#if defined(_HAS_CXX17) || (__cplusplus >= 201703L)
#define MSTL_SUPPORT_CONSTEXPR__	1
#endif
#if defined(_HAS_CXX17) || (__cplusplus >= 201703L)
#define MSTL_SUPPORT_IFCONSTEXPR__	1
#endif
#if defined(_HAS_CXX17) || (__cplusplus >= 201703L)
#define MSTL_SUPPORT_NORETURN__		1
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

#ifdef MSTL_SUPPORT_IFCONSTEXPR__
#define MSTL_IFCONSTEXPR(COND) if constexpr(COND)
#else
#define MSTL_IFCONSTEXPR(COND) if (COND)
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

extern void split_line(std::ostream & _out = std::cout, size_t _size = MSTL_SPLIT_LENGTH);

MSTL_END_NAMESPACE__
#endif // MSTL_BASICLIB_H__
