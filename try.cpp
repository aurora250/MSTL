#include "try.h"
USE_MSTL
using MSTL::size_t;


const string TEST_FILE = "test_temp_file.txt";
const string TEST_DIR = "test_temp_dir";
const string TEST_SUB_DIR = TEST_DIR + "/sub_dir";
const string TEST_CONTENT = "Hello, File Class!\nSecond line.\r\nThird line";

void test_file_basic_operations() {
    bool create_ok = file::create_and_write(TEST_FILE, TEST_CONTENT);
    assert(create_ok);
    assert(file::exists(TEST_FILE));
    assert(file::is_file(TEST_FILE));
    assert(!file::is_directory(TEST_FILE));

    assert(file::file_size(TEST_FILE) == TEST_CONTENT.size());

    string read_content;
    bool read_ok = file::read_all(TEST_FILE, read_content);
    assert(read_ok);
    assert(read_content == TEST_CONTENT);

    file f;
    assert(!f.opened());
    assert(f.open(TEST_FILE, FILE_ACCESS_MODE::READ_WRITE));
    assert(f.opened());
    assert(f.file_path() == TEST_FILE);

    string line;
    assert(f.read_line(line));
    assert(line == "Hello, File Class!");
    assert(f.read_line(line));
    assert(line == "Second line.");
    assert(f.read_line(line));
    assert(line == "Third line");

    assert(f.seek(0, FILE_POINT_ORIGIN::BEGIN));
    assert(f.tell() == 0);
    assert(f.seek(5, FILE_POINT_ORIGIN::CURRENT));
    assert(f.tell() == 5);

    assert(f.seek(0));
    string append_str = " Append";
    size_t written = f.write(append_str, append_str.size());
    assert(written == append_str.size());
    assert(f.flush());
    assert(f.size() == TEST_CONTENT.size() + append_str.size());

    assert(f.truncate(10));
    assert(f.size() == 10);

    f.close();
    assert(!f.opened());
}

void test_directory_operations() {
    assert(!file::exists(TEST_SUB_DIR));
    bool dir_ok = file::create_directories(TEST_SUB_DIR);
    assert(dir_ok);
    assert(file::exists(TEST_SUB_DIR));
    assert(file::is_directory(TEST_SUB_DIR));

    string sub_file = TEST_SUB_DIR + "/sub_file.txt";
    assert(file::create_and_write(sub_file, "sub content"));
    assert(file::exists(sub_file));
}

void test_file_attributes_and_times() {
    file f(TEST_FILE);
    assert(f.open(TEST_FILE));

    _MSTL FILE_ATTRIBUTE original_attr = f.attributes();
    bool set_attr_ok = f.set_attributes(_MSTL FILE_ATTRIBUTE::READONLY);
    assert(set_attr_ok);
    assert(static_cast<bool>(f.attributes() & _MSTL FILE_ATTRIBUTE::READONLY));
    assert(f.set_attributes(original_attr));
    assert(f.attributes() == original_attr);

    _MSTL datetime now = _MSTL datetime::now();
    bool set_time_ok = f.set_last_write_time(now);
    assert(set_time_ok);
    println(f.last_write_time(), now);
    assert(f.last_write_time().to_string() == now.to_string());

    f.close();
}

void test_file_lock_and_other_operations() {
    file f(TEST_FILE);
    assert(f.open(TEST_FILE));

    bool locked = f.lock(0, 10, _MSTL FILE_LOCK_MODE::EXCLUSIVE);
    assert(locked);
    bool unlocked = f.unlock(0, 10);
    assert(unlocked);

    string copy_file = TEST_FILE + ".copy";
    assert(file::copy(TEST_FILE, copy_file));
    assert(file::exists(copy_file));
    string copy_content;
    file::read_all(copy_file, copy_content);
    assert(copy_content.size() == f.size());

    string move_file = TEST_DIR + "/moved_file.txt";
    assert(file::move(copy_file, move_file));
    assert(!file::exists(copy_file));
    assert(file::exists(move_file));

    string rename_file = TEST_DIR + "/renamed_file.txt";
    assert(file::rename(move_file, rename_file));
    assert(!file::exists(move_file));
    assert(file::exists(rename_file));

    f.close();
}

void test_move_semantics() {
    file f1(TEST_FILE);
    assert(f1.open(TEST_FILE));
    file f2 = _MSTL move(f1);
    assert(!f1.opened());
    assert(f2.opened());
    assert(f2.file_path() == TEST_FILE);

    file f3;
    f3 = _MSTL move(f2);
    assert(!f2.opened());
    assert(f3.opened());
    assert(f3.file_path() == TEST_FILE);
}

void clean_up() {
    if (file::exists(TEST_FILE)) {
        if (file::is_directory(TEST_FILE)) {
            file::remove_directory(TEST_FILE);
        } else {
            file::remove(TEST_FILE);
        }
    }
    if (file::exists(TEST_FILE)) {
        file::remove(TEST_FILE);
    }
    string sub_file = TEST_SUB_DIR + "/sub_file.txt";
    if (file::exists(sub_file)) {
        file::remove(sub_file);
    }
    string rename_file = TEST_DIR + "/renamed_file.txt";
    if (file::exists(rename_file)) {
        file::remove(rename_file);
    }
    if (file::exists(TEST_SUB_DIR)) {
        file::remove_directory(TEST_SUB_DIR);
    }
    if (file::exists(TEST_DIR)) {
        file::remove_directory(TEST_DIR);
    }
}

void test_file() {
    clean_up();
    try {
        test_file_basic_operations();
        test_directory_operations();
        test_file_attributes_and_times();
        test_file_lock_and_other_operations();
        test_move_semantics();
        clean_up();
        println("All test passed");
    } catch (...) {
        clean_up();
        println("Test failed");
    }
}


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

    println("test_date passed");
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

    println("test_time passed");
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

    println("test_datetime passed");
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

    println("test_timestamp passed");
}

void test_utc_conversion() {
    _MSTL datetime dt(2024, 1, 1, 0, 0, 0);
    _MSTL datetime utc = _MSTL datetime::to_utc(dt);
    _MSTL datetime local = _MSTL datetime::from_utc(utc);
    assert(local == dt);

    println("test_utc_conversion passed");
}


void test_datetimes() {
    test_date();
    test_time();
    test_datetime();
    test_timestamp();
    test_utc_conversion();
    println(datetime::now());
}

void test_print() {
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
    variant<int, char> var{v[0]};
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
    const char* emoji = "\n胡Hello, World! 😇👩‍🦳🎗️⚽🥠🍋‍🟩⛴️🪣💖🚯🕕😊🌟🚀✔";
    const wchar_t* wemoji = L"\n胡Hello, World! 😇👩‍🦳🎗️⚽🥠🍋‍🟩⛴️🪣💖🚯🕕😊🌟🚀✔";
    const char16_t* u16emoji = u"\n胡Hello, World! 😇👩‍🦳🎗️⚽🥠🍋‍🟩⛴️🪣💖🚯🕕😊🌟🚀✔";
    const char32_t* u32emoji = U"\n胡Hello, World! 😇👩‍🦳🎗️⚽🥠🍋‍🟩⛴️🪣💖🚯🕕😊🌟🚀✔";
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
    println_feature(test_any, test_check, initialize<int>, make_shared<int, int>, _MSTL datetime::epoch);
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
    println(test_any, test_check, initialize<int>, make_shared<int, int>, _MSTL datetime::epoch);
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

void test_rnd() {
    println(_MSTL secret::is_supported(), secret::next_double(), secret::next_int(1, 10));
    println(_MSTL random_lcd::next_int(10, 20), random_lcd::next_int(10, 20), random_lcd::next_int(10, 20));
    println(_MSTL random_mt::next_int(10, 20), random_mt::next_int(10, 20), random_mt::next_int(10, 20));
}

void test_exce() {
    try {
        Exception(MemoryError("test"));
    }
    catch (Error&) {
        Exit(true, test_list);
        test_list();
    }
}
void test_list() {
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

    list<int> long_list;
    constexpr MSTL::size_t element_count = 100000;
    for (MSTL::size_t i = 0; i < element_count; ++i) {
        if (i % 2 == 0) {
            long_list.push_back(i);
        } else {
            long_list.push_front(i);
        }
    }
    for (MSTL::size_t i = 0; i < element_count; ++i) {
        if (i % 2 == 0) {
            long_list.pop_back();
        } else {
            long_list.pop_back();
        }
    }
    // println(long_list);
}

void test_check() {
    println(check_type<string>());
    println(check_type<const volatile void* const*&>());
    println(check_type<int(*)[]>());
    println(check_type<const volatile void* (&)[10]>()); // void const volatile * (&) [10]
    println(check_type<int[1][2][3]>());              // int [1] [2] [3]
    println(check_type<char(*(* const)(const int(&)[10]))[10]>());
    println(check_type<int (Foo::* const)[3]>());
    println(check_type<int (Foo::* const)(int, Foo&&, int) volatile>());
    string cstr("const string");
    const string* sr = new string("hai");
    split_line();
    println(check_type<decltype((cstr))>());
    println(check_type<decltype(MSTL::move(cstr))>());
    split_line();
    println(check_type<decltype(sr)>());
    split_line();
    delete sr;
}

void test_copy() {
    //int ia[] = { 0,1,2,3,4,5,6,7,8 };
    //MSTL::copy(ia + 2, ia + 7, ia);
    //MSTL::for_each(ia, ia + 9, [](int x) { std::cout << x << std::endl; }); //2 3 4 5 6 5 6 7 8
    //std::cout << std::endl;

    int ia[] = { 0,1,2,3,4,5,6,7,8 };
    MSTL::copy(ia + 2, ia + 7, ia + 4);
    println(ia); //0 1 2 3 2 3 4 5 6
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
void test_deque() {
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

void test_stack() {
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
    for (MSTL::size_t i = 0; i < element_count; ++i) {
        long_stack.pop();
    }
    // println(long_stack);
}
void test_vector() {
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
        println(v.empty());
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
    }

    vector<int> long_vector;
    constexpr MSTL::size_t element_count = 100000;
    for (MSTL::size_t i = 0; i < element_count; ++i) {
        long_vector.push_back(i);
    }
    for (MSTL::size_t i = 0; i < element_count; ++i) {
        long_vector.pop_back();
    }
    // println(long_vector);
}
void test_pqueue() {
    priority_queue<int> q;
    println(typeid(priority_queue<int*>).name());
    q.push(6); q.push(9); q.push(1); q.push(5);
    q.push(8); q.push(4); q.emplace(7);
    println(q); // 9 8 7 5 6 1 4
    q.pop();
    println(q);

    priority_queue<int> long_pque;
    constexpr MSTL::size_t element_count = 100000;
    for (int i = 0; i < element_count; ++i) {
        long_pque.push(random_lcd::next_int(10000));
    }
    for (int i = 0; i < element_count; ++i) {
        long_pque.pop();
    }
    // println(long_pque);
}

void test_rbtree() {
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
        long_map.insert({key, random_lcd::next_double(0, 10000)});
    }
    for (int i = 0; i < 100000; ++i) {
        long_map.erase(i);
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
        long_multimap.insert({key, random_lcd::next_double(0, 10000)});
    }
    for (int i = 0; i < 100000; ++i) {
        long_multimap.erase(i);
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
    for (int i = 0; i < 100000; ++i) {
        long_set.erase(i);
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
    for (int i = 0; i < 100000; ++i) {
        long_multiset.erase(i);
    }
    // println(long_multiset);
}

void test_tuple() {
    tuple<int, char, const char*> t(1, 't', "MSTL");
    auto a = get<0>(t);
    println(MSTL::get<1>(t));
    auto forw = MSTL::make_tuple(9, 0);

    tuple<int, double> tuple1(1, 3.14);
    tuple<MSTL::string> tuple2("hello");
    tuple<char> tuple3('A');

    auto combinedTuple = MSTL::tuple_cat(tuple1, tuple2, tuple3);
    println(check_type<decltype(combinedTuple)>());

    println("Combined tuple elements:");
    println(MSTL::get<0>(combinedTuple));
    println(MSTL::get<1>(combinedTuple));
    println(MSTL::get<2>(combinedTuple));
    println(MSTL::get<3>(combinedTuple));

    tuple<int, int, int> args(1, 2, 3);
    int sum = MSTL::apply(sum_3, args);
    println("Sum:", sum);

    tuple<int, int> mulArgs(4, 5);
    int product = MSTL::apply(multiplies<int>(), mulArgs);
    println("Product:", product);
}

void test_hash() {
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

void test_math() {
    println(power(2, 10));
    println(power(3, 10));
    println(factorial(10));
    println(sine(1));
    println(cosine(angular2radian(270)));
    println(remainder(73.263, 0.9973));
    println(float_part(constants::PI));
    println(exponential(3));
    println(logarithm_e(165));
    println(logarithm_10(147));
    println(logarithm_2(500));
    println(arctangent(100));
    println(radian2angular(arctangent(100)));
    println(arcsine(1), arcsine(0), arcsine(-1));
    println(arccosine(1), arccosine(0), arccosine(-1));
    println(arctangent(DECIMAL_MAX_POSI_SIZE), arctangent(DECIMAL_MIN_NEGA_SIZE));
    // println(tangent(constants::PI / 2));  // MathError
    println(tangent(0));
    println(around_pi(constants::PI), " : ", around_pi(6.28));
}

void test_sort() {
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

struct var_visitor {
    int operator()(int arg) const { return arg * 2; }
    int operator()(const string& arg) const { return arg.length(); }
};

void test_variant() {
    variant<int, string> v1;
    println(v1.index());
    variant<int, string> v2;
    v2.emplace<1>("hello");
    println(v2.index());

    auto& str = v2.get<string>();
    println(str);
    auto ptr = v2.get_if<string>();
    if (!ptr || *ptr != "hello") {
        println("get_if method test failed.");
        return;
    }
    v2.emplace<int>(42);
    println(v2.index(), ":", v2.get<int>());
    int result = v2.visit(var_visitor());
    println(result);

    hash<variant<int, string>> hasher{};
    println(hasher(v1));
}

string generate_random_string(MSTL::size_t length) {
    const string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string s;
    s.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        s += chars[random_lcd::next_int() % chars.size()];
    }
    return s;
}

void test_short_strings(size_t count, size_t length) {
    vector<string> strings;
    strings.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        if (i % 10000 == 0)
            print(i, " ");
        strings.emplace_back(generate_random_string(length));
    }

    strings.clear();
    strings.shrink_to_fit();

    println("Test 1:", count, "short strings (", length, " chars)");
}

void test_long_string_concat(size_t iterations, size_t chunk_size) {
    string long_str;
    long_str.reserve(iterations * chunk_size);

    for (size_t i = 0; i < iterations; ++i) {
        long_str += generate_random_string(chunk_size);
    }

    println("Test 2: Long string concat (", iterations, "chunks,"
              , chunk_size, "chars each, total: ", long_str.size(), " chars)");
}

void test_string_modification(size_t initial_length, size_t operations) {
    string str = generate_random_string(initial_length);

    for (size_t i = 0; i < operations; ++i) {
        if (i % 2 == 0) {
            size_t pos = random_lcd::next_int() % (str.size() + 1);
            str.insert(pos, 1, 'X');
        } else {
            if (str.empty()) break;
            size_t pos = random_lcd::next_int() % str.size();
            str.erase(pos, 1);
        }
    }

    println("Test 3: ", operations, " modify operations (initial: "
              , initial_length, " chars, final: ", str.size(), " chars)");
}

void test_string_search_replace(size_t str_length, size_t pattern_count) {
    string str = generate_random_string(str_length);
    const string pattern = "ABC";
    const string replacement = "XYZ";

    for (size_t i = 0; i < pattern_count; ++i) {
        size_t pos = random_lcd::next_int() % (str.size() - pattern.size() + 1);
        str.replace(pos, pattern.size(), pattern);
    }

    size_t replace_count = 0;
    size_t pos = 0;
    while ((pos = str.find(pattern, pos)) != string::npos) {
        str.replace(pos, pattern.size(), replacement);
        pos += replacement.size();
        replace_count++;
    }

    println("Test 4: Search & replace (str length: ", str_length
              , ", patterns found: ", replace_count, ")");
}

void test_max_memory_string() {
    try {
        size_t available_memory = get_available_memory();
        size_t max_test_size = available_memory / 2;

        const size_t upper_limit = 4ULL * 1024 * 1024 * 1024;
        max_test_size = _MSTL min(max_test_size, upper_limit);

        if (max_test_size == 0) {
            Exception(MemoryError("Insufficient system memory for test."));
        }

        string huge_str;
        huge_str.reserve(max_test_size);

        size_t chunk = 1024 * 1024;
        size_t total_written = 0;

        println("Testing max memory string (target size: "
                  , max_test_size / (1024 * 1024), " MB)");

        while (total_written < max_test_size) {
            size_t write = _MSTL min(chunk, max_test_size - total_written);
            huge_str.append(write, 'A');
            total_written += write;

            if (total_written % (100 * 1024 * 1024) == 0) {
                println("Allocated ", total_written / (1024 * 1024), " MB...");
            }
        }
        println("Test 5: Success. Allocated "
                  , total_written / (1024 * 1024), " MB string.");
    }
    catch (const Error& e) {
        println("Test 5: ", e.what());
    }
}

void test_string() {
    test_short_strings(1000000, 32);
    test_long_string_concat(100000, 1024);
    test_string_modification(100000, 500000);
    test_string_search_replace(1000000, 10000);
    test_max_memory_string();

    stringstream ss;
    ss << "a" << 'b' << 333 << " " << 9.333 << MSTL::string("hello") << false << MSTL::move(MSTL::string("a"));
    println_feature(ss);
    ss.str("where");
    println_feature(ss);
}


void test_option() {
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
        println("opt3 has a value.");
    } else {
        println("opt3 has no value.");
    }

    int default_val = opt1.value_or(300);
    println("Value of opt1 or default: ", default_val);

    auto result1 = opt1.or_else([]() { return MSTL::optional<int>(400); });
    println(result1);

    auto result2 = opt3.and_then([](int x) { return MSTL::optional<int>(x * 2); });
    println(result2);

    auto result3 = opt3.transform([](int x) { return x + 1; });
    println(result3);
}

void test_any() {
    MSTL::any a1;
    println("Testing default constructor:");
    println(a1);
    println("Has value: ", (a1.has_value() ? "Yes" : "No"));

    MSTL::any a2(42);
    println("\nTesting constructor with value:");
    println(a2);
    println("Has value: ", (a2.has_value() ? "Yes" : "No"));
    const int* ptr = MSTL::any_cast<int>(&a2);
    if (ptr) {
        println("Value: ", *ptr);
    }

    MSTL::any a3(a2);
    println("\nTesting copy constructor:");
    println(a3);
    println("Has value: ", (a3.has_value() ? "Yes" : "No"));
    ptr = MSTL::any_cast<int>(&a3);
    if (ptr) {
        println("Value: ", *ptr);
    }

    MSTL::any a4(MSTL::any(123));
    println("\nTesting move constructor:");
    println(a4);
    println("Has value: ", (a4.has_value() ? "Yes" : "No"));
    ptr = MSTL::any_cast<int>(&a4);
    if (ptr) {
        println("Value: ", *ptr);
    }

    a1 = a4;
    println("\nTesting assignment operator:");
    println(a1);
    println("Has value: ", (a1.has_value() ? "Yes" : "No"));
    ptr = MSTL::any_cast<int>(&a1);
    if (ptr) {
        println("Value: ", *ptr);
    }

    string str = "Hello, World!";
    string result = a1.emplace<string>(str);
    println("\nTesting emplace method:");
    println(a1);
    println("Has value: ", (a1.has_value() ? "Yes" : "No"));
    const string* strPtr = MSTL::any_cast<string>(&a1);
    if (strPtr) {
        println("Value: ", *strPtr);
    }

    a1.reset();
    println("\nTesting reset method:");
    println(a1);
    println("Has value: ", (a1.has_value() ? "Yes" : "No"));

    a1 = 10;
    a2 = 20;
    println("\nBefore swap:");
    println("a1: ", a1);
    println("a2: ", a2);
    a1.swap(a2);
    println("After swap:");
    println("a1: ", a1);
    println("a2: ", a2);
}

void test_timer(){
    timer t;

    auto node1 = t.add(1000, [](const timer_node& node) {
        println("定时器1（ID: ", node.id(), "）执行，到期时间: ", node.expire());
    });
    println("已添加定时器1，ID: ", node1.id());

    auto node2 = t.add(2000, [](const timer_node& node) {
        println("定时器2（ID: ", node.id(), "）执行，到期时间: ", node.expire());
    });
    println("已添加定时器2，ID: ", node2.id());

    auto node3 = t.add(1500, [](const timer_node& node) {
        println("定时器3（ID: ", node.id(), "）执行（此信息不应输出）");
    });
    println("已添加定时器3，ID: ", node3.id() , "，准备移除...");
    bool is_erased = t.erase(node3);
    if (is_erased) {
        println("定时器3移除成功");
    }
    while (true) {
        int64_t sleep_us = t.sleep();
        if (sleep_us == -1) {
            break;
        }
        if (sleep_us > 0) {
            println("距离下一个定时器到期还有 ", sleep_us, " 微秒，休眠中...");
            std::this_thread::sleep_for(std::chrono::microseconds(sleep_us));
        }
        while (t.check()) {}
    }
}


void test_dbpool() {
#ifdef MSTL_SUPPORT_MYSQL__
    std::clock_t begin = clock();
    database_settings::dbname("book");
    database_settings::password("147258hu");

    database_pool& pool = get_instance_database_pool();
    for (int i = 0; i < 5000; i++) {
        bool fin = pool.get_connect()->update("SELECT 1");
        // print(fin, " ");
    }
    println(1.0 * (clock() - begin) / CLOCKS_PER_SEC);

    auto result = pool.get_connect()->query("SELECT * FROM book");
    while (result.next()) {
        for (int i = 0; i < result.column_count(); i++) {
            if (i == 2) {
                int count = result.at_int16(i);
                println("collected :", count, ", ");
            }
            else if (i == 3) {
                float count = result.at_float32(i);
                println("usable :", count, ", ");
            }
            else if (i == 5) {
                _MSTL datetime dt = result.at_datetime(i);
                println("date: ", dt, ", ");
            }
            else
                println(result.at(i), ", ");
        }
        println();
    }
    println(result.row_count(), ", ", result.column_count());

    split_line();

    begin = clock();
    for (int i = 0; i < 5000; i++) {
        char sql[power(2, 10)] = {};
        sprintf(sql, "SELECT 1");
        auto* conn = new database_connect();
        if(conn->connect_to()) {
            bool fin = conn->update(sql);
            // print(fin, " ");
        }
        delete conn;
    }
    println(1.0 * (clock() - begin) / CLOCKS_PER_SEC);
#endif
}

void test_dns() {
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

void test_tpool() {
    thread_pool& pool = get_instance_thread_pool();
    pool.start();
    pool.submit_task(test_list);
    pool.submit_task(test_deque);
    pool.submit_task(test_hash);
    pool.submit_task(test_vector);
    pool.stop();
    pool.set_mode(THREAD_POOL_MODE::MODE_CACHED);
    pool.start();
    pool.submit_task(test_math);
    pool.submit_task(test_timer);
    pool.submit_task(test_string);
    pool.submit_task(test_tuple);
    pool.submit_task(test_rbtree);
    pool.submit_task(test_variant);
    pool.submit_task(test_option);
    pool.submit_task(test_check);
    pool.submit_task(test_any);
    pool.submit_task(test_datetimes);
    pool.submit_task(test_rnd);
    pool.submit_task(test_print);
    pool.submit_task(test_file);
    // pool.submit_task(try_db);
    pool.stop();
}
