#ifndef TRY_H
#define TRY_H
#include "MSTL/mstlc++.hpp"
USE_MSTL

void try_lls();
void try_exc();

class Foo {};
void try_check();

void try_copy();
void try_deq();
void try_stack();
void try_vec();
void try_pque();
void try_rb();

inline int sum_3(int a, int b, int c) {
    return a + b + c;
}

void try_tup();
void try_hash();
void try_math();

struct Person {
    string name;
    int age;
};
inline std::ostream& operator <<(std::ostream& out, const Person& p) {
    out << p.name << " : " << p.age << " ";
    return out;
}

void try_sort();
void try_ss();
void try_var();
void try_str();

template <typename T>
void print_optional(const MSTL::optional<T>& opt) {
    if (opt) {
        std::cout << "Optional has value: " << *opt << std::endl;
    } else {
        std::cout << "Optional has no value." << std::endl;
    }
}

void try_opt();

inline void printAnyType(const MSTL::any& a) {
    if (a.has_value()) {
        std::cout << "Type: " << a.type().name() << std::endl;
    } else {
        std::cout << "Type: void" << std::endl;
    }
}

void try_any();
void try_db();
void try_pool();
void try_dns();

#endif //TRY_H
