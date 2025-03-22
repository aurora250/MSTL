#ifndef MSTL_SAFE_STREAM_HPP__
#define MSTL_SAFE_STREAM_HPP__
#include "basiclib.h"
#include <mutex>
MSTL_BEGIN_NAMESPACE__

template <typename Derived>
class basic_stream {
public:
    template <typename T>
    Derived& operator <<(const T& data) {
        static_cast<Derived&>(*this).write(data);
        return static_cast<Derived&>(*this);
    }

    Derived& operator <<(std::ostream& (*manip)(std::ostream&)) {
        static_cast<Derived&>(*this).write(manip);
        return static_cast<Derived&>(*this);
    }
};


class safe_ostream : public basic_stream<safe_ostream> {
private:
    std::ostream& os;
    std::mutex mtx;

public:
    explicit safe_ostream(std::ostream& stream) : os(stream) {}

    class stream_guard {
    private:
        safe_ostream& safe_os;
        std::unique_lock<std::mutex> lock{};

    public:
        explicit stream_guard(safe_ostream& os) : safe_os(os), lock(os.mtx) {}

        template <typename T>
        stream_guard& operator<<(const T& data) {
            safe_os.os << data;
            return *this;
        }

        stream_guard& operator<<(std::ostream& (*manip)(std::ostream&)) {
            safe_os.os << manip;
            return *this;
        }
    };

    stream_guard lock() { return stream_guard(*this); }

    template <typename T>
    void write(const T& data) {
        auto guard = lock();
        guard << data;
    }
};

class unsafe_ostream : public basic_stream<unsafe_ostream> {
private:
    std::ostream& os;

public:
    explicit unsafe_ostream(std::ostream& stream) : os(stream) {}

    template <typename T>
    void write(const T& data) {
        os << data;
    }
    void write(std::ostream& (*manip)(std::ostream&)) {
        os << manip;
    }
};


template <typename Derived>
basic_stream<Derived>& endl(basic_stream<Derived>& stream) {
    stream << '\n';
    return stream;
}

extern MSTL_INLINE17 safe_ostream sout(std::cout);
extern MSTL_INLINE17 unsafe_ostream usout(std::cout);

MSTL_END_NAMESPACE__
#endif // MSTL_SAFE_STREAM_HPP__
