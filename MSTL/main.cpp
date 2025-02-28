#include "mstlc++.hpp"

void try_lls();
void try_exc() {
	USE_MSTL;
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
    detailof(lls);
    lls.push_back(3);
    lls.push_back(4);
    lls.push_front(10);
    detailof(lls);
    lls.reverse();
    detailof(lls);
    lls.sort();
    lls.pop_back();
    lls.pop_front();
    detailof(lls);
    list<int> lls2 = { 5,3,2,1,1 };
    detailof(lls2);
    lls2.remove(5);
    lls2.sort();
    detailof(lls2);
    lls2.unique();
    detailof(lls2);
    list<unique_ptr<int>> nocopy;
    // nocopy.emplace_back(2); not support in std
    lls.clear();
}

class Foo {};
void try_check() {
    USE_MSTL;
    std::cout << check_type<string>() << std::endl;
    std::cout << check_type<const volatile void* const*&>() << std::endl;
    std::cout << check_type<int(*)[]>() << std::endl;
    std::cout << check_type<const volatile void* (&)[10]>() << std::endl; // void const volatile * (&) [10]
    std::cout << check_type<int[1][2][3]>() << std::endl;              // int [1] [2] [3]
    std::cout << check_type<char(*(* const)(const int(&)[10]))[10]>() << std::endl;
    std::cout << check_type<int (Foo::* const)[3]>() << std::endl;
    std::cout << check_type<int (Foo::* const)(int, Foo&&, int) volatile>() << std::endl;
    string cstr("const string");
    const string* sr = new string("hai");
    split_line();
    std::cout << check_type<decltype(*(& cstr))>() << std::endl;
    std::cout << check_type<decltype(MSTL::move(cstr))>() << std::endl;
    split_line();
    std::cout << check_type<decltype(sr)>() << std::endl;
    split_line();
}

void try_copy() {
    //int ia[] = { 0,1,2,3,4,5,6,7,8 };
    //MSTL::copy(ia + 2, ia + 7, ia);
    //MSTL::for_each(ia, ia + 9, [](int x) { std::cout << x << std::endl; }); //2 3 4 5 6 5 6 7 8
    //std::cout << std::endl;

    //int ia[] = { 0,1,2,3,4,5,6,7,8 };
    ////输出区间的起点和输入区间出现重叠 可能会出现问题
    //MSTL::copy(ia + 2, ia + 7, ia + 4);
    //std::for_each(ia, ia + 9, display<int>()); //0 1 2 3 2 3 4 5 6
    //std::cout << std::endl;
    ////本例结果正确 因为调用的copy算法使用memmove()执行实际复制操作
    USE_MSTL;
    //int ia[] = { 0,1,2,3,4,5,6,7,8 };
    //vector<int>id(ia, ia + 9);
    //vector<int>::iterator first = id.begin();
    //vector<int>::iterator last = id.end();
    //++first;  //advance(first,2)  2
    //std::cout << *++first << std::endl;
    //--last;  //advance(last,-2) 7
    //std::cout << *last << std::endl;
    //vector<int>::iterator result = id.begin();
    //std::cout << *result << std::endl; //0
    ////输出区间的终点和输入区间重叠 没有问题
    //vector<int> fin(9, 1);
    //MSTL::copy(first, last, result);
    //detailof(id);

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
#include <deque>
void try_deq() {
    using namespace MSTL;
    deque<int> a;
    a.push_back(2);
    a.push_front(10);
    a.push_back(3);
    a.push_back(7);
    a.push_back(6);
    a.insert(a.end(), 100);
    a.emplace(a.begin(), 0);
    detailof(a);
    a.pop_back();
    a.pop_front();
    detailof(a);
    a.assign(10, 5);
    detailof(a);
    deque<int> b{ 1,2,3,4,5 };
    detailof(b);
    deque<int> c(MSTL::move(b));
    c.resize(10, 6);
    detailof(c);
    deque<char> d{ 'a', 'b' };
}
void try_stack() {
    using namespace MSTL;
    stack<int> s;
    s.push(2);
    s.push(3);
    s.push(5);
    s.push(4);
    detailof(s);
    s.pop();
    detailof(s);
}
void try_vec() {
    USE_MSTL;
    MSTL_TRY__{
        vector<int> v{ 1,2,3,4 };
        v.push_back(3);
        v.push_back(4);
        detailof(v);
        vector<int> v2(v);
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
    USE_MSTL;
    priority_queue<int> q;
    std::cout << typeid(priority_queue<int*>).name() << std::endl;
    q.push(6); q.push(9); q.push(1); q.push(5);
    q.push(8); q.push(4); q.emplace(7);
    detailof(q); // 9 8 7 5 6 1 4
    q.pop();
    detailof(q);
}

void try_rb() {
    USE_MSTL;
    map<int, char> m;
    m.insert(pair<int, char>(1, 'c'));
    m.emplace(3, 'c');
    m.emplace_hint(m.end(), 4, 'd');
    m[1] = 'a';
    m[100] = 'x';
    m[2] = 'b';
    detailof(m);
    m.erase(m.begin());
    detailof(m);
    m.clear();

    multimap<int, const char*> mm;
    mm.emplace(1, "c");
    mm.emplace(2, "b");
    mm.emplace(1, "a");
    detailof(mm);
    mm.erase(mm.begin());
    mm.insert(mm.begin(), pair<int, const char*>(1, "a"));
    detailof(mm);
    mm.clear();

    set<int> s{ 1,2,3,4,5 };
    s.insert(s.begin(), 1);
    s.emplace(2);
    detailof(s);

    multiset<int> ms{ 4,5,6,7,8,8 };
    ms.insert(ms.begin(), 9);
    ms.emplace(10);
    detailof(ms);
}
void try_tup() {
    USE_MSTL;
    tuple<int, char, const char*> t(1, 't', "MSTL");
    auto a = get<0>(t);
    std::cout << get<1>(t) << std::endl;
    auto forw = MSTL::make_tuple(9, 0);
}
void try_hash() {
    USE_MSTL;
    unordered_map<int, char> m;
    m[1] = 'a';
    m[2] = 'b';
    m.insert(pair<int, char>(3, 'c'));
    m.emplace(2, 'c');
    m.insert(pair<int, char>(1, 'b'));
    detailof(m);
    unordered_map<int, char> m2;
    m2.insert(m.begin(), m.end());
    detailof(m2);
    unordered_multimap<string, int> mm;
    mm.emplace("a", 1);
    mm.emplace("a", 2);
    mm.insert(pair<string, int>(string("a"), 1));
    detailof(mm);
    mm.clear();
    detailof(mm);
    unordered_map<int, unique_ptr<int>> uncopy;
    uncopy.emplace(1, make_unique<int>(1));
    detailof(uncopy);
    uncopy.erase(uncopy.begin());

    unordered_set<pair<int, char>> us;
    us.emplace(1, 'c');
    us.insert(pair<int, char>(4, 'r'));
    detailof(us);
    us.erase(pair<int, char>(4, 'r'));
    us.erase(us.begin());
    detailof(us);

    unordered_multiset<pair<int, const char*>> ms;
    ms.emplace(1, "234");
    ms.insert(make_pair(2, "345"));
    ms.emplace(1, "234");
    detailof(ms);
    ms.erase(ms.begin());
    detailof(ms);
}

void try_pool() {
    USE_MSTL;
    {
        ThreadPool pool;
        pool.start();
        pool.submit_task(try_lls);
        pool.submit_task(try_deq);
        pool.submit_task(try_hash);
    }
}

void try_math() {
    USE_MSTL;
    std::cout << power(2, 10) << std::endl;
    std::cout << (factorial(10)) << std::endl;
    std::cout << sine(1) << " : " << std::sin(1) << std::endl;
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
    //int fib[fibonacci(4)];
    //int len[leonardo(4)];
}
void try_sql() {
#if MSTL_DLL_LINK__
    USE_MSTL;

    clock_t begin = clock();
    DBConnectPool* pool = new DBConnectPool("root", "147258hu", "book");
    for (int i = 0; i < 5000; i++) {
        std::shared_ptr<DBConnect> ptr = pool->get_connect();
        char sql[power(2, 10)] = { 0 };
        sprintf_s(sql, "INSERT INTO Manager VALUES('%s', '%s', '%c', %d)",
            "ADM000009", "Hu", 'M', 5000);
        ptr->exec(sql);
    }
    std::cout << clock() - begin << std::endl;
    delete pool;
#endif // MSTL_DLL_LINK__
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
    USE_MSTL;
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
    USE_MSTL;
    vector<int> v1 = { 1, 3, 5 };
    vector<int> v2 = { 2, 4, 6 };
    vector<int> result(v1.size() + v2.size());
    merge(v1.begin(), v1.end(), v2.begin(), v2.end(), result.begin());
    detailof(result);
    vector<int> v3 = { 1, 3, 5, 2, 4, 6 };
    auto middle = v3.begin() + 3;
    inplace_merge(v3.begin(), middle, v3.end());
    detailof(v3);
}
void func_hello(int i) {
    printf("#%d Hello\n", i);
}

struct func_printnum_t {
    void operator()(int i) const {
        printf("#%d Numbers are: %d, %d\n", i, x, y);
    }
    int x;
    int y;
};

void repeat(const MSTL::function<void(int)>& func) {
    func(1);
    func(2);
}

void try_func() {
    USE_MSTL;
    int x = 4;
    int y = 2;
    repeat([=](int i) {
        printf("#%d Numbers are: %d, %d\n", i, x, y);
        });
    func_printnum_t func_printnum{ x, y };
    repeat(func_printnum);
    repeat(func_hello);

    function<void(int)> f{ [](int i) {
        printf("i = %d\n", i);
    } };

    f(2);
    auto ff = f;
    ff(3);
}
void try_ss() {
    USE_MSTL;
    stringstream ss;
    ss << "a" << 'b' << 333 << " " << 9.333 << string("hello") << false << MSTL::move(string("aaaa"));
    std::cout << ss.str() << std::endl;
    ss.str("wert");
    std::cout << ss.str() << std::endl;
}

int main() {
    USE_MSTL;
    try_math();
    return 0;
}