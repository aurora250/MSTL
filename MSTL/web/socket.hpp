#ifndef MSTL_SOCKET_HPP__
#define MSTL_SOCKET_HPP__
#include "../basiclib.hpp"
#ifdef MSTL_PLATFORM_LINUX__
#include <sys/socket.h>
#include "../errorlib.hpp"
MSTL_BEGIN_NAMESPACE__

enum class SOCKET_DOMAIN : uint16_t {
    IPV4 = AF_INET, IPV6 = AF_INET6,
    UNIX = AF_UNIX, LOCAL = AF_LOCAL,
    NETLINK = AF_NETLINK, BLUETOOTH = AF_BLUETOOTH
};

enum class SOCKET_TYPE : uint16_t {
    STREAM = SOCK_STREAM, DATAGRAM = SOCK_DGRAM,
    RAW = SOCK_RAW, SEQPACKET = SOCK_SEQPACKET
};

enum class SOCKET_PROTOCOL : uint16_t {
    TCP = IPPROTO_TCP, UDP = IPPROTO_UDP,
    IPV4 = IPPROTO_IP, IPV6 = IPPROTO_IPV6,
    ICMP = IPPROTO_ICMP, ICMP6 = IPPROTO_ICMPV6,
    SCTP = IPPROTO_SCTP, AUTO = 0
};


class socket {
private:
    int sockfd_ = -1;

public:
    socket(
        const SOCKET_DOMAIN domain,
        const SOCKET_TYPE type = SOCKET_TYPE::STREAM,
        const SOCKET_PROTOCOL protocol = SOCKET_PROTOCOL::AUTO) {
        sockfd_ = ::socket(
            static_cast<uint16_t>(domain),
            static_cast<uint16_t>(type),
            static_cast<uint16_t>(protocol));
        if (sockfd_ < 0) {
            Exception(LinkError("Failed to create socket"));
        }
    }

    socket() = default;

    ~socket() {
        if (sockfd_ >= 0) {
            ::close(sockfd_);
        }
    }

    socket(const socket&) = delete;
    socket& operator=(const socket&) = delete;

    socket(socket&& other) noexcept : sockfd_(other.sockfd_) {
        other.sockfd_ = -1;
    }
    socket& operator =(socket&& other) noexcept {
        if (this != &other) {
            if (sockfd_ >= 0) {
                ::close(sockfd_);
            }
            sockfd_ = other.sockfd_;
            other.sockfd_ = -1;
        }
        return *this;
    }

    int get() const { return sockfd_; }
    bool is_valid() const { return sockfd_ >= 0; }
};

MSTL_END_NAMESPACE__
#endif
#endif // MSTL_SOCKET_HPP__
