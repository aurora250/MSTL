#ifndef NEFORCE_NETWORK_SSL_SSL_ACCEPTOR_HPP__
#define NEFORCE_NETWORK_SSL_SSL_ACCEPTOR_HPP__

/**
 * @file ssl_acceptor.hpp
 * @brief SSL/TLS Acceptor实现
 *
 * 此文件提供了SSL/TLS Acceptor的实现。
 */

#include "NeForce/network/ssl/ssl_socket.hpp"
#include "NeForce/network/tcp/tcp_acceptor.hpp"
NEFORCE_BEGIN_NAMESPACE__

/**
 * @addtogroup SSL SSL/TLS
 * @{
 */

/**
 * @class ssl_acceptor
 * @brief SSL/TLS服务器Acceptor类
 *
 * 实现SSL/TLS服务器的连接接受功能，继承自tcp_acceptor。
 * 自动为每个接受的连接完成SSL/TLS握手，返回加密的ssl_socket。
 *
 * 使用示例：
 * @code
 * // 创建SSL上下文
 * ssl_context ctx(ssl_method::TLS_SERVER);
 * ctx.load_certificate("server.crt", "server.key");
 * ctx.load_verify_locations("ca.crt");
 * ctx.require_client_certificate();  // 可选：要求客户端证书
 *
 * // 创建SSL Acceptor
 * ssl_acceptor acceptor;
 * acceptor.set_ssl_context(move(ctx));
 *
 * // 绑定并监听端口
 * auto endpoint = ip_address::any(ports::https);
 * acceptor.open(endpoint, 128);
 *
 * println("TLS Server listening on ", endpoint);
 *
 * while (true) {
 *     try {
 *         // 接受TLS客户端连接
 *         auto client = acceptor.accept_ssl();
 *
 *         // 获取客户端地址
 *         auto client_addr = client.remote_endpoint();
 *         if (client_addr) {
 *             println("New TLS connection from ", client_addr);
 *         }
 *
 *         // 验证客户端证书（如果要求）
 *         if (client.verify_peer()) {
 *             println("Client certificate verified");
 *             println(client.peer_certificate_info());
 *         }
 *
 *         // 处理加密通信
 *         char buffer[1024];
 *         ssize_t received = client.receive({buffer, sizeof(buffer)});
 *         if (received > 0) {
 *             client.send_all({"Hello from TLS server!"});
 *         }
 *     } catch (const exception& e) {
 *         println("Error: ", e.what());
 *     }
 * }
 * @endcode
 *
 * @note 需要先设置有效的SSL上下文才能接受连接。
 *       SSL上下文应包含服务器证书和私钥。
 */
class NEFORCE_API ssl_acceptor final : public tcp_acceptor {
private:
    ssl_context ctx_; ///< SSL上下文

public:
    /**
     * @brief 默认构造函数
     */
    ssl_acceptor() = default;

    /**
     * @brief 设置SSL上下文
     * @param ctx SSL上下文对象
     * @throws ssl_exception 上下文无效时抛出
     *
     * 设置用于TLS握手的SSL上下文。
     * 必须在调用accept_ssl()之前调用。
     */
    void set_ssl_context(ssl_context ctx);

    /**
     * @brief 获取SSL上下文引用
     * @return SSL上下文引用
     */
    NEFORCE_NODISCARD ssl_context& context() noexcept { return ctx_; }

    /**
     * @brief 获取SSL上下文常量引用
     * @return SSL上下文常量引用
     */
    NEFORCE_NODISCARD const ssl_context& context() const noexcept { return ctx_; }

    /**
     * @brief 接受TLS客户端连接
     * @return 已建立TLS连接的SSL socket
     * @throws socket_exception 接受连接失败时抛出
     * @throws ssl_exception SSL握手失败时抛出
     * @throws value_exception acceptor未打开或SSL上下文无效时抛出
     */
    NEFORCE_NODISCARD ssl_socket accept_ssl();

    /**
     * @brief 非阻塞接受TLS客户端连接
     * @return 有新连接返回已建立TLS连接的SSL socket，无连接返回none
     * @throws socket_exception 发生错误时抛出
     * @throws ssl_exception SSL握手失败时抛出
     *
     * 非阻塞地尝试接受新的TLS连接并完成握手，需要先设置acceptor为非阻塞模式。
     */
    NEFORCE_NODISCARD optional<ssl_socket> accept_ssl_nonblock();

    /**
     * @brief 异步接受 TLS 客户端连接
     * @param ctx 异步 I/O 执行上下文
     * @param handler 完成回调 void(error_code, ssl_socket)
     *
     * 异步接受 TCP 连接并完成 SSL/TLS 握手，返回已建立 TLS 连接的 ssl_socket。
     * 需要先设置有效的 SSL 上下文。
     */
    void async_accept(io_context& ctx, function<void(error_code, ssl_socket)> handler);

    /**
     * @brief 带取消槽的异步接受 TLS 客户端连接
     * @param ctx 异步 I/O 执行上下文
     * @param slot 取消槽
     * @param handler 完成回调 void(error_code, ssl_socket)
     */
    void async_accept(io_context& ctx, cancellation_slot& slot, function<void(error_code, ssl_socket)> handler);

    /**
     * @brief 异步接受任意可调用对象
     * @tparam Token 可调用对象类型，需满足 void(error_code, ssl_socket) 签名
     * @param ctx 异步 I/O 执行上下文
     * @param token 完成令牌
     */
    template <typename Token, enable_if_t<!is_same_v<decay_t<Token>, function<void(error_code, ssl_socket)>>, int> = 0>
    void async_accept(io_context& ctx, Token&& token) {
        async_accept(ctx, function<void(error_code, ssl_socket)>(forward<Token>(token)));
    }

    /**
     * @brief future 异步接受
     * @param ctx 异步 I/O 执行上下文
     * @return 已建立 TLS 连接的 socket
     */
    future<ssl_socket> async_accept(io_context& ctx, use_future_t /*unused*/) {
        async_result<use_future_t, void(error_code, ssl_socket)> result(use_future);
        async_accept(ctx, function<void(error_code, ssl_socket)>(result.get_handler()));
        return result.get();
    }

    /**
     * @brief detached 异步接受
     * @param ctx 异步 I/O 执行上下文
     */
    void async_accept(io_context& ctx, detached_t /*unused*/) override {
        async_accept(ctx, function<void(error_code, ssl_socket)>([](error_code, ssl_socket) {}));
    }

#ifdef NEFORCE_STANDARD_20
    /**
     * @brief awaitable 异步接受
     * @param ctx 异步 I/O 执行上下文
     * @return 可协程等待的结果
     */
    awaitable<error_code, ssl_socket> async_accept(io_context& ctx, use_awaitable_t /*unused*/) {
        async_result<use_awaitable_t, void(error_code, ssl_socket)> result(use_awaitable);
        async_accept(ctx, function<void(error_code, ssl_socket)>(result.get_handler()));
        return result.get();
    }
#endif
};

/** @} */ // SSL/TLS

NEFORCE_END_NAMESPACE__
#endif // NEFORCE_NETWORK_SSL_SSL_ACCEPTOR_HPP__
