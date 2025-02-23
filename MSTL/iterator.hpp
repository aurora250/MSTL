#ifndef MSTL_ITERATOR_HPP__
#define MSTL_ITERATOR_HPP__
#include "concepts.hpp"
#include "errorlib.h"
MSTL_BEGIN_NAMESPACE__

template <typename Iterator>
MSTL_TRAITS_DEPRE MSTL_NODISCARD MSTL_CONSTEXPR iter_cat_t<Iterator>
iterator_category(const Iterator&) noexcept {
    return iter_cat_t<Iterator>();
}
template <typename Iterator>
MSTL_TRAITS_DEPRE MSTL_NODISCARD MSTL_CONSTEXPR iter_dif_t<Iterator>*
distance_type(const Iterator&) noexcept {
    return static_cast<iter_dif_t<Iterator>*>(0);
}
template <typename Iterator>
MSTL_TRAITS_DEPRE MSTL_NODISCARD MSTL_CONSTEXPR
iter_val_t<Iterator>* value_type(const Iterator&) noexcept {
    return static_cast<iter_val_t<Iterator>*>(0);
}


#ifndef MSTL_VERSION_17__
template <typename Ptr, enable_if_t<is_pointer_v<Ptr>, int> = 0>
MSTL_CONSTEXPR iter_ptr_t<Ptr> __to_pointer_aux(Ptr iter) {
    return iter;
}
template <typename Iterator, enable_if_t<!is_pointer_v<Iterator>, int> = 0>
MSTL_CONSTEXPR iter_ptr_t<Iterator> __to_pointer_aux(Iterator iter) {
    return iter.operator->();
}
template <typename Iterator>
MSTL_CONSTEXPR iter_ptr_t<Iterator> to_pointer(Iterator iter) {
    return MSTL::__to_pointer_aux(iter);
}
#else
template <typename Iterator>
MSTL_CONSTEXPR iter_ptr_t<Iterator> to_pointer(Iterator tmp) {
    if constexpr (is_pointer_v<Iterator>)
        return tmp;
    else
        return tmp.operator->();
}
#endif // MSTL_VERSION_17__


#ifndef MSTL_VERSION_17__
template <typename Iterator, typename Distance>
MSTL_CONSTEXPR17 void __advance_aux(Iterator& i, Distance n, std::random_access_iterator_tag) {
    i += n;
}
template <typename Iterator, typename Distance>
MSTL_CONSTEXPR17 void __advance_aux(Iterator& i, Distance n, std::bidirectional_iterator_tag) {
    for (; n < 0; ++n) --i;
    for (; 0 < n; --n) ++i;
}
template <typename Iterator, typename Distance>
MSTL_CONSTEXPR17 void __advance_aux(Iterator& i, Distance n, std::input_iterator_tag) {
    MSTL_DEBUG_VERIFY__(is_signed_v<Distance> && n >= 0, "negative advance of non-bidirectional iterator");
    for (; 0 < n; --n) ++i;
}
template <typename Iterator, typename Distance, enable_if_t<is_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR17 void advance(Iterator& i, Distance n) {
    MSTL::__advance_aux(i, n, iter_cat_t<Iterator>());
}
#else
template <typename Iterator, typename Distance, enable_if_t<is_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR17 void advance(Iterator& i, Distance n) {
    if constexpr (is_rnd_iter_v<Iterator>) {
        i += n;
    }
    else {
        if constexpr (is_signed_v<Distance> && !is_bid_iter_v<Iterator>) {
            MSTL_DEBUG_VERIFY__(n >= 0, "negative advance of non-bidirectional iterator");
        }
        if constexpr (is_signed_v<Distance> && is_bid_iter_v<Iterator>) {
            for (; n < 0; ++n)
                --i;
        }
        for (; 0 < n; --n)
            ++i;
    }
}
#endif // MSTL_VERSION_17__

template <typename Iterator>
MSTL_CONSTEXPR17 Iterator prev(Iterator iter, iter_dif_t<Iterator> n = -1) {
    MSTL::advance(iter, n);
    return iter;
}
template <typename Iterator>
MSTL_CONSTEXPR17 Iterator next(Iterator iter, iter_dif_t<Iterator> n = 1) {
    MSTL::advance(iter, n);
    return iter;
}


#ifndef MSTL_VERSION_17__
template <typename Iterator>
MSTL_CONSTEXPR17 iter_dif_t<Iterator> __distance_aux(
    Iterator first, Iterator last, std::random_access_iterator_tag) {
    return last - first;
}
template <typename Iterator>
MSTL_CONSTEXPR17 iter_dif_t<Iterator> __distance_aux(
    Iterator first, Iterator last, std::input_iterator_tag) {
    iter_dif_t<Iterator> n = 0;
    while (first != last) { ++first; ++n; }
    return n;
}
template <typename Iterator, enable_if_t<is_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR17 iter_dif_t<Iterator> distance(Iterator first, Iterator last) {
    return MSTL::__distance_aux(first, last, iter_cat_t<Iterator>());
}
#else
template <typename Iterator, enable_if_t<is_iter_v<Iterator>, int> = 0>
MSTL_CONSTEXPR17 iter_dif_t<Iterator> distance(Iterator first, Iterator last) {
    if constexpr (is_rnd_iter_v<Iterator>)
        return last - first;
    else {
        iter_dif_t<Iterator> n = 0;
        while (first != last) { ++first; ++n; }
        return n;
    }
}
#endif // MSTL_VERSION_17__


template <typename Container>
class back_insert_iterator {
public:
    using iterator_category = std::output_iterator_tag;
    using value_type        = void;
    using difference_type   = void;
    using pointer           = void;
    using reference         = void;
    using self              = back_insert_iterator<Container>;

    MSTL_CONSTEXPR explicit back_insert_iterator(Container& x) noexcept
        : container(MSTL::addressof(x)) {}
    MSTL_CONSTEXPR self& operator =(const typename Container::value_type& value) {
        container->push_back(value);
        return *this;
    }
    MSTL_CONSTEXPR self& operator =(typename Container::value_type&& value) {
        container->push_back(MSTL::move(value));
        return *this;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR self& operator *() noexcept { return *this; }
    MSTL_CONSTEXPR self& operator ++() noexcept { return *this; }
    MSTL_CONSTEXPR self& operator ++(int) noexcept { return *this; }

private:
    Container* container;
};
template <typename Container>
MSTL_NODISCARD MSTL_CONSTEXPR back_insert_iterator<Container> back_inserter(Container& x) noexcept {
    return back_insert_iterator<Container>(x);
}

template <typename Container>
class front_insert_iterator {
public:
    using iterator_category = std::output_iterator_tag;
    using value_type        = void;
    using difference_type   = void;
    using pointer           = void;
    using reference         = void;
    using self              = front_insert_iterator<Container>;

    MSTL_CONSTEXPR explicit front_insert_iterator(Container& x) noexcept
        : container(MSTL::addressof(x)) {}
    MSTL_CONSTEXPR self& operator =(const typename Container::value_type& value) {
        container->push_front(value);
        return *this;
    }
    MSTL_CONSTEXPR self& operator =(typename Container::value_type&& value) {
        container->push_front(MSTL::move(value));
        return *this;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR self& operator *() noexcept { return *this; }
    MSTL_CONSTEXPR self& operator ++() noexcept { return *this; }
    MSTL_CONSTEXPR self& operator ++(int) noexcept { return *this; }

private:
    Container* container;
};
template <typename Container>
MSTL_NODISCARD MSTL_CONSTEXPR front_insert_iterator<Container> front_inserter(Container& x) noexcept {
    return front_insert_iterator<Container>(x);
}

template <typename Container>
class insert_iterator {
public:
    using iterator_category = std::output_iterator_tag;
    using value_type        = void;
    using difference_type   = void;
    using pointer           = void;
    using reference         = void;
    using self              = insert_iterator<Container>;

    MSTL_CONSTEXPR insert_iterator(Container& x, typename Container::iterator it) noexcept
        : container(MSTL::addressof(x)), iter(MSTL::move(it)) {}
    MSTL_CONSTEXPR self& operator =(const typename Container::value_type& value) {
        iter = container->insert(iter, value);
        ++iter;
        return *this;
    }
    MSTL_CONSTEXPR self& operator =(typename Container::value_type&& value) {
        iter = container->insert(iter, MSTL::move(value));
        ++iter;
        return *this;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR self& operator *() noexcept { return *this; }
    MSTL_CONSTEXPR self& operator ++() noexcept { return *this; }
    MSTL_CONSTEXPR self& operator ++(int) noexcept { return *this; }

private:
    Container* container;
    typename Container::iterator iter;
};
template <typename Container>
MSTL_NODISCARD MSTL_CONSTEXPR insert_iterator<Container> 
inserter(Container& x, typename Container::iterator it) noexcept {
    return insert_iterator<Container>(x, it);
}

template <typename Iterator>
class reverse_iterator {
    static_assert(is_bid_iter_v<Iterator>, "reverse iterator requires bidirectional iterator.");

public:
    using iterator_category = iter_cat_t<Iterator>;
    using value_type        = iter_val_t<Iterator>;
    using difference_type   = iter_dif_t<Iterator>;
    using pointer           = iter_ptr_t<Iterator>;
    using reference         = iter_ref_t<Iterator>;
    using self              = reverse_iterator<Iterator>;

public:
    MSTL_CONSTEXPR reverse_iterator() = default;

    MSTL_CONSTEXPR explicit reverse_iterator(Iterator x)
        noexcept(is_nothrow_move_constructible_v<Iterator>)
        : current(MSTL::move(x)) {}

    template <typename U>
#ifdef MSTL_VERSION_20__
        requires(!same_as<U, Iterator> && convertible_to<const U&, Iterator>)
#endif // MSTL_VERSION_20__
    MSTL_CONSTEXPR explicit reverse_iterator(const reverse_iterator<U>& x)
        noexcept(is_nothrow_constructible_v<Iterator, const U&>)
        : current(x.current) {}

    template <typename U>
#ifdef MSTL_VERSION_20__
        requires(!same_as<U, Iterator> && convertible_to<const U&, Iterator> 
    && assignable_from<Iterator&, const U&>)
#endif // MSTL_VERSION_20__
    MSTL_CONSTEXPR self& operator =(const reverse_iterator<U>& x)
        noexcept(is_nothrow_assignable_v<self&, const U&>) {
        current = x.current;
        return *this;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR Iterator base() const
        noexcept(is_nothrow_copy_constructible_v<Iterator>) {
        return current;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR reference operator *() const
        noexcept(is_nothrow_copy_assignable_v<Iterator> && noexcept(*--(MSTL::declval<Iterator&>()))) {
        Iterator iter = current;
        return *--iter;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR pointer operator ->() const
        noexcept(is_nothrow_copy_constructible_v<Iterator> && noexcept(--(MSTL::declval<Iterator&>()))
            && is_nothrow_arrow<Iterator&, pointer>)
#ifdef MSTL_VERSION_20__
        requires (is_pointer_v<Iterator> || requires(const Iterator it) { it.operator->(); })
#endif // MSTL_VERSION_20__
    {
        Iterator tmp = current;
        --tmp;
        return MSTL::to_pointer(tmp);
    }

    MSTL_CONSTEXPR self& operator ++()
         noexcept(noexcept(--current)) {
        --current;
        return *this;
    }
    MSTL_CONSTEXPR self operator ++(int)
        noexcept(is_nothrow_copy_constructible_v<Iterator> && noexcept(--current)) {
        self tmp = *this;
        --current;
        return tmp;
    }
    MSTL_CONSTEXPR self& operator --()
        noexcept(noexcept(++current)) {
        ++current;
        return *this;
    }
    MSTL_CONSTEXPR self operator --(int)
        noexcept(is_nothrow_copy_constructible_v<Iterator> && noexcept(++current)) {
        self tmp = *this;
        ++current;
        return tmp;
    }

    MSTL_CONSTEXPR self operator +(const difference_type n) const 
        noexcept(noexcept(self(current - n))) {
        return self(current - n);
    }
    MSTL_CONSTEXPR self& operator +=(const difference_type n) 
        noexcept(noexcept(current -= n)) {
        current -= n;
        return *this;
    }
    MSTL_CONSTEXPR self operator -(const difference_type n) const 
        noexcept(noexcept(self(current + n))) {
        return self(current + n);
    }
    MSTL_CONSTEXPR self& operator -=(const difference_type n)
        noexcept(noexcept(current += n)) {
        current += n;
        return *this;
    }

    MSTL_CONSTEXPR reference operator[](const difference_type n) const
        noexcept(noexcept(MSTL::declcopy<reference>(*(*this + n)))) {
        return *(*this + n);
    }

    MSTL_NODISCARD MSTL_CONSTEXPR const Iterator& get_current() const noexcept {
        return current;
    }

private:
    Iterator current;
};
template <typename Iterator1, typename Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() == y.get_current()))) 
#ifdef MSTL_VERSION_20__
    requires requires { { x.get_current() == y.get_current() } -> convertible_to<bool>; }
#endif // MSTL_VERSION_20__
{
    return x.get_current() == y.get_current();
}
template <typename Iterator1, typename Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() != y.get_current())))
#ifdef MSTL_VERSION_20__
    requires requires { { x.get_current() != y.get_current() } -> convertible_to<bool>; }
#endif // MSTL_VERSION_20__
{
    return x.get_current() != y.get_current();
}
template <typename Iterator1, typename Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() > y.get_current())))
#ifdef MSTL_VERSION_20__
    requires requires { { x.get_current() > y.get_current() } -> convertible_to<bool>; }
#endif
{
    return x.get_current() > y.get_current();
}
template <typename Iterator1, typename Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() < y.get_current())))
#ifdef MSTL_VERSION_20__
    requires requires { { x.get_current() < y.get_current() } -> convertible_to<bool>; }
#endif // MSTL_VERSION_20__
{
    return x.get_current() < y.get_current();
}
template <typename Iterator1, typename Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() >= y.get_current())))
#ifdef MSTL_VERSION_20__
    requires requires { { x.get_current() >= y.get_current() } -> convertible_to<bool>; }
#endif // MSTL_VERSION_20__
{
    return x.get_current() >= y.get_current();
}
template <typename Iterator1, typename Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() <= y.get_current())))
#ifdef MSTL_VERSION_20__
    requires requires { { x.get_current() <= y.get_current() } -> convertible_to<bool>; }
#endif // MSTL_VERSION_20__
{
    return x.get_current() <= y.get_current();
}

template <typename Iterator1, typename Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) operator -(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(y.get_current() - x.get_current())) {
    return y.get_current() - x.get_current();
}
template <typename Iterator>
MSTL_CONSTEXPR reverse_iterator<Iterator> operator +(
    iter_dif_t<Iterator> n, const reverse_iterator<Iterator>& x)
    noexcept(noexcept(x + n)) {
    return x + n;
}
template <typename Iterator>
MSTL_NODISCARD MSTL_CONSTEXPR reverse_iterator<Iterator> 
make_reverse_iterator(Iterator it) noexcept(is_nothrow_move_constructible_v<Iterator>) {
    return reverse_iterator<Iterator>(MSTL::move(it));
}

MSTL_END_NAMESPACE__
#endif // MSTL_ITERATOR_HPP__
