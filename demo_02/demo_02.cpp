#include "array.h"
#include "error.h"
#include "string.h"
#include "pair.h"
#include "vector.h"
#include "check_type.h"
#include "depositary.h"
#include <iostream>
#include <algorithm>
#include <memory>

void try_vec();
inline void try_arr() {
    MSTL::array<size_t, 3> arr{ 1,2,3 };
    MSTL::array<size_t, 3> arr2(arr);
    std::cout << arr.size() << ' ' << arr.back() << ' ' << arr.find(3) << std::endl;
    for (MSTL::array<size_t, 3>::iterator i = arr.begin(); i != arr.end(); i++) {
        std::cout << *i << std::endl;
    }
    if (arr == arr2) std::cout << "yes" << std::endl;
    //std::cout << arr;
    arr.__det__();
}
inline void try_exc() {
    try {
        MSTL::Exception(false, new MSTL::AttributeError());
    }
    catch (MSTL::Error e) {
        e.__det__();
        MSTL::StopIterator it;
        it.__det__();
        MSTL::Exit(true, try_vec);
        try_vec();
    }
}
inline void try_vec() {
    
}
inline void try_str() {
    MSTL::string str("abc");
    MSTL::string str2("def");
    str.__det__();
    str.push('a');
    str.__det__();
    std::cout << str.pop();;
    auto i = str.ctype();
    str.insert(3, str2);
    str.__det__();
}
inline void try_pir() {
    MSTL::string s("hello!");
    MSTL::pair<int, MSTL::string> p(1, s);
    MSTL::pair<int, MSTL::string> p2(p);
    p.first = 10;
    std::cout << p.first << ' ' << p.second << std::endl;
    p.__det__();
    p2.__det__();
}
class Foo {};
inline void try_che() {
    using namespace MSTL;
    std::cout << check_type<MSTL::string>() << std::endl;
    std::cout << check_type<const volatile void* const*&>() << std::endl;
    std::cout << check_type<int(*)[]>() << std::endl;
    std::cout << check_type<const volatile void* (&)[10]>() << std::endl; // void const volatile * (&) [10]
    std::cout << check_type<int[1][2][3]>() << std::endl;              // int [1] [2] [3]
    std::cout << check_type<char(*(* const)(const int(&)[10]))[10]>() << std::endl;   // 常函数指针，参数是一个常int数组的引用，
                                                                                      // 返回char数组指针
    std::cout << check_type<int (Foo::* const)[3]>() << std::endl;   // 常类成员指针，指向Foo里的int[3]成员
    std::cout << check_type<int (Foo::* const)(int, Foo&&, int) volatile>() << std::endl;   // 常类成员函数指针，指向Foo里的volatile成员函数
    SPLIT_LINE;
    const int a = 0;
    MSTL::string cstr("const string");
    const MSTL::string* sr = new MSTL::string("hai");
    split_line();
    std::cout << check_type<decltype(*(& cstr))>() << std::endl;
    std::cout << check_type<decltype(std::move(cstr))>() << std::endl;
    split_line();
    std::cout << check_type(sr) << std::endl;
    std::cout << check_type<decltype(sr)>() << std::endl;
    split_line();
    std::cout << to_definestr(int (Foo::* const)(int, Foo&&, int) volatile) << std::endl;
    std::any an = sr;
    std::cout << an.type().name() << std::endl;
}
inline void try_dep() {
    using namespace MSTL;
    depositary manager;
    auto add = [](int a, double b, double& c)->double {
            c = a + b;
            return a + b;
        };
    deposit<double, int, double, double&> add_service(add);
    manager.register_deposit<deposit<double, int, double, double&>>("add", add_service);
    double test = 0;
    std::cout << manager.excute<double>("add", 2, 1.08, test) << std::endl;
    std::cout << check_type<deposit<double, int, double, double&>>() << std::endl;
}

int main()
{
    //try_che();
    /*int a[3] = { 1, 2 ,3 };
    std::copy(a, a + 3, std::ostream_iterator<int>(std::cout, " "));*/
}

