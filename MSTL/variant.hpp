#ifndef MSTL_VARIANT_HPP__
#define MSTL_VARIANT_HPP__
#include "algobase.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename, typename>
struct variant_index;
template <typename, size_t>
struct variant_alternative;

template <typename... Types>
struct variant {
private:
    size_t index_;

    alignas(MSTL::max({ alignof(Types)... })) char union_[MSTL::max({ sizeof(Types)... })]{};

    using destruct_function = void(*)(char*);

    static destruct_function* destructors_table() noexcept {
        static destruct_function function_ptrs[sizeof...(Types)] = {
            [](const char* union_p) noexcept {
                reinterpret_cast<Types*>(union_p)->~Ts();
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
            [](const char* union_p, Lambda&& lambda) -> common_type_t<MSTL::invoke_result_t<Lambda, Types&>...> {
                return MSTL::invoke(MSTL::forward<Lambda>(lambda), *reinterpret_cast<Types*>(union_p));
            }...
        };
        return function_ptrs;
    }

public:
    template <typename T, enable_if_t<disjunction_v<is_same<T, Types>...>, int> = 0>
    explicit variant(T value) : index_(variant_index<variant, T>::value) {
        T* p = reinterpret_cast<T*>(union_);
        new (p) T(value);
    }

    variant(variant const& that) : index_(that.index_) {
        copy_constructors_table()[index()](union_, that.union_);
    }

    variant& operator =(variant const& that) {
        if(MSTL::addressof(that) == this) return *this;
        index_ = that.index_;
        copy_assigment_functions_table()[index()](union_, that.union_);
        return *this;
    }

    variant(variant&& that) noexcept : index_(that.index_) {
        move_constructors_table()[index()](union_, that.union_);
    }

    variant& operator =(variant&& that) noexcept {
        if(MSTL::addressof(that) == this) return *this;
        index_ = that.index_;
        move_assigment_functions_table()[index()](union_, that.union_);
        return *this;
    }

    template <size_t Idx, typename... Args>
    explicit variant(exact_arg_construct_tag, Args&&... value_args) : index_(Idx) {
        new (union_) typename variant_alternative<variant, Idx>::type (MSTL::forward<Args>(value_args)...);
    }

    ~variant() noexcept {
        destructors_table()[index()](union_);
    }

    template <class Lambda>
    common_type_t<MSTL::invoke_result_t<Lambda, Types&>...> visit(Lambda&& lambda) {
        return visitors_table<Lambda>()[index()](union_, MSTL::forward<Lambda>(lambda));
    }

    template <class Lambda>
    common_type_t<MSTL::invoke_result_t<Lambda, Types const&>...> visit(Lambda&& lambda) const {
        return const_visitors_table<Lambda>()[index()](union_, MSTL::forward<Lambda>(lambda));
    }

    MSTL_NODISCARD constexpr size_t index() const noexcept {
        return index_;
    }
    template <typename T>
    MSTL_NODISCARD constexpr bool holds_alternative() const noexcept {
        return variant_index<variant, T>::value == index();
    }

    template <size_t Idx>
    typename variant_alternative<variant, Idx>::type& get() {
        static_assert(Idx < sizeof...(Types), "variant index out of range.");
        Exception(index_ == Idx, ValueError("Template index not match."));
        return *reinterpret_cast<typename variant_alternative<variant, Idx>::type*>(union_);
    }

    template <typename T>
    T& get() {
        return get<variant_index<variant, T>::value>();
    }

    template <size_t Idx>
    typename variant_alternative<variant, Idx>::type const& get() const {
        static_assert(Idx < sizeof...(Types), "variant index out of range.");
        Exception(index_ == Idx, ValueError("Template index not match."));
        return *reinterpret_cast<typename variant_alternative<variant, Idx>::type const*>(union_);
    }

    template <typename T>
    T const& get() const {
        return get<variant_index<variant, T>::value>();
    }

    template <size_t Idx>
    typename variant_alternative<variant, Idx>::type* get_if() {
        static_assert(Idx < sizeof...(Types), "variant index out of range.");
        if (index_ != Idx) return nullptr;
        return reinterpret_cast<typename variant_alternative<variant, Idx>::type*>(union_);
    }

    template <typename T>
    T* get_if() {
        return get_if<variant_index<variant, T>::value>();
    }

    template <size_t Idx>
    typename variant_alternative<variant, Idx>::type const* get_if() const {
        static_assert(Idx < sizeof...(Types), "variant index out of range.");
        if (index_ != Idx) return nullptr;
        return reinterpret_cast<typename variant_alternative<variant, Idx>::type const*>(union_);
    }

    template <typename T>
    T const* get_if() const {
        return get_if<variant_index<variant, T>::value>();
    }
};

template <typename T, typename ...Ts>
struct variant_alternative<variant<T, Ts...>, 0> {
    using type = T;
};
template <typename T, typename ...Ts, size_t I>
struct variant_alternative<variant<T, Ts...>, I> {
    using type = typename variant_alternative<variant<Ts...>, I - 1>::type;
};

template <typename T, typename ...Ts>
struct variant_index<variant<T, Ts...>, T> {
    static constexpr size_t value = 0;
};
template <typename T0, typename T, typename ...Ts>
struct variant_index<variant<T0, Ts...>, T> {
    static constexpr size_t value = variant_index<variant<Ts...>, T>::value + 1;
};

MSTL_END_NAMESPACE__
#endif // MSTL_VARIANT_HPP__
