#ifndef MSTL_serlvet_HPP__
#define MSTL_serlvet_HPP__
#include "basiclib.hpp"
#ifdef MSTL_PLATFORM_LINUX__
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include "errorlib.hpp"
#include "stringstream.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_FINAL_CLASS(HttpError, LinkError, "Http Actions Failed");

enum class HTTP_METHOD {
    GET, POST, PUT, DELETE, PATCH, OPTIONS, HEAD
};

struct http_request {
    string method;
    string path;
    string version;
    string body;
};

struct http_response {
    string version;
    int status_code;
    string status_msg;
    string body;
    string content_type;
};


class servlet {
private:
    int server_socket_;
    uint16_t port_;
    int backlog_;
    bool running_;
    ::sockaddr_in server_addr_{};

    void accept_conns() {
        while (running_) {
            ::sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);

            int client_socket = ::accept(server_socket_, reinterpret_cast<struct sockaddr *>(&client_addr), &client_len);
            if (client_socket < 0) {
                if (running_) {
                    Exception(HttpError("Failed to accept connection"));
                }
                continue;
            }

            http_request request = parse_request(client_socket);
            http_response response = handle_request(request);
            string response_str = build_response_str(response);

            const char* ptr = response_str.c_str();
            size_t total = response_str.size();
            ssize_t sent = 0;

            while (total > 0) {
                sent = ::send(client_socket, ptr, total, 0);
                if (sent <= 0) {
                    if (running_) {
                        perror("send failed");
                    }
                    break;
                }
                total -= sent;
                ptr += sent;
            }
            ::close(client_socket);
        }
    }

public:
    explicit servlet(const uint16_t port, const int backlog = 5)
        : server_socket_(-1), port_(port), backlog_(backlog), running_(false) {
        _MSTL memory_set(&server_addr_, 0, sizeof(server_addr_));
    }

    virtual ~servlet() {
        stop();
    }


    bool start() {
        server_socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket_ < 0) {
            Exception(HttpError("Failed to create socket"));
            return false;
        }

        constexpr int opt = 1;
        if (::setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            Exception(HttpError("Failed to set socket options"));
            close(server_socket_);
            return false;
        }

        server_addr_.sin_family = AF_INET;
        server_addr_.sin_addr.s_addr = INADDR_ANY;
        server_addr_.sin_port = htons(port_);

        if (::bind(server_socket_, reinterpret_cast<sockaddr*>(&server_addr_), sizeof(server_addr_)) < 0) {
            Exception(HttpError("Failed to bind socket"));
            ::close(server_socket_);
           return false;
        }

        if (::listen(server_socket_, backlog_) < 0) {
            Exception(HttpError("Failed to listen on socket"));
            ::close(server_socket_);
            return false;
        }

        running_ = true;
        std::thread(&servlet::accept_conns, this).detach();
        return true;
    }

    void stop() {
        if (running_) {
            ::close(server_socket_);
            server_socket_ = -1;
            running_ = false;
        }
    }

    virtual http_response handle_request(const http_request& request) = 0;

protected:
    static http_request parse_request(const int client_socket)  {
        http_request request{};
        char buffer[1024] = {};

        const ssize_t bytes_read = ::read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            return request;
        }

        string request_str(buffer, bytes_read);
        istringstream iss(request_str);
        string line;

        if (iss.getline(line)) {
            istringstream line_ss(line);
            line_ss >> request.method >> request.path >> request.version;
        }

        std::cout << "Full Request:\n" << string(buffer, bytes_read) << "\n---END REQUEST---\n";
        return request;
    }

    static string build_response_str(const http_response& response) {
        ostringstream ss;

        ss << response.version << " " << response.status_code << " " << response.status_msg << "\r\n";
        ss << "Content-Type: " << response.content_type << "\r\n";
        ss << "Content-Length: " << response.body.size() << "\r\n";
        ss << "Connection: close\r\n";
        ss << "\r\n";

        ss << response.body;

        return ss.str();
    }
};

MSTL_END_NAMESPACE__
#endif
#endif // MSTL_serlvet_HPP__
