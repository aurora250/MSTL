#ifndef MSTL_FUNCTION_HPP__
#define MSTL_FUNCTION_HPP__
#include "memory.hpp"
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
        explicit __func_impl(exact_arg_construct_tag, U&&... args)
            : func_(MSTL::forward<U>(args)...) {}

        template <typename U = Ret, enable_if_t<is_void_v<U>, int> = 0>
        inline void __do_call_aux(Args... args) {
            (void)std::invoke(func_, MSTL::forward<Args>(args)...);
        }
        template <typename U = Ret, enable_if_t<!is_void_v<U>, int> = 0>
        inline Ret __do_call_aux(Args... args) {
            return std::invoke(func_, MSTL::forward<Args>(args)...);
        }

        Ret do_call(Args... args) override {
            return __do_call_aux(MSTL::forward<Args>(args)...);
        }

        std::unique_ptr<__func_base> copy() const override {
            return std::make_unique<__func_impl>(exact_arg_construct_tag{}, func_);
        }
        std::type_info const& target_type() const override {
            return typeid(Fcn);
        }
    };

    std::unique_ptr<__func_base> base_;

public:
    function() = default;
    function(nullptr_t) noexcept : function() {}
    
    template <class Fcn, enable_if_t<
        is_copy_constructible_v<Fcn> && !is_same_v<decay_t<Fcn>, function<Ret(Args...)>>, int> = 0>
    function(Fcn&& func) : base_(MSTL::move(std::make_unique<
        __func_impl<decay_t<Fcn>>>(exact_arg_construct_tag{}, MSTL::forward<Fcn>(func)))) { }

    function(function&&) = default;
    function& operator =(function&&) = default;

    function(const function& x) : base_(x.base_ ? x.base_->copy() : nullptr) {}

    function& operator =(const function& x) {
        if (x.base_)
            base_ = x.base_->copy();
        else
            base_ = nullptr;
        return *this;
    }

    explicit operator bool() const noexcept {
        return base_ != nullptr;
    }

    bool operator ==(nullptr_t) const noexcept {
        return base_ == nullptr;
    }

    bool operator !=(nullptr_t) const noexcept {
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
