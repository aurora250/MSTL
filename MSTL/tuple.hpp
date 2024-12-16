#ifndef MSTL_TUPLE_HPP__
#define MSTL_TUPLE_HPP__
#include "basiclib.h"
MSTL_BEGIN_NAMESPACE__

#ifdef MSTL_SUPPORT_CONCEPTS__

template<typename... Types>
class __tuple_impl;

MSTL_TEMPLATE_NULL__ struct __tuple_impl<> {
    explicit __tuple_impl() = default;
    ~__tuple_impl() noexcept = default;
};

template<typename Head, typename... Tail>
class __tuple_impl<Head, Tail...> : public __tuple_impl<Tail...> {
public:
    typedef Head value_type;
    typedef __tuple_impl<Tail...> base_type;
    
    value_type value;
    
    __tuple_impl() {};
    explicit __tuple_impl(Head&& head, Tail&&... tail)
        : base_type(std::forward<Tail>(tail)...), value(std::forward<Head>(head)) {}
    ~__tuple_impl() noexcept = default;
};


template <size_t, typename...>
struct tuple_helper;
template <typename... Types>
struct tuple_helper<0, Types...> {
    using type = __tuple_impl<Types...>;
};
template <size_t Index, typename Head, typename... Tail> requires (Index != 0)
struct tuple_helper<Index, Head, Tail...> {
    using type = typename tuple_helper<Index - 1, Tail...>::type;
};

template <size_t Index, typename... Types>
struct tuple_element;
template <typename Head, typename... Tail>
struct tuple_element<0, Head, Tail...> {
    using type = Head;
};
template <size_t Index, typename Head, typename... Tail> requires (Index != 0)
struct tuple_element<Index, Head, Tail...> {
    using type = typename tuple_element<Index - 1, Tail...>::type;
};

template <typename... Types>
class tuple;

template <typename... Types>
class __tuple : __tuple_impl<Types...> {
public:
    typedef __tuple_impl<Types...> base_type;
    
    explicit __tuple(Types&&... args) : base_type(std::forward<Types>(args)...) {}
    ~__tuple() noexcept = default;

    template <size_t Index, typename... ElemTypes>
    friend decltype(auto) get(const __tuple<ElemTypes...>& tuple);
    template <size_t Index, typename... ElemTypes>
    friend decltype(auto) get(__tuple<ElemTypes...>&& tuple);
    friend class tuple<Types...>;
    
    template <size_t Index, typename... ElemTypes>
    friend decltype(auto) get(const tuple<ElemTypes...>& tuple);
    template <size_t Index, typename... ElemTypes>
    friend decltype(auto) get(tuple<ElemTypes...>&& tuple);
};
MSTL_TEMPLATE_NULL__ class __tuple<> {};

template <size_t Index, typename... ElemTypes>
decltype(auto) get(const __tuple<ElemTypes...>& tup) {
    using base_type = typename tuple_helper<Index, ElemTypes...>::type;
    return (static_cast<const base_type&>(tup).value);
}

template <size_t Index, typename... ElemTypes>
decltype(auto) get(__tuple<ElemTypes...>&& tup) {
    using base_type = typename tuple_helper<Index, ElemTypes...>::type;
    using value_type = typename tuple_element<Index, ElemTypes...>::type;
    return std::forward<value_type>(static_cast<base_type&&>(tup).value);
}

template <typename... Types>
class tuple {
public:
    using base_type = __tuple<Types...>;
    using size_type = size_t;
private:
    base_type* tup;
    size_type _size;
public:
    explicit tuple(Types&&... args) : tup(nullptr), _size(sizeof...(Types)) {
        tup = new base_type(std::forward<Types>(args)...);
    }
    ~tuple() noexcept {
        delete tup;
        _size = 0;
    }
    template <size_type idx> requires (idx < sizeof...(Types))
    decltype(auto) get() {
        using base = typename tuple_helper<idx, Types...>::type;
        return (static_cast<const base*>(tup)->value);
    }
    size_type size() const { return _size; }

    template <size_t Index, typename... ElemTypes>
    friend decltype(auto) get(const tuple<ElemTypes...>& tuple);
    template <size_t Index, typename... ElemTypes>
    friend decltype(auto) get(tuple<ElemTypes...>&& tuple);
};

template <size_t Index, typename... ElemTypes>
decltype(auto) get(const tuple<ElemTypes...>& tup) {
    using base_type = typename tuple_helper<Index, ElemTypes...>::type;
    return (static_cast<const base_type&>(*(tup.tup)).value);
}

template <size_t Index, typename... ElemTypes>
decltype(auto) get(tuple<ElemTypes...>&& tup) {
    using base_type = typename tuple_helper<Index, ElemTypes...>::type;
    using value_type = typename tuple_element<Index, ElemTypes...>::type;
    return std::forward<value_type>(static_cast<base_type&&>(*(tup.tup)).value);
}

template <typename... Types>
tuple<Types...> make_tuple(Types&&... args) {
    return tuple<Types...>(std::forward<Types>(args)...);
}

#endif // MSTL_SUPPORT_CONCEPTS__

MSTL_END_NAMESPACE__

#endif // MSTL_TUPLE_HPP__
