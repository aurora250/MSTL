#ifndef MSTL_THREADSAFE_PRINT_H__
#define MSTL_THREADSAFE_PRINT_H__
#include "queue.hpp"
#include "concepts.hpp"
#include "stringstream.hpp"
#include <shared_mutex>
#include <string>
#include <thread>
#include <condition_variable>
#include <sstream>
#include <atomic>
#include <functional>
MSTL_BEGIN_NAMESPACE__

class Output;
class ThreadsafeOutput;

template <typename T>
concept SStreamOverloaded = requires(const T & t, stringstream & ss) {
    { ss << t } -> convertible_to<stringstream&>;
};
template <typename T>
concept OutputManipulator = requires(T t, Output & out) {
    { t(out) } -> convertible_to<Output&>;
};
template <typename T>
concept ThreadsafeOutputManipulator = requires(T t, ThreadsafeOutput & out) {
    { t(out) } -> convertible_to<ThreadsafeOutput&>;
};

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
    string str() const;
    void clear();
private:
    stringstream str_;
};


class ThreadsafeOutput {
public:
    ThreadsafeOutput();
    ~ThreadsafeOutput();

    template <typename T>
    ThreadsafeOutput& operator <<(const T& str) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            buffer_ << str;
            queue_.push(buffer_.str());
            buffer_.str(""); buffer_.clear();
        }
        cv_.notify_all();
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

    stringstream buffer_;
    queue<string> queue_;
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


void split_line(Output& _out, size_t _size = MSTL_SPLIT_LENGTH);

template <typename Container>
    requires(is_detailable<Container>)
inline void show_data_only(const Container& c, Output& _out) {
    using const_iterator = typename Container::const_iterator;
    size_t _band = c.size() - 1; size_t vl = 0;
    _out << '[';
    for (const_iterator iter = c.cbegin(); iter != c.cend(); ++iter, ++vl) {
        _out << *iter;
        if (vl != _band) _out << ", ";
    }
    _out << ']';
}
template <typename Container>
    requires(is_detailable<Container>)
void detailof_safe(const Container& c, ThreadsafeOutput& out = sout) {
    Output _out;
    split_line(_out);
    _out << "type: " << MSTL::check_type<Container>() << endl;
    _out << "size: " << c.size() << endl;
    _out << "data: ";
    show_data_only(c, _out);
    _out << endl;
    split_line(_out);
    out << _out;
}

MSTL_END_NAMESPACE__
#endif
