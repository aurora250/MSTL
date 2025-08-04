#ifndef TRY_H
#define TRY_H
#include "MSTL/mstlc++.hpp"
USE_MSTL

void try_datetime();
void try_print();
void try_rnd();
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

template <>
struct MSTL::printer <Person> {
    static void print (const Person& p) {
        std::cout << p.name << " : " << p.age << " ";
    }
    static void print_feature(const Person& p) {
        printer::print(p);
    }
};

void try_sort();
void try_ss();
void try_var();
void try_str();

void try_opt();

void try_any();
void try_timer();
void try_db();
void try_pool();
void try_dns();

#endif //TRY_H
