#ifndef MSTL_VARIANT_HPP__
#define MSTL_VARIANT_HPP__
#include "algobase.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename, typename>
struct variant_index;
template <typename Variant, typename T>
constexpr size_t variant_index_v = variant_index<Variant, T>::value;

template <typename, size_t>
struct variant_alternative;
template <typename Variant, size_t Idx>
using variant_alternative_t = typename variant_alternative<Variant, Idx>::type;

template <typename... Types>
struct variant {
private:
    using self = variant<Types...>;

    size_t index_;

    alignas(MSTL::max({ alignof(Types)... })) char union_[MSTL::max({ sizeof(Types)... })]{};

    using destruct_function = void(*)(char*);

    static destruct_function* destructors_table() noexcept {
        static destruct_function function_ptrs[sizeof...(Types)] = {
            [](char* union_p) noexcept {
                reinterpret_cast<Types*>(union_p)->~Types();
            }...
        };
        return function_ptrs;
    }

    using copy_construct_function = void(*)(char*, char const*);

    static copy_construct_function* copy_constructors_table() noexcept {
        static copy_construct_function function_ptrs[sizeof...(Types)] = {
            [](char* union_dst, char const* union_src) noexcept {
                new (union_dst) Types(*reinterpret_cast<Types const*>(union_src));
            }...
        };
        return function_ptrs;
    }

    using copy_assignment_function = void(*)(char*, char const*);

    static copy_assignment_function* copy_assigment_functions_table() noexcept {
        static copy_assignment_function function_ptrs[sizeof...(Types)] = {
            [](char* union_dst, char const* union_src) noexcept {
                *reinterpret_cast<Types*>(union_dst) = *reinterpret_cast<Types const*>(union_src);
            }...
        };
        return function_ptrs;
    }

    using move_construct_function = void(*)(char*, char*);

    static move_construct_function* move_constructors_table() noexcept {
        static move_construct_function function_ptrs[sizeof...(Types)] = {
            [](char* union_dst, const char* union_src) noexcept {
                new (union_dst) Types(MSTL::move(*reinterpret_cast<Types const*>(union_src)));
            }...
        };
        return function_ptrs;
    }

    using move_assignment_function = void(*)(char*, char*);

    static move_assignment_function* move_assigment_functions_table() noexcept {
        static move_assignment_function function_ptrs[sizeof...(Types)] = {
            [](char* union_dst, const char* union_src) noexcept {
                *reinterpret_cast<Types*>(union_dst) = MSTL::move(*reinterpret_cast<Types*>(union_src));
            }...
        };
        return function_ptrs;
    }

    template <class Lambda>
    using const_visitor_function = common_type_t<
        MSTL::invoke_result_t<Lambda, Types const&>...>(*)(char const*, Lambda&&);

    template <class Lambda>
    static const_visitor_function<Lambda>* const_visitors_table() noexcept {
        static const_visitor_function<Lambda> function_ptrs[sizeof...(Types)] = {
            [](char const* union_p, Lambda&& lambda) -> MSTL::invoke_result_t<Lambda, Types const&> {
                MSTL::invoke(MSTL::forward<Lambda>(lambda), *reinterpret_cast<Types const*>(union_p));
            }...
        };
        return function_ptrs;
    }

    template <class Lambda>
    using visitor_function = common_type_t<MSTL::invoke_result_t<Lambda, Types&>...>(*)(char*, Lambda&&);

    template <class Lambda>
    static visitor_function<Lambda>* visitors_table() noexcept {
        static visitor_function<Lambda> function_ptrs[sizeof...(Types)] = {
            [](char* union_p, Lambda&& lambda) -> common_type_t<MSTL::invoke_result_t<Lambda, Types&>...> {
                return MSTL::invoke(MSTL::forward<Lambda>(lambda), *reinterpret_cast<Types*>(union_p));
            }...
        };
        return function_ptrs;
    }

public:
    variant() : index_(0) {
        new (union_) typename variant_alternative<variant, 0>::type();
    }

    template <typename T, enable_if_t<disjunction_v<is_same<T, Types>...>, int> = 0>
    variant(T&& value) : index_(variant_index_v<variant, T>) {
        T* p = reinterpret_cast<T*>(union_);
        new (p) T(MSTL::forward<T>(value));
    }

    variant(const self& x) : index_(x.index_) {
        copy_constructors_table()[index()](union_, x.union_);
    }
    self& operator =(const self& x) {
        if(MSTL::addressof(x) == this) return *this;
        index_ = x.index_;
        copy_assigment_functions_table()[index()](union_, x.union_);
        return *this;
    }

    variant(self&& x) noexcept : index_(x.index_) {
        move_constructors_table()[index()](union_, x.union_);
    }
    self& operator =(self&& x) noexcept {
        if(MSTL::addressof(x) == this) return *this;
        index_ = x.index_;
        move_assigment_functions_table()[index()](union_, x.union_);
        return *this;
    }

    template <size_t Idx, typename... Args>
    explicit variant(exact_arg_construct_tag, Args&&... value_args) : index_(Idx) {
        new (union_) variant_alternative_t<variant, Idx> (MSTL::forward<Args>(value_args)...);
    }
    template <typename... Args, enable_if_t<(is_constructible_v<Types, Args...> || ...), int> = 0>
    variant(Args&&... args) {
        auto construct = [&]<size_t... Is>(MSTL::index_sequence<Is...>) {
            ((is_constructible_v<variant_alternative_t<variant, Is>, Args...> && [&]() {
                if constexpr (is_constructible_v<variant_alternative_t<variant, Is>, Args...>) {
                    index_ = Is;
                    new (union_) variant_alternative_t<variant, Is>(MSTL::forward<Args>(args)...);
                    return true;
                }
                return false;
            }()) || ...);
        };
        construct(MSTL::make_index_sequence<sizeof...(Types)>{});
    }

    ~variant() noexcept {
        destructors_table()[index()](union_);
    }

    template <typename Lambda>
    common_type_t<MSTL::invoke_result_t<Lambda, Types&>...> visit(Lambda&& lambda) noexcept {
        return visitors_table<Lambda>()[index()](union_, MSTL::forward<Lambda>(lambda));
    }
    template <typename Lambda>
    common_type_t<MSTL::invoke_result_t<Lambda, Types const&>...> visit(Lambda&& lambda) const noexcept {
        return const_visitors_table<Lambda>()[index()](union_, MSTL::forward<Lambda>(lambda));
    }

    MSTL_NODISCARD constexpr size_t index() const noexcept {
        return index_;
    }
    template <typename T>
    MSTL_NODISCARD constexpr bool holds_alternative() const noexcept {
        return variant_index_v<variant, T> == index_;
    }

    template <size_t Idx>
    typename variant_alternative<variant, Idx>::type& get() {
        static_assert(Idx < sizeof...(Types), "variant index out of range.");
        Exception(index_ == Idx, ValueError("Template index not match."));
        return *reinterpret_cast<typename variant_alternative<variant, Idx>::type*>(union_);
    }

    template <typename T>
    T& get() {
        return get<variant_index_v<variant, T>>();
    }

    template <size_t Idx>
    variant_alternative_t<variant, Idx> const& get() const {
        static_assert(Idx < sizeof...(Types), "variant index out of range.");
        Exception(index_ == Idx, ValueError("Template index not match."));
        return *reinterpret_cast<variant_alternative_t<variant, Idx> const*>(union_);
    }

    template <typename T>
    T const& get() const {
        return get<variant_index_v<variant, T>>();
    }

    template <size_t Idx>
    variant_alternative_t<variant, Idx>* get_if() {
        static_assert(Idx < sizeof...(Types), "variant index out of range.");
        if (index_ != Idx) return nullptr;
        return reinterpret_cast<variant_alternative_t<variant, Idx>*>(union_);
    }

    template <typename T>
    T* get_if() {
        return get_if<variant_index_v<variant, T>>();
    }

    template <size_t Idx>
    variant_alternative_t<variant, Idx> const* get_if() const {
        static_assert(Idx < sizeof...(Types), "variant index out of range.");
        if (index_ != Idx) return nullptr;
        return reinterpret_cast<variant_alternative_t<variant, Idx> const*>(union_);
    }

    template <typename T>
    T const* get_if() const {
        return get_if<variant_index_v<variant, T>>();
    }

    template <size_t Idx, typename... Args>
    void emplace(Args&&... args) {
        destructors_table()[index()](union_);
        index_ = Idx;
        new (union_) variant_alternative_t<variant, Idx>(MSTL::forward<Args>(args)...);
    }
    template <size_t Idx, typename... Args>
    enable_if_t<Idx >= sizeof...(Types)> emplace(Args&&...) = delete;

    template <typename T, typename... Args>
    void emplace(Args&&... args) {
        emplace<variant_index_v<variant, T>>(MSTL::forward<Args>(args)...);
    }

    void swap(variant& x) noexcept {
        if (MSTL::addressof(x) == this) return;

        size_t this_index = index_;
        size_t other_index = x.index_;
        alignas(MSTL::max({ alignof(Types)... })) char temp_union[MSTL::max({ sizeof(Types)... })];
        move_constructors_table()[this_index](reinterpret_cast<char*>(temp_union), union_);
        destructors_table()[this_index](union_);

        move_constructors_table()[other_index](union_, x.union_);
        destructors_table()[other_index](x.union_);
        move_constructors_table()[this_index](x.union_, reinterpret_cast<char*>(temp_union));
        destructors_table()[this_index](reinterpret_cast<char*>(temp_union));

        index_ = other_index;
        x.index_ = this_index;
    }

    MSTL_NODISCARD bool operator ==(const variant& rh) const noexcept {
        if (index_ != rh.index_) return false;
        return visit([&](const auto& value) {
            return rh.visit([&](const auto& other_value) {
                return value == other_value;
            });
        });
    }
    MSTL_NODISCARD bool operator !=(const variant& rh) const noexcept {
        return !(*this == rh);
    }

    MSTL_NODISCARD bool operator <(const variant& rh) const noexcept {
        if (index_ < rh.index_) return true;
        if (index_ > rh.index_) return false;
        return visit([&](const auto& value) {
            return rh.visit([&](const auto& other_value) {
                return value < other_value;
            });
        });
    }
    MSTL_NODISCARD bool operator >(const variant& rh) const noexcept {
        return rh < *this;
    }
    MSTL_NODISCARD bool operator <=(const variant& rh) const noexcept {
        return !(*this > rh);
    }
    MSTL_NODISCARD bool operator >=(const variant& rh) const noexcept {
        return !(*this < rh);
    }
};

template <typename T, typename ...Types>
struct variant_alternative<variant<T, Types...>, 0> {
    using type = T;
};
template <typename T, typename ...Types, size_t Idx>
struct variant_alternative<variant<T, Types...>, Idx> {
    using type = typename variant_alternative<variant<Types...>, Idx - 1>::type;
};

template <typename T, typename ...Types>
struct variant_index<variant<T, Types...>, T> {
    static constexpr size_t value = 0;
};
template <typename T0, typename T, typename ...Types>
struct variant_index<variant<T0, Types...>, T> {
    static constexpr size_t value = variant_index<variant<Types...>, T>::value + 1;
};

template <size_t Idx, typename... Types>
variant_alternative_t<variant<Types...>, Idx>& get(variant<Types...>& v) {
    using T = variant_alternative_t<variant<Types...>, Idx>;
    using variant_type = variant<Types...>;
    return static_cast<T&>(static_cast<variant_type&>(v).template get<Idx>());
}
template <size_t Idx, typename... Types>
const variant_alternative_t<variant<Types...>, Idx>& get(const variant<Types...>& v) {
    using T = variant_alternative_t<variant<Types...>, Idx>;
    using variant_type = variant<Types...>;
    return static_cast<const T&>(static_cast<const variant_type&>(v).template get<Idx>());
}
template <size_t Idx, typename... Types>
variant_alternative_t<variant<Types...>, Idx>&& get(variant<Types...>&& v) {
    using T = variant_alternative_t<variant<Types...>, Idx>;
    using variant_type = variant<Types...>;
    return static_cast<T&&>(static_cast<variant_type&&>(v).template get<Idx>());
}
template <size_t Idx, typename... Types>
const variant_alternative_t<variant<Types...>, Idx>&& get(const variant<Types...>&& v) {
    using T = variant_alternative_t<variant<Types...>, Idx>;
    using variant_type = variant<Types...>;
    return static_cast<const T&&>(static_cast<const variant_type&&>(v).template get<Idx>());
}

template <class Lambda, typename... Types>
decltype(auto) visit(Lambda&& lambda, variant<Types...>& var) {
    using variant_type = variant<Types...>;
    return static_cast<variant_type&>(var).visit(MSTL::forward<Lambda>(lambda));
}
template <class Lambda, typename... Types>
decltype(auto) visit(Lambda&& lambda, const variant<Types...>& var) {
    using variant_type = variant<Types...>;
    return static_cast<const variant_type&>(var).visit(MSTL::forward<Lambda>(lambda));
}
template <class Lambda, typename... Types>
decltype(auto) visit(Lambda&& lambda, variant<Types...>&& var) {
    using variant_type = variant<Types...>;
    return static_cast<variant_type&&>(var).visit(MSTL::forward<Lambda>(lambda));
}
template <class Lambda, typename... Types>
decltype(auto) visit(Lambda&& lambda, const variant<Types...>&& var) {
    using variant_type = variant<Types...>;
    return static_cast<const variant_type&&>(var).visit(MSTL::forward<Lambda>(lambda));
}

template <typename... Types>
struct hash<MSTL::variant<Types...>> {
    size_t operator()(const MSTL::variant<Types...>& var) const {
        auto hasher = [](const auto& value) {
            return hash<decay_t<decltype(value)>>{}(value);
        };
        return var.visit(hasher);
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_VARIANT_HPP__
