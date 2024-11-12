#include "algobase.h"
MSTL_BEGIN_NAMESPACE__
inline char* copy(const char* first, const char* last, char* result) {
	memmove(result, first, last - first);
	return result + (last - first);
}
inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + (last - first);
}
MSTL_END_NAMESPACE__