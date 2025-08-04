#ifndef MSTL_RANDOM_HPP__
#define MSTL_RANDOM_HPP__
#include "datetime.hpp"
MSTL_BEGIN_NAMESPACE__

// based on LCD algorithm to generate pseudorandom number
class random {
public:
    using seed_type = uint32_t;

private:
    static constexpr seed_type a = 1103515245;
    static constexpr seed_type c = 12345;
    static constexpr seed_type m = 1u << 31;

    static seed_type& get_seed() {
        static seed_type seed = 0;
        return seed;
    }

public:
    static void set_seed(const seed_type seed = 0) {
        if (seed == 0) {
            get_seed() = static_cast<seed_type>(_MSTL timestamp::now().get_seconds());
        } else {
            get_seed() = seed;
        }
    }

    static int next_int(const int max) {
        get_seed() = a * get_seed() + c;
        get_seed() %= m;
        return static_cast<int>(static_cast<double>(get_seed()) / m * max);
    }

    static int next_int(const int min, const int max) {
        return min + next_int(max - min);
    }

    static int next_int() {
        return next_int(0, INT32_MAX_SIZE);
    }

    static double next_double() {
        get_seed() = a * get_seed() + c;
        get_seed() %= m;
        return static_cast<double>(get_seed()) / m;
    }

    static double next_double(const double min, const double max) {
        return min + (max - min) * next_double();
    }

    static double next_double(const double max) {
        return next_double(0, max);
    }
};


// based on hardware noise to generate true random number
class secret {
public:
    static int32_t next_int(const int32_t min, const int32_t max) {
        Exception(min < max, ValueError("min is ge to max"));
        const auto range = static_cast<uint32_t>(max - min + 1);

        int bits = 0;
        uint32_t temp = range - 1;
        while (temp > 0) {
            temp >>= 1;
            bits++;
        }

        uint32_t value;
        do {
            get_random_bytes(reinterpret_cast<uint8_t*>(&value), sizeof(value));
            value &= (1ULL << bits) - 1;
        } while (value >= range);

        return min + static_cast<int32_t>(value);
    }

    static int32_t next_int(const int32_t max) {
        return next_int(0, max);
    }

    static double next_double() {
        uint64_t value;
        get_random_bytes(reinterpret_cast<uint8_t*>(&value), sizeof(value));
        return static_cast<double>(value) / (static_cast<double>(1ULL << 63) + 1.0);
    }

    static bool is_supported() {
#ifdef MSTL_PLATFORM_WINDOWS__
        HCRYPTPROV hProv;
        const bool supported = CryptAcquireContext(
            &hProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
        if (supported) {
            CryptReleaseContext(hProv, 0);
        }
        return supported;
#elif defined(MSTL_PLATFORM_LINUX__)
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd == -1) {
            return false;
        }
        close(fd);
        return true;
#else
        return false;
#endif
    }

private:
    static void get_random_bytes(byte_t* buffer, size_t length) {
        Exception(!(buffer == nullptr || length == 0), ValueError("Invalid buffer or length"));

#ifdef MSTL_PLATFORM_WINDOWS__
        HCRYPTPROV hProv = 0;
        if (!CryptAcquireContext(&hProv, nullptr, nullptr,
            PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
            Exception(DeviceOperateError("Failed to acquire crypto context"));
        }

        if (!CryptGenRandom(hProv, static_cast<DWORD>(length), reinterpret_cast<BYTE*>(buffer))) {
            CryptReleaseContext(hProv, 0);
            Exception(DeviceOperateError("Failed to generate random bytes"));
        }

        CryptReleaseContext(hProv, 0);
#elif defined(MSTL_PLATFORM_LINUX__)
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd == -1) {
            throw std::runtime_error("Failed to open /dev/urandom");
        }
        Exception(fd != -1, FileOperateError("Failed to open /dev/urandom"))

        ssize_t bytesRead = 0;
        while (bytesRead < static_cast<ssize_t>(length)) {
            ssize_t result = read(fd, buffer + bytesRead, length - bytesRead);
            if (result == -1) {
                close(fd);
                Exception(fd != -1, FileOperateError("Failed to open /dev/urandom"))
            }
            bytesRead += result;
        }
        close(fd);
#endif
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_RANDOM_HPP__
