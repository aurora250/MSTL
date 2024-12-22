#ifndef MSTL_THREADSAFE_PRINT_H__
#define MSTL_THREADSAFE_PRINT_H__
#include "basiclib.h"
#include "queue.hpp"
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <condition_variable>
#include <sstream>
#include <atomic>
#include <functional>
#include <type_traits>

MSTL_BEGIN_NAMESPACE__

class Output;
class ThreadsafeOutput;

namespace concepts {
    template <typename T>
    concept SStreamOverloaded = requires(const T& t, std::stringstream& ss) {
        { ss << t } -> std::convertible_to<std::stringstream&>;
    };
    template <typename T>
    concept OutputManipulator = requires(T t, Output& out) {
        { t(out) } -> std::convertible_to<Output&>;
    };
    template <typename T>
    concept ThreadsafeOutputManipulator = requires(T t, ThreadsafeOutput& out) {
        { t(out) } -> std::convertible_to<ThreadsafeOutput&>;
    };
}

using namespace concepts;

class Output {
public:
    Output() = default;
    Output(const Output&) = default;
    ~Output() = default;

    template <typename T>
    Output& operator <<(const T& str) {
        str_ << str;
        return *this;
    };

    Output& operator <<(Output& out);

    template <typename Manipulator> 
        requires(OutputManipulator<Manipulator>)
    Output& operator <<(Manipulator& func) {
        return func(*this);
    }
    std::string str() const;
    void clear();
private:
    std::stringstream str_;
};
// static Output sout_aux;  // unsafe


class ThreadsafeOutput {
public:
    ThreadsafeOutput();
    ~ThreadsafeOutput();

    template <typename T>
    ThreadsafeOutput& operator <<(const T& str) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            buffer_ << str;
            queue_.push(buffer_.str());
            buffer_.str(""); buffer_.clear();
        }
        cv_.notify_one();
        return *this;
    };

    ThreadsafeOutput& operator <<(Output& str);

    template <typename Manipulator> 
    requires(ThreadsafeOutputManipulator<Manipulator>)
    ThreadsafeOutput& operator <<(Manipulator& func) {
        return func(*this);
    }
    
    ThreadsafeOutput(const ThreadsafeOutput&) = delete;
    ThreadsafeOutput(ThreadsafeOutput&&) = delete;
private:
    void output_loop();

    std::ostringstream buffer_;
    queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic_bool done_;
    std::thread output_thread_;
};
static ThreadsafeOutput sout; // safe output


struct AuxEndl {
    Output& operator ()(Output& out);
    ThreadsafeOutput& operator() (ThreadsafeOutput& out);
};
static AuxEndl endl;


void split_line(Output& _out, size_t _size = MSTL_SPLIT_LENGHT);

template <typename Container>
    requires(Detailable<Container>)
inline void __show_data_only(const Container& c, Output& _out) {
    using const_iterator = typename Container::const_iterator;
    size_t _band = c.size() - 1; size_t vl = 0;
    _out << '[';
    for (const_iterator iter = c.const_begin(); iter != c.const_end(); ++iter, ++vl) {
        _out << *iter;
        if (vl != _band) _out << ", ";
    }
    _out << ']';
}
template <typename T1, typename T2> 
    requires (Printable<T1> && Printable<T2>)
inline void __show_data_only(const std::pair<T1, T2>& p, Output& _out) {
    _out << "{ " << p.first << ", " << p.second << " }";
}
template <typename Container> 
    requires(Detailable<Container>)
void detailof_safe(const Container& c, ThreadsafeOutput& out = sout) {
    Output _out;
    split_line(_out);
    _out << "type: " << check_type<Container>() << endl;
    _out << "size: " << c.size() << endl;
    _out << "data: ";
    __show_data_only(c, _out);
    _out << endl;
    split_line(_out);
    out << _out;
}

MSTL_END_NAMESPACE__
#endif
