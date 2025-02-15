#ifndef MSTL_FUNCTION_HPP__
#define MSTL_FUNCTION_HPP__
#include "functor.hpp"
#include "errorlib.h"
MSTL_BEGIN_NAMESPACE__

template <typename>
struct function {
    static_assert(false, "invalid function signature");
};

template <typename Ret, typename ...Args>
struct function<Ret(Args...)> {
private:
    struct __func_base {
        virtual Ret do_call(Args...) = 0;
        virtual std::unique_ptr<__func_base> copy() const = 0;
        virtual std::type_info const& target_type() const = 0;
        virtual ~__func_base() = default;
    };

    template <typename Fcn>
    struct __func_impl final : __func_base {
        Fcn func_;

        template <typename... U>
        explicit __func_impl(inplace_construct_tag, U &&... args) 
            : func_(MSTL::forward<U>(args)...) {}

        Ret do_call(Args... args) override {
            if constexpr (is_void_v<Ret>) 
                (void)std::invoke(func_, MSTL::forward<Args>(args)...);
            else 
                return std::invoke(func_, MSTL::forward<Args>(args)...);
        }

        std::unique_ptr<__func_base> copy() const override {
            return std::make_unique<__func_impl>(inplace_construct_t, func_);
        }
        std::type_info const& target_type() const override {
            return typeid(Fcn);
        }
    };

    std::unique_ptr<__func_base> base_;

public:
    function() = default;
    function(std::nullptr_t) noexcept : function() {}
    
    template <class Fcn, class = enable_if_t<
        std::is_invocable_r_v<Ret, decay_t<Fcn>, Args...> && is_copy_constructible_v<Fcn>
        && !is_same_v<decay_t<Fcn>, function<Ret(Args...)>> >>
        function(Fcn&& func)
        : base_(std::make_unique<__func_impl<decay_t<Fcn>>>(inplace_construct_t, MSTL::forward<Fcn>(func))) {}

    function(function&&) = default;
    function& operator=(function&&) = default;

    function(const function& x) : base_(x.base_ ? x.base_->copy() : nullptr) {}

    function& operator =(const function& x) {
        if (x.base_)
            base_ = x.base_->copy();
        else
            base_ = nullptr;
    }

    explicit operator bool() const noexcept {
        return base_ != nullptr;
    }

    bool operator ==(std::nullptr_t) const noexcept {
        return base_ == nullptr;
    }

    bool operator !=(std::nullptr_t) const noexcept {
        return base_ != nullptr;
    }

    Ret operator ()(Args... args) const {
        if (!base_) MSTL_UNLIKELY
            Exception(ValueError("function arguments invalid."));
        return base_->do_call(MSTL::forward<Args>(args)...);
    }

    std::type_info const& target_type() const noexcept {
        return base_ ? base_->target_type() : typeid(void);
    }

    template <class Fcn>
    Fcn* target() const noexcept {
        return base_ && typeid(Fcn) == base_->target_type() ?
            MSTL::addressof(static_cast<__func_impl<Fcn>*>(base_.get())->func_) : nullptr;
    }

    void swap(function& x) const noexcept {
        if(MSTL::addressof(x) != this)
            base_.swap(x.base_);
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_FUNCTION_HPP__
