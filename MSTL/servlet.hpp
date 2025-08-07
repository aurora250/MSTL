#ifndef MSTL_SERVLET_HPP__
#define MSTL_SERVLET_HPP__
#include "basiclib.hpp"
#ifdef MSTL_PLATFORM_LINUX__
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include "errorlib.hpp"
#include "stringstream.hpp"
#include "unordered_map.hpp"
#include "map.hpp"
#include "vector.hpp"

MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_FINAL_CLASS(HttpError, LinkError, "Http Actions Failed");

struct http_response;

struct http_request {
    string method = "GET";
    string path = "/";
    string version = "HTTP/1.1";
    unordered_map<string, string> headers;
    string query{};
    string body{};

    http_request() = default;

    http_request(const http_request&) = delete;
    http_request& operator =(const http_request&) = delete;

    http_request(http_request&& req) noexcept
    : method(_MSTL move(req.method)), path(_MSTL move(req.path)),
    version(_MSTL move(req.version)), headers(_MSTL move(req.headers)),
    query(_MSTL move(req.query)), body(_MSTL move(req.body)) {}

    http_request& operator =(http_request&& req) noexcept {
        if (this == _MSTL addressof(req)) return *this;
        method = _MSTL move(req.method);
        path = _MSTL move(req.path);
        version = _MSTL move(req.version);
        headers = _MSTL move(req.headers);
        query = _MSTL move(req.query);
        body = _MSTL move(req.body);
        return *this;
    }

    ~http_request() = default;

    http_response forward(string url) const;
};

struct http_response {
public:
    string version = "HTTP/1.1";
    int status_code = 404;
    string status_msg = "Not Found";
    unordered_map<string, string> headers;
    string body{};
    string redirect_url{}; // return 302 and location automatically, raise new request
    string forward_path{}; // no new request

    http_response() {
        headers["Content-Type"] = "text/html";
        headers["Connection"] = "close";
    }

    http_response(const http_response&) = delete;
    http_response& operator =(const http_response&) = delete;

    http_response(http_response&& res) noexcept
    : version(_MSTL move(res.version)), status_code(res.status_code),
    status_msg(_MSTL move(res.status_msg)),
    headers(_MSTL move(res.headers)),
    body(_MSTL move(res.body)) {}

    http_response& operator =(http_response&& res) noexcept {
        if (this == _MSTL addressof(res)) return *this;
        version = _MSTL move(res.version);
        status_code = res.status_code;
        status_msg = _MSTL move(res.status_msg);
        headers = _MSTL move(res.headers);
        body = _MSTL move(res.body);
        return *this;
    }

    http_response redirect(string url) {
        http_response res = _MSTL move(*this);
        res.redirect_url = _MSTL move(url);
        return res;
    }
};

inline http_response http_request::forward(string url) const {
    http_response res;
    res.forward_path = _MSTL move(url);
    return res;
}


class servlet {
private:
    int server_socket_ = -1;
    uint16_t port_;
    int backlog_;
    std::atomic<bool> running_{false};
    ::sockaddr_in server_addr_{};
    vector<std::thread> worker_threads_;

    void start_workers(const int thread_count) {
        for (int i = 0; i < thread_count; ++i) {
            worker_threads_.emplace_back(&servlet::accept_conns, this);
        }
    }

    void accept_conns() {
        while (running_) {
            ::sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);

            const int client_socket = ::accept(server_socket_,
                reinterpret_cast<struct sockaddr *>(&client_addr), &client_len);
            if (client_socket < 0) {
                if (running_) {
                    perror("accept failed");
                }
                continue;
            }

            try {
                handle_client(client_socket);
            } catch (const Error& e) {
                println("Error handling client:", e.what());
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
            for (auto iter = response.headers.begin(); iter != response.headers.end(); ++iter) {
                std::cout << iter->first << ": " << iter->second << std::endl;
            }
            send_response(client_socket, response);
            return;
        } while (true);
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
            if (line.empty())
                break;

            const size_t colon_pos = line.find(':');
            if (colon_pos != string::npos) {
                string key = line.substr(0, colon_pos).trim();
                string value = line.substr(colon_pos+1).trim();
                req.headers[key] = _MSTL move(value);
            }
        }

        // request body
        const size_t body_start = request_data.find("\r\n\r\n");
        if (body_start != string::npos && body_start + 4 < request_data.size()) {
            req.body = request_data.substr(body_start + 4);
        }
        return req;
    }

    virtual string build_response_str(const http_response& response) {
        ostringstream ss;

        if (!response.redirect_url.empty()) {
            ss << response.version << " 302 Found\r\n";
            ss << "Location: " << response.redirect_url << "\r\n";
            ss << "Content-Length: 0\r\n\r\n";
            return ss.str();
        }

        ss << response.version << " "
           << response.status_code << " "
           << response.status_msg << "\r\n";

        if (response.headers.find("Content-Length") == response.headers.end()) {
            ss << "Content-Length: " << response.body.size() << "\r\n";
        }
        for (const auto& [key, value] : response.headers) {
            ss << key << ": " << value << "\r\n";
        }

        ss << "\r\n";
        ss << response.body;
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

public:
    explicit servlet(const uint16_t port, const int backlog = 128)
        : port_(port), backlog_(backlog) {
        memory_set(&server_addr_, 0, sizeof(server_addr_));
    }

    virtual ~servlet() {
        stop();
    }

    bool start(const uint32_t thread_count = std::thread::hardware_concurrency()) {
        if (running_) return true;

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
    }

    virtual http_response handle_request(const http_request& request) = 0;
};

MSTL_END_NAMESPACE__
#endif
#endif // MSTL_SERVLET_HPP__