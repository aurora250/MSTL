#include "hashtable.h"
MSTL_BEGIN_NAMESPACE__

size_t __hash_string(const char* s) {
    unsigned long h = 0;
    for (; *s; ++s)
        h = 5 * h + *s;
    return size_t(h);
}
unsigned long __next_prime(unsigned long n) {
    const unsigned long* first = __stl_prime_list;
    const unsigned long* last = __stl_prime_list + __stl_num_primes;
    const unsigned long* pos = lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

MSTL_END_NAMESPACE__
