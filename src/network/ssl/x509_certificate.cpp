#include <NeForce/network/ssl/x509_certificate.hpp>
#include <openssl/x509.h>
#include <openssl/pem.h>
NEFORCE_BEGIN_NAMESPACE__

namespace {
    string name_to_string(const ::X509_NAME* name) {
        ::BIO* bio = ::BIO_new(::BIO_s_mem());
        if (bio == nullptr) {
            NEFORCE_THROW_EXCEPTION(value_exception("Failed to create BIO"));
        }
        if (::X509_NAME_print_ex(bio, name, 0, XN_FLAG_RFC2253) < 0) {
            ::BIO_free(bio);
            NEFORCE_THROW_EXCEPTION(value_exception("Failed to print name"));
        }
        char* data = nullptr;
        const long len = ::BIO_get_mem_data(bio, &data);
        string result(data, len);
        ::BIO_free(bio);
        return result;
    }

    string asn1_time_to_string(const ::ASN1_TIME* time) {
        if (time == nullptr) {
            NEFORCE_THROW_EXCEPTION(value_exception("Invalid time"));
        }
        ::BIO* bio = ::BIO_new(::BIO_s_mem());
        if (bio == nullptr) {
            NEFORCE_THROW_EXCEPTION(value_exception("Failed to create BIO"));
        }
        if (::ASN1_TIME_print(bio, time) != 1) {
            ::BIO_free(bio);
            NEFORCE_THROW_EXCEPTION(value_exception("Failed to print time"));
        }
        char* data = nullptr;
        const long len = ::BIO_get_mem_data(bio, &data);
        string result(data, len);
        ::BIO_free(bio);
        return result;
    }

    NEFORCE_ALWAYS_INLINE_INLINE void check_valid(const void* ptr) {
        if (ptr == nullptr) {
            NEFORCE_THROW_EXCEPTION(value_exception("Certificate is empty"));
        }
    }
} // namespace


x509_certificate::x509_certificate(const x509_certificate& other) noexcept :
cert_(other.cert_) {
    if (cert_ != nullptr) {
        ::X509_up_ref(static_cast<::X509*>(cert_));
    }
}

x509_certificate::~x509_certificate() {
    if (cert_ != nullptr) {
        ::X509_free(static_cast<::X509*>(cert_));
    }
}

x509_certificate& x509_certificate::operator=(const x509_certificate& other) noexcept {
    if (this != &other) {
        auto* tmp = static_cast<::X509*>(other.cert_);
        if (tmp != nullptr) {
            ::X509_up_ref(tmp);
        }
        if (cert_ != nullptr) {
            ::X509_free(static_cast<::X509*>(cert_));
        }
        cert_ = tmp;
    }
    return *this;
}

x509_certificate& x509_certificate::operator=(x509_certificate&& other) noexcept {
    if (this != &other) {
        if (cert_ != nullptr) {
            ::X509_free(static_cast<::X509*>(cert_));
        }
        cert_ = other.cert_;
        other.cert_ = nullptr;
    }
    return *this;
}

void* x509_certificate::release() noexcept {
    auto* tmp = static_cast<::X509*>(cert_);
    cert_ = nullptr;
    return tmp;
}

void x509_certificate::reset(void* cert) noexcept {
    if (cert_ != cert) {
        if (cert_ != nullptr) {
            ::X509_free(static_cast<::X509*>(cert_));
        }
        cert_ = cert;
    }
}

long x509_certificate::version() const {
    check_valid(cert_);
    return ::X509_get_version(static_cast<::X509*>(cert_));
}

string x509_certificate::serial_number() const {
    check_valid(cert_);
    const ::ASN1_INTEGER* serial = ::X509_get_serialNumber(static_cast<::X509*>(cert_));
    if (serial == nullptr) {
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to get serial number"));
    }
    ::BIGNUM* bn = ::ASN1_INTEGER_to_BN(serial, nullptr);
    if (bn == nullptr) {
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to convert serial number"));
    }
    char* hex = ::BN_bn2hex(bn);
    string result(hex != nullptr ? hex : "");
    ::OPENSSL_free(hex);
    ::BN_free(bn);
    return result;
}

string x509_certificate::subject_name() const {
    check_valid(cert_);
    const ::X509_NAME* name = ::X509_get_subject_name(static_cast<::X509*>(cert_));
    if (name == nullptr) {
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to get subject name"));
    }
    return name_to_string(name);
}

string x509_certificate::issuer_name() const {
    check_valid(cert_);
    const ::X509_NAME* name = ::X509_get_issuer_name(static_cast<::X509*>(cert_));
    if (name == nullptr) {
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to get issuer name"));
    }
    return name_to_string(name);
}

string x509_certificate::not_before() const {
    check_valid(cert_);
    return asn1_time_to_string(::X509_get0_notBefore(static_cast<::X509*>(cert_)));
}

string x509_certificate::not_after() const {
    check_valid(cert_);
    return asn1_time_to_string(::X509_get0_notAfter(static_cast<::X509*>(cert_)));
}

string x509_certificate::public_key_pem() const {
    check_valid(cert_);
    ::EVP_PKEY* pkey = ::X509_get_pubkey(static_cast<::X509*>(cert_));
    if (pkey == nullptr) {
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to get public key"));
    }
    ::BIO* bio = ::BIO_new(::BIO_s_mem());
    if (bio == nullptr) {
        ::EVP_PKEY_free(pkey);
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to create BIO"));
    }
    if (::PEM_write_bio_PUBKEY(bio, pkey) != 1) {
        ::BIO_free(bio);
        ::EVP_PKEY_free(pkey);
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to write public key"));
    }
    char* data = nullptr;
    const long len = ::BIO_get_mem_data(bio, &data);
    string result(data, len);
    ::BIO_free(bio);
    ::EVP_PKEY_free(pkey);
    return result;
}

string x509_certificate::to_pem() const {
    check_valid(cert_);
    ::BIO* bio = ::BIO_new(::BIO_s_mem());
    if (bio == nullptr) {
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to create BIO"));
    }
    if (::PEM_write_bio_X509(bio, static_cast<::X509*>(cert_)) != 1) {
        ::BIO_free(bio);
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to write certificate"));
    }
    char* data = nullptr;
    const long len = ::BIO_get_mem_data(bio, &data);
    string result(data, len);
    ::BIO_free(bio);
    return result;
}

x509_certificate x509_certificate::from_pem(const string& pem) {
    ::BIO* bio = ::BIO_new_mem_buf(pem.data(), static_cast<int>(pem.size()));
    if (bio == nullptr) {
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to create BIO"));
    }
    ::X509* cert = ::PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);
    ::BIO_free(bio);
    if (cert == nullptr) {
        NEFORCE_THROW_EXCEPTION(value_exception("Failed to parse PEM certificate"));
    }
    return x509_certificate(cert);
}

NEFORCE_END_NAMESPACE__
