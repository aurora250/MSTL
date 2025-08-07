#ifndef TRY_H
#define TRY_H
#include "MSTL/mstlc++.hpp"
USE_MSTL

void test_file();
void test_datetimes();
void test_print();
void test_rnd();

#ifdef MSTL_PLATFORM_LINUX__
class example_servlet final : public servlet {
public:
    using servlet::servlet;

   http_response handle_request(const http_request& req) override {
        http_response res;

        println("[", req.method, "]", req.path,
                "Query:", req.query,
                "Body size:", req.body.size());

        if (req.path == "/old-link") {
            return res.redirect("/new-link");
        }
        if (req.path == "/forward-me") {
            return req.forward("/forward-target");
        }
        if (req.path == "/forward-target") {
            res.status_code = 200;
            res.body = "Forwarded successfully!";
            return res;
        }

        if (req.path == "/detail.css" ||
            req.path == "/apple.jpg" ||
            req.path.ends_with(".css") ||
            req.path.ends_with(".jpg")) {

            file f("../resource" + req.path);
            if (f.exists()) {
                res.status_code = 200;

                if (req.path.ends_with(".css")) {
                    res.headers["Content-Type"] = "text/css";
                    res.body = f.read();
                } else if (req.path.ends_with(".jpg")) {
                    res.headers["Content-Type"] = "image/jpeg";
                    res.body = f.read_binary();
                }
            } else {
                res.status_code = 404;
                res.body = "Resource not found: " + req.path;
            }
            return res;
            }

        if (req.path == "/") {
            res.status_code = 200;
            res.status_msg = "OK";
            static file f("../resource/index.html");
            res.body = f.read();
        }
        else if (req.path == "/api/data") {
            res.status_code = 200;
            res.headers["Content-Type"] = "application/json";
            res.body = R"({"status":"success"})";
        }
        else if (req.path == "/detail") {
            res.status_code = 200;
            res.status_msg = "OK";
            static file f("../resource/detail.html");
            res.body = f.read();
        }
        else if (req.path == "/new-link") {
            res.status_code = 200;
            res.status_msg = "OK";
            println("redirect to index");
            static file f("../resource/index.html");
            res.body = f.read();
        }
        else {
            res.status_code = 404;
            static file f("../resource/404err.html");
            res.body = f.read();
        }

        return res;
    }
};

void test_serv();
#endif

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
