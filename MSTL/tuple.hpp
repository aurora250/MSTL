#ifndef MSTL_TUPLE_HPP__
#define MSTL_TUPLE_HPP__
#include "basiclib.h"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename... Args>
struct tuple;

MSTL_END_NAMESPACE__

namespace std {
	template <typename... Types>
	struct tuple_size<MSTL::tuple<Types...>> 
		: integral_constant<size_t, sizeof...(Types)> {};  // size of MSTL tuple
}

MSTL_BEGIN_NAMESPACE__
MSTL_SET_CONCEPTS__

template <bool Same, typename Dest, typename... Srcs>
constexpr bool tuple_constructible_v0 = false;
template <typename... Dests, typename... Srcs>
constexpr bool tuple_constructible_v0<true, tuple<Dests...>, Srcs...> = 
	std::conjunction_v<std::is_constructible<Dests, Srcs>...>;
template <typename Dest, typename... Srcs>
constexpr bool tuple_constructible_v =
	tuple_constructible_v0<std::tuple_size_v<Dest> == sizeof...(Srcs), Dest, Srcs...>;
template <typename Dest, typename... Srcs>
struct tuple_constructible : std::bool_constant<tuple_constructible_v<Dest, Srcs...>> {};

template <typename Dest, typename... Srcs>
concept TupleConstructableFrom = tuple_constructible_v<Dest, Srcs...>;


template <bool Same, typename Dest, typename... Srcs>
constexpr bool tuple_explicitable_v0 = false;
template <typename... Dests, typename... Srcs>
constexpr bool tuple_explicitable_v0<true, tuple<Dests...>, Srcs...> =
	!std::conjunction_v<std::is_convertible<Srcs, Dests>...>;
template <typename Dest, typename... Srcs>
constexpr bool tuple_explicitable_v =
	tuple_explicitable_v0<std::tuple_size_v<Dest> == sizeof...(Srcs), Dest, Srcs...>;

template <typename Dest, typename... Srcs>
concept TupleExplicitable = tuple_explicitable_v<Dest, Srcs...>;


template <bool Same, typename Dest, typename... Srcs>
constexpr bool tuple_nothrow_constructible_v0 = false;
template <typename... Dests, typename... Srcs>
constexpr bool tuple_nothrow_constructible_v0<true, tuple<Dests...>, Srcs...> =
	std::conjunction_v<std::is_nothrow_constructible<Dests, Srcs>...>;
template <typename Dest, typename... Srcs>
constexpr bool tuple_nothrow_constructible_v =
	tuple_nothrow_constructible_v0<std::tuple_size_v<Dest> == sizeof...(Srcs), Dest, Srcs...>;

template <typename Dest, typename... Srcs>
concept TupleNothrowConstructableFrom = tuple_nothrow_constructible_v<Dest, Srcs...>;


template <typename Self, typename Tuple, typename... U>
struct tuple_convertable_v0 : std::true_type {};
template <typename Self, typename Tuple, typename U>
struct tuple_convertable_v0<tuple<Self>, Tuple, U>
	: std::bool_constant<!std::disjunction_v<std::is_same<Self, U>, 
	std::is_constructible<Self, Tuple>, std::is_convertible<Tuple, Self>>> {};
template <typename Self, typename Tuple, typename... U>
constexpr bool tuple_convertable_v = tuple_convertable_v0<Self, Tuple, U...>::value;

template <typename Self, typename Tuple, typename... U>
concept TupleConvertableFrom = tuple_convertable_v<Self, Tuple, U...>;


template <bool Same, typename Dest, typename... Srcs>
constexpr bool tuple_assignable_v0 = false;
template <typename... Dests, typename... Srcs>
constexpr bool tuple_assignable_v0<true, tuple<Dests...>, Srcs...> =
	std::conjunction_v<std::is_assignable<Dests&, Srcs>...>;
template <typename Dest, typename... Srcs>
constexpr bool tuple_assignable_v = 
	tuple_assignable_v0<std::tuple_size_v<Dest> == sizeof...(Srcs), Dest, Srcs...>;
template <typename Dest, typename... Srcs>
struct tuple_assignable : std::bool_constant<tuple_assignable_v<Dest, Srcs...>> {};

template <typename Dest, typename... Srcs>
concept TupleAssignableFrom = tuple_assignable<Dest, Srcs...>::value;


template <bool Same, typename Dest, typename... Srcs>
constexpr bool tuple_nothrow_assignable_v0 = false;
template <typename... Dests, typename... Srcs>
constexpr bool tuple_nothrow_assignable_v0<true, tuple<Dests...>, Srcs...> =
	std::conjunction_v<std::is_nothrow_assignable<Dests&, Srcs>...>;
template <typename Dest, typename... Srcs>
constexpr bool tuple_nothrow_assignable_v =
	tuple_nothrow_assignable_v0<std::tuple_size_v<Dest> == sizeof...(Srcs), Dest, Srcs...>;

template <typename Dest, typename... Srcs>
concept TupleNothrowAssignable = tuple_nothrow_assignable_v<Dest, Srcs...>;

MSTL_END_CONCEPTS__
MSTL_CONCEPTS__

template <size_t Index, typename... Types>
struct tuple_helper;
template <typename... Types>
struct tuple_helper<0, Types...> {
    using type = tuple<Types...>;
};
template <size_t Index, typename Head, typename... Tail> requires(Index != 0)
struct tuple_helper<Index, Head, Tail...> {
    using type = typename tuple_helper<Index - 1, Tail...>::type;
};
template <size_t Index, typename... Types>
using tuple_helper_t = tuple_helper<Index, Types...>;

template <size_t Index, typename... Types>
struct tuple_element;
template <typename This, typename... Rest>
struct tuple_element<0, This, Rest...> {
    using type = This;
};
template <size_t Index, typename This, typename... Rest> requires(Index != 0)
struct tuple_element<Index, This, Rest...> {
    using type = typename tuple_element<Index - 1, Rest...>::type;
};
template <size_t Index, typename... Types>
using tuple_element_t = typename tuple_element<Index, Types...>::type;


template <>
struct tuple<> {
	MSTL_CONSTEXPR tuple() noexcept = default;
	MSTL_CONSTEXPR tuple(const tuple&) noexcept = default;
	MSTL_CONSTEXPR tuple& operator =(const tuple&) noexcept = default;
	MSTL_CONSTEXPR void swap(const tuple&) noexcept {}
	MSTL_CONSTEXPR bool equal_to(const tuple&) const noexcept { return true; }
};

template <typename This, typename... Rest>
struct tuple<This, Rest...> : private tuple<Rest...> {
public:
	typedef This this_type;
	typedef tuple<Rest...> base_type;

	template <typename U1, typename... U2>
	MSTL_CONSTEXPR tuple(U1&& this_, U2&&... rest_) 
		: base_type(std::forward<U2>(rest_)...), data_(std::forward<U1>(this_)) {}

	template <typename T, size_t... Index>
	MSTL_CONSTEXPR tuple(T&& x, std::index_sequence<Index...>);  // will use get() to initialize
	/*template <typename T>
	MSTL_CONSTEXPR tuple(T&& x) : tuple(std::forward<T>(x), 
		std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>{}) {}*/

	template <typename T = This>
		requires(DefaultConstructible<T> && DefaultConstructible<Rest...>)
	MSTL_CONSTEXPR explicit(!(DefaultImplicitConstructible<T, Rest...>)) 
		tuple() noexcept(NothrowCopyConstructible<T, Rest...>)
		: base_type(), data_() {}

	template <typename T = This>
		requires(TupleConstructableFrom<tuple, const T&, const Rest&...>)
	MSTL_CONSTEXPR explicit(TupleExplicitable<tuple, const T&, const Rest&...>)
		tuple(const T& this_, const Rest&... rest_) 
		noexcept(NothrowCopyConstructible<T, Rest...>) 
		: base_type(rest_...), data_(this_) {}

	template <typename T, typename... U>
		requires(TupleConstructableFrom<tuple, T&, U&...>)
	MSTL_CONSTEXPR explicit(TupleExplicitable<tuple, T, U...>)
		tuple(T&& this_, U&&... rest_)
		noexcept(TupleNothrowConstructableFrom<tuple, T, U...>)
		: base_type(std::forward<U>(rest_)...), data_(std::forward<T>(this_)) {}

	tuple(const tuple&) = default;
	tuple(tuple&&)		= default;

	template <typename... U>
		requires(TupleConstructableFrom<tuple, const U&...> 
	&& TupleConvertableFrom<tuple, const tuple<U...>&, U...>)
	MSTL_CONSTEXPR explicit(TupleExplicitable<tuple, const U&...>)
		tuple(const tuple<U...>& t) 
		noexcept(TupleNothrowConstructableFrom<tuple, const U&...>)
		: tuple(t, std::make_index_sequence<std::tuple_size_v<
			std::remove_reference_t<const tuple<U...>&>>>{}) {
	}

	template <typename... U>
		requires(TupleConstructableFrom<tuple, U...>
	&& TupleConvertableFrom<tuple, const tuple<U...>&, U...>)
		MSTL_CONSTEXPR explicit(TupleExplicitable<tuple, U...>)
		tuple(tuple<U...>&& t)
		noexcept(TupleNothrowConstructableFrom<tuple, U...>)
		: tuple(std::move(t), std::make_index_sequence<std::tuple_size_v<
			std::remove_reference_t<tuple<U...>&&>>>{}) {}

	tuple& operator =(const volatile tuple&) = delete;

	template <typename Self = tuple, typename T = This>
		requires(CopyAssignable<T, Rest...>)
	tuple& operator =(const tuple& t)
		noexcept(NothrowCopyAssignable<T, Rest...>) {
		data_ = t.data_;
		get_rest() = t.get_rest();
		return *this;
	}
	template <typename Self = tuple, typename T = This>
		requires(MoveAssignable<T, Rest...>)
	tuple& operator =(tuple&& t)
		noexcept(NothrowMoveAssignable<T, Rest...>) {
		data_ = std::forward<T>(t.data_);
		get_rest() = std::forward<base_type>(t.get_rest());
		return *this;
	}

	template <typename... U>
		requires(std::conjunction_v<std::negation<std::is_same<tuple, tuple<U...>>>,
	tuple_assignable<tuple, const U&...>>)
	tuple& operator =(const tuple<U...>& t) 
		noexcept(TupleNothrowAssignable<tuple, const U&...>) {
		data_ = t.data_;
		get_rest() = t.get_rest();
		return *this;
	}
	template <typename... U>
		requires(std::conjunction_v<std::negation<std::is_same<tuple, tuple<U...>>>,
	tuple_assignable<tuple, U...>>)
	tuple& operator =(tuple<U...>&& t)
		noexcept(TupleNothrowAssignable<tuple, U...>) {
		data_ = std::forward<typename tuple<U...>::this_type>(t.data_);
		get_rest() = std::forward<typename tuple<U...>::base_type>(t.get_rest());
		return *this;
	}

	MSTL_CONSTEXPR void swap(tuple t) 
		noexcept(NothrowSwappable<This, Rest...>) {
		std::swap(data_, t.data_);
		base_type::swap(t.get_rest());
	}
	MSTL_CONSTEXPR base_type& get_rest() noexcept { return *this; }
	MSTL_CONSTEXPR const base_type& get_rest() const noexcept { return *this; }
	template <typename... U>
	MSTL_NODISCARD MSTL_CONSTEXPR bool equal_to(const tuple<U...>& t) const {
		return data_ == t.data_ && base_type::equal_to(t.get_rest());
	}
	template <typename... U>
	MSTL_NODISCARD MSTL_CONSTEXPR bool less_to(const tuple<U...>& rh) const {
		return data_ < rh.data_ || (!(rh.data_ < data_) && base_type::less_to(rh.get_rest()));
	}

	template <size_t Index, typename... Types>
	friend MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, Types...>& 
		get(tuple<Types...>& t) noexcept;
	template <size_t Index, typename... Types>
	friend MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, Types...>& 
		get(const tuple<Types...>& t) noexcept;
	template <size_t Index, typename... Types>
	friend MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, Types...>&& 
		get(tuple<Types...>&& t) noexcept;
	template <size_t Index, typename... Types>
	friend MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, Types...>&& 
		get(const tuple<Types...>&& t) noexcept;

private:
	this_type data_;
};

template <typename... T1, typename... T2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(const tuple<T1...>& lh, const tuple<T2...>& rh) {
	static_assert(sizeof...(T1) == sizeof...(T2), "cannot compare tuples of different sizes");
	return lh.equal_to(rh);
}
template <typename... T1, typename... T2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(const tuple<T1...>& lh, const tuple<T2...>& rh) {
	return !(lh == rh);
}
template <typename... T1, typename... T2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(const tuple<T1...>& lh, const tuple<T2...>& rh) {
	static_assert(sizeof...(T1) == sizeof...(T2), "cannot compare tuples of different sizes");
	return lh.less_to(rh);
}
template <typename... T1, typename... T2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(const tuple<T1...>& lh, const tuple<T2...>& rh) {
	return !(lh < rh);
}
template <typename... T1, typename... T2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(const tuple<T1...>& lh, const tuple<T2...>& rh) {
	return rh < lh;
}
template <typename... T1, typename... T2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(const tuple<T1...>& lh, const tuple<T2...>& rh) {
	return !(rh < lh);
}
template <typename... T>
	requires(Swappable<T...>)
MSTL_CONSTEXPR void swap(tuple<T...>& lh, tuple<T...>& rh) noexcept(noexcept(lh.swap(rh))) {
	lh.swap(rh);
}

template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, Types...>& 
get(tuple<Types...>& t) noexcept {
	using value_type = typename tuple_element<Index, Types...>::type;
	using base_type = typename tuple_helper<Index, Types...>::type;
	return static_cast<value_type&>(static_cast<base_type&>(t).data_);
}
template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, Types...>& 
get(const tuple<Types...>& t) noexcept {
	using value_type = typename tuple_element<Index, Types...>::type;
	using base_type = typename tuple_helper<Index, Types...>::type;
	return static_cast<const value_type&>(static_cast<const base_type&>(t).data_);
}
template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, Types...>&& 
get(tuple<Types...>&& t) noexcept {
	using value_type = typename tuple_element<Index, Types...>::type;
	using base_type = typename tuple_helper<Index, Types...>::type;
	return static_cast<value_type&&>(static_cast<base_type&&>(t).data_);
}
template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index,Types...>&& 
get(const tuple<Types...>&& t) noexcept {
	using value_type = typename tuple_element<Index, Types...>::type;
	using base_type = typename tuple_helper<Index, Types...>::type;
	return static_cast<const value_type&&>(static_cast<const base_type&&>(t).data_);
}

MSTL_SET_CONCEPTS__

template <typename T, typename Tuple, typename Sequence =
	std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>>
constexpr bool constructable_from_tuple = false;
template <typename T, typename Tuple, size_t... Index>
constexpr bool constructable_from_tuple<T, Tuple, std::index_sequence<Index...>> =
	std::is_constructible_v<T, decltype(MSTL::get<Index>(std::declval<Tuple>()))...>;

template <typename T, typename Tuple>
concept ConstructableFromTuple = constructable_from_tuple<T, Tuple>;

MSTL_END_CONCEPTS__

template <typename This, typename... Rest>
template <typename T, size_t... Index>
MSTL_CONSTEXPR tuple<This, Rest...>::tuple(T&& x, std::index_sequence<Index...>)
	: tuple(MSTL::get<Index>(std::forward<T>(x))...) {}

template <typename T, typename Tuple, size_t... Index>
MSTL_NODISCARD MSTL_CONSTEXPR T __make_from_tuple(Tuple&& t, std::index_sequence<Index...>)
noexcept(NothrowConstructibleFrom<T, decltype(MSTL::get<Index>(std::forward<Tuple>(t)))...>) {
	return T(MSTL::get<Index>(std::forward<Tuple>(t))...);
}
template <typename T, typename Tuple>
	requires(ConstructableFromTuple<T, Tuple>)
MSTL_NODISCARD MSTL_CONSTEXPR T make_from_tuple(Tuple&& t) 
noexcept(noexcept(__make_from_tuple<T>(std::forward<Tuple>(t),
	std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{}))) {
	return __make_from_tuple<T>(std::forward<Tuple>(t), 
		std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

template <typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) make_tuple(Types&&... args) {
	return tuple<Types...>(std::forward<Types>(args)...);
}
template <typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) tie(Types&... args) noexcept {
	return tuple<Types&...>(args...);
}
template <typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple<Types&&...> forward_as_tuple(Types&&... args) noexcept {
	return tuple<Types&&...>(std::forward<Types>(args)...);
}

MSTL_END_NAMESPACE__

#endif // MSTL_TUPLE_HPP__
