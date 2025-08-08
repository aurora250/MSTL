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
    example_servlet(const int id) : servlet(id) {
        auto cors_filt = new cors_filter();
        auto log_filt = new logging_filter();
        add_filter(cors_filt);
        add_filter(log_filt);
    }

    void do_get(http_request& request, http_response& response) override {
        do_post(request, response);
    }

    void do_post(http_request &request, http_response &response) override {
        if (request.path == "/old-link") {
            response.redirect("/new-link");
            return;
        }
        if (request.path == "/forward-me") {
            response.forward("/forward-target");
            return;
        }
        if (request.path == "/forward-target") {
            response.status_code = 200;
            response.body = "Forward Successfully";
            return;
        }

        if (request.path.ends_with(".css") || request.path.ends_with(".jpg")) {
            file f("../resource" + request.path);
            if (f.exists()) {
                response.status_code = 200;

                if (request.path.ends_with(".css")) {
                    response.headers["Content-Type"] = "text/css";
                    response.body = f.read();
                } else if (request.path.ends_with(".jpg")) {
                    response.headers["Content-Type"] = "image/jpeg";
                    response.body = f.read_binary();
                }
                return;
            } else {
                response.status_code = 404;
                response.body = "Resource not found: " + request.path;
                return;
            }
        }

        if (request.path == "/api/session") {
            handle_session_api(request, response);
            return;
        }
        if (request.path == "/api/session-attribute") {
            handle_session_attribute(request, response);
            return;
        }
        if (request.path == "/api/cookie") {
            handle_cookie_api(request, response);
            return;
        }
        if (request.path == "/api/logger-test") {
            response.status_code = 200;
            response.body = "Logging filter test successful";
            return;
        }
        if (request.path == "/api/data") {
            response.status_code = 200;
            response.headers["Content-Type"] = "application/json";
            response.body = R"({"status":"success"})";
            return;
        }

        if (request.path == "/") {
            response.status_code = 200;
            response.status_msg = "OK";
            static file f("../resource/index.html");
            response.body = f.read();
        } else if (request.path == "/detail") {
            response.status_code = 200;
            response.status_msg = "OK";
            static file f("../resource/detail.html");
            response.body = f.read();
        } else if (request.path == "/new-link") {
            response.status_code = 200;
            response.status_msg = "OK";
            println("redirect to index");
            static file f("../resource/index.html");
            response.body = f.read();
        } else if (request.path == "/test") {
            response.status_code = 200;
            response.status_msg = "OK";
            static file f("../resource/test.html");
            response.body = f.read();
        } else {
            response.status_code = 404;
            static file f("../resource/404err.html");
            response.body = f.read();
        }
    }

    private:
    void handle_session_api(http_request& request, http_response& response) {
        session* session = get_session(request, false);
        string action = request.get_parameter("action");

        if (action == "create") {
            session = get_session(request, true);

            bool is_https = false;
            auto proto_header = request.get_header("X-Forwarded-Proto");
            if (proto_header == "https") {
                is_https = true;
            }

            if (session && session->is_new) {
                cookie session_cookie(get_session_cookie_name(), session->id);
                session_cookie.path = "/";
                session_cookie.http_only = true;
                session_cookie.secure = is_https;

                if (!is_https) {
                    session_cookie.same_site = "Lax";
                } else {
                    session_cookie.same_site = "Strict";
                }

                response.add_cookie(session_cookie);
                session->is_new = false;
            }

            response.status_code = 200;
            response.headers["Content-Type"] = "application/json";
            response.body = R"({"sessionId":")" + session->id + R"("})";
        }
        else if (action == "invalidate" && session) {
            session->invalidate();
            response.status_code = 200;
            response.body = R"({"message":"Session invalidated"})";
        }
        else if (action == "info" && session) {
            ostringstream json;
            json << R"({"sessionId":")" << session->id << R"(",)";
            json << R"("createTime":)" << datetime::to_utc(session->create_time).to_string() << ",";
            json << R"("lastAccess":)" << datetime::to_utc(session->last_access).to_string() << ",";
            json << R"("attributes":{)";

            bool first = true;
            for (const auto& [key, value] : session->data) {
                if (!first) json << ",";
                json << R"(")" << key << R"(":")" << value << R"(")";
                first = false;
            }
            json << "}}";

            response.status_code = 200;
            response.headers["Content-Type"] = "application/json";
            response.body = json.str();
        }
        else {
            response.status_code = 400;
            response.body = R"({"error":"Invalid session action"})";
        }
    }

    void handle_session_attribute(http_request& request, http_response& response) {
        string attrName, attrValue;
        string content_type = request.get_header("Content-Type");
        if (content_type.find("application/json") == 0) {
            json_parser parser(request.body);
            unique_ptr<json_value> root = parser.parse();
            println(*root.get());
            if (root->is_object()) {

            }
            attrName = request.get_parameter("name");
        } else {
            attrName = request.get_parameter("attrName");
            attrValue = request.get_parameter("attrValue");
        }

        session* session = get_session(request, true);

        if (!attrName.empty()) {
            (*session)[attrName] = attrValue;
            response.status_code = 200;
            response.headers["Content-Type"] = "application/json";
            response.body = R"({"attrName":")" + attrName + R"(", "attrValue":")" + attrValue + R"("})";
        } else {
            response.status_code = 400;
            response.body = R"({"error":"Missing attribute name"})";
        }
    }

    void handle_cookie_api(http_request& request, http_response& response) {
        if (request.method == "POST") {
            string name = request.get_parameter("name");
            string value = request.get_parameter("value");
            string maxAgeStr = request.get_parameter("maxAge");

            if (!name.empty()) {
                cookie cookie(name, value);
                if (!maxAgeStr.empty()) {
                    cookie.max_age = _MSTL to_int32(maxAgeStr.c_str());
                }
                response.add_cookie(cookie);

                response.status_code = 200;
                response.body = R"({"name":")" + name + R"(", "value":")" + value + R"("})";
            } else {
                response.status_code = 400;
                response.body = R"({"error":"Missing cookie name"})";
            }
        }
        else if (request.method == "DELETE") {
            string name = request.get_parameter("name");
            if (!name.empty()) {
                cookie cookie(name, "");
                cookie.max_age = 0; // 立即过期
                response.add_cookie(cookie);

                response.status_code = 200;
                response.body = R"({"name":")" + name + R"("})";
            } else {
                response.status_code = 400;
                response.body = R"({"error":"Missing cookie name"})";
            }
        }
    }
};

void test_serv();
#endif

void test_list();
void test_exce();
void test_json();

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
