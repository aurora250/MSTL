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
    explicit example_servlet(const uint16_t port) : servlet(port) {}

    http_response handle_request(const http_request& request) override {
        http_response response;
        response.version = "HTTP/1.1";

        std::cout << "Received " << request.method << " request for " << request.path << std::endl;

        if (request.path == "/favicon.ico") {
            response.status_code = 404;
            response.status_msg = "Not Found";
            response.body = "";
            response.content_type = "text/plain";
            return response;
        }

        if (request.path == "/") {
            response.status_code = 200;
            response.status_msg = "OK";
            response.content_type = "text/html";
            response.body = "<html><head><title>Example Servlet</title></head>"
                           "<body><h1>Welcome to Example Servlet</h1>"
                           "<p>This is a C++ servlet implementation</p>"
                           "</body></html>";
        } else if (request.path == "/hello") {
            response.status_code = 200;
            response.status_msg = "OK";
            response.content_type = "text/plain";
            response.body = "Hello, World!";
        } else {
            response.status_code = 404;
            response.status_msg = "Not Found";
            response.content_type = "text/html";
            response.body = "<html><head><title>Not Found</title></head>"
                           "<body><h1>404 Not Found</h1>"
                           "<p>The requested resource was not found</p>"
                           "</body></html>";
        }

        return response;
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
