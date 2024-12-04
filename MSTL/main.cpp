#include "mstlc++.hpp"

void try_lls();
//void try_arr() {
//    MSTL::array<size_t, 3> arr{ 1,2,3 };
//    MSTL::array<size_t, 3> arr2(arr);
//    std::cout << arr.size() << ' ' << arr.back() << ' ' << arr.find(3) << std::endl;
//    for (MSTL::array<size_t, 3>::iterator i = arr.begin(); i != arr.end(); i++) {
//        std::cout << *i << std::endl;
//    }
//    if (arr == arr2) std::cout << "yes" << std::endl;
//    //std::cout << arr;
//    arr.__det__();
//}
void try_exc() {
	MSTL_NAMESPACE__;
    try {
        MSTL_EXEC_MEMORY__;
    }
    catch (Error& e) {
        StopIterator it;
        Exit(true, try_lls);
        try_lls();
    }
}
void try_lls() {
    using namespace MSTL;
    list<int> lls{ 1,2,3,4,5,6,7 };
    lls.__det__();
    lls.push_back(3);
    lls.push_back(4);
    lls.push_front(10);
    lls.__det__();
    lls.reverse();
    lls.__det__();
    lls.sort();
    std::cout << lls << std::endl;
    lls.pop_back();
    lls.pop_front();
    lls.__det__();
    std::cout << lls.at(0) << ':' << lls[0] << std::endl;
    std::cout << lls.front() << ':' << lls.back() << std::endl;
    lls.clear();
    std::cout << lls.empty() <<std::endl;
}
//void try_str() {
//    MSTL::string str("abc");
//    MSTL::string str2("def");
//    str.__det__();
//    str.push('a');
//    str.__det__();
//    std::cout << str.pop();
//    str.insert(3, str2);
//    str.__det__();
//}
//void try_pir() {
//    MSTL::string s("hello!");
//    MSTL::pair<int, MSTL::string> p(1, s);
//    MSTL::pair<int, MSTL::string> p2(p);
//    p.first = 10;
//    std::cout << p << std::endl;
//}
class Foo {};
void try_check() {
    MSTL_NAMESPACE__;
    using std::string;
    std::cout << check_type<string>() << std::endl;
    std::cout << check_type<const volatile void* const*&>() << std::endl;
    std::cout << check_type<int(*)[]>() << std::endl;
    std::cout << check_type<const volatile void* (&)[10]>() << std::endl; // void const volatile * (&) [10]
    std::cout << check_type<int[1][2][3]>() << std::endl;              // int [1] [2] [3]
    std::cout << check_type<char(*(* const)(const int(&)[10]))[10]>() << std::endl;   // 常函数指针，参数是一个常int数组的引用，
                                                                                      // 返回char数组指针
    std::cout << check_type<int (Foo::* const)[3]>() << std::endl;   // 常类成员指针，指向Foo里的int[3]成员
    std::cout << check_type<int (Foo::* const)(int, Foo&&, int) volatile>() << std::endl;   // 常类成员函数指针，指向Foo里的volatile成员函数
    string cstr("const string");
    const string* sr = new string("hai");
    split_line();
    std::cout << check_type<decltype(*(& cstr))>() << std::endl;
    std::cout << check_type<decltype(std::move(cstr))>() << std::endl;
    split_line();
    // std::cout << check_type(sr) << std::endl;
    std::cout << check_type<decltype(sr)>() << std::endl;
    split_line();
}
void try_dep() {
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

void try_copy() {
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
    MSTL_NAMESPACE__;
    int ia[] = { 0,1,2,3,4,5,6,7,8 };
    deque<int>id(ia, ia + 9);
    deque<int>::iterator first = id.begin();
    deque<int>::iterator last = id.end();
    ++++first;  //advance(first,2)  2
    std::cout << *first << std::endl;
    ----last;  //advance(last,-2) 7
    std::cout << *last << std::endl;
    deque<int>::iterator result = id.begin();
    std::cout << *result << std::endl; //0
    //输出区间的终点和输入区间重叠 没有问题
    MSTL::copy(first, last, result);
    MSTL::for_each(id.begin(), id.end(), display<int>()); //2 3 4 5 6 5 6 7 8
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
void try_deq() {
    using namespace MSTL;
    deque<int> a;
    a.push_back(2);
    a.__det__();
    a.push_front(10);
    a.__det__();
    a.push_back(3);
    a.push_back(7);
    a.push_back(6);
    a.__det__();
    a.pop_back();
    a.pop_front();
    a.__det__();
    deque<int> b{ 1,2,3,4,5 };
    b.__det__();
    std::cout << b << std::endl;
    std::cout << b.front() << std::endl;
    b.clear();
    b.__det__();
}
void try_stack() {
    using namespace MSTL;
    stack<int> s;
    s.push(2);
    s.push(3);
    s.__det__();
    s.push(5);
    s.push(4);
    s.__det__();
    s.pop();
    s.__det__();
    queue<int> q;
    q.push(2);
    q.__det__();
}
void try_vec() {
    MSTL_NAMESPACE__;
    MSTL_TRY__{
        vector<int> v;  //{ 1,2,3,4 }
        v.__det__();
        v.push_back(3);
        v.push_back(4);
        v.__det__();
        vector<int> v2(v);
        display_endl<int> out;
        out(v.front());
        MSTL_TRY__{
            out(v[10]);
        }
        MSTL_CATCH_ERROR__{
            std::cout << "err" << std::endl;
        }
        v.insert(v.end(), v2.const_begin(), v2.const_end());
        v2.__det__();
        v.__det__();
        v.pop_back();
        v.__det__();
        v.clear();
        std::cout << v.empty() << std::endl;
        v.insert(v.end(), v2.const_begin(), v2.const_end());
        container* c = &v;
        c->__det__();
        FOR_EACH(it, v2) {
            std::cout << *it << std::endl;
        }
        MSTL_EXEC_MEMORY__;
    }
    MSTL_CATCH_ERROR__{
        e.__show_data_only(std::cout);
        std::cout << std::endl;
    }
}
void try_pque() {
    MSTL_NAMESPACE__;
    priority_queue<int> q;
    q.push(3);
    q.push(5);
    q.__det__();
    q.pop();
    q.__det__();
}
void try_map() {
    MSTL_NAMESPACE__;
    map<int, char> m;
    m[1] = 'c';
    m[100] = 'x';
    m[2] = 'l';
    m.__det__();
    std::cout << m[1] << std::endl;
}
void try_tup() {
    MSTL_NAMESPACE__;
    tuple<int, char, const char*> t(1, 't', "MSTL");

}
void try_hash() {
    MSTL_NAMESPACE__;
    hash_map<int, char> m;
    m[1] = 'a';
    m[2] = 'b';
}

int main() {
    MSTL_NAMESPACE__
    try_vec();
}
