#ifndef MSTL_OPTIONAL_HPP__
#define MSTL_OPTIONAL_HPP__
#include "errorlib.h"
#include "utility.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_CLASS(OptionalAccessError, MemoryError, "Access the null value of optional.")

MSTL_INDEPENDENT_TAG_NAMESPACE_SETTING namespace tags {
    struct nullopt_t {
        constexpr explicit nullopt_t() noexcept = default;
    };
    constexpr nullopt_t nullopt;
}

template <class T>
struct optional {
    static_assert(!is_any_of_v<remove_cv_t<T>, nullopt_t, inplace_construct_tag>,
        "optional do not contains nullopt_t and inplace_construct_tag types.");
    static_assert(is_object_v<T> && !is_array_v<T>, "optional only contains non-array object types.");

private:
    MSTL_BUILD_TYPE_ALIAS(T)
    using null_type     = nullopt_t;
    using self          = optional<T>;

    template <typename U>
    using is_valid_optional = bool_constant<!is_any_of_v<remove_cv_t<U>, nullopt_t, inplace_construct_tag> && is_object_v<U> && !is_array_v<U>>;

    template <typename U>
    using convertible_from_optional = disjunction<
        is_constructible<T, const optional<U>&>,
        is_constructible<T, optional<U>&>,
        is_constructible<T, const optional<U>&&>,
        is_constructible<T, optional<U>&&>,
        is_convertible<const optional<U>&, T>,
        is_convertible<optional<U>&, T>,
        is_convertible<const optional<U>&&, T>,
        is_convertible<optional<U>&&, T>>;

    template <typename U>
    using assignable_from_optional = disjunction<
        is_assignable<T&, const optional<U>&>,
        is_assignable<T&, optional<U>&>,
        is_assignable<T&, const optional<U>&&>,
        is_assignable<T&, optional<U>&&>>;

    bool have_value_ = false;
    union { T value_{}; };

public:
    constexpr optional() noexcept = default;

    constexpr optional(null_type) noexcept {}
    MSTL_CONSTEXPR20 self& operator =(null_type) noexcept {
        reset();
        return *this;
    }

    template <typename U, enable_if_t<
        is_valid_optional<U>::value && !is_same_v<remove_cvref_t<U>, self> && is_constructible_v<T, U> && is_convertible_v<U, T>, int> = 0>
    constexpr optional(U&& value) noexcept(is_nothrow_constructible_v<T, U>)
    : value_(MSTL::forward<U>(value)) {}

    template <typename U, enable_if_t<
        is_valid_optional<U>::value && !is_same_v<remove_cvref_t<U>, self> && is_constructible_v<T, U> && !is_convertible_v<U, T>, int> = 0>
    constexpr explicit optional(U&& value) noexcept(is_nothrow_constructible_v<T, U>)
    : value_(MSTL::forward<U>(value)) {}

    template <typename U = T, enable_if_t<!is_same_v<remove_cvref_t<U>, self>
        && negation_v<conjunction<is_scalar<T>, is_same<T, decay_t<U>>>>
        && is_constructible_v<T, U> && is_assignable_v<T&, U>, int> = 0>
    MSTL_CONSTEXPR20 self& operator =(U&& value)
    noexcept(is_nothrow_constructible_v<T, U> && is_nothrow_assignable_v<T&, U>) {
        if (have_value_)
            value_ = MSTL::forward<U>(value);
        else
            MSTL::construct(&value_ ,MSTL::forward<U>(value));
        return *this;
    }

    template <class U, enable_if_t<!is_same_v<T, U> && is_constructible_v<T, const U&> &&
        is_convertible_v<const U&, T> && convertible_from_optional<U>::value, int> = 0>
    constexpr optional(const optional<U>& x) noexcept(is_nothrow_constructible_v<T, const U&>) {
        if (x) emplace(*x);
    }

    template <class U, enable_if_t<!is_same_v<T, U> && is_constructible_v<T, const U&> &&
        !is_convertible_v<const U&, T> && convertible_from_optional<U>::value, int> = 0>
    constexpr explicit optional(const optional<U>& x) noexcept(is_nothrow_constructible_v<T, const U&>) {
        if (x) emplace(*x);
    }

    template <typename U = T, enable_if_t<!is_same_v<remove_cvref_t<U>, self>
        && is_constructible_v<T, const U&> && is_assignable_v<T&, const U&>
        && !convertible_from_optional<U>::value && !assignable_from_optional<U>::value, int> = 0>
    MSTL_CONSTEXPR20 self& operator =(const optional<U>& x)
    noexcept(is_nothrow_constructible_v<T, const U&> && is_nothrow_assignable_v<T&, const U&>) {
        if (MSTL::addressof(x) == this) return *this;
        if (x) {
            if (have_value_)
                value_ = *x;
            else
                MSTL::construct(&value_, *x);
        }
        else
            reset();
        return *this;
    }

    template <class U, enable_if_t<!is_same_v<T, U> && is_constructible_v<T, U> &&
        is_convertible_v<U, T> && convertible_from_optional<U>::value, int> = 0>
    constexpr optional(optional<U>&& x) noexcept(is_nothrow_constructible_v<T, U>) {
        if (x) emplace(MSTL::move(*x));
    }

    template <class U, enable_if_t<!is_same_v<T, U> && is_constructible_v<T, U> &&
        !is_convertible_v<U, T> && convertible_from_optional<U>::value, int> = 0>
    constexpr optional(optional<U>&& x) noexcept(is_nothrow_constructible_v<T, U>) {
        if (x) emplace(MSTL::move(*x));
    }

    template <typename U = T, enable_if_t<!is_same_v<remove_cvref_t<U>, self>
        && is_constructible_v<T, U> && is_assignable_v<T&, U>
        && !convertible_from_optional<U>::value && !assignable_from_optional<U>::value, int> = 0>
    MSTL_CONSTEXPR20 self& operator =(const optional<U>& x)
    noexcept(is_nothrow_constructible_v<T, U> && is_nothrow_assignable_v<T&, U>) {
        if (MSTL::addressof(x) == this) return *this;
        if (x) {
            if (have_value_)
                value_ = MSTL::move(*x);
            else
                MSTL::construct(&value_, MSTL::move(*x));
        }
        else
            reset();
        return *this;
    }

    template <typename ...Types, enable_if_t<is_constructible_v<T, Types...>, int> = 0>
    constexpr explicit optional(inplace_construct_tag, Types&&... args)
    noexcept(is_nothrow_constructible_v<T, Types...>)
    : have_value_(true), value_(MSTL::forward<Types>(args)...) {}

    template <class U, class ...Types, enable_if_t<is_constructible_v<T, std::initializer_list<U>&, Types...>, int> = 0>
    constexpr explicit optional(inplace_construct_tag, std::initializer_list<U> ilist, Types &&...args)
    noexcept(is_nothrow_constructible_v<T, std::initializer_list<U>&, Types...>)
    : have_value_(true), value_(ilist, MSTL::forward<Types>(args)...) {}

    MSTL_CONSTEXPR20 ~optional() noexcept {
        if (have_value_) {
            MSTL::destroy(&value_);
            have_value_ = false;
        }
    }

    template <typename... Types, enable_if_t<is_constructible_v<T, Types...>, int> = 0>
    MSTL_CONSTEXPR20 void emplace(Types&&... args)
    noexcept(is_nothrow_constructible_v<T, Types...>) {
        reset();
        MSTL::construct(&value_, MSTL::forward<Types>(args)...);
        have_value_ = true;
    }

    template <typename U, typename... Types,
        enable_if_t<is_constructible_v<T, std::initializer_list<U>&, Types...>, int> = 0>
    MSTL_CONSTEXPR20 void emplace(std::initializer_list<U> ilist, Types&&... args)
    noexcept(is_nothrow_constructible_v<T, std::initializer_list<U>&, Types...>) {
        reset();
        MSTL::construct(&value_, ilist, MSTL::forward<Types>(args)...);
        have_value_ = true;
    }

    MSTL_CONSTEXPR20 void reset() noexcept {
        if (have_value_) {
            MSTL::destroy(&value_);
            have_value_ = false;
        }
    }

    MSTL_NODISCARD constexpr bool has_value() const noexcept {
        return have_value_;
    }
    constexpr explicit operator bool() const noexcept {
        return have_value_;
    }

    constexpr const_reference value() const & {
        Exception(have_value_, OptionalAccessError());
        return value_;
    }
    constexpr reference value() & {
        Exception(have_value_, OptionalAccessError());
        return value_;
    }
    constexpr const value_type&& value() const && {
        Exception(have_value_, OptionalAccessError());
        return MSTL::move(value_);
    }
    constexpr value_type&& value() && {
        Exception(have_value_, OptionalAccessError());
        return MSTL::move(value_);
    }

    constexpr value_type value_or(value_type value) const & {
        if (!have_value_)
            return value;
        return value_;
    }
    constexpr value_type value_or(value_type value) && noexcept {
        if (!have_value_)
            return value;
        return MSTL::move(value_);
    }

    template <typename F, enable_if_t<is_copy_constructible_v<T>, int> = 0>
    constexpr self or_else(F&& f) const & {
        if (have_value_) {
            return *this;
        }
        return MSTL::forward<F>(f)();
    }
    template <typename F, enable_if_t<is_move_constructible_v<T>, int> = 0>
    constexpr self or_else(F &&f) && {
        if (have_value_) {
            return MSTL::move(*this);
        }
        return MSTL::forward<F>(f)();
    }

    template <typename F>
    constexpr decltype(auto) and_then(F&& f) const & {
        if (have_value_) {
            return MSTL::forward<F>(f)(value_);
        }
        return remove_cv_t<remove_reference_t<decltype(f(value_))>>{};
    }
    template <typename F>
    constexpr decltype(auto) and_then(F&& f) & {
        if (have_value_) {
            return MSTL::forward<F>(f)(value_);
        }
        return remove_cv_t<remove_reference_t<decltype(f(value_))>>{};
    }
    template <typename F>
    constexpr decltype(auto) and_then(F&& f) const && {
        if (have_value_) {
            return MSTL::forward<F>(f)(MSTL::move(value_));
        }
        return remove_cv_t<remove_reference_t<decltype(f(MSTL::move(value_)))>>{};
    }
    template <typename F>
    constexpr decltype(auto) and_then(F&& f) && {
        if (have_value_) {
            return MSTL::forward<F>(f)(MSTL::move(value_));
        }
        return remove_cv_t<remove_reference_t<decltype(f(MSTL::move(value_)))>>{};
    }

    template <typename F>
    constexpr auto transform(F&& f) const & -> optional<remove_cvref_t<decltype(f(value_))>> {
        if (have_value_) {
            return MSTL::forward<F>(f)(value_);
        }
        return nullopt;
    }
    template <typename F>
    constexpr auto transform(F&& f) & -> optional<remove_cvref_t<decltype(f(value_))>> {
        if (have_value_) {
            return MSTL::forward<F>(f)(value_);
        }
        return nullopt;
    }
    template <typename F>
    constexpr auto transform(F&& f) const && -> optional<remove_cvref_t<decltype(f(MSTL::move(value_)))>> {
        if (have_value_) {
            return MSTL::forward<F>(f)(MSTL::move(value_));
        }
        return nullopt;
    }
    template <typename F>
    constexpr auto transform(F&& f) && -> optional<remove_cvref_t<decltype(f(MSTL::move(value_)))>> {
        if (have_value_) {
            return MSTL::forward<F>(f)(MSTL::move(value_));
        }
        return nullopt;
    }

    constexpr const_pointer operator ->() const noexcept {
        return MSTL::addressof(value_);
    }
    constexpr pointer operator ->() noexcept {
        return MSTL::addressof(value_);
    }

    constexpr const_reference operator *() const & noexcept {
        return value_;
    }
    constexpr reference operator *() & noexcept {
        return value_;
    }
    constexpr const value_type&& operator *() const && noexcept {
        return MSTL::move(value_);
    }
    constexpr value_type&& operator *() && noexcept {
        return MSTL::move(value_);
    }

    constexpr bool operator ==(nullopt_t) const noexcept {
        return !have_value_;
    }
    friend constexpr bool operator ==(nullopt_t, const self& rh) noexcept {
        return !rh.have_value_;
    }
    constexpr bool operator ==(const self& rh) const noexcept {
        if (have_value_ != rh.have_value_)
            return false;
        if (have_value_)
            return value_ == rh.value_;
        return true;
    }

    constexpr bool operator !=(nullopt_t) const noexcept {
        return have_value_;
    }
    friend constexpr bool operator !=(nullopt_t, const self& rh) noexcept {
        return rh.have_value_;
    }
    constexpr bool operator !=(const self& rh) const noexcept {
        return !(*this == rh);
    }

    constexpr bool operator >(nullopt_t) const noexcept {
        return have_value_;
    }
    friend constexpr bool operator >(nullopt_t, const self&) noexcept {
        return false;
    }
    constexpr bool operator >(const self& rh) const noexcept {
        if (!have_value_ || !rh.have_value_)
            return false;
        return value_ > rh.value_;
    }

    constexpr bool operator <(nullopt_t) const noexcept {
        return false;
    }
    friend constexpr bool operator <(nullopt_t, const self& rh) noexcept {
        return rh.have_value_;
    }
    constexpr bool operator <(const self& rh) const noexcept {
        return rh > *this;
    }

    constexpr bool operator >=(nullopt_t) const noexcept {
        return true;
    }
    friend constexpr bool operator >=(nullopt_t, const self& rh) noexcept {
        return !rh.have_value_;
    }
    constexpr bool operator >=(const self& rh) const noexcept {
        return !(*this < rh);
    }

    constexpr bool operator <=(nullopt_t) const noexcept {
        return !have_value_;
    }
    friend constexpr bool operator <=(nullopt_t, const self& rh) noexcept {
        return true;
    }
    constexpr bool operator <=(const self& rh) const noexcept {
        return !(*this < rh);
    }

    MSTL_CONSTEXPR20 void swap(self& x)
    noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_swappable_v<T>) {
        if(MSTL::addressof(x) == this) return;
        if (have_value_ && x.have_value_) {
            MSTL::swap(value_, x.value_);
        }
        else if (!have_value_ && !x.have_value_) {}
        else if (have_value_) {
            x.emplace(MSTL::move(value_));
            reset();
        }
        else {
            emplace(MSTL::move(x.value_));
            x.reset();
        }
    }
};
#ifdef MSTL_SUPPORT_DEDUCTION_GUIDES__
template <class T>
optional(T) -> optional<T>;
#endif

template <typename T, enable_if_t<is_move_constructible_v<T> && is_swappable_v<T>, int> = 0>
MSTL_CONSTEXPR20 void swap(optional<T>& lh, optional<T>& rh) noexcept {
    lh.swap(rh);
}

template <typename T, enable_if_t<is_constructible_v<decay_t<T>, T>, int> = 0>
constexpr optional<decay_t<T>> make_optional(T&& value)
noexcept(is_nothrow_constructible_v<optional<decay_t<T>>, T>) {
    return optional<decay_t<T>>{ MSTL::forward<T>(value) };
}

template <typename T, typename... Args, enable_if_t<is_constructible_v<T, Args...>, int> = 0>
constexpr optional<T> make_optional(Args&&... args)
noexcept(is_nothrow_constructible_v<T, Args...>) {
    return optional<T>{ inplace_construct_tag{}, MSTL::forward<Args>(args)... };
}

template <typename T, typename U, typename... Args>
constexpr enable_if_t<is_constructible_v<T, std::initializer_list<U>&, Args...>,
optional<T>> make_optional(std::initializer_list<U> ilist, Args&&... args)
noexcept(is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>) {
    return optional<T>{ inplace_construct_tag{}, ilist, MSTL::forward<Args>(args)... };
}

template <typename T>
struct hash<optional<T>> {
    size_t operator()(const optional<T>& x) const noexcept {
        return hash<T>()(*x);
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_OPTIONAL_HPP__
