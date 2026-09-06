#ifndef NEFORCE_NETWORK_SSL_X509_CERTIFICATE_HPP__
#define NEFORCE_NETWORK_SSL_X509_CERTIFICATE_HPP__
#include "NeForce/core/string/string.hpp"
NEFORCE_BEGIN_NAMESPACE__

class NEFORCE_API x509_certificate {
private:
    void* cert_{nullptr};

public:
    x509_certificate() noexcept = default;

    explicit x509_certificate(void* cert) noexcept :
    cert_(cert) {}

    x509_certificate(const x509_certificate& other) noexcept;

    x509_certificate(x509_certificate&& other) noexcept :
    cert_(_NEFORCE exchange(other.cert_, nullptr)) {}

    ~x509_certificate();

    x509_certificate& operator=(const x509_certificate& other) noexcept;
    x509_certificate& operator=(x509_certificate&& other) noexcept;

    NEFORCE_NODISCARD void* native_handle() const noexcept { return cert_; }
    NEFORCE_NODISCARD void* release() noexcept;

    void reset(void* cert = nullptr) noexcept;

    NEFORCE_NODISCARD bool is_valid() const noexcept { return cert_ != nullptr; }

    NEFORCE_NODISCARD long version() const;
    NEFORCE_NODISCARD string serial_number() const;
    NEFORCE_NODISCARD string subject_name() const;
    NEFORCE_NODISCARD string issuer_name() const;
    NEFORCE_NODISCARD string not_before() const;
    NEFORCE_NODISCARD string not_after() const;
    NEFORCE_NODISCARD string public_key_pem() const;

    NEFORCE_NODISCARD string to_pem() const;
    NEFORCE_NODISCARD static x509_certificate from_pem(const string& pem);
};

NEFORCE_END_NAMESPACE__
#endif // NEFORCE_NETWORK_SSL_X509_CERTIFICATE_HPP__
