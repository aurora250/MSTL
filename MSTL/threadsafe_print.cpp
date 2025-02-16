#include "threadsafe_print.h"
MSTL_BEGIN_NAMESPACE__

Output& Output::operator <<(Output& out) {
    str_ << out.str();
    return *this;
}
std::string Output::str() const { return str_.str(); }
void Output::clear() { str_.str(""); }

ThreadsafeOutput::ThreadsafeOutput()
    : done_(false),
    output_thread_(&ThreadsafeOutput::output_loop, this) {
    //output_thread_.detach();  // abort
}
ThreadsafeOutput::~ThreadsafeOutput() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        done_ = true;
    }
    cv_.notify_all();
    output_thread_.join();
}
ThreadsafeOutput& ThreadsafeOutput::operator <<(Output& str) { // throwaway output
    {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_ << str.str();
        str.clear();
        queue_.push(buffer_.str());
        buffer_.str(""); buffer_.clear();
    }
    cv_.notify_all();
    return *this;
}
void ThreadsafeOutput::output_loop() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [&]() -> bool { return (not queue_.empty()) || done_; });
            if (not queue_.empty()) {
                std::cout << queue_.front() << std::flush;
                queue_.pop();
            }
            if (queue_.empty() && done_) {
                break;
            }
        }
    }
}

Output& AuxEndl::operator ()(Output& out) {
    out << '\n';
    return out;
}
ThreadsafeOutput& AuxEndl::operator() (ThreadsafeOutput& out) {
    out << '\n';
    return out;
}

void split_line(Output& _out, size_t _size) {
    while (_size) _out << '-', _size--;
    _out << endl;
}

MSTL_END_NAMESPACE__
