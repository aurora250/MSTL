#ifndef MSTL_CHECK_TYPE_H
#define MSTL_CHECK_TYPE_H
#include <sstream>
#include "string.h"
#if defined(__GNUC__)
#include <memory>
#include <cxxabi.h>
#endif
#include "basiclib.h"

MSTL_BEGIN_NAMESPACE__

class output {
private:
    bool is_compact_;
    std::string& sr_;

    template <typename T>
    bool check_empty(const T&) { return false; }
    bool check_empty(const char* val);

    template <typename T>
    void out(const T& val) {
        if (this->check_empty(val)) return;
        if (not this->is_compact_) sr_ += " ";
        using ss_t = std::ostringstream;
        this->sr_ += static_cast<ss_t>(ss_t() << val).str();
#if defined(MSTL_COMPILE_GCC__)
        this->sr_ += static_cast<ss_t&>(ss_t() << val).str();
#else
        this->sr_ += static_cast<ss_t>(ss_t() << val).str();
#endif
        this->is_compact_ = false;
    }
public:
    output(std::string& sr);
    output& operator()(void);
    output& compact(void);

    template <typename T1, typename... T>
    output& operator()(const T1& val, const T&... args) {
        this->out(val);
        return operator()(args...);
    }
};

template <bool>
struct bracket {   // ()
    output& out_;
    bracket(output& out, const char* = nullptr) : out_(out) {
        out_("(").compact();
    }
    ~bracket(void) {
        out_.compact()(")");
    }
};
template <>
struct bracket<false> {
    bracket(output& out, const char* str = nullptr) { out(str); }
};

template <size_t N = 0>
struct bound {   // [N]
    output& out_;

    bound(output& out) : out_(out) {}
    ~bound(void) {
        if (N == 0) out_("[]");
        else out_("[").compact()(N).compact()("]");
    }
};

template <bool, typename... P>
struct parameter;   // 变参

template <bool IsStart>
struct parameter<IsStart> {
    output& out_;

    parameter(output& out) : out_(out) {}
    ~parameter(void) { bracket<IsStart> { out_ }; }
};

template <typename T, bool IsBase = false>
struct check {
    output out_;

    check(const output& out) : out_(out) {
#if defined(MSTL_COMPILE_GNUC__)
        const char* typeid_name = typeid(T).name();
        auto deleter = [](char* p)
            {
                if (p) free(p);
            };
        std::unique_ptr<char, decltype(deleter)> real_name
        {
            abi::__cxa_demangle(typeid_name, nullptr, nullptr, nullptr), deleter
        };
        out_(real_name ? real_name.get() : typeid_name);
#else
        out_(typeid(T).name());
#endif
    }
};

template <typename T, bool IsBase>
struct check<T[], IsBase> : check<T, true> {   // 数组
    using base_t = check<T, true>;
    using base_t::out_;
    bound<>         bound_;
    bracket<IsBase> bracket_;

    check(const output& out) : base_t(out), bound_(out_), bracket_(out_) {}
};

#define CHECK_TYPE__(OPT) \
    template <typename T, bool IsBase> \
    struct check<T OPT, IsBase> : check<T, true> \
    { \
        using base_t = check<T, true>; \
        using base_t::out_; \
        check(const output& out) : base_t(out) { out_(#OPT); } \
    };

CHECK_TYPE__(const)
CHECK_TYPE__(volatile)
CHECK_TYPE__(const volatile)
CHECK_TYPE__(&)
CHECK_TYPE__(&&)
CHECK_TYPE__(*)


template <bool IsStart, typename P1, typename... P>
struct parameter<IsStart, P1, P...> {
    output& out_;
    parameter(output& out) : out_(out) {}
    ~parameter(void) {
        [this](bracket<IsStart>&&) {
            check<P1> { out_ };
            parameter<false, P...> { out_.compact() };
        } (bracket<IsStart> { out_, "," });
    }
};

#define CHECK_TYPE_ARRAY__(CV_OPT, BOUND_OPT, ...) \
    template <typename T, bool IsBase __VA_ARGS__> \
    struct check<T CV_OPT [BOUND_OPT], IsBase> : check<T CV_OPT, !std::is_array<T>::value> \
    { \
        using base_t = check<T CV_OPT, !std::is_array<T>::value>; \
        using base_t::out_; \
    \
        bound<BOUND_OPT> bound_; \
        bracket<IsBase>  bracket_; \
    \
        check(const output& out) : base_t(out) \
            , bound_  (out_) \
            , bracket_(out_) \
        {} \
    };

#define CHECK_TYPE_ARRAY_CV__(BOUND_OPT, ...) \
    CHECK_TYPE_ARRAY__(, BOUND_OPT, ,##__VA_ARGS__) \
    CHECK_TYPE_ARRAY__(const, BOUND_OPT, ,##__VA_ARGS__) \
    CHECK_TYPE_ARRAY__(volatile, BOUND_OPT, ,##__VA_ARGS__) \
    CHECK_TYPE_ARRAY__(const volatile, BOUND_OPT, ,##__VA_ARGS__)
#if defined(MSTL_COMPILE_GNUC__)
CHECK_TYPE_ARRAY_CV__(0)
#endif
CHECK_TYPE_ARRAY_CV__(N, size_t N)
CHECK_TYPE_ARRAY__(const, , )
CHECK_TYPE_ARRAY__(volatile, , )
CHECK_TYPE_ARRAY__(const volatile, , )

template <typename T, bool IsBase, typename... P>
struct check<T(P...), IsBase> : check<T, true> {   // 函数
    using base_t = check<T, true>;
    using base_t::out_;
    parameter<true, P...> parameter_;
    bracket<IsBase>       bracket_;

    check(const output& out) : base_t(out), parameter_(out_), bracket_(out_) {}
};

template <typename T, bool IsBase, typename C>
struct check<T C::*, IsBase> : check<T, true> {   // 类成员指针
    using base_t = check<T, true>;
    using base_t::out_;

    check(const output& out) : base_t(out) {
        check<C> { out_ };
        out_.compact()("::*");
    }
};

template <typename T, bool IsBase, typename C, typename... P>
struct check<T(C::*)(P...), IsBase> : check<T(P...), true> {   // 类成员函数指针
    using base_t = check<T(P...), true>;
    using base_t::out_;

    check(const output& out) : base_t(out) {
        check<C> { out_ };
        out_.compact()("::*");
    }
};

struct at_destruct {
    output& out_;
    const char* str_;

    at_destruct(output& out, const char* str = nullptr);
    ~at_destruct(void);
    void set_str(const char* str = nullptr);
};

#define CHECK_TYPE_MEM_FUNC__(...) \
    template <typename T, bool IsBase, typename C, typename... P> \
    struct check<T(C::*)(P...) __VA_ARGS__, IsBase> \
    { \
        at_destruct cv_; \
        check<T(P...), true> base_; \
        output& out_ = base_.out_; \
    \
        check(const output& out) \
            : cv_(base_.out_) \
            , base_(out) \
        { \
            cv_.set_str(#__VA_ARGS__); \
            check<C> { out_ }; \
            out_.compact()("::*"); \
        } \
    };

CHECK_TYPE_MEM_FUNC__(const)
CHECK_TYPE_MEM_FUNC__(volatile)
CHECK_TYPE_MEM_FUNC__(const volatile)

template <typename T>
std::string check_type() {
    // check_type<decltype(object)>()
    std::string str;
    check<T> { str };
    return str;
}

MSTL_END_NAMESPACE__

#endif // MSTL_CHECK_TYPE_H
