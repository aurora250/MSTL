#include "try.h"
USE_MSTL

void test_date() {
    _MSTL date d1(2024, 2, 29);
    assert(d1.get_year() == 2024 && d1.get_month() == 2 && d1.get_day() == 29);

    _MSTL date d2(2023, 2, 29);
    assert(d2 == _MSTL date::epoch());

    assert(_MSTL date::is_leap_year(2020) == true);
    assert(_MSTL date::is_leap_year(2019) == false);
    assert(_MSTL date::is_leap_year(2100) == false);
    assert(_MSTL date::is_leap_year(2400) == true);

    assert(_MSTL date::get_month_day_(2024, 2) == 29);
    assert(_MSTL date::get_month_day_(2023, 2) == 28);
    assert(_MSTL date::get_month_day_(2023, 4) == 30);

    _MSTL date d3(2024, 1, 1);
    assert(d3.day_of_week() == 1);

    _MSTL date d4(2024, 3, 1);
    assert(d4.day_of_year() == 61);

    _MSTL date d5(2024, 2, 28);
    d5 += 2;
    assert(d5.get_month() == 3 && d5.get_day() == 1);

    _MSTL date d6(2024, 3, 1);
    d6 -= 1;
    assert(d6.get_month() == 2 && d6.get_day() == 29);

    assert(_MSTL date(2024, 1, 1) < _MSTL date(2024, 1, 2));
    assert(_MSTL date(2024, 1, 1) > _MSTL date(2023, 12, 31));

    auto str = _MSTL date(2024, 5, 10).to_string();
    assert(str == "2024-05-10");
    assert(_MSTL date::from_string("2024-05-10") == _MSTL date(2024, 5, 10));
    assert(_MSTL date::from_string("invalid") == _MSTL date::epoch());

    std::cout << "test_date passed\n";
}

void test_time() {
    using MSTL::time;
    _MSTL time t1(23, 59, 59);
    assert(t1.get_hours() == 23 && t1.get_minutes() == 59 && t1.get_seconds() == 59);

    _MSTL time t2(25, 60, 60);
    assert(t2 == _MSTL time(0, 0, 0));

    assert(_MSTL time(1, 2, 3).to_seconds() == 3600 + 120 + 3);

    _MSTL time t3(23, 59, 59);
    t3 += 2;  // 00:00:01
    assert(t3.get_hours() == 0 && t3.get_seconds() == 1);

    _MSTL time t4(0, 0, 1);
    t4 -= 2;  // 23:59:59
    assert(t4.get_hours() == 23 && t4.get_seconds() == 59);

    assert(_MSTL time(12, 0, 0) < _MSTL time(13, 0, 0));
    assert(_MSTL time(12, 30, 0) > _MSTL time(12, 29, 59));

    assert(_MSTL time(9, 8, 7).to_string() == "09:08:07");
    assert(_MSTL time::from_string("09:08:07") == _MSTL time(9, 8, 7));
    assert(_MSTL time::from_string("invalid") == _MSTL time(0, 0, 0));

    std::cout << "test_time passed\n";
}

void test_datetime() {
    using MSTL::time;
    _MSTL datetime dt1(_MSTL date(2024, 1, 1), _MSTL time(12, 0, 0));
    assert(dt1.get_year() == 2024 && dt1.get_hours() == 12);

    _MSTL datetime dt2(2024, 2, 28, 23, 59, 59);
    dt2 += 2;
    assert(dt2.get_month() == 2 && dt2.get_day() == 29 && dt2.get_seconds() == 1);

    _MSTL datetime dt3(2024, 3, 1, 0, 0, 0);
    dt3 -= 1;
    assert(dt3.get_month() == 2 && dt3.get_day() == 29);

    _MSTL datetime dt4(2024, 1, 1, 0, 0, 0);
    _MSTL datetime dt5(2023, 12, 31, 23, 59, 59);
    assert(dt4 - dt5 == 1);

    assert(_MSTL datetime(2024, 5, 10, 9, 8, 7).to_string() == "2024-05-10 09:08:07");
    assert(_MSTL datetime::from_string("2024-05-10 09:08:07")
        == _MSTL datetime(2024, 5, 10, 9, 8, 7));
    assert(_MSTL datetime::from_string("invalid") == _MSTL datetime::epoch());

    std::cout << "test_datetime passed\n";
}

void test_timestamp() {
    _MSTL datetime epoch = _MSTL datetime::epoch();
    _MSTL timestamp ts1(epoch);
    assert(ts1.get_seconds() == 0);
    assert(ts1.to_datetime() == epoch);

    _MSTL timestamp ts2(86400);
    _MSTL datetime dt = ts2.to_datetime();
    assert(dt.get_day() == 2);

    _MSTL timestamp ts3(100);
    _MSTL timestamp ts4(200);
    assert(ts3 < ts4);
    assert(ts4 - ts3 == 100);

    std::cout << "test_timestamp passed\n";
}

void test_utc_conversion() {
    _MSTL datetime dt(2024, 1, 1, 0, 0, 0);
    _MSTL datetime utc = _MSTL datetime::to_utc(dt);
    _MSTL datetime local = _MSTL datetime::from_utc(utc);
    assert(local == dt);

    std::cout << "test_utc_conversion passed\n";
}


void try_datetime() {
    test_date();
    test_time();
    test_datetime();
    test_timestamp();
    test_utc_conversion();
    println(datetime::now());
}

void try_print() {
    decimal_t f = constants::PI;
    FUNCTION_MANAGE_OPERATE enu = FUNCTION_MANAGE_OPERATE::GET_PTR;
    __nocopy_type uni{};
    bit_reference obj{};
    int c_arr[2];
    int* pa = new int[2];
    delete[] pa;
    println_feature(pa);
    pa = nullptr;
    int* p = &c_arr[0];
    const char* cs = "Hello World!";
    int pair<int, char>::* mop = &pair<int, char>::first;
    int pair<int, char>::* null_mop = nullptr;
#ifdef MSTL_VERSION_14__
    void (bit_reference::* mfp)() const = &bit_reference::flip;
#else
    void (bit_reference::* mfp)() const noexcept = &bit_reference::flip;
#endif
    DatabaseTypeCastError err;
    compressed_pair<printer<int>, int> cp;
    tuple<int, char, decimal_t, int*> tup{1, 't', f, pa};
    pair<int, char> pir{1, '1'};
    vector<int> v{1, 2, 3};
    vector<int>::iterator iter = v.begin();
    vector<pair<int, char>> pir_vec{{1, '1'}, {2, '2'}, {3, '3'}};
    temporary_buffer<vector<int>::iterator> tb{v.begin(), v.end()};
    shared_ptr<int> sp = make_shared<int>(1);
    shared_ptr<int> spc = sp;
    unique_ptr<int> up = make_unique<int>(1);
    hash<int> ih;
    array<int, 5> arr{1,2,3,4,5};
    variant<int, char> var = {v[0]};
    var.emplace<1>('c');
    list<int> lls{arr.begin(), arr.end()};
    map<int, int> mmi{{1,2}};
    unordered_map<int, int> umi{{1,2}};
    set<int> s{1,2,3};
    unordered_set<int> us{1,2,3};
    bitmap bm{10,false};
    string str = "胡";
    string_view sv = cs;
    wstring ws = L"WSTRING胡";
    // ensure you used external utf-8 console
    const char* emoji = "\n胡Hello, World! 😊 🌟 🚀 ✔";
    const wchar_t* wemoji = L"\n胡Hello, World! 😊 🌟 🚀 ✔";
    const char16_t* u16emoji = u"\n胡Hello, World! 😊 🌟 🚀 ✔";
    const char32_t* u32emoji = U"\n胡Hello, World! 😊 🌟 🚀 ✔";
#ifdef MSTL_VERSION_20__
    const char8_t* u8emoji = u8"\n胡Hello, World! 😊 🌟 🚀 ✔";
    println_feature(u8emoji);
#endif

    println_feature(tup, var);
    println_feature(pir, cp, ih);
    println_feature('c', nullptr);
    println_feature(&RB_TREE_RED__, &RB_TREE_BLACK__);
    println_feature(f, enu);
    println_feature(uni, obj);
    println_feature("\n\\\"\v", cs, err);
    println_feature(try_any, try_check, initialize<int>, make_shared<int, int>, _MSTL datetime::epoch);
    println_feature(mop, null_mop, mfp);
    println_feature(p, iter);
    println_feature(tb);
    println_feature(sp, spc, up);
    println_feature(c_arr, arr);
    println_feature(lls);
    println_feature(bm);
    println_feature(v);
    println_feature(str, sv, ws);
    println_feature(emoji, wemoji);
    println_feature(u16emoji, u32emoji);

    split_line();

    println(tup, var);
    println(pir, cp, ih);
    println('c', nullptr);
    println(&RB_TREE_RED__, &RB_TREE_BLACK__);
    println(f, enu);
    println(uni, obj);
    println("\n\\\"\v", cs, err);
    println(try_any, try_check, initialize<int>, make_shared<int, int>, _MSTL datetime::epoch);
    println(mop, null_mop, mfp);
    println(p, iter);
    println(tb);
    println(sp, spc, up);
    println(c_arr, arr);
    println(lls);
    println(bm);
    println(v);
    println(str, sv, ws);
    println(emoji, wemoji);
    println(u16emoji, u32emoji);
    println(mmi, umi);
    println(s, us);
}

void try_rnd() {
    print(_MSTL secret::is_supported(), secret::next_double(), secret::next_int(1, 10));
    random rnd;
    print(rnd.next_int(10, 20), rnd.next_int(10, 20), rnd.next_int(10, 20));
}

void try_exc() {
    try {
        Exception(MemoryError("test"));
    }
    catch (Error&) {
        Exit(true, try_lls);
        try_lls();
    }
}
void try_lls() {
    list<int> lls{ 1,2,3,4,5,6,7 };
    println(lls);
    lls.push_back(3);
    lls.push_back(4);
    lls.push_front(10);
    println(lls);
    lls.reverse();
    println(lls);
    lls.sort();
    lls.pop_back();
    lls.pop_front();
    println(lls);
    list<int> lls2 = { 5,3,2,1,1 };
    println(lls2);
    lls2.remove(5);
    lls2.sort();
    println(lls2);
    lls2.unique();
    println(lls2);
    list<MSTL::unique_ptr<int>> nocopy;
    // nocopy.emplace_back(2); also not support in std
    lls.clear();

    // list<int> long_list;
    // constexpr MSTL::size_t element_count = 100000;
    // for (MSTL::size_t i = 0; i < element_count; ++i) {
    //     if (i % 2 == 0) {
    //         long_list.push_back(i);
    //     } else {
    //         long_list.push_front(i);
    //     }
    // }
    // println(long_list);
}

void try_check() {
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
    std::cout << check_type<decltype((cstr))>() << std::endl;
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

    int ia[] = { 0,1,2,3,4,5,6,7,8 };
    MSTL::copy(ia + 2, ia + 7, ia + 4);
    println(ia); //0 1 2 3 2 3 4 5 6
    std::cout << std::endl;
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
    //vector<int> fin(9, 1);
    //MSTL::copy(first, last, result);
    //println(id);

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
    //MSTL::copy(first, last, result);
    //std::for_each(id.begin(), id.end(), display<int>()); //0 1 2 3 2 3 4 5 6
    //std::cout << std::endl;
}
void try_deq() {
    deque<int> a{1,2,3,4,5,6,7,8,9,10};
    println(a);
    a.push_back(2);
    a.push_front(10);
    a.push_back(3);
    a.push_back(7);
    a.push_back(6);
    a.insert(a.end(), 100);
    a.emplace(a.begin(), 0);
    println(a);
    a.pop_back();
    a.pop_front();
    println(a);
    a.assign(10, 5);
    println(a);
    deque<int> b{ 1,2,3,4,5 };
    println(b);
    deque<int> c(MSTL::move(b));
    c.resize(10, 6);
    println(c);

    deque<int> long_deque;
    constexpr MSTL::size_t element_count = 100000;
    for (MSTL::size_t i = 0; i < element_count; ++i) {
        if (i % 2 == 0) {
            long_deque.push_back(i);
        } else {
            long_deque.push_front(i);
        }
    }
    // println(long_deque);
    for (MSTL::size_t i = 0; i < element_count; ++i) {
        if (i % 2 == 0) {
            long_deque.pop_back();
        } else {
            long_deque.pop_front();
        }
    }
    // println(long_deque);
}

void try_stack() {
    stack<int> s;
    s.push(2);
    s.push(3);
    s.push(5);
    s.push(4);
    println(s);
    s.pop();
    println(s);

    stack<int> long_stack;
    constexpr MSTL::size_t element_count = 100000;
    for (MSTL::size_t i = 0; i < element_count; ++i) {
        long_stack.push(i);
    }
    // println(long_stack);
}
void try_vec() {
    try{
        vector<int> v{ 1,2,3,4 };
        v.push_back(3);
        v.push_back(4);
        println(v);
        vector<int> v2(v);
        v.insert(v.end(), v2.cbegin(), v2.cend());
        println(v);
        v.pop_back();
        v.clear();
        std::cout << v.empty() << std::endl;
        v.insert(v.end(), v2.cbegin(), v2.cend());
        println(v);
        const auto v3 = MSTL::move(v2);
        println(v3);
        vector<int> v4 = { 3,2,1 };
        v4.shrink_to_fit();
        v4.emplace(v4.begin() + 1, 5);
        v4.erase(--v4.end());
        println(v4);
        vector<int, ctype_allocator<int>> cvec;
        cvec.emplace_back(3);
        cvec.emplace_back(4);
        println(cvec);

        vector<int> vec;
        vec.assign(5, 10);
        println(vec);
        vec.assign({ 1, 2, 3, 4, 5 });
        println(vec);
        vector<int> anotherVec = { 6, 7, 8 };
        vec.assign(anotherVec.begin(), anotherVec.end());
        println(vec);
    }
    catch (Error& error) {
        println(error);
        std::cout << std::endl;
    }

    vector<int> long_vector;
    constexpr MSTL::size_t element_count = 100000;
    for (MSTL::size_t i = 0; i < element_count; ++i) {
        long_vector.push_back(i);
    }
    // println(long_vector);
}
void try_pque() {
    priority_queue<int> q;
    std::cout << typeid(priority_queue<int*>).name() << std::endl;
    q.push(6); q.push(9); q.push(1); q.push(5);
    q.push(8); q.push(4); q.emplace(7);
    println(q); // 9 8 7 5 6 1 4
    q.pop();
    println(q);

    random rnd(timestamp::now().get_seconds());
    priority_queue<int> long_pque;
    constexpr MSTL::size_t element_count = 100000;
    for (int i = 0; i < element_count; ++i) {
        long_pque.push(rnd.next_int(10000));
    }
    // println(long_pque);
}

void try_rb() {
    random rnd(timestamp::now().get_seconds());

    map<int, char> m;
    m.insert(pair<int, char>(1, 'c'));
    m.emplace(3, 'c');
    m.emplace_hint(m.end(), 4, 'd');
    m[1] = 'a';
    m[100] = 'x';
    m[2] = 'b';
    println(m);
    m.erase(m.begin());
    println(m);
    m.clear();

    map<int, float> long_map;
    for (int i = 0; i < 100000; ++i) {
        int key = i;
        long_map.insert({key, rnd.next_double(0, 10000)});
    }
    // println(long_map);


    multimap<int, const char*> mm;
    mm.emplace(1, "c");
    mm.emplace(2, "b");
    mm.emplace(1, "a");
    println(mm);
    mm.erase(mm.begin());
    mm.insert(mm.begin(), pair<int, const char*>(1, "a"));
    println(mm);
    mm.clear();

    multimap<int, float> long_multimap;
    for (int i = 0; i < 100000; ++i) {
        int key = i;
        long_multimap.insert({key, rnd.next_double(0, 10000)});
    }
    // println(long_multimap);


    set<int> s{ 1,2,3,4,5 };
    s.insert(s.begin(), 1);
    s.emplace(2);
    println(s);
    s.erase(s.begin());
    println(s);
    s.clear();
    println(s);

    set<int> long_set;
    for (int i = 0; i < 100000; ++i) {
        long_set.emplace(i);
    }
    // println(long_set);


    multiset<int> ms{ 4,5,6,7,8,8 };
    ms.insert(ms.begin(), 9);
    ms.emplace(10);
    println(ms);

    multiset<int> long_multiset;
    for (int i = 0; i < 100000; ++i) {
        long_multiset.emplace(i);
    }
    // println(long_multiset);
}

void try_tup() {
    tuple<int, char, const char*> t(1, 't', "MSTL");
    auto a = get<0>(t);
    std::cout << MSTL::get<1>(t) << std::endl;
    auto forw = MSTL::make_tuple(9, 0);

    tuple<int, double> tuple1(1, 3.14);
    tuple<MSTL::string> tuple2("hello");
    tuple<char> tuple3('A');

    auto combinedTuple = MSTL::tuple_cat(tuple1, tuple2, tuple3);
    std::cout << check_type<decltype(combinedTuple)>() << std::endl;

    std::cout << "Combined tuple elements:" << std::endl;
    std::cout << MSTL::get<0>(combinedTuple) << std::endl;
    std::cout << MSTL::get<1>(combinedTuple) << std::endl;
    std::cout << MSTL::get<2>(combinedTuple) << std::endl;
    std::cout << MSTL::get<3>(combinedTuple) << std::endl;

    tuple<int, int, int> args(1, 2, 3);
    int sum = MSTL::apply(sum_3, args);
    std::cout << "Sum: " << sum << std::endl;

    tuple<int, int> mulArgs(4, 5);
    int product = MSTL::apply(multiplies<int>(), mulArgs);
    std::cout << "Product: " << product << std::endl;
}

void try_hash() {
    unordered_map<int, char> m;
    m[1] = 'a';
    m[2] = 'b';
    m.insert(pair<int, char>(3, 'c'));
    m.emplace(2, 'c');
    m.insert(pair<int, char>(1, 'b'));
    println(m);
    unordered_map<int, char> m2;
    m2.insert(m.begin(), m.end());
    println(m2);
    unordered_multimap<string, int> mm;
    mm.emplace("a", 1);
    mm.emplace("a", 2);
    mm.insert(pair<string, int>(string("a"), 1));
    println(mm);
    mm.clear();
    println(mm);
    unordered_map<int, unique_ptr<int>> uncopy;
    uncopy.emplace(1, MSTL::make_unique<int>(1));
    println(uncopy);
    uncopy.erase(uncopy.begin());

    unordered_set<pair<int, char>> us;
    us.emplace(1, 'c');
    us.insert(pair<int, char>(4, 'r'));
    println(us);
    us.erase(pair<int, char>(4, 'r'));
    us.erase(us.begin());
    println(us);

    unordered_multiset<pair<int, const char*>> ms;
    ms.emplace(1, "234");
    ms.insert(MSTL::make_pair(2, "345"));
    ms.emplace(1, "234");
    println(ms);
    ms.erase(ms.begin());
    println(ms);
}

void try_math() {
    std::cout << power(2, 10) << std::endl;
    std::cout << factorial(10) << std::endl;
    std::cout << sine(1) << " : " << std::endl;
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
}

void try_sort() {
    USE_MSTL;
    MSTL::vector<int> vec{ 6,9,1,5,8,4,7 };
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
    println(vec);
    //vector<Person> people = {
    //{"Alice", 25},
    //{"Bob", 20},
    //{"Charlie", 30},
    //{"David", 20}
    //};
    //counting_sort(people.begin(), people.end(),
    //    [](const Person& a, const Person& b) -> bool { return a.age < b.age; },
    //    [](const Person& p) -> int { return p.age; });
    //radix_sort_greater(people.begin(), people.end(), [](const Person& x) -> int { return x.age; });
    //println(people);
}

void try_ss() {
    stringstream ss;
    ss << "a" << 'b' << 333 << " " << 9.333 << MSTL::string("hello") << false << MSTL::move(MSTL::string("a"));
    std::cout << ss.str() << std::endl;
    ss.str("where");
    std::cout << ss.str() << std::endl;
}

void try_var() {
#ifdef MSTL_VERSION_17__
    variant<int, string> v1;
    std::cout << v1.index() << std::endl;
    variant<int, string> v2;
    v2.emplace<1>("hello");
    std::cout << v2.index() << std::endl;

    auto& str = v2.get<string>();
    std::cout << str << std::endl;
    auto ptr = v2.get_if<string>();
    if (!ptr || *ptr != "hello") {
        std::cerr << "get_if method test failed." << std::endl;
        return;
    }
    v2.emplace<int>(42);
    std::cout << v2.index() << ":" << v2.get<int>() << std::endl;
    auto visitor = [](auto&& arg) {
        using T = decay_t<decltype(arg)>;
        if constexpr (is_same_v<T, int>) {
            return arg * 2;
        } else if constexpr (is_same_v<T, string>) {
            return arg.length();
        }
    };
    int result = v2.visit(visitor);
    std::cout << result << std::endl;

    hash<variant<int, string>> hasher{};
    std::cout << hasher(v1) << std::endl;
#endif
}


using MSTL::size_t;

string generate_random_string(MSTL::size_t length) {
    const string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string s;
    s.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        s += chars[rand() % chars.size()];
    }
    return s;
}

void test_short_strings(size_t count, size_t length) {
    vector<string> strings;
    strings.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        strings.emplace_back(generate_random_string(length));
    }

    strings.clear();
    strings.shrink_to_fit();

    std::cout << "Test 1: " << count << " short strings (" << length << " chars)\n";
}

void test_long_string_concat(size_t iterations, size_t chunk_size) {
    string long_str;
    long_str.reserve(iterations * chunk_size);

    for (size_t i = 0; i < iterations; ++i) {
        long_str += generate_random_string(chunk_size);
    }

    std::cout << "Test 2: Long string concat (" << iterations << " chunks, "
              << chunk_size << " chars each, total: " << long_str.size() << " chars)\n";
}

void test_string_modification(size_t initial_length, size_t operations) {
    string str = generate_random_string(initial_length);

    for (size_t i = 0; i < operations; ++i) {
        if (i % 2 == 0) {
            size_t pos = rand() % (str.size() + 1);
            str.insert(pos, 1, 'X');
        } else {
            if (str.empty()) break;
            size_t pos = rand() % str.size();
            str.erase(pos, 1);
        }
    }

    std::cout << "Test 3: " << operations << " modify operations (initial: "
              << initial_length << " chars, final: " << str.size() << " chars)\n";
}

void test_string_search_replace(size_t str_length, size_t pattern_count) {
    string str = generate_random_string(str_length);
    const string pattern = "ABC";
    const string replacement = "XYZ";

    for (size_t i = 0; i < pattern_count; ++i) {
        size_t pos = rand() % (str.size() - pattern.size() + 1);
        str.replace(pos, pattern.size(), pattern);
    }

    size_t replace_count = 0;
    size_t pos = 0;
    while ((pos = str.find(pattern, pos)) != string::npos) {
        str.replace(pos, pattern.size(), replacement);
        pos += replacement.size();
        replace_count++;
    }

    std::cout << "Test 4: Search & replace (str length: " << str_length
              << ", patterns found: " << replace_count << ")\n";
}

void test_max_memory_string() {
    try {
        size_t available_memory = get_available_memory();
        size_t max_test_size = available_memory / 2;

        const size_t upper_limit = 4ULL * 1024 * 1024 * 1024;
        max_test_size = _MSTL min(max_test_size, upper_limit);

        if (max_test_size == 0) {
            throw Error("Insufficient system memory for test.");
        }

        string huge_str;
        huge_str.reserve(max_test_size);

        size_t chunk = 1024 * 1024;
        size_t total_written = 0;

        std::cout << "Testing max memory string (target size: "
                  << max_test_size / (1024 * 1024) << " MB)\n";

        while (total_written < max_test_size) {
            size_t write = _MSTL min(chunk, max_test_size - total_written);
            huge_str.append(write, 'A');
            total_written += write;

            if (total_written % (100 * 1024 * 1024) == 0) {
                std::cout << "Allocated " << total_written / (1024 * 1024) << " MB...\n";
            }
        }
        std::cout << "Test 5: Success. Allocated "
                  << total_written / (1024 * 1024) << " MB string.\n";
    }
    catch (const Error& e) {
        std::cout << "Test 5: " << e.what() << "\n";
    }
}

void try_str() {
    srand(std::time(nullptr));
    test_short_strings(1000000, 32);
    test_long_string_concat(100000, 1024);
    test_string_modification(100000, 500000);
    test_string_search_replace(1000000, 10000);
    test_max_memory_string();
}


void try_opt() {
    optional<int> opt1;
    println(opt1);

    optional<int> opt2(nullopt);
    println(opt2);

    optional<int> opt3(42);
    println(opt3);

    opt1 = 100;
    println(opt1);

    optional<int> opt4(opt3);
    println(opt4);

    opt2 = opt3;
    println(opt2);

    optional<string> opt5(inplace_construct_tag{}, "Hello, World!");
    println(opt5);

    opt1.emplace(200);
    println(opt1);

    opt1.reset();
    println(opt1);

    if (opt3.has_value()) {
        std::cout << "opt3 has a value." << std::endl;
    } else {
        std::cout << "opt3 has no value." << std::endl;
    }

    int default_val = opt1.value_or(300);
    std::cout << "Value of opt1 or default: " << default_val << std::endl;

    auto result1 = opt1.or_else([]() { return MSTL::optional<int>(400); });
    println(result1);

    auto result2 = opt3.and_then([](int x) { return MSTL::optional<int>(x * 2); });
    println(result2);

    auto result3 = opt3.transform([](int x) { return x + 1; });
    println(result3);
}

void try_any() {
    MSTL::any a1;
    std::cout << "Testing default constructor:" << std::endl;
    println(a1);
    std::cout << "Has value: " << (a1.has_value() ? "Yes" : "No") << std::endl;

    MSTL::any a2(42);
    std::cout << "\nTesting constructor with value:" << std::endl;
    println(a2);
    std::cout << "Has value: " << (a2.has_value() ? "Yes" : "No") << std::endl;
    const int* ptr = MSTL::any_cast<int>(&a2);
    if (ptr) {
        std::cout << "Value: " << *ptr << std::endl;
    }

    MSTL::any a3(a2);
    std::cout << "\nTesting copy constructor:" << std::endl;
    println(a3);
    std::cout << "Has value: " << (a3.has_value() ? "Yes" : "No") << std::endl;
    ptr = MSTL::any_cast<int>(&a3);
    if (ptr) {
        std::cout << "Value: " << *ptr << std::endl;
    }

    MSTL::any a4(MSTL::any(123));
    std::cout << "\nTesting move constructor:" << std::endl;
    println(a4);
    std::cout << "Has value: " << (a4.has_value() ? "Yes" : "No") << std::endl;
    ptr = MSTL::any_cast<int>(&a4);
    if (ptr) {
        std::cout << "Value: " << *ptr << std::endl;
    }

    a1 = a4;
    std::cout << "\nTesting assignment operator:" << std::endl;
    println(a1);
    std::cout << "Has value: " << (a1.has_value() ? "Yes" : "No") << std::endl;
    ptr = MSTL::any_cast<int>(&a1);
    if (ptr) {
        std::cout << "Value: " << *ptr << std::endl;
    }

    string str = "Hello, World!";
    string result = a1.emplace<string>(str);
    std::cout << "\nTesting emplace method:" << std::endl;
    println(a1);
    std::cout << "Has value: " << (a1.has_value() ? "Yes" : "No") << std::endl;
    const string* strPtr = MSTL::any_cast<string>(&a1);
    if (strPtr) {
        std::cout << "Value: " << *strPtr << std::endl;
    }

    a1.reset();
    std::cout << "\nTesting reset method:" << std::endl;
    println(a1);
    std::cout << "Has value: " << (a1.has_value() ? "Yes" : "No") << std::endl;

    a1 = 10;
    a2 = 20;
    std::cout << "\nBefore swap:" << std::endl;
    std::cout << "a1: "; println(a1);
    std::cout << "a2: "; println(a2);
    a1.swap(a2);
    std::cout << "After swap:" << std::endl;
    std::cout << "a1: "; println(a1);
    std::cout << "a2: "; println(a2);
}

void try_db() {
#ifdef MSTL_SUPPORT_MYSQL__
    std::clock_t begin = clock();
    database_settings::dbname = "book";
    database_settings::password = "147258hu";

    database_pool& pool = get_instance_database_pool();
    for (int i = 0; i < 5000; i++) {
        bool fin = pool.get_connect()->update("SELECT 1");
        // std::cout << fin << " ";
    }
    std::cout << std::endl << (1.0 * (clock() - begin) / CLOCKS_PER_SEC) << std::endl;

    auto result = pool.get_connect()->query("SELECT * FROM book");
    while (result.next()) {
        for (int i = 0; i < result.column_count(); i++) {
            if (i == 2) {
                int count = result.at_int16(i);
                std::cout << "collected :" << count << ", ";
            }
            else if (i == 3) {
                float count = result.at_float32(i);
                std::cout << "usable :" << count << ", ";
            }
            else if (i == 5) {
                _MSTL datetime dt = result.at_datetime(i);
                std::cout << "date: " << dt.to_string() << ", ";
            }
            else
                println(result.at(i), ", ");
        }
        std::cout << std::endl;
    }
    std::cout << result.row_count() << ", " << result.column_count() << std::endl;

    split_line();

    begin = clock();
    for (int i = 0; i < 5000; i++) {
        char sql[power(2, 10)] = {};
        sprintf(sql, "SELECT 1");
        auto* conn = new database_connect();
        if(conn->connect_to()) {
            bool fin = conn->update(sql);
            // std::cout << fin << " ";
        }
        delete conn;
    }
    std::cout << std::endl << (1.0 * (clock() - begin) / CLOCKS_PER_SEC) << std::endl;
#endif
}

void try_dns() {
#ifdef MSTL_PLATFORM_LINUX__
    int n = 10;
    while (n--) {
        const clock_t begin = clock();
        auto &pool = get_instance_thread_pool();
        pool.start();
        for (int i = 0; i < size(DOMAINS); i++) {
            pool.submit_task([domain = DOMAINS[i]] { dns_client_commit(domain); });
        }
        const clock_t end = clock();
        printf("\n\nRunning Time??%lfs\n", static_cast<double>(end - begin) / CLOCKS_PER_SEC);
    }
#endif
}

void try_pool() {
    thread_pool& pool = get_instance_thread_pool();
    pool.start();
    pool.submit_task(try_lls);
    pool.submit_task(try_deq);
    pool.submit_task(try_hash);
    pool.submit_task(try_vec);
    pool.submit_task(try_math);
    pool.stop();
    pool.set_mode(THREAD_POOL_MODE::MODE_CACHED);
    pool.start();
    pool.submit_task(try_str);
    pool.submit_task(try_tup);
    pool.submit_task(try_rb);
    pool.submit_task(try_var);
    pool.submit_task(try_opt);
    pool.submit_task(try_check);
    pool.submit_task(try_any);
    pool.submit_task(try_datetime);
    pool.submit_task(try_rnd);
    pool.submit_task(try_print);
    // pool.submit_task(try_db);
    pool.stop();
}
