#ifndef TRY_H
#define TRY_H
#include "MSTL/mstlc++.hpp"
USE_MSTL

void test_file();
void test_datetimes();
void test_print();
void test_rnd();
void test_list();
void test_exce();

class Foo {};
void test_check();

void test_copy();
void test_deque();
void test_stack();
void test_vector();
void test_pqueue();
void test_rbtree();

inline int sum_3(int a, int b, int c) {
    return a + b + c;
}

void test_tuple();
void test_hash();
void test_math();

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

void test_sort();
void test_variant();
void test_string();

void test_option();

void test_any();
void test_timer();
void test_dbpool();
void test_tpool();
void test_dns();

#endif //TRY_H
