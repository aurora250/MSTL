#ifndef MSTL_serlvet_HPP__
#define MSTL_serlvet_HPP__
#include "basiclib.hpp"
#ifdef MSTL_PLATFORM_LINUX__
#include "stringstream.hpp"
#include <sys/socket.h>
#include <netinet/in.h>

MSTL_ERROR_BUILD_FINAL_CLASS(HttpError, LinkError, "Http Actions Failed");

enum class HTTP_METHOD {
    GET, POST, PUT, DELETE, PATCH, OPTIONS, HEAD
};

struct http_request {
    HTTP_METHOD method;
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


class serlvet {
public:
    serlvet(uint16_t port, int backlog = 5)
        : port_(port), backlog_(backlog), running_(false), server_socket_(-1) {
        _MSTL memory_set(&server_addr_, 0, sizeof(server_addr_));
    }
    virtual ~serlvet() {
        stop();
    }

    bool start() {
    server_socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        Exception(HttpError("Failed to create socket"));
        return false;
    }
    
    int opt = 1;
    if (::setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Exception(HttpError("Failed to set socket options"));
        ::close(server_socket_);
        return false;
    }
    
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = INADDR_ANY;
    server_addr_.sin_port = htons(port_);

    if (std::bind(server_socket_, (struct sockaddr*)&server_addr_, sizeof(server_addr_)) < 0) {
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
    std::thread(&serlvet::acceptConnections, this).detach();
    return true;
}
    
    void stop() {
    if (running_) {
        ::close(server_socket_);
        server_socket_ = -1;
        running_ = false;
    }
}
    
    virtual http_response handleRequest(const http_request& request) = 0;
    
protected:
    http_request parseRequest(int client_socket)  {
    http_request request;
    char buffer[1024] = {0};
    
    ssize_t bytes_read = ::read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        return request;
    }
    
    string request_str(buffer, bytes_read);
    std::istringstream iss(request_str);
    string line;
    
    if (std::getline(iss, line)) {
        std::istringstream line_ss(line);
        line_ss >> request.method >> request.path >> request.version;
    }
    
    // 处理HTTP头和Content-Length等
    
    return request;
}
    
    string buildResponseString(const http_response& response) {
    stringstream ss;
    
    ss << response.version << " " << response.status_code << " " << response.status_msg << "\r\n";
    ss << "Content-Type: " << response.content_type << "\r\n";
    ss << "Content-Length: " << response.body.size() << "\r\n";
    ss << "Connection: close\r\n";
    ss << "\r\n";

    ss << response.body;
    
    return ss.str();
}
    
private:
    int server_socket_;
    uint16_t port_;
    int backlog_;
    bool running_;
    struct sockaddr_in server_addr_;
    
    void acceptConnections() {
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = ::accept(server_socket_, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            if (running_) {
                Exception(HttpError("Failed to accept connection"));
            }
            continue;
        }
        
        http_request request = parseRequest(client_socket);
        http_response response = handleRequest(request);
        string response_str = buildResponseString(response);
        ::send(client_socket, response_str.c_str(), response_str.size(), 0);
        
        ::close(client_socket);
    }
}
};

#endif
#endif // MSTL_serlvet_HPP__
