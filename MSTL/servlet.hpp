#ifndef MSTL_SERVLET_HPP__
#define MSTL_SERVLET_HPP__
#include "basiclib.hpp"
#ifdef MSTL_PLATFORM_LINUX__
#include <atomic>
#include <mutex>
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "errorlib.hpp"
#include "stringstream.hpp"
#include "unordered_map.hpp"
#include "vector.hpp"
#include "hexadecimal.hpp"
#include "json.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_FINAL_CLASS(HttpError, LinkError, "Http Actions Failed");


class cookie {
public:
    string name;
    string value;
    string domain;
    string path = "/";
    int max_age = -1;  // -1 means session cookie
    bool secure = false;
    bool http_only = false;
    string same_site; // "Strict", "Lax", "None"
    datetime expires;

    cookie() = default;
    cookie(string name, string value) : name(_MSTL move(name)), value(_MSTL move(value)) {}

    string to_string() const {
        ostringstream ss;
        ss << name << "=" << value;

        if (!domain.empty()) ss << "; Domain=" << domain;
        if (!path.empty()) ss << "; Path=" << path;

        if (max_age >= 0) {
            ss << "; Max-Age=" << max_age;
        }

        if (expires > datetime::epoch()) {
            ss << "; Expires=" << expires.to_gmt();
        }

        if (secure) ss << "; Secure";
        if (http_only) ss << "; HttpOnly";
        if (!same_site.empty()) ss << "; SameSite=" << same_site;

        return ss.str();
    }
};


class session {
public:
    string id;
    unordered_map<string, string> data;
    datetime last_access;
    datetime create_time;
    int max_age = 1800;  // 30 minutes default
    bool is_new = true;
    bool invalidated = false;

    explicit session(string session_id)
        : id(_MSTL move(session_id)),
          last_access(datetime::now()),
          create_time(last_access) {}

    string& operator[](const string& key) {
        last_access = datetime::now();
        return data[key];
    }

    bool contains(const string& key) const {
        return data.find(key) != data.end();
    }

    void remove_attribute(const string& key) {
        data.erase(key);
        last_access = datetime::now();
    }

    void invalidate() {
        invalidated = true;
        data.clear();
    }

    bool is_valid() const {
        return !invalidated && !expired();
    }

    bool expired(int max_inactive = 0) const {
        if (max_inactive <= 0) max_inactive = max_age;
        const datetime now = datetime::now();
        const int64_t diff = now - last_access;
        return diff > max_inactive;
    }

    void set_max_inactive_interval(int seconds) {
        max_age = seconds;
    }

    int get_max_inactive_interval() const {
        return max_age;
    }
};


class __session_manager {
private:
    unordered_map<string, session> sessions_;
    std::mutex mutex_;
    std::atomic<bool> cleanup_running_{false};
    std::thread cleanup_thread_;

    static string generate_session_id() {
        ostringstream ss;
        for (int i = 0; i < 32; ++i) {
            ss << hexadecimal(random_mt::next_int(0, 15)).to_std_string();
        }
        return ss.str();
    }

    void cleanup_expired_sessions() {
        while (cleanup_running_) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                datetime now = datetime::now();
                auto it = sessions_.begin();
                while (it != sessions_.end()) {
                    const int64_t diff = now - it->second.last_access;
                    if (!it->second.is_valid() ||
                        diff > it->second.get_max_inactive_interval()) {
                        it = sessions_.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::minutes(5));
        }
    }


public:
    __session_manager() {
        cleanup_running_ = true;
        cleanup_thread_ = std::thread(&__session_manager::cleanup_expired_sessions, this);
    }

    ~__session_manager() {
        cleanup_running_ = false;
        if (cleanup_thread_.joinable()) {
            cleanup_thread_.join();
        }
    }

    session* get_session(const string& session_id, bool create = true) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = sessions_.find(session_id);
        if (it != sessions_.end()) {
            if (it->second.is_valid()) {
                it->second.is_new = false;
                return &it->second;
            } else {
                sessions_.erase(it);
            }
        }

        if (create) {
            string new_id = session_id.empty() ? generate_session_id() : session_id;
            auto [new_it, inserted] = sessions_.emplace(new_id, session(new_id));
            return &new_it->second;
        }

        return nullptr;
    }

    session* create_session() {
        return get_session("", true);
    }

    void remove_session(const string& session_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        sessions_.erase(session_id);
    }

    bool session_exists(const string& session_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = sessions_.find(session_id);
        return it != sessions_.end() && it->second.is_valid();
    }
};

// Request 结构体增强
struct http_request {
    string method = "GET";
    string path = "/";
    string version = "HTTP/1.1";
    unordered_map<string, string> headers;
    unordered_map<string, string> cookies;
    unordered_map<string, string> parameters; // query + body parameters
    string query{};
    string body{};
    session* session = nullptr;

    http_request() = default;
    http_request(const http_request&) = delete;
    http_request& operator=(const http_request&) = delete;
    http_request(http_request&& req) noexcept = default;
    http_request& operator=(http_request&& req) noexcept = default;
    ~http_request() = default;

    string get_parameter(const string& name) const {
        auto it = parameters.find(name);
        return it != parameters.end() ? it->second : string{};
    }

    string get_cookie(const string& name) const {
        auto it = cookies.find(name);
        return it != cookies.end() ? it->second : string{};
    }

    string get_header(const string& name) const {
        auto it = headers.find(name);
        return it != headers.end() ? it->second : string{};
    }
};


struct http_response {
public:
    string version = "HTTP/1.1";
    int status_code = 404;
    string status_msg = "Not Found";
    unordered_map<string, string> headers;
    vector<cookie> cookies;
    string body{};
    string redirect_url{};
    string forward_path{};

    http_response() {
        headers["Content-Type"] = "text/html; charset=utf-8";
        headers["Connection"] = "close";
    }

    http_response(const http_response&) = delete;
    http_response& operator=(const http_response&) = delete;
    http_response(http_response&& res) noexcept = default;
    http_response& operator=(http_response&& res) noexcept = default;

    void add_cookie(const cookie& cookie) {
        cookies.push_back(cookie);
    }

    void add_cookie(const string& name, const string& value) {
        cookies.emplace_back(name, value);
    }

    void set_header(const string& name, const string& value) {
        headers[name] = value;
    }

    void redirect(string url) {
        redirect_url = _MSTL move(url);
    }

    void forward(string url) {
        forward_path = _MSTL move(url);
    }
};


class filter {
public:
    virtual ~filter() = default;
    virtual void do_filter(http_request& request, http_response& response) = 0;
    virtual bool pre_filter(http_request& request, http_response& response) { return true; }
    virtual void post_filter(http_request& request, http_response& response) {}
};

class filter_chain {
private:
    vector<filter*> filters_;
    size_t current_index_ = 0;

public:
    void add_filter(filter* filter) {
        filters_.push_back(filter);
    }

    bool execute_pre_filters(http_request& request, http_response& response) {
        for (auto* filter : filters_) {
            if (!filter->pre_filter(request, response)) {
                return false;
            }
        }
        return true;
    }

    void execute_post_filters(http_request& request, http_response& response) {
        for (auto it = filters_.rbegin(); it != filters_.rend(); ++it) {
            (*it)->post_filter(request, response);
        }
    }

    void execute_filters(http_request& request, http_response& response) {
        for (auto* filter : filters_) {
            filter->do_filter(request, response);
        }
    }
};


class cors_filter : public filter {
private:
    string allowed_origins_ = "*";
    string allowed_methods_ = "GET, POST, PUT, DELETE, OPTIONS";
    string allowed_headers_ = "Content-Type, Authorization";

public:
    void set_allowed_origins(const string& origins) { allowed_origins_ = origins; }
    void set_allowed_methods(const string& methods) { allowed_methods_ = methods; }
    void set_allowed_headers(const string& headers) { allowed_headers_ = headers; }

    void do_filter(http_request& request, http_response& response) override {
        response.set_header("Access-Control-Allow-Origin", allowed_origins_);
        response.set_header("Access-Control-Allow-Methods", allowed_methods_);
        response.set_header("Access-Control-Allow-Headers", allowed_headers_);
        response.set_header("Access-Control-Allow-Credentials", "true");

        if (request.method == "OPTIONS") {
            response.status_code = 200;
            response.status_msg = "OK";
            response.body = "";
        }
    }
};

class logging_filter : public filter {
public:
    bool pre_filter(http_request& request, http_response& response) override {
        println("[", datetime::now(), "] Request: ", request.method, " ", request.path);
        return true;
    }

    void post_filter(http_request& request, http_response& response) override {
        println("[", datetime::now(), "] Response: ", response.status_code, " ", response.status_msg);
    }

    void do_filter(http_request& request, http_response& response) override {}
};


class servlet {
private:
    int server_socket_ = -1;
    uint16_t port_;
    int backlog_;
    std::atomic<bool> running_{false};
    ::sockaddr_in server_addr_{};
    vector<std::thread> worker_threads_;
    __session_manager session_manager_;
    filter_chain filter_chain_;
    string session_cookie_name_ = "JSESSIONID";

private:
    void start_workers(const int thread_count) {
        for (int i = 0; i < thread_count; ++i) {
            worker_threads_.emplace_back(&servlet::accept_conns, this);
        }
    }

    void accept_conns() {
        while (running_) {
            ::sockaddr_in client_addr{};
            ::socklen_t client_len = sizeof(client_addr);

            const int client_socket = ::accept(server_socket_,
                reinterpret_cast<::sockaddr *>(&client_addr), &client_len);
            if (client_socket < 0) {
                if (running_) {
                    perror("accept failed");
                }
                continue;
            }

            try {
                handle_client(client_socket);
            } catch (const Error& e) {
                perror(e.what());
            }
            ::close(client_socket);
        }
    }

    void handle_client(const int client_socket) {
        http_request request = parse_request(client_socket);
        int forward_count = 0;
        constexpr int MAX_FORWARD = 5;

        do {
            http_response response = handle_request(request);
            if (!response.forward_path.empty() && forward_count < MAX_FORWARD) {
                request.path = response.forward_path;
                forward_count++;
                continue;
            }
            send_response(client_socket, response);
            return;
        } while (true);
    }

    static void parse_cookies(const string &cookie_header, http_request &request) {
        if (cookie_header.empty())
            return;

        vector<string> cookie_pairs;
        size_t start = 0;
        size_t end = cookie_header.find(';');

        while (end != string::npos) {
            cookie_pairs.push_back(cookie_header.substr(start, end - start).trim());
            start = end + 1;
            end = cookie_header.find(';', start);
        }
        cookie_pairs.push_back(cookie_header.substr(start).trim());

        for (const auto &pair: cookie_pairs) {
            const size_t eq_pos = pair.find('=');
            if (eq_pos != string::npos) {
                string name = pair.substr(0, eq_pos).trim();
                string value = pair.substr(eq_pos + 1).trim();
                request.cookies[name] = value;
            }
        }
    }

    static void parse_parameters(http_request &request) {
        if (!request.query.empty()) {
            parse_url_encoded(request.query, request.parameters);
        }
        if (request.method == "POST" && !request.body.empty()) {
            string content_type = request.get_header("Content-Type");
            if (content_type.find("application/x-www-form-urlencoded") == 0) {
                parse_url_encoded(request.body, request.parameters);
            }
        }
    }

    static void parse_url_encoded(const string &data, unordered_map<string, string> &params) {
        if (data.empty())
            return;

        vector<string> pairs;
        size_t start = 0;
        size_t end = data.find('&');

        while (end != string::npos) {
            pairs.push_back(data.substr(start, end - start));
            start = end + 1;
            end = data.find('&', start);
        }
        pairs.push_back(data.substr(start));

        for (const auto &pair: pairs) {
            const size_t eq_pos = pair.find('=');
            if (eq_pos != string::npos) {
                string name = url_decode(pair.substr(0, eq_pos));
                string value = url_decode(pair.substr(eq_pos + 1));
                params[name] = value;
            }
        }
    }

    static string url_decode(const string& str) {
        string result;
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '%' && i + 2 < str.length()) {
                try {
                    hexadecimal hex_val(str.substr(i + 1, 2));
                    result += static_cast<char>(hex_val.to_decimal());
                } catch (const Error&) {
                    result += str[i];
                }
            } else if (str[i] == '+') {
                result += ' ';
            } else {
                result += str[i];
            }
        }
        return result;
    }

protected:
    virtual http_request parse_request(const int client_socket) {
        http_request req;
        char buffer[4096];
        string request_data;

        ssize_t total_read = 0;
        while (true) {
            ssize_t bytes_read = ::read(client_socket, buffer, sizeof(buffer)-1);
            if (bytes_read <= 0) break;

            buffer[bytes_read] = '\0';
            request_data.append(buffer, bytes_read);
            total_read += bytes_read;

            const size_t header_end = request_data.find("\r\n\r\n");
            if (header_end != string::npos) {
                size_t content_length = 0;
                const size_t content_start = header_end + 4;

                const size_t cl_pos = request_data.find("Content-Length:");
                if (cl_pos != string::npos) {
                    const size_t cl_end = request_data.find("\r\n", cl_pos);
                    if (cl_end != string::npos) {
                        string cl_str = request_data.substr(
                            cl_pos + 15, cl_end - cl_pos - 15
                            ).trim();
                        content_length = _MSTL to_uint32(cl_str.c_str());
                    }
                }

                const auto body_read = static_cast<ssize_t>(total_read - content_start);
                if (body_read < static_cast<ssize_t>(content_length)) {
                    ssize_t remaining = static_cast<ssize_t>(content_length - body_read);
                    while (remaining > 0) {
                        bytes_read = ::read(client_socket, buffer,
                            _MSTL min(static_cast<ssize_t>(sizeof(buffer)), remaining));
                        if (bytes_read <= 0) break;
                        request_data.append(buffer, bytes_read);
                        remaining -= bytes_read;
                    }
                }
                break;
            }

            if (total_read > 1024 * 16) break;
        }

        istringstream iss(request_data);
        string line;
        if (iss.getline(line)) {
            line.trim();
            const size_t pos1 = line.find(' ');
            if (pos1 != string::npos) {
                req.method = line.substr(0, pos1);
                const size_t pos2 = line.find(' ', pos1 + 1);
                if (pos2 != string::npos) {
                    req.path = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    req.version = line.substr(pos2 + 1);
                }
            }
        }

        // url query
        const size_t query_pos = req.path.find('?');
        if (query_pos != string::npos) {
            req.query = req.path.substr(query_pos+1);
            req.path = req.path.substr(0, query_pos);
        }

        // request header
        while (iss.getline(line)) {
            line.trim();
            if (line.empty()) break;

            const size_t colon_pos = line.find(':');
            if (colon_pos != string::npos) {
                string key = line.substr(0, colon_pos).trim();
                string value = line.substr(colon_pos+1).trim();
                req.headers[key] = _MSTL move(value);
            }
        }

        // request body
        const size_t body_start = request_data.find("\r \r");
        if (body_start != string::npos && body_start + 4 < request_data.size()) {
            req.body = request_data.substr(body_start + 4);
        }

        // Parse cookies
        auto cookie_it = req.headers.find("Cookie");
        if (cookie_it != req.headers.end()) {
            parse_cookies(cookie_it->second, req);
        }

        // Parse parameters
        parse_parameters(req);

        // Handle session
        string session_id = req.get_cookie(session_cookie_name_);
        if (!session_id.empty() && session_manager_.session_exists(session_id)) {
            req.session = session_manager_.get_session(session_id, false);
        }

        return req;
    }

    virtual string build_response_str(const http_response& response) {
        ostringstream ss;

        if (!response.redirect_url.empty()) {
            ss << response.version << " 302 Found\r\n";
            ss << "Location: " << response.redirect_url << "\r\n";
        } else {
            ss << response.version << " "
               << response.status_code << " "
               << response.status_msg << "\r\n";
        }

        for (const auto& cookie : response.cookies) {
            ss << "Set-Cookie: " << cookie.to_string() << "\r\n";
        }
        if (response.redirect_url.empty() &&
            response.headers.find("Content-Length") == response.headers.end()) {
            ss << "Content-Length: " << response.body.size() << "\r\n";
        }
        for (const auto& [key, value] : response.headers) {
            ss << key << ": " << value << "\r\n";
        }
        ss << "\r\n";

        if (!response.redirect_url.empty()) {
        } else {
            ss << response.body;
        }
        return ss.str();
    }

    void send_response(const int client_socket, const http_response& response) {
        string response_str = build_response_str(response);
        const char *data = response_str.data();
        const size_t total = response_str.size();
        size_t sent = 0;

        while (sent < total) {
            const ssize_t bytes_sent = ::send(client_socket, data + sent, total - sent, 0);
            if (bytes_sent <= 0) {
                perror("send failed");
                break;
            }
            sent += bytes_sent;
        }
    }

protected:
    void add_filter(filter* filter) {
        filter_chain_.add_filter(filter);
    }

    session* get_session(http_request& request, bool create = true) {
        if (request.session) return request.session;

        string session_id = request.get_cookie(session_cookie_name_);
        session* session = nullptr;

        if (!session_id.empty()) {
            session = session_manager_.get_session(session_id, false);
        }

        if (!session && create) {
            session = session_manager_.create_session();
        }

        request.session = session;
        return session;
    }

    void add_session_cookie(http_request& request, http_response& response, session* session) const {
        if (session && session->is_new) {
            cookie session_cookie(session_cookie_name_, session->id);
            session_cookie.path = "/";
            session_cookie.http_only = true;

            auto proto_header = request.get_header("X-Forwarded-Proto");
            const bool is_https = proto_header == "https";
            session_cookie.secure = is_https;
            session_cookie.same_site = is_https ? "Strict" : "Lax";

            response.add_cookie(session_cookie);
            session->is_new = false;
        }
    }

    virtual bool init() { return true; }
    virtual void destroy() {}

    virtual http_response handle_request(http_request& request) {
        http_response response;

        if (!filter_chain_.execute_pre_filters(request, response)) {
            return response;
        }

        session* session = get_session(request, true);
        if (session) {
            add_session_cookie(request, response, session);
        }
        filter_chain_.execute_filters(request, response);


        println(
            "[", request.method, "]",
            request.path, " Query:", request.query,
            " Body size:", request.body.size(),
            " Session:", (request.session ? request.session->id : "none"));

        if (request.method == "GET") {
            do_get(request, response);
        } else if (request.method == "POST") {
            do_post(request, response);
        } else if (request.method == "PUT") {
            do_put(request, response);
        } else if (request.method == "DELETE") {
            do_delete(request, response);
        } else if (request.method == "HEAD") {
            do_head(request, response);
        } else if (request.method == "OPTIONS") {
            do_options(request, response);
        } else if (request.method == "TRACE") {
            do_trace(request, response);
        } else if (request.method == "CONNECT") {
            do_connect(request, response);
        } else {
            response.status_code = 405;
            response.status_msg = "Method Not Allowed";
            response.body = "Method Not Allowed";
        }

        filter_chain_.execute_post_filters(request, response);
        return response;
    }

    virtual void do_get(http_request& request, http_response& response) = 0;
    virtual void do_post(http_request& request, http_response& response) = 0;

    virtual void do_put(http_request& request, http_response& response) {
        do_post(request, response);
    }
    virtual void do_delete(http_request& request, http_response& response) {
        do_post(request, response);
    }
    virtual void do_head(http_request& request, http_response& response) {
        do_get(request, response);
        response.body.clear(); // HEAD should not return body
    }
    virtual void do_options(http_request& request, http_response& response) {
        response.status_code = 200;
        response.status_msg = "OK";
        response.set_header("Allow", "GET, POST, PUT, DELETE, HEAD, OPTIONS");
    }
    virtual void do_trace(http_request& request, http_response& response) {
        response.status_code = 200;
        response.status_msg = "OK";
        response.set_header("Content-Type", "message/http");

        ostringstream ss;
        ss << request.method << " " << request.path;
        if (!request.query.empty()) ss << "?" << request.query;
        ss << " " << request.version << "\r";
        for (const auto& [key, value] : request.headers) {
            ss << key << ": " << value << "\r";
        }
        ss << "\r" << request.body;
        response.body = ss.str();
    }
    virtual void do_connect(http_request& request, http_response& response) {
        response.status_code = 405;
        response.status_msg = "Method Not Allowed";
        response.body = "CONNECT method not supported";
    }

public:
    explicit servlet(const uint16_t port, const int backlog = 128)
        : port_(port), backlog_(backlog) {
        _MSTL memory_set(&server_addr_, 0, sizeof(server_addr_));
    }

    virtual ~servlet() {
        this->stop();
    }

    bool start(const uint32_t thread_count = std::thread::hardware_concurrency()) {
        if (running_) return true;
        if (!init()) return false;

        server_socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket_ < 0) {
            perror("socket creation failed");
            return false;
        }

        constexpr int opt = 1;
        if (::setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            perror("setsockopt failed");
            ::close(server_socket_);
            return false;
        }

        server_addr_.sin_family = AF_INET;
        server_addr_.sin_addr.s_addr = INADDR_ANY;
        server_addr_.sin_port = htons(port_);

        if (::bind(server_socket_,
            reinterpret_cast<sockaddr*>(&server_addr_),
            sizeof(server_addr_))) {
            perror("bind failed");
            ::close(server_socket_);
            return false;
        }

        if (::listen(server_socket_, backlog_) < 0) {
            perror("listen failed");
            ::close(server_socket_);
            return false;
        }

        running_ = true;
        start_workers(static_cast<int32_t>(thread_count));
        println("Server started on port", port_, "with", worker_threads_.size(), "workers");
        return true;
    }

    void stop() {
        if (!running_) return;

        running_ = false;
        ::close(server_socket_);
        server_socket_ = -1;

        for (auto& t : worker_threads_) {
            if (t.joinable()) t.join();
        }
        worker_threads_.clear();
        destroy();
    }

    void set_session_cookie_name(const string& name) {
        session_cookie_name_ = name;
    }
    const string& get_session_cookie_name() const {
        return session_cookie_name_;
    }
};

MSTL_END_NAMESPACE__
#endif
#endif // MSTL_SERVLET_HPP__
