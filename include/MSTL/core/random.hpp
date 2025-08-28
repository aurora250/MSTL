#ifndef MSTL_RANDOM_HPP__
#define MSTL_RANDOM_HPP__
#include "datetime.hpp"
#ifdef MSTL_PLATFORM_LINUX__
#include <sys/fcntl.h>
#include <unistd.h>
#endif
MSTL_BEGIN_NAMESPACE__

// based on LCD algorithm to generate pseudorandom number
class random_lcd {
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


// based on Mersenne Twister algorithm to generate pseudorandom number
class random_mt {
public:
    using seed_type = uint32_t;

private:
    static constexpr size_t n = 624;
    static constexpr size_t m = 397;
    static constexpr seed_type a = 0x9908b0df;
    static constexpr seed_type u = 11;
    static constexpr seed_type s = 7;
    static constexpr seed_type b = 0x9d2c5680;
    static constexpr seed_type t = 15;
    static constexpr seed_type c = 0xefc60000;
    static constexpr seed_type l = 18;

    static seed_type* state() {
        static seed_type state[n] = {};
        return state;
    }
    static size_t& index() {
        static size_t index = n;
        return index;
    }

    static void twist() {
        for (size_t i = 0; i < n; ++i) {
            const seed_type y = (state()[i] & 0x80000000) + (state()[(i + 1) % n] & 0x7fffffff);
            state()[i] = state()[(i + m) % n] ^ (y >> 1);
            if (y % 2 != 0) {
                state()[i] ^= a;
            }
        }
        index() = 0;
    }

    static seed_type* get_state() {
        static bool initialized = false;
        if (!initialized) {
            set_seed();
            initialized = true;
        }
        return state();
    }

    static size_t& get_index() {
        return index();
    }

public:
    static void set_seed(const seed_type seed = 0) {
        seed_type init_seed = seed;
        if (init_seed == 0) {
            init_seed = static_cast<seed_type>(_MSTL timestamp::now().get_seconds());
        }

        state()[0] = init_seed;
        for (size_t i = 1; i < n; ++i) {
            state()[i] = 1812433253 * (state()[i - 1] ^ (state()[i - 1] >> 30)) + i;
        }
        index() = n;
    }

    static int next_int(const int max) {
        if (max <= 0) return 0;

        seed_type* state = get_state();
        size_t& idx = get_index();

        if (idx >= n) {
            twist();
        }

        seed_type y = state[idx++];
        y ^= (y >> u);
        y ^= (y << s) & b;
        y ^= (y << t) & c;
        y ^= (y >> l);

        return static_cast<int>(static_cast<double>(y) / UINT32_MAX * max);
    }
    static int next_int(const int min, const int max) {
        if (min >= max) return min;
        return min + next_int(max - min);
    }

    static int next_int() {
        return next_int(0, INT32_MAX);
    }

    static double next_double() {
        seed_type* state = get_state();
        size_t& idx = get_index();

        if (idx >= n) {
            twist();
        }

        seed_type y = state[idx++];
        y ^= (y >> u);
        y ^= (y << s) & b;
        y ^= (y << t) & c;
        y ^= (y >> l);

        return static_cast<double>(y) / UINT32_MAX;
    }

    static double next_double(const double min, const double max) {
        if (min >= max) return min;
        return min + (max - min) * next_double();
    }

    static double next_double(const double max) {
        return next_double(0.0, max);
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
        ::HCRYPTPROV hProv;
        const bool supported = ::CryptAcquireContext(
            &hProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
        if (supported) {
            ::CryptReleaseContext(hProv, 0);
        }
        return supported;
#elif defined(MSTL_PLATFORM_LINUX__)
        const int fd = ::open("/dev/urandom", O_RDONLY);
        if (fd == -1) {
            return false;
        }
        ::close(fd);
        return true;
#else
        return false;
#endif
    }

private:
    static void get_random_bytes(byte_t* buffer, size_t length) {
        Exception(buffer != nullptr && length != 0, ValueError("Invalid buffer or length"));

#ifdef MSTL_PLATFORM_WINDOWS__
        HCRYPTPROV hProv = 0;
        if (!::CryptAcquireContext(&hProv, nullptr, nullptr,
            PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
            Exception(DeviceOperateError("Failed to acquire crypto context"));
        }

        if (!::CryptGenRandom(hProv, static_cast<DWORD>(length), reinterpret_cast<BYTE*>(buffer))) {
            ::CryptReleaseContext(hProv, 0);
            Exception(DeviceOperateError("Failed to generate random bytes"));
        }

        ::CryptReleaseContext(hProv, 0);
#elif defined(MSTL_PLATFORM_LINUX__)
        const int fd = open("/dev/urandom", O_RDONLY);
        Exception(fd != -1, FileOperateError("Failed to open /dev/urandom"));

        ssize_t bytesRead = 0;
        while (bytesRead < static_cast<ssize_t>(length)) {
            const ssize_t result = ::read(fd, buffer + bytesRead, length - bytesRead);
            if (result == -1) {
                ::close(fd);
                Exception(fd != -1, FileOperateError("Failed to open /dev/urandom"));
            }
            bytesRead += result;
        }
        ::close(fd);
#endif
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_RANDOM_HPP__
