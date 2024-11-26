#ifndef MSTL_TUPLE_H__
#define MSTL_TUPLE_H__
#include "basiclib.h"
MSTL_BEGIN_NAMESPACE__

template<size_t Index, typename T>
struct _tuple_element {
    T value;
    _tuple_element() : value(T()) {}
    explicit _tuple_element(const T& val) : value(val) {}
    explicit _tuple_element(T&& val) : value(std::move(val)) {}
};

template<size_t Index, typename... Types>
class _tuple_impl;

template<size_t Index, typename Head, typename... Tail>
class _tuple_impl<Index, Head, Tail...> :
    _tuple_impl<Index + 1, Tail...>, _tuple_element<Index, Head> {
private:
    typedef Head value_type;
    using _tuple_element<Index, Head>::value;
public:
    _tuple_impl() {};
    explicit _tuple_impl(const Head& head, const Tail&... tail)
        : _tuple_element<Index, Head>(head), _tuple_impl<Index + 1, Tail...>(tail...) {}

    explicit _tuple_impl(Head&& head, Tail&&... tail)
        : _tuple_element<Index, Head>(std::forward<Head>(head)),
          _tuple_impl<Index + 1, Tail...>(std::forward<Tail>(tail)...) {}
};
template<size_t Index>
class _tuple_impl<Index> {};

template<typename... Types>
class tuple : _tuple_impl<0, Types...> {
private:
    using Base = _tuple_impl<0, Types...>;
public:
    tuple() {};
    explicit tuple(const Types&... args) : Base(args...) {}
    explicit tuple(Types&&... args) : Base(std::forward<Types>(args)...) {}
};
template<>
class tuple<> {};   // µ›πÈ÷’÷π


template<size_t Index, typename T>
T& get(_tuple_element<Index, T>& element) {
    return element.value;
}
template<size_t Index, typename Head, typename... Tail>
auto& get(_tuple_impl<Index, Head, Tail...>& tuple) {
    return get<Index>(static_cast<_tuple_element<Index, Head>&>(tuple));
}

template<size_t Index, typename T>
const T& get(const _tuple_element<Index, T>& element) {
    return element.value;
}
template<size_t Index, typename Head, typename... Tail>
const auto& get(const _tuple_impl<Index, Head, Tail...>& tuple) {
    return get<Index>(static_cast<const _tuple_element<Index, Head>&>(tuple));
}

template<typename... Types>
tuple<Types...> make_tuple(Types&&... args) {
    return tuple<Types...>(std::forward<Types>(args)...);
}

MSTL_END_NAMESPACE__
#endif // MSTL_TUPLE_H__
