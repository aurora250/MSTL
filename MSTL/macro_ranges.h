#ifndef MSTL_MACRO_RANGES_H__
#define MSTL_MACRO_RANGES_H__
#include "basiclib.h"
MSTL_BEGIN_NAMESPACE__

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

MSTL_END_NAMESPACE__
#endif // MSTL_MACRO_RANGES_H__
