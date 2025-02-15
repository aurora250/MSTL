#ifndef MSTL_TUPLE_HPP__
#define MSTL_TUPLE_HPP__
#include "utility.hpp"
MSTL_BEGIN_NAMESPACE__

template <bool Same, typename Dest, typename... Srcs>
constexpr bool tuple_constructible_v0 = false;
template <typename... Dests, typename... Srcs>
constexpr bool tuple_constructible_v0<true, tuple<Dests...>, Srcs...> = 
	conjunction_v<is_constructible<Dests, Srcs>...>;

template <typename Dest, typename... Srcs>
constexpr bool tuple_constructible_v =
	tuple_constructible_v0<tuple_size_v<Dest> == sizeof...(Srcs), Dest, Srcs...>;

template <typename Dest, typename... Srcs>
struct tuple_constructible : bool_constant<tuple_constructible_v<Dest, Srcs...>> {};

template <typename Dest, typename... Srcs>
concept TupleConstructibleFrom = tuple_constructible_v<Dest, Srcs...>;


template <bool Same, typename Dest, typename... Srcs>
constexpr bool tuple_explicit_v0 = false;
template <typename... Dests, typename... Srcs>
constexpr bool tuple_explicit_v0<true, tuple<Dests...>, Srcs...> = 
!conjunction_v<is_convertible<Srcs, Dests>...>;

template <typename Dest, typename... Srcs>
constexpr bool tuple_explicitly_convertible_v =
tuple_explicit_v0<tuple_size_v<Dest> == sizeof...(Srcs), Dest, Srcs...>;

template <typename Dest, typename... Srcs>
struct tuple_explicitly_convertible : bool_constant<tuple_explicitly_convertible_v<Dest, Srcs...>> {};

template <typename Dest, typename... Srcs>
concept TupleExplicitlyConvertible = tuple_explicitly_convertible_v<Dest, Srcs...>;


template <typename, typename, typename...>
struct tuple_perfect_forward : true_type {};
template <typename Tuple1, typename Tuple2>
struct tuple_perfect_forward<Tuple1, Tuple2> : bool_constant<!is_same_v<Tuple1, remove_cvref_t<Tuple2>>> {};

template <typename T1, typename T2, typename U1, typename U2>
struct tuple_perfect_forward<tuple<T1, T2>, U1, U2>
	: bool_constant<disjunction_v<negation<is_same<remove_cvref_t<U1>, std::allocator_arg_t>>,
	is_same<remove_cvref_t<T1>, std::allocator_arg_t>>> {};

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
struct tuple_perfect_forward<tuple<T1, T2, T3>, U1, U2, U3>
	: bool_constant<disjunction_v<negation<is_same<remove_cvref_t<U1>, std::allocator_arg_t>>,
	is_same<remove_cvref_t<T1>, std::allocator_arg_t>>> {};


template <bool Same, typename Dest, typename... Srcs>
constexpr bool tuple_nothrow_constructible_v0 = false;
template <typename... Dests, typename... Srcs>
constexpr bool tuple_nothrow_constructible_v0<true, tuple<Dests...>, Srcs...> =
conjunction_v<is_nothrow_constructible<Dests, Srcs>...>;

template <typename Dest, typename... Srcs>
constexpr bool tuple_nothrow_constructible_v =
	tuple_nothrow_constructible_v0<tuple_size_v<Dest> == sizeof...(Srcs), Dest, Srcs...>;
template <typename Dest, typename... Srcs>
concept TupleNothrowConstructibleFrom = tuple_nothrow_constructible_v<Dest, Srcs...>;


template <typename Self, typename Tuple, typename... U>
struct tuple_convertible_v0 : true_type {};
template <typename Self, typename Tuple, typename U>
struct tuple_convertible_v0<tuple<Self>, Tuple, U>
	: bool_constant<!disjunction_v<
	is_same<Self, U>, is_constructible<Self, Tuple>, is_convertible<Tuple, Self>>> {};
template <typename Self, typename Tuple, typename... U>
constexpr bool tuple_convertible_v = tuple_convertible_v0<Self, Tuple, U...>::value;

template <typename Self, typename Tuple, typename... U>
struct tuple_convertible : bool_constant<tuple_convertible_v<Self, Tuple, U...>> {};

template <typename Self, typename Tuple, typename... U>
concept TupleConvertibleFrom = tuple_convertible_v<Self, Tuple, U...>;


template <bool Same, typename Dest, typename... Srcs>
constexpr bool tuple_assignable_v0 = false;
template <typename... Dests, typename... Srcs>
constexpr bool tuple_assignable_v0<true, tuple<Dests...>, Srcs...> = conjunction_v<
	is_assignable<Dests&, Srcs>...>;

template <typename Dest, typename... Srcs>
constexpr bool tuple_assignable_v = tuple_assignable_v0<
	tuple_size_v<Dest> == sizeof...(Srcs), Dest, Srcs...>;

template <typename Dest, typename... Srcs>
struct tuple_assignable : bool_constant<tuple_assignable_v<Dest, Srcs...>> {};

template <typename Dest, typename... Srcs>
concept TupleAssignableFrom = tuple_assignable<Dest, Srcs...>::value;


template <bool Same, typename Dest, typename... Srcs>
constexpr bool tuple_nothrow_assignable_v0 = false;
template <typename... Dests, typename... Srcs>
constexpr bool tuple_nothrow_assignable_v0<true, tuple<Dests...>, Srcs...> =
	conjunction_v<is_nothrow_assignable<Dests&, Srcs>...>;

template <typename Dest, typename... Srcs>
constexpr bool tuple_nothrow_assignable_v =	tuple_nothrow_assignable_v0<
	tuple_size_v<Dest> == sizeof...(Srcs), Dest, Srcs...>;

template <typename Dest, typename... Srcs>
struct tuple_nothrow_assignable : bool_constant<tuple_nothrow_assignable_v<Dest, Srcs...>> {};

template <typename Dest, typename... Srcs>
concept TupleNothrowAssignable = tuple_nothrow_assignable_v<Dest, Srcs...>;


// construct by arguments
struct exact_arg_construct_tag {
	MSTL_CONSTEXPR explicit exact_arg_construct_tag() = default;
};
// construct by unpacking a tuple or pair
struct unpack_tuple_construct_tag {
	MSTL_CONSTEXPR explicit unpack_tuple_construct_tag() = default;
};


TEMNULL__ struct tuple<> {
	MSTL_CONSTEXPR tuple() noexcept = default;
	MSTL_CONSTEXPR tuple(const tuple&) noexcept = default;
	template <typename Tag>
		requires(is_same_v<Tag, exact_arg_construct_tag>)
	MSTL_CONSTEXPR tuple(Tag) noexcept {}

	MSTL_CONSTEXPR tuple& operator =(const tuple&) noexcept = default;

	MSTL_CONSTEXPR void swap(const tuple&) noexcept {}
	MSTL_NODISCARD MSTL_CONSTEXPR bool equal_to(const tuple&) const noexcept { return true; }
	MSTL_NODISCARD MSTL_CONSTEXPR bool less_to(const tuple&) const noexcept { return false; }
};

template <typename This, typename... Rest>
struct tuple<This, Rest...> : private tuple<Rest...> {
public:
	using this_type = This;
	using base_type = tuple<Rest...>;

	template <typename Tag, typename U1, typename... U2>
		requires(is_same_v<Tag, exact_arg_construct_tag>)
	MSTL_CONSTEXPR tuple(Tag, U1&& this_arg, U2&&... rest_arg)
		: base_type(exact_arg_construct_tag{}, MSTL::forward<U2>(rest_arg)...), data_(MSTL::forward<U1>(this_arg)) {}

	template <typename Tag, typename Tuple, size_t... Index>
		requires(is_same_v<Tag, unpack_tuple_construct_tag>)
	MSTL_CONSTEXPR tuple(Tag, Tuple&&, index_sequence<Index...>);

	template <typename Tag, typename Tuple>
		requires(is_same_v<Tag, unpack_tuple_construct_tag>)
	MSTL_CONSTEXPR tuple(Tag, Tuple&& tup) 
		: tuple(unpack_tuple_construct_tag{}, MSTL::forward<Tuple>(tup), 
		make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{}) {}


	template <typename T = This>
		requires(conjunction_v<is_default_constructible<T>, is_default_constructible<Rest>...>)
	MSTL_CONSTEXPR explicit(!conjunction_v<
		is_implicitly_default_constructible<T>, is_implicitly_default_constructible<Rest>...>)
		tuple() noexcept(conjunction_v<
			is_nothrow_default_constructible<T>, is_nothrow_default_constructible<Rest>...>)
		: base_type(), data_() {}

	template <typename T = This>
		requires(tuple_constructible_v<tuple, const T&, const Rest&...>)
	MSTL_CONSTEXPR explicit(tuple_explicitly_convertible_v<tuple, const T&, const Rest&...>)
		tuple(const T& this_arg, const Rest&... rest_arg) noexcept(conjunction_v<
			is_nothrow_copy_constructible<T>, is_nothrow_copy_constructible<Rest>...>)
		: tuple(exact_arg_construct_tag{}, this_arg, rest_arg...) {}

	template <typename U1, typename... U2>
		requires(conjunction_v<tuple_perfect_forward<tuple, U1, U2...>, tuple_constructible<tuple, U1, U2...>>)
	MSTL_CONSTEXPR explicit(tuple_explicitly_convertible_v<tuple, U1, U2...>)
		tuple(U1&& this_arg, U2&&... rest_arg) noexcept(tuple_nothrow_constructible_v<tuple, U1, U2...>)
		: tuple(exact_arg_construct_tag{}, MSTL::forward<U1>(this_arg), MSTL::forward<U2>(rest_arg)...) {}


	tuple(const tuple&) = default;
	tuple(tuple&&)		= default;

	template <typename... U>
		requires(conjunction_v<tuple_constructible<tuple, const U&...>, tuple_convertible<tuple, const tuple<U...>&, U...>>)
	MSTL_CONSTEXPR explicit(tuple_explicitly_convertible_v<tuple, const U&...>)
		tuple(const tuple<U...>& tup) noexcept(tuple_nothrow_constructible_v<tuple, const U&...>)
		: tuple(unpack_tuple_construct_tag{}, tup) {}

	template <typename... U>
		requires(conjunction_v<tuple_constructible<tuple, U...>, tuple_convertible<tuple, tuple<U...>, U...>>)
	MSTL_CONSTEXPR explicit(tuple_explicitly_convertible_v<tuple, U...>)
		tuple(tuple<U...>&& tup) noexcept(tuple_nothrow_constructible_v<tuple, U...>)
		: tuple(unpack_tuple_construct_tag{}, MSTL::move(tup)) {}

	template <typename T1, typename T2>
		requires(tuple_constructible_v<tuple, const T1&, const T2&>)
	MSTL_CONSTEXPR explicit(tuple_explicitly_convertible_v<tuple, const T1&, const T2&>)
        tuple(const pair<T1, T2>& pir) noexcept(tuple_nothrow_constructible_v<tuple, const T1&, const T2&>)
        : tuple(unpack_tuple_construct_tag{}, pir) {}

    template <typename T1, typename T2>
		requires(tuple_constructible_v<tuple, T1, T2>)
	MSTL_CONSTEXPR explicit(tuple_explicitly_convertible_v<tuple, T1, T2>)
		tuple(pair<T1, T2>&& pir) noexcept(tuple_nothrow_constructible_v<tuple, T1, T2>)
        : tuple(unpack_tuple_construct_tag{}, MSTL::move(pir)) {}


	template <typename Self = tuple, typename T = This>
		requires(conjunction_v<is_copy_assignable<T>, is_copy_assignable<Rest>...>)
	MSTL_CONSTEXPR tuple& operator =(type_identity_t<const tuple&> tup) noexcept(conjunction_v<
		is_nothrow_copy_assignable<T>, is_nothrow_copy_assignable<Rest>...>) {
		data_ = tup.data_;
		get_rest() = tup.get_rest();
		return *this;
	}

	template <typename Self = tuple, typename T = This>
		requires(conjunction_v<is_move_assignable<T>, is_move_assignable<Rest>...>)
	MSTL_CONSTEXPR tuple& operator =(type_identity_t<tuple&&> tup) noexcept(conjunction_v<
		is_nothrow_move_assignable<T>, is_nothrow_move_assignable<Rest>...>) {
		data_ = MSTL::forward<T>(tup.data_);
		get_rest() = MSTL::forward<base_type>(tup.get_rest());
		return *this;
	}

	template <typename... U>
		requires(conjunction_v<negation<is_same<tuple, tuple<U...>>>, tuple_assignable<tuple, const U&...>>)
	MSTL_CONSTEXPR tuple& operator =(const tuple<U...>& tup) noexcept(tuple_nothrow_assignable_v<tuple, const U&...>) {
		data_ = tup.data_;
		get_rest() = tup.get_rest();
		return *this;
	}

	template <typename... U>
		requires(conjunction_v<negation<is_same<tuple, tuple<U...>>>, tuple_assignable<tuple, U...>>)
	MSTL_CONSTEXPR tuple& operator =(tuple<U...>&& tup) noexcept(tuple_nothrow_assignable_v<tuple, U...>) {
		data_ = MSTL::forward<typename tuple<U...>::this_type>(tup.data_);
		get_rest() = MSTL::forward<typename tuple<U...>::base_type>(tup.get_rest());
		return *this;
	}

	template <typename T1, typename T2>
		requires(tuple_assignable_v<tuple, const T1&, const T2&>)
	MSTL_CONSTEXPR tuple& operator=(const pair<T1, T2>& pir) noexcept(
		tuple_nothrow_assignable_v<tuple, const T1&, const T2&>) {
		data_ = pir.first;
		get_rest().data_ = pir.second;
		return *this;
	}

	template <typename T1, typename T2>
		requires(tuple_assignable_v<tuple, T1, T2>)
	MSTL_CONSTEXPR tuple& operator=(pair<T1, T2>&& pir) noexcept(
		tuple_nothrow_assignable_v<tuple, T1, T2>) {
		data_ = MSTL::forward<T1>(pir.first);
		get_rest().data_ = MSTL::forward<T2>(pir.second);
		return *this;
	}

	tuple& operator =(const volatile tuple&) = delete;


	MSTL_CONSTEXPR void swap(tuple t) noexcept(conjunction_v<
		is_nothrow_swappable<This>, is_nothrow_swappable<Rest>...>) {
		MSTL::swap(data_, t.data_);
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
	friend MSTL_CONSTEXPR tuple_element_t<Index, Types...>& get(tuple<Types...>&) noexcept;
	template <size_t Index, typename... Types>
	friend MSTL_CONSTEXPR const tuple_element_t<Index, Types...>& get(const tuple<Types...>&) noexcept;
	template <size_t Index, typename... Types>
	friend MSTL_CONSTEXPR tuple_element_t<Index, Types...>&& get(tuple<Types...>&&) noexcept;
	template <size_t Index, typename... Types>
	friend MSTL_CONSTEXPR const tuple_element_t<Index, Types...>&& get(const tuple<Types...>&&) noexcept;

	template <size_t Index, typename... Types>
	friend MSTL_CONSTEXPR tuple_element_t<Index, Types...>&& pair_get_from_tuple(tuple<Types...>&&) noexcept;

private:
	this_type data_;
};
#ifdef MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename... Types>
tuple(Types...) -> tuple<Types...>;

template <typename T1, typename T2>
tuple(pair<T1, T2>) -> tuple<T1, T2>;
#endif

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
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(const tuple<T1...>& lh, const tuple<T2...>& rh) {
	return rh < lh;
}
template <typename... T1, typename... T2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(const tuple<T1...>& lh, const tuple<T2...>& rh) {
	return !(rh < lh);
}
template <typename... T1, typename... T2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(const tuple<T1...>& lh, const tuple<T2...>& rh) {
	return !(lh < rh);
}
template <typename... T>
	requires(conjunction_v<is_swappable<T>...>)
MSTL_CONSTEXPR void swap(tuple<T...>& lh, tuple<T...>& rh) noexcept(noexcept(lh.swap(rh))) {
	lh.swap(rh);
}


template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, Types...>& get(tuple<Types...>& tup) noexcept {
	using T = tuple_element_t<Index, tuple<Types...>>;
	using tuple_type = tuple_extract_base_t<Index, tuple<Types...>>;
	return static_cast<T&>(static_cast<tuple_type&>(tup).data_);
}
template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, Types...>& get(const tuple<Types...>& tup) noexcept {
	using T = tuple_element_t<Index, tuple<Types...>>;
	using tuple_type = tuple_extract_base_t<Index, tuple<Types...>>;
	return static_cast<const T&>(static_cast<const tuple_type&>(tup).data_);
}
template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, Types...>&& get(tuple<Types...>&& tup) noexcept {
	using T = tuple_element_t<Index, tuple<Types...>>;
	using tuple_type = tuple_extract_base_t<Index, tuple<Types...>>;
	return static_cast<T&&>(static_cast<tuple_type&&>(tup).data_);
}
template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR const tuple_element_t<Index, Types...>&& get(const tuple<Types...>&& tup) noexcept {
	using T = tuple_element_t<Index, tuple<Types...>>;
	using tuple_type = tuple_extract_base_t<Index, tuple<Types...>>;
	return static_cast<const T&&>(static_cast<const tuple_type&&>(tup).data_);
}

template <size_t Index, typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple_element_t<Index, Types...>&& pair_get_from_tuple(tuple<Types...>&& tup) noexcept {
	using T = tuple_element_t<Index, tuple<Types...>>;
	using tuple_type = tuple_extract_base_t<Index, tuple<Types...>>;
	return static_cast<T&&>(static_cast<tuple_type&>(tup).data_);
}


template <typename This, typename... Rest>
template <typename Tag, typename Tuple, size_t... Index>
	requires(is_same_v<Tag, unpack_tuple_construct_tag>)
MSTL_CONSTEXPR tuple<This, Rest...>::tuple(Tag, Tuple&& tup, index_sequence<Index...>)
	: tuple(exact_arg_construct_tag{}, MSTL::get<Index>(MSTL::forward<Tuple>(tup))...) {}


template <typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple<unwrap_ref_decay_t<Types>...> make_tuple(Types&&... args) {
	using tuple_type = tuple<unwrap_ref_decay_t<Types>...>;
	return tuple_type(MSTL::forward<Types>(args)...);
}
template <typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple<Types&...> tie(Types&... args) noexcept {
	using tuple_type = tuple<Types&...>;
	return tuple_type(args...);
}
template <typename... Types>
MSTL_NODISCARD MSTL_CONSTEXPR tuple<Types&&...> forward_as_tuple(Types&&... args) noexcept {
	using tuple_type = tuple<Types&&...>;
	return tuple_type(MSTL::forward<Types>(args)...);
}


template <typename, typename Tuple, typename Sequence = make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>>
constexpr bool constructible_from_tuple = false;
template <typename T, typename Tuple, size_t... Index>
constexpr bool constructible_from_tuple<T, Tuple, index_sequence<Index...>> =
is_constructible_v<T, decltype(MSTL::get<Index>(MSTL::declval<Tuple>()))...>;

template <typename T, typename Tuple>
concept ConstructibleFromTuple = constructible_from_tuple<T, Tuple>;


template <typename T, typename Tuple, size_t... Index>
MSTL_NODISCARD MSTL_CONSTEXPR T broaden_make_from_tuple(Tuple&& tup, index_sequence<Index...>)
noexcept(is_nothrow_constructible_v<T, decltype(MSTL::get<Index>(MSTL::forward<Tuple>(tup)))...>) {
	return T(MSTL::get<Index>(MSTL::forward<Tuple>(tup))...);
}

template <typename T, typename Tuple>
	requires(constructible_from_tuple<T, Tuple>)
MSTL_NODISCARD MSTL_CONSTEXPR T make_from_tuple(Tuple&& tup) noexcept(noexcept(MSTL::broaden_make_from_tuple<T>(
	MSTL::forward<Tuple>(tup), make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{}))) {
	return MSTL::broaden_make_from_tuple<T>(
		MSTL::forward<Tuple>(tup), make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
}

template <typename... Args>
struct hash<MSTL::tuple<Args...>> {
private:
	template <typename Tuple, size_t... Idx>
	MSTL_CONSTEXPR size_t broaden_tuple(const Tuple& tup, MSTL::index_sequence<Idx...>) const noexcept {
		return (hash<remove_cvref_t<tuple_element_t<Idx, Tuple>>>()(MSTL::get<Idx>(tup)) ^ ...);
	}
public:
	MSTL_NODISCARD MSTL_CONSTEXPR size_t operator()(const MSTL::tuple<Args...>& tup) const noexcept {
		return (broaden_tuple)(tup, MSTL::index_sequence_for<Args...>());
	}
};

MSTL_END_NAMESPACE__
#endif // MSTL_TUPLE_HPP__
