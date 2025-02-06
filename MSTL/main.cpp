#include "mstlc++.hpp"

void try_lls();
void try_exc() {
	MSTL_NAMESPACE__;
    try {
        MSTL_EXEC_MEMORY__;
    }
    catch (Error&) {
        Exit(true, try_lls);
        try_lls();
    }
}
void try_lls() {
    using namespace MSTL;
    list<int> lls{ 1,2,3,4,5,6,7 };
    detailof_safe(lls);
    lls.push_back(3);
    lls.push_back(4);
    lls.push_front(10);
    detailof_safe(lls);
    lls.reverse();
    detailof_safe(lls);
    lls.sort();
    lls.pop_back();
    lls.pop_front();
    detailof_safe(lls);
    list<int> lls2 = { 5,3,2,1,1 };
    detailof(lls2);
    lls2.remove(5);
    lls2.sort();
    detailof(lls2);
    lls2.unique();
    detailof(lls2);
    std::list<std::unique_ptr<int>> nocopy;
    // nocopy.emplace_back(2); not support in std
    Output out;
    out << lls.at(0) << ':' << lls[0] << endl;
    sout << out;
    out << lls.front() << ':' << lls.back() << endl;
    sout << out;
    lls.clear();
    sout << lls.empty() << endl;
}

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
    std::cout << *++first << std::endl;
    ----last;  //advance(last,-2) 7
    std::cout << *last << std::endl;
    deque<int>::iterator result = id.begin();
    std::cout << *result << std::endl; //0
    //输出区间的终点和输入区间重叠 没有问题
    MSTL::copy(first, last, result);
    MSTL::for_each(id.begin(), id.end(), display()); //2 3 4 5 6 5 6 7 8
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
    ////此处会出现错误，原因在于copy算法不再使用memcove()执行实际的复制操作 但是实际没有出现错误
    //std::cout << std::endl;
}
void try_deq() {
    using namespace MSTL;
    deque<int> a;
    a.push_back(2);
    a.push_front(10);
    a.push_back(3);
    a.push_back(7);
    a.push_back(6);
    detailof_safe(a);
    a.pop_back();
    a.pop_front();
    deque<int> b{ 1,2,3,4,5 };
    sout << b.front() << endl;
    detailof_safe(b);
    deque<int> c(std::move(b));
    c.resize(10, 6);
    detailof_safe(c);
    std::cout << c.at(5) << std::endl;
    deque<char> d = { 'a', 'b' };
    //sout << b.empty() <<endl;
}
void try_stack() {
    using namespace MSTL;
    stack<int> s;
    s.push(2);
    s.push(3);
    s.push(5);
    s.push(4);
    s.pop();
    queue<int> q;
    q.push(2);
}
void try_vec() {
    MSTL_NAMESPACE__;
    MSTL_TRY__{
        vector<int> v{ 1,2,3,4 };
        v.push_back(3);
        v.push_back(4);
        detailof(v);
        vector<int> v2(v);
        display out("\n");
        out(v.front());
        MSTL_TRY__{
            out(v[1]);
        }
        MSTL_CATCH_ERROR_UNUSE__{
            std::cout << "err" << std::endl;
        }
        v.insert(v.end(), v2.cbegin(), v2.cend());
        detailof(v);
        v.pop_back();
        v.clear();
        std::cout << v.empty() << std::endl;
        v.insert(v.end(), v2.cbegin(), v2.cend());
        detailof(v);
        FOR_EACH(it, v2) {
            std::cout << *it << std::endl;
        }
        auto v3 = MSTL::move(v2);
        detailof(v3);
        vector<int> v4 = { 3,2,1 };
        v4.emplace(v4.begin() + 1, 5);
        v4.erase(--v4.end());
        detailof(v4);
        out(*v4.data());
        vector<int, ctype_allocator<int>> cvec;
        cvec.emplace_back(3);
        cvec.emplace_back(4);
        detailof(cvec);

        vector<int> vec;
        vec.assign(5, 10);
        detailof(vec);
        vec.assign({ 1, 2, 3, 4, 5 });
        detailof(vec);
        vector<int> anotherVec = { 6, 7, 8 };
        vec.assign(anotherVec.begin(), anotherVec.end());
        detailof(vec);
    }
    MSTL_CATCH_ERROR__{
        show_data_only(error, std::cout);
        std::cout << std::endl;
    }
}
void try_pque() {
    MSTL_NAMESPACE__;
    priority_queue<int> q;
    std::cout << typeid(priority_queue<int*>).name() << std::endl;
    q.push(6); q.push(9); q.push(1); q.push(5);
    q.push(8); q.push(4); q.push(7);
    detailof(q); // 9 8 7 5 6 1 4
    q.pop();
    detailof(q);
}
void try_map() {
    MSTL_NAMESPACE__;
    map<int, char> m;
    m.insert(pair(1, 'c'));
    m[1] = 'c';
    m[100] = 'x';
    m[2] = 'l';
    detailof(m);
    sout << m[1] << endl;
}
void try_tup() {
    MSTL_NAMESPACE__;
    tuple<int, char, const char*> t(1, 't', "MSTL");
    auto a = get<0>(t);
    std::cout << get<1>(t) << std::endl;
    auto forw = make_tuple(9, 0);
}
void try_hash() {
    MSTL_NAMESPACE__;
    hash_map<int, char> m;
    m[1] = 'a';
    m[2] = 'b';
    m.insert(pair(3, 'c'));
    m.emplace(2, 'c');
    m.insert(pair(1, 'b'));
    detailof(m);
    auto m2 = std::move(m);
    detailof(m2);
    std::cout << *++m2.begin() << ":";
    // std::cout << m2.at(4) << std::endl;
    hash_multimap<std::string, int> mm;
    mm.insert(pair(std::string("a"), 1));
    mm.emplace("a", 1);
    detailof(mm);
    mm.clear();
    detailof(mm);
    hash_map<int, std::unique_ptr<int>> uncopy;
    uncopy.emplace(1, std::make_unique<int>(1));
    detailof(uncopy);
}

void try_pool() {
    using namespace std;
    MSTL_NAMESPACE__;
    {
        ThreadPool pool;
        pool.start();
        pool.submit_task(try_lls);
        pool.submit_task(try_deq);
        pool.submit_task(try_deq);
    }
}

void try_math() {
    MSTL_NAMESPACE__;
    std::cout << power(2, 10) << std::endl;
    std::cout << (factorial(10)) << std::endl;
    std::cout << sine(1) << std::endl;
    std::cout << cosine(angular2radian(270)) << std::endl;
    //std::cout << remainder(73.263, 0.9973) << std::endl;
    std::cout << float_part(constants::PI) << std::endl;
    std::cout << exponential(3) << std::endl;
    std::cout << logarithm_e(165) << std::endl;
    std::cout << logarithm_10(147) << std::endl;
    std::cout << logarithm_2(500) << std::endl;
    std::cout << arctangent(100) << std::endl;
    std::cout << radian2angular(arctangent(100)) << std::endl;
    std::cout << arcsine(0.21) << std::endl;
    std::cout << arccosine(0.62) << std::endl;
    //std::cout << tangent(PI / 2) << std::endl;  // MathError
    std::cout << around_pi(constants::PI * 2 + 0.00000001) << " : " << around_pi(6.28) << std::endl;
    sout << sum_n(1, 2, 3, 4, 5) << endl << gcd(5, 10) << endl << lcm(10, 25) << endl;
    int a = 2, b = 3;
    int* p1 = &a;
    int* p2 = &b;
    iter_swap(p1, p2);
    sout << *p1 << ' ' << *p2 << endl;
    //int fib[fibonacci(4)];
    //int len[leonardo(4)];
}
void try_sql() {
    MSTL_NAMESPACE__;

    clock_t begin = clock();
    DBConnectPool* pool = new DBConnectPool("root", "147258hu", "book");
    for (int i = 0; i < 5000; i++) {
        std::shared_ptr<DBConnect> ptr = pool->get_connect();
        ptr->INSERT_INTO("Manager", "ADM00009", "Hu", 'M', 5000);
    }
    delete pool;
    std::cout << clock() - begin << std::endl;

    begin = clock();
    for (int i = 0; i < 5000; i++) {
        char sql[power(2, 10)] = { 0 };
        sprintf_s(sql, "INSERT INTO Manager VALUES('%s', '%s', '%c', %d)",
            "ADM000009", "Hu", 'M', 5000);
        DBConnect* con = new DBConnect();
        con->connect_to("root", "147258hu", "book");
        con->exec(sql);
        delete con;
    }
    std::cout << clock() - begin << std::endl;
}
void try_log() {
    MSTL_NAMESPACE__;
    set_log_level(LOG_LEVEL_VERBOSE);
    LOG_VERBOSE("test log v");
    TIMERLOG_START(task);
    std::thread t1([]() {
        for (int i = 0; i < 5; i++) { LOG_DEBUG("i: %d", i); }
        });
    std::thread t2([]() {
        for (char c = 'a'; c < 'f'; c++) { LOG_DEBUG("c: %c", c); }
        });
    t1.join();
    t2.join();
    TIMERLOG_END(task);
}
struct Person {
    std::string name;
    int age;
};
inline std::ostream& operator <<(std::ostream& out, const Person& p) {
    out << p.name << " : " << p.age << " ";
    return out;
}
void try_sort() {
    MSTL_NAMESPACE__;
    vector<int> vec{ 6,9,1,5,8,4,7 };
    //insertion_sort(vec.begin(), vec.end());
    //bubble_sort(vec.begin(), vec.end());
    //select_sort(vec.begin(), vec.end());
    //shell_sort(vec.begin(), vec.end());
    //partial_sort(vec.begin(), vec.end(), vec.end());
    //counting_sort(vec.begin(), vec.end());
    //sort(vec.begin(), vec.end());
    //introspective_sort(vec.begin(), vec.end(), (size_t)logarithm_2(vec.end() - vec.begin()) * 2);
    //quick_sort(vec.begin(), vec.end());
    //merge_sort(vec.begin(), vec.end());
    //bucket_sort(vec.begin(), vec.end());
    radix_sort(vec.begin(), vec.end());
    //tim_sort(vec.begin(), vec.end());
    //monkey_sort(vec.begin(), vec.end());
    //smooth_sort(vec.begin(), vec.end());
    //cocktail_sort(vec.begin(), vec.end());
    detailof(vec);
    /*vector<Person> people = {
    {"Alice", 25},
    {"Bob", 20},
    {"Charlie", 30},
    {"David", 20}
    };
    counting_sort(people.begin(), people.end(),
        [](const Person& a, const Person& b) -> bool { return a.age < b.age; },
        [](const Person& p) -> int { return p.age; });
    radix_sort_greater(people.begin(), people.end(), [](const Person& x) -> int { return x.age; });
    detailof(people);*/
}

void try_algo() {
    MSTL_NAMESPACE__;
    std::vector<int> v1 = { 1, 3, 5 };
    std::vector<int> v2 = { 2, 4, 6 };
    std::vector<int> result(v1.size() + v2.size());
    merge(v1.begin(), v1.end(), v2.begin(), v2.end(), result.begin());
    detailof(result);
    std::vector<int> v3 = { 1, 3, 5, 2, 4, 6 };
    auto middle = v3.begin() + 3;
    inplace_merge(v3.begin(), middle, v3.end());
    detailof(v3);
}

int main() {
    MSTL_NAMESPACE__;
    static_assert(random_access_iterator<deque_iterator<int>>);
    try_deq();
    return 0;
}
