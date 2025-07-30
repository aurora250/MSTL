#ifndef MSTL_PRINT_HPP__
#define MSTL_PRINT_HPP__
#include "basiclib.hpp"
#ifdef MSTL_VERSION_17__
#include <iostream>
#include <iomanip>
#include "variant.hpp"
#include "optional.hpp"
#include "any.hpp"
MSTL_BEGIN_NAMESPACE__

template <class T, class = void>
struct printer {
    static void print(const T& t) {
        std::cout << t;
    }

    using is_default_print = true_type;
};

template <class T, class = void>
struct _is_default_printable : false_type {};

template <class T>
struct _is_default_printable<T, void_t<_MSTL pair<typename printer<T>::is_default_print,
    decltype(_MSTL declval<std::ostream &>() << _MSTL declval<T const &>())>>> : true_type {};

template <class T, class = void>
struct _is_printer_printable : std::true_type {
};

    template <class T>
    struct _is_printer_printable<T, std::void_t<typename printer<T>::is_default_print>> : std::false_type {
    };

    template <class T, class = void>
    struct is_printable : std::disjunction<_is_default_printable<T>, _is_printer_printable<T>> {};

    template <class T, class U = void, class = void>
    struct _enable_if_tuple {
        using not_type = U;
    };

    template <class T, class U>
    struct _enable_if_tuple<T, U, std::void_t<decltype(std::tuple_size<T>::value)>> {
        using type = U;
    };

    template <class T, class U = void, class = void>
    struct _enable_if_map {
        using not_type = U;
    };

    template <class T, class U>
    struct _enable_if_map<T, U, std::enable_if_t<std::is_same_v<typename T::value_type, std::pair<typename T::key_type const, typename T::mapped_type>>>> {
        using type = U;
    };

    template <class T, class U = void, class = void>
    struct _enable_if_iterable {
        using not_type = U;
    };

    template <class T, class U>
    struct _enable_if_iterable<T, U, std::void_t<typename std::iterator_traits<decltype(std::begin(std::declval<T const &>()))>::value_type>> {
        using type = U;
    };

    template <class, class = void, class = void>
    struct _enable_if_has_print {};

    template <class T, class U>
    struct _enable_if_has_print<T, U, void_t<decltype(_MSTL declval<T const &>().do_print())>> {
        using type = U;
    };

    template <class T, class U = void, class = void>
    struct _enable_if_char {
        using not_type = U;
    };

    template <class T, class U>
    struct _enable_if_char<T, U, std::enable_if_t<is_character_v<T>>> {
        using type = U;
    };

    template <class T, class U = void, class = void>
    struct _enable_if_string {
        using not_type = U;
    };

    template <class T, class Alloc, class Traits, class U>
    struct _enable_if_string<std::basic_string<T, Alloc, Traits>, U, typename _enable_if_char<T>::type> {
        using type = U;
    };

    template <class T, class Traits, class U>
    struct _enable_if_string<std::basic_string_view<T, Traits>, U, typename _enable_if_char<T>::type> {
        using type = U;
    };

    template <class T, class U = void, class = void>
    struct _enable_if_c_str {
        using not_type = U;
    };

    template <class T, class U>
    struct _enable_if_c_str<T, U, enable_if_t<
        is_pointer_v<decay_t<T>> && is_character_v<remove_const_t<remove_pointer_t<decay_t<T>>>>>> {
        using type = U;
    };

    template <class T, class U = void, class = void>
    struct _enable_if_optional {
        using not_type = U;
    };

    template <class T, class U>
    struct _enable_if_optional<_MSTL optional<T>, U, void> {
        using type = U;
    };

    template <class T, class U = void, class = void>
    struct _enable_if_variant {
        using not_type = U;
    };

    template <class ...Ts, class U>
    struct _enable_if_variant<_MSTL variant<Ts...>, U, void> {
        using type = U;
    };

    template <class T>
    struct printer<T, typename _enable_if_has_print<T>::type> {
        static void print(T const &t) {
            t.do_print();
        }
    };

    template <class T>
    struct printer<T, typename _enable_if_has_print<T, typename _enable_if_iterable<T, typename _enable_if_c_str<T, typename _enable_if_string<T, typename _enable_if_map<T>::not_type>::not_type>::not_type>::type>::not_type> {
        static void print(T const &t) {
            std::cout << "{";
            bool once = false;
            for (auto const &v: t) {
                if (once) {
                    std::cout << ", ";
                } else {
                    once = true;
                }
                printer<remove_cvref_t<decltype(v)>>::print(v);
            }
            std::cout << "}";
        }
    };

    template <class T>
    struct printer<T, typename _enable_if_has_print<T, typename _enable_if_tuple<T, typename _enable_if_iterable<T>::not_type>::type>::not_type> {
        template <std::size_t ...Is>
        static void _unrolled_print(T const &t, std::index_sequence<Is...>) {
            std::cout << "{";
            ((printer<remove_cvref_t<std::tuple_element_t<Is, T>>>::print(std::get<Is>(t)), std::cout << ", "), ...);
            if constexpr (sizeof...(Is) != 0) printer<remove_cvref_t<std::tuple_element_t<sizeof...(Is), T>>>::print(std::get<sizeof...(Is)>(t));
            std::cout << "}";
        }

        static void print(T const &t) {
            _unrolled_print(t, std::make_index_sequence<std::max(static_cast<std::size_t>(1), std::tuple_size_v<T>) - 1>{});
        }
    };

    template <class T>
    struct printer<T, typename _enable_if_has_print<T, typename _enable_if_map<T>::type>::not_type> {
        static void print(T const &t) {
            std::cout << "{";
            bool once = false;
            for (auto const &[k, v]: t) {
                if (once) {
                    std::cout << ", ";
                } else {
                    once = true;
                }
                printer<remove_cvref_t<decltype(k)>>::print(k);
                std::cout << ": ";
                printer<remove_cvref_t<decltype(v)>>::print(v);
            }
            std::cout << "}";
        }
    };

    template <class T>
    struct printer<T, typename _enable_if_has_print<T, typename _enable_if_string<T>::type>::not_type> {
        static void print(T const &t) {
            std::cout << std::quoted(t);
        }
    };

    template <class T>
    struct printer<T, typename _enable_if_c_str<T>::type> {
        static void print(T const &t) {
            std::cout << t;
        }
    };

    template <class T>
    struct printer<T, enable_if_t<is_character_v<T>>> {
        static void print(T const &t) {
            T s[2] = { t, initialize<T>() };
            std::cout << std::quoted(s, T('\''));
        }
    };

    template <>
    struct printer<nullptr_t, void> {
        static void print(nullptr_t const &) {
            std::cout << "nullptr";
        }
    };
    template <>
    struct printer<_MSTL nullopt_t, void> {
        static void print(_MSTL nullopt_t const &) {
            std::cout << "nullopt";
        }
    };

    template <class T>
    struct printer<T, typename _enable_if_has_print<T, typename _enable_if_optional<T>::type>::not_type> {
        static void print(T const &t) {
            if (t.has_value()) {
                printer<typename T::value_type>::print(*t);
            } else {
                printer<_MSTL nullopt_t>::print(_MSTL nullopt);
            }
        }
    };

    template <class T>
    struct printer<T, typename _enable_if_has_print<T, typename _enable_if_variant<T>::type>::not_type> {
        static void print(T const &t) {
            _MSTL visit([] (auto const &v) {
                printer<remove_cvref_t<decltype(v)>>::print(v);
            }, t);
        }
    };

    template <>
    struct printer<bool, void> {
        static void print(bool const &t) {
            if (t) {
                std::cout << "true";
            } else {
                std::cout << "false";
            }
        }
    };

    template <class T0, class ...Ts>
    void println(T0 const &t0, Ts const &...ts) {
        printer<remove_cvref_t<T0>>::print(t0);
        ((std::cout << " ", printer<remove_cvref_t<Ts>>::print(ts)), ...);
        std::cout << "\n";
    }

    template <class T0, class ...Ts>
    void print(T0 const &t0, Ts const &...ts) {
        printer<remove_cvref_t<T0>>::print(t0);
        ((std::cout << " ", printer<remove_cvref_t<Ts>>::print(ts)), ...);
    }

    template <class T, class = void>
    class print_adaptor {
        T const &t;

    public:
        explicit constexpr print_adaptor(T const &t_) : t(t_) {}

        friend std::ostream &operator<<(std::ostream &os, print_adaptor const &&self) {
            auto oldflags = os.flags();
            os << "[object 0x" << std::hex << reinterpret_cast<uintptr_t>(
                reinterpret_cast<void const volatile *>(std::addressof(self.t))) << ']';
            os.flags(oldflags);
            return os;
        }
    };

template <class T>
class print_adaptor<T, enable_if_t<is_printable<T>::value>> {
    T const &t;

public:
    explicit constexpr print_adaptor(T const &t_) : t(t_) {}

    friend std::ostream &operator<<(std::ostream &os, print_adaptor const &&self) {
        print(self.t);
        return os;
    }
};

#ifdef MSTL_SUPPORT_DEDUCTION_GUIDES__
template <class T>
explicit print_adaptor(T const &) -> print_adaptor<T>;
#endif
MSTL_END_NAMESPACE__
#endif
#endif // MSTL_PRINT_HPP__
