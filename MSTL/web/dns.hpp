#ifndef MSTL_DNS_REQUEST_HPP__
#define MSTL_DNS_REQUEST_HPP__
#include "../basiclib.hpp"
#ifdef MSTL_PLATFORM_LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
MSTL_BEGIN_NAMESPACE__

constexpr auto DNS_SVR = "114.114.114.114";
constexpr int DNS_HOST = 1;
constexpr int DNS_CNAME = 5;

constexpr const char* DOMAINS[44] = {
    "bojing.wang",
    "www.baidu.com",
    "tieba.baidu.com",
    "news.baidu.com",
    "zhidao.baidu.com",
    "music.baidu.com",
    "image.baidu.com",
    "v.baidu.com",
    "map.baidu.com",
    "baijiahao.baidu.com",
    "xueshu.baidu.com",
    "cloud.baidu.com",
    "www.163.com",
    "open.163.com",
    "auto.163.com",
    "gov.163.com",
    "money.163.com",
    "sports.163.com",
    "tech.163.com",
    "edu.163.com",
    "www.taobao.com",
    "q.taobao.com",
    "sf.taobao.com",
    "yun.taobao.com",
    "baoxian.taobao.com",
    "www.tmall.com",
    "suning.tmall.com",
    "www.tencent.com",
    "www.qq.com",
    "www.aliyun.com",
    "www.ctrip.com",
    "hotels.ctrip.com",
    "hotels.ctrip.com",
    "vacations.ctrip.com",
    "flights.ctrip.com",
    "trains.ctrip.com",
    "bus.ctrip.com",
    "car.ctrip.com",
    "piao.ctrip.com",
    "tuan.ctrip.com",
    "you.ctrip.com",
    "g.ctrip.com",
    "lipin.ctrip.com",
    "ct.ctrip.com"
};


// header of DNS packets
class dns_header {
public:
    unsigned short id;
    unsigned short flags{};
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;

    dns_header() {
        id = random_lcd::next_int(0, 65535);
        flags |= htons(0x0100);
        qdcount = htons(1);
        ancount = 0;
        nscount = 0;
        arcount = 0;
    }
};

// text of DNS
class dns_question {
public:
    size_t length;
    unsigned short qtype;
    unsigned short qclass;
    char *qname;

    explicit dns_question(const char *hostname) {
        length = string_length(hostname) + 2;
        qtype = htons(1);
        qclass = htons(1);

        qname = static_cast<char *>(malloc(length));
        if (qname == nullptr) return;

        constexpr char delim[2] = ".";
        char *hostname_dup = string_duplicate(hostname);
        const char *token = string_token(hostname_dup, delim);
        char *qname_p = qname;

        while (token != nullptr) {
            const size_t len = string_length(token);
            *qname_p = static_cast<char>(len);
            qname_p++;
            string_n_copy(qname_p, token, len);
            qname_p += len;
            token = string_token(nullptr, delim);
        }
        free(hostname_dup);
    }

    ~dns_question() {
        if (qname) {
            free(qname);
        }
    }
};

// ip message from dns service
struct dns_item {
    char *domain;
    char *ip;
};

inline size_t dns_build_request(const dns_header &header, const dns_question &question, char *request) {
    constexpr int header_s = sizeof(dns_header);
    const size_t question_s = question.length + sizeof(question.qtype) + sizeof(question.qclass);
    const size_t length = question_s + header_s;

    size_t offset = 0;
    memory_copy(request + offset, &header, sizeof(dns_header));
    offset += sizeof(dns_header);
    memory_copy(request + offset, question.qname, question.length);
    offset += question.length;
    memory_copy(request + offset, &question.qtype, sizeof(question.qtype));
    offset += sizeof(question.qtype);
    memory_copy(request + offset, &question.qclass, sizeof(question.qclass));

    return length;
}

static void dns_parse_name(unsigned char *chunk, unsigned char *ptr, char *out, int *len) {
    int flag = 0, n = 0;
    char *pos = out + *len;
    while (true) {
        flag = static_cast<int>(ptr[0]);
        if (flag == 0) break;
        if ((flag & 0xC0) == 0xC0) {
            n = static_cast<int>(ptr[1]);
            ptr = chunk + n;
            dns_parse_name(chunk, ptr, out, len);
            break;
        }
        ptr++;
        memory_copy(pos, ptr, flag);
        pos += flag;
        ptr += flag;
        *len += flag;
        if (static_cast<int>(ptr[0]) != 0) {
            memory_copy(pos, ".", 1);
            pos += 1;
            *len += 1;
        }
    }
}

static int dns_parse_response(unsigned char *buffer, dns_item **domains) {
    uint16_t i = 0;
    unsigned char *ptr = buffer;
    ptr += 4;
    const uint16_t query = ntohs(*reinterpret_cast<unsigned short *>(ptr));
    ptr += 2;
    const uint16_t answers = ntohs(*reinterpret_cast<unsigned short *>(ptr));

    ptr += 6;
    for (i = 0; i < query; i++) {
        while (true) {
            const int flag = ptr[0];
            ptr += flag + 1;

            if (flag == 0) break;
        }
        ptr += 4;
    }

    char cname[128], aname[128], ip[20], netip[4];
    int len;
    int cnt = 0;
    auto *list = static_cast<struct dns_item *>(calloc(answers, sizeof(struct dns_item)));
    if (list == nullptr) return -1;

    for (i = 0; i < answers; i++) {
        memory_zero(aname, sizeof(aname));
        len = 0;

        dns_parse_name(buffer, ptr, aname, &len);
        ptr += 2;
        const uint16_t type = htons(*reinterpret_cast<unsigned short *>(ptr));
        ptr += 4;
        const uint16_t ttl = htons(*reinterpret_cast<unsigned short *>(ptr));
        ptr += 4;
        const uint16_t datalen = ntohs(*reinterpret_cast<unsigned short *>(ptr));
        ptr += 2;

        if (type == DNS_CNAME) {
            memory_zero(cname, sizeof(cname));
            len = 0;
            dns_parse_name(buffer, ptr, cname, &len);
            ptr += datalen;
        }
        else if (type == DNS_HOST) {
            memory_zero(ip, sizeof(ip));

            if (datalen == 4) {
                memory_copy(netip, ptr, datalen);
                inet_ntop(AF_INET, netip, ip, sizeof(ip));

                printf("%s has address %s\n", aname, ip);
                printf("\tTime to live: %d minutes , %d seconds\n", ttl / 60, ttl % 60);

                list[cnt].domain = static_cast<char *>(calloc(string_length(aname) + 1, 1));
                memory_copy(list[cnt].domain, aname, string_length(aname));
                list[cnt].ip = static_cast<char *>(calloc(string_length(ip) + 1, 1));
                memory_copy(list[cnt].ip, ip, string_length(ip));

                cnt++;
            }
            ptr += datalen;
        }
    }
    *domains = list;
    ptr += 2;
    return cnt;
}

inline int dns_client_commit(const char *domain) {
    const int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("create socket failed\n");
        exit(-1);
    }

    timeval timeout{};
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        close(sockfd);
        return -1;
    }

    printf("url:%s\n", domain);

    sockaddr_in dest{};
    memory_zero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(DNS_SVR);

    const int ret = connect(sockfd, reinterpret_cast<struct sockaddr *>(&dest), sizeof(dest));
    printf("connect :%d\n", ret);

    const dns_header header;
    const dns_question question(domain);

    char request[1024] = {};
    const size_t req_len = dns_build_request(header, question, request);
    ssize_t slen = sendto(sockfd, request, req_len, 0, reinterpret_cast<struct sockaddr *>(&dest), sizeof(struct sockaddr));

    unsigned char buffer[1024] = {};
    sockaddr_in addr{};
    size_t addr_len = sizeof(struct sockaddr_in);

    const ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0,
        reinterpret_cast<struct sockaddr *>(&addr), reinterpret_cast<socklen_t *>(&addr_len));

    printf("recvfrom n : %ld\n", n);
    dns_item *domains = nullptr;
    dns_parse_response(buffer, &domains);

    return 0;
}

MSTL_END_NAMESPACE__
#endif
#endif // MSTL_DNS_REQUEST_HPP__
