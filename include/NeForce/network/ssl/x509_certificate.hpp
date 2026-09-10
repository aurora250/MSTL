#ifndef NEFORCE_NETWORK_SSL_X509_CERTIFICATE_HPP__
#define NEFORCE_NETWORK_SSL_X509_CERTIFICATE_HPP__

/**
 * @file x509_certificate.hpp
 * @brief X.509证书封装
 *
 * 此文件提供了X.509证书的封装类，用于SSL/TLS通信中的证书管理。
 */

#include "NeForce/core/string/string.hpp"
NEFORCE_BEGIN_NAMESPACE__

/**
 * @addtogroup SSL SSL/TLS
 * @{
 */

/**
 * @class x509_certificate
 * @brief X.509证书
 *
 * 提供证书信息的访问接口。
 * 支持证书的复制、移动和PEM格式的序列化/反序列化。
 */
class NEFORCE_API x509_certificate {
private:
    void* cert_{nullptr}; ///< X509对象指针

public:
    /**
     * @brief 默认构造函数
     */
    x509_certificate() noexcept = default;

    /**
     * @brief 从原生OpenSSL句柄构造
     * @param cert OpenSSL X509对象指针
     * @warning 接管传入指针的所有权，不需要手动调用X509_free
     */
    explicit x509_certificate(void* cert) noexcept :
    cert_(cert) {}

    /**
     * @brief 拷贝构造函数
     * @param other 要复制的证书对象
     */
    x509_certificate(const x509_certificate& other) noexcept;

    /**
     * @brief 移动构造函数
     * @param other 要移动的证书对象
     */
    x509_certificate(x509_certificate&& other) noexcept :
    cert_(_NEFORCE exchange(other.cert_, nullptr)) {}

    /**
     * @brief 析构函数
     */
    ~x509_certificate();

    /**
     * @brief 拷贝赋值运算符
     * @param other 要复制的证书对象
     * @return 当前对象的引用
     */
    x509_certificate& operator=(const x509_certificate& other) noexcept;

    /**
     * @brief 移动赋值运算符
     * @param other 要移动的证书对象
     * @return 当前对象的引用
     */
    x509_certificate& operator=(x509_certificate&& other) noexcept;

    /**
     * @brief 获取原生OpenSSL句柄
     * @return OpenSSL X509对象指针
     * @note 调用者不应修改或释放返回的指针
     */
    NEFORCE_NODISCARD void* native_handle() const noexcept { return cert_; }

    /**
     * @brief 释放底层OpenSSL句柄的所有权
     * @return OpenSSL X509对象指针
     * @note 调用者负责释放资源
     */
    NEFORCE_NODISCARD void* release() noexcept;

    /**
     * @brief 重置证书
     * @param cert 新的OpenSSL X509对象指针
     */
    void reset(void* cert = nullptr) noexcept;

    /**
     * @brief 检查证书是否有效
     * @return 证书是否有效
     */
    NEFORCE_NODISCARD bool is_valid() const noexcept { return cert_ != nullptr; }

    /**
     * @brief 获取证书版本
     * @return 证书版本号
     * @throws value_exception 证书无效时抛出
     */
    NEFORCE_NODISCARD long version() const;

    /**
     * @brief 获取证书序列号
     * @return 证书序列号的十六进制字符串表示
     * @throws value_exception 证书无效或获取失败时抛出
     */
    NEFORCE_NODISCARD string serial_number() const;

    /**
     * @brief 获取证书主题名称
     * @return 主题名称的RFC2253格式字符串
     * @throws value_exception 证书无效或获取失败时抛出
     */
    NEFORCE_NODISCARD string subject_name() const;

    /**
     * @brief 获取证书颁发者名称
     * @return 颁发者名称的RFC2253格式字符串
     * @throws value_exception 证书无效或获取失败时抛出
     */
    NEFORCE_NODISCARD string issuer_name() const;

    /**
     * @brief 获取证书有效期起始时间
     * @return 有效期起始时间的字符串表示
     * @throws value_exception 证书无效或获取失败时抛出
     */
    NEFORCE_NODISCARD string not_before() const;

    /**
     * @brief 获取证书有效期结束时间
     * @return 有效期结束时间的字符串表示
     * @throws value_exception 证书无效或获取失败时抛出
     */
    NEFORCE_NODISCARD string not_after() const;

    /**
     * @brief 获取证书公钥的PEM格式
     * @return 公钥的PEM编码字符串
     * @throws value_exception 证书无效或获取失败时抛出
     */
    NEFORCE_NODISCARD string public_key_pem() const;

    /**
     * @brief 将证书导出为PEM格式
     * @return 证书的PEM编码字符串
     * @throws value_exception 证书无效或导出失败时抛出
     *
     * 包含"-----BEGIN CERTIFICATE-----"和"-----END CERTIFICATE-----"标记。
     */
    NEFORCE_NODISCARD string to_pem() const;

    /**
     * @brief 从PEM格式加载证书
     * @param pem PEM编码的证书字符串
     * @return 解析后的证书对象
     * @throws value_exception 解析失败时抛出
     *
     * 输入必须包含"-----BEGIN CERTIFICATE-----"和"-----END CERTIFICATE-----"标记。
     */
    NEFORCE_NODISCARD static x509_certificate from_pem(const string& pem);
};

/** @} */ // SSL/TLS

NEFORCE_END_NAMESPACE__
#endif // NEFORCE_NETWORK_SSL_X509_CERTIFICATE_HPP__
