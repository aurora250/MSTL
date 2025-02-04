#ifndef MSTL_ITERATOR_HPP__
#define MSTL_ITERATOR_HPP__
#include "concepts.hpp"
#include "iterator_traits.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Iterator>
MSTL_NODISCARD MSTL_CONSTEXPR iter_cat_t<Iterator> iterator_category(const Iterator&) {
    using category = iter_cat_t<Iterator>;
    return category();
}
template <typename Iterator>
MSTL_NODISCARD MSTL_CONSTEXPR iter_dif_t<Iterator>* distance_type(const Iterator&) {
    return static_cast<iter_dif_t<Iterator>*>(0);
}
template <typename Iterator>
MSTL_NODISCARD MSTL_CONSTEXPR iter_val_t<Iterator>* value_type(const Iterator&) {
    return static_cast<iter_val_t<Iterator>*>(0);
}

template <typename Iterator, typename Distance>
    requires(iterator_typedef<Iterator>)
MSTL_CONSTEXPR void advance(Iterator& i, Distance n) {
    if constexpr (random_access_iterator<Iterator>)
        i += n;
    else if constexpr (bidirectional_iterator<Iterator>) {
        if (n >= 0)
            while (n--) ++i;
        else
            while (n++) --i;
    }
    else 
        while (n--) ++i;
}

template <typename Iterator>
    requires(input_iterator<Iterator>)
MSTL_CONSTEXPR Iterator prev(Iterator iter, iter_dif_t<Iterator> n = -1) {
    MSTL::advance(iter, n);
    return iter;
}
template <typename Iterator>
    requires(input_iterator<Iterator>)
MSTL_CONSTEXPR Iterator next(Iterator iter, iter_dif_t<Iterator> n = 1) {
    MSTL::advance(iter, n);
    return iter;
}

template <typename Iterator>
    requires(random_access_iterator<Iterator>)
MSTL_CONSTEXPR iter_dif_t<Iterator> distance(Iterator first, Iterator last) {
    if constexpr (random_access_iterator<Iterator>)
        return last - first;
    else {
        iter_dif_t<Iterator> n = 0;
        while (first != last) { ++first; ++n; }
        return n;
    }
}


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

template <class Iterator>
    requires(bidirectional_iterator<Iterator>)
class reverse_iterator {
public:
    using iterator_category = iter_cat_t<Iterator>;
    using value_type        = iter_val_t<Iterator>;
    using difference_type   = iter_dif_t<Iterator>;
    using pointer           = iter_ptr_t<Iterator>;
    using reference         = iter_ref_t<Iterator>;
    using self              = reverse_iterator<Iterator>;

    MSTL_CONSTEXPR reverse_iterator() = default;

    MSTL_CONSTEXPR explicit reverse_iterator(Iterator x)
        noexcept(is_nothrow_move_constructible_v<Iterator>)
        : current(MSTL::move(x)) {}

    template <typename U>
        requires(!same_as<U, Iterator>&& convertible_to<const U&, Iterator>)
    MSTL_CONSTEXPR explicit reverse_iterator(const reverse_iterator<U>& x)
        noexcept(is_nothrow_constructible_v<Iterator, const U&>)
        : current(x.current) {}

    template <typename U>
        requires(!same_as<U, Iterator>&& convertible_to<const U&, Iterator>
    && assignable_from<Iterator&, const U&>)
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
        requires (is_pointer_v<Iterator> || requires(const Iterator it) { it.operator->(); })
    {
        Iterator tmp = current;
        --tmp;
        if constexpr (is_pointer_v<Iterator>)
            return tmp;
        else
            return tmp.operator->();
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
template <class Iterator1, class Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() == y.get_current()))) 
    requires requires { { x.get_current() == y.get_current() } -> convertible_to<bool>; } {
    return x.get_current() == y.get_current();
}
template <class Iterator1, class Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() != y.get_current())))
    requires requires { { x.get_current() != y.get_current() } -> convertible_to<bool>; } {
    return x.get_current() != y.get_current();
}
template <class Iterator1, class Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() > y.get_current())))
    requires requires { { x.get_current() > y.get_current() } -> convertible_to<bool>; } {
    return x.get_current() > y.get_current();
}
template <class Iterator1, class Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() < y.get_current())))
    requires requires { { x.get_current() < y.get_current() } -> convertible_to<bool>; } {
    return x.get_current() < y.get_current();
}
template <class Iterator1, class Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() >= y.get_current())))
    requires requires { { x.get_current() >= y.get_current() } -> convertible_to<bool>; } {
    return x.get_current() >= y.get_current();
}
template <class Iterator1, class Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(MSTL::declcopy<bool>(x.get_current() <= y.get_current())))
    requires requires { { x.get_current() <= y.get_current() } -> convertible_to<bool>; } {
    return x.get_current() <= y.get_current();
}

template <class Iterator1, class Iterator2>
MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) operator -(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(y.get_current() - x.get_current())) {
    return y.get_current() - x.get_current();
}
template <class Iterator>
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
