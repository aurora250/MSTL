﻿#include "mstlc++.h"
#include "array.hpp"
#include "string.h"
#include "pair.hpp"
#include "vector.hpp"
#include "linkedlist.hpp"
#include <iostream>

void try_lls();
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
        MSTL::Exit(true, try_lls);
        try_lls();
    }
}
inline void try_lls() {
    using namespace MSTL;
    doublylist<int> lls;
    lls.__det__();
    lls.push_back(3);
    lls.push_back(4);
    lls.push_front(10);
    lls.__det__();
    lls.reverse();
    lls.__det__();
    std::cout << lls << std::endl;
    lls.pop_back();
    lls.pop_front();
    lls.__det__();
    std::cout << lls.at(0) << ':' << lls[0] << std::endl;
    std::cout << lls.front() << ':' << lls.back() << std::endl;
    lls.pop_back();
    std::cout << lls.empty();
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
    std::cout << check_type<string>() << std::endl;
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
    string cstr("const string");
    const string* sr = new string("hai");
    split_line();
    std::cout << check_type<decltype(*(& cstr))>() << std::endl;
    std::cout << check_type<decltype(std::move(cstr))>() << std::endl;
    split_line();
    std::cout << check_type(sr) << std::endl;
    std::cout << check_type<decltype(sr)>() << std::endl;
    split_line();
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

#include <vector>
#include <deque>
inline void try_copy() {
    //int ia[] = { 0,1,2,3,4,5,6,7,8 };
    ////输出区间的终点和输入区间出现重叠 没有问题
    //MSTL::copy(ia + 2, ia + 7, ia);
    //std::for_each(ia, ia + 9, display<int>()); //2 3 4 5 6 5 6 7 8
    //std::cout << std::endl;

    //int ia[] = { 0,1,2,3,4,5,6,7,8 };
    ////输出区间的起点和输入区间出现重叠 可能会出现问题
    //MSTL::copy(ia + 2, ia + 7, ia + 4);
    //std::for_each(ia, ia + 9, display<int>()); //0 1 2 3 2 3 4 5 6
    //std::cout << std::endl;
    ////本例结果正确 因为调用的copy算法使用memmove()执行实际复制操作

    int ia[] = { 0,1,2,3,4,5,6,7,8 };
    std::deque<int>id(ia, ia + 9);
    std::deque<int>::iterator first = id.begin();
    std::deque<int>::iterator last = id.end();
    ++++first;  //advance(first,2)  2
    std::cout << *first << std::endl;
    ----last;  //advance(last,-2) 7
    std::cout << *last << std::endl;
    std::deque<int>::iterator result = id.begin();
    std::cout << *result << std::endl; //0
    //输出区间的终点和输入区间重叠 没有问题
    MSTL::copy(first, last, result);
    MSTL::for_each(id.begin(), id.end(), MSTL::display<int>()); //2 3 4 5 6 5 6 7 8
    std::cout << std::endl;

    //int ia[] = { 0,1,2,3,4,5,6,7,8 };
    //std::deque<int>id(ia, ia + 9);
    //std::deque<int>::iterator first = id.begin();
    //std::deque<int>::iterator last = id.end();
    //++++first;  //advance(first,2)  2
    //std::cout << *first << std::endl;
    //----last;  //advance(last,-2)
    //std::cout << *last << std::endl;
    //std::deque<int>::iterator result = id.begin();
    //std::advance(result, 4);
    //std::cout << *result << std::endl; //4
    ////输出区间的起点和输入区间重叠 可能会出现问题
    //MSTL::copy(first, last, result);
    //std::for_each(id.begin(), id.end(), display<int>()); //0 1 2 3 2 3 4 5 6
    ////STL源码剖析 此处会出现错误，原因在于copy算法不再使用memcove()执行实际的复制操作
    ////但是实际没有出现错误
    //std::cout << std::endl;
}

int main()
{
    try_copy();
    /*int a[3] = { 1, 2 ,3 };
    std::copy(a, a + 3, std::ostream_iterator<int>(std::cout, " "));*/
}
