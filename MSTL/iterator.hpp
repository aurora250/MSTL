#ifndef MSTL_ITERATOR_HPP__
#define MSTL_ITERATOR_HPP__
#include "basiclib.h"
#include "concepts.hpp"
#include "algobase.hpp"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

template <typename Iterator>
    requires(IteratorTypedef<Iterator>)
MSTL_CONSTEXPR typename std::iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    using category = typename std::iterator_traits<Iterator>::iterator_category;
    return category();
}
template <typename Iterator>
    requires(IteratorTypedef<Iterator>)
MSTL_CONSTEXPR typename std::iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
    return static_cast<typename std::iterator_traits<Iterator>::difference_type*>(0);
}
template <typename Iterator>
    requires(IteratorTypedef<Iterator>)
MSTL_CONSTEXPR typename std::iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
    return static_cast<typename std::iterator_traits<Iterator>::value_type*>(0);
}

template <typename Iterator, typename Distance>
    requires(InputIterator<Iterator>)
MSTL_CONSTEXPR void advance(Iterator& i, Distance n) {
    while (n--) ++i;
}
template <typename Iterator, typename Distance>
    requires(BidirectionalIterator<Iterator>)
MSTL_CONSTEXPR void advance(Iterator& i, Distance n) {
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}
template <typename Iterator, typename Distance>
    requires(RandomAccessIterator<Iterator>)
MSTL_CONSTEXPR void advance(Iterator& i, Distance n) {
    i += n;
}

template <typename Iterator>
    requires(InputIterator<Iterator>)
MSTL_CONSTEXPR typename std::iterator_traits<Iterator>::difference_type
distance(Iterator first, Iterator last) {
    typename std::iterator_traits<Iterator>::difference_type n = 0;
    while (first != last) { ++first; ++n; }
    return n;
}
template <typename Iterator>
    requires(RandomAccessIterator<Iterator>)
MSTL_CONSTEXPR typename std::iterator_traits<Iterator>::difference_type 
distance(Iterator first, Iterator last) {
    return last - first;
}


template <typename Container>
    requires requires(Container& c, const typename Container::value_type& lv,
typename Container::value_type&& rv) { c.push_back(lv); c.push_back(rv); }
class back_insert_iterator {
public:
    typedef std::output_iterator_tag        iterator_category;
    typedef void                            value_type;
    typedef void                            difference_type;
    typedef void                            pointer;
    typedef void                            reference;
    typedef back_insert_iterator<Container> self;

    MSTL_CONSTEXPR explicit back_insert_iterator(Container& x) noexcept
        : container(std::addressof(x)) {}
    MSTL_CONSTEXPR self& operator =(const typename Container::value_type& value) {
        container->push_back(value);
        return *this;
    }
    MSTL_CONSTEXPR self& operator =(typename Container::value_type&& value) {
        container->push_back(std::move(value));
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
    requires requires(Container& c, const typename Container::value_type& lv,
typename Container::value_type&& rv) { c.push_front(lv); c.push_front(rv); }
class front_insert_iterator {
public:
    typedef std::output_iterator_tag    iterator_category;
    typedef void                        value_type;
    typedef void                        difference_type;
    typedef void                        pointer;
    typedef void                        reference;
    typedef front_insert_iterator<Container> self;

    MSTL_CONSTEXPR explicit front_insert_iterator(Container& x) noexcept
        : container(std::addressof(x)) {}
    MSTL_CONSTEXPR self& operator =(const typename Container::value_type& value) {
        container->push_front(value);
        return *this;
    }
    MSTL_CONSTEXPR self& operator =(typename Container::value_type&& value) {
        container->push_front(std::move(value));
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
    requires requires(Container& c, const typename Container::value_type& lv,
typename Container::value_type&& rv, typename Container::iterator it) 
{ c.insert(it, lv); c.insert(it, rv); }
class insert_iterator {
public:
    typedef std::output_iterator_tag    iterator_category;
    typedef void                        value_type;
    typedef void                        difference_type;
    typedef void                        pointer;
    typedef void                        reference;
    typedef insert_iterator<Container>  self;

    MSTL_CONSTEXPR insert_iterator(Container& x, typename Container::iterator it) noexcept
        : container(std::addressof(x)), iter(std::move(it)) {}
    MSTL_CONSTEXPR self& operator =(const typename Container::value_type& value) {
        iter = container->insert(iter, value);
        ++iter;
        return *this;
    }
    MSTL_CONSTEXPR self& operator =(typename Container::value_type&& value) {
        iter = container->insert(iter, std::move(value));
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

template <class Iterator, class T = typename Iterator::value_type,
    class Dist = typename Iterator::difference_type>
    requires(RandomAccessIterator<Iterator>)
class reverse_iterator {
public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef T                               value_type;
    typedef Dist                            difference_type;
    typedef T*                              pointer;
    typedef T&                              reference;
    typedef reverse_iterator<Iterator, T, Dist> self;

    MSTL_CONSTEXPR reverse_iterator() = default;
    MSTL_CONSTEXPR explicit reverse_iterator(Iterator x)
        noexcept(NothrowMoveConstructible<Iterator>)
        : current(std::move(x)) {}

    template <typename U>
        requires(!SameTo<U, Iterator> && Convertible<const U&, Iterator>)
    MSTL_CONSTEXPR explicit reverse_iterator(const reverse_iterator<U>& x)
        noexcept(NothrowConstructibleFrom<self, const U&>)
        : current(x.current) {}

    template <typename U>
        requires(!SameTo<U, Iterator> && Convertible<const U&, Iterator>
    && AssignableFrom<Iterator&, const U&>)
        MSTL_CONSTEXPR self& operator =(const reverse_iterator<U>& x)
        noexcept(NothrowAssignableFrom<self&, const U&>) {
        current = x.current;
        return *this;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR const Iterator& base() const noexcept {
        return current;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR reference operator *() const 
        noexcept(NothrowCopyConstructible<Iterator> && noexcept(*--(std::declval<Iterator&>()))) {
        Iterator iter = current;
        return *--iter;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR pointer operator ->() const
        noexcept(NothrowCopyConstructible<Iterator> && noexcept(*--(std::declval<Iterator&>()))) {
        return &(operator*());
    }
    MSTL_CONSTEXPR self& operator ++()
         noexcept(noexcept(--current)) {
        --current;
        return *this;
    }
    MSTL_CONSTEXPR self operator ++(int)
        noexcept(NothrowCopyConstructible<Iterator> && noexcept(--current)) {
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
        noexcept(NothrowCopyConstructible<Iterator> && noexcept(++current)) {
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
        noexcept(noexcept(*this + n)) {
        return *(*this + n);
    }

private:
    Iterator current;
};
template <class Iterator1, class Iterator2>
    requires(RandomAccessIterator<Iterator1> && RandomAccessIterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(x.base() == y.base())) requires requires {
        { x.base() == y.base() } -> std::convertible_to<bool>; } {
    return x.base() == y.base();
}
template <class Iterator1, class Iterator2>
    requires(RandomAccessIterator<Iterator1>&& RandomAccessIterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(x.base() != y.base())) requires requires {
        { x.base() != y.base() } -> std::convertible_to<bool>; } {
    return x.base() != y.base();
}
template <class Iterator1, class Iterator2>
    requires(RandomAccessIterator<Iterator1>&& RandomAccessIterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(x.base() > y.base())) requires requires {
        { x.base() > y.base() } -> std::convertible_to<bool>; } {
    return x.base() > y.base();
}
template <class Iterator1, class Iterator2>
    requires(RandomAccessIterator<Iterator1>&& RandomAccessIterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(x.base() < y.base())) requires requires {
        { x.base() < y.base() } -> std::convertible_to<bool>; } {
    return x.base() < y.base();
}
template <class Iterator1, class Iterator2>
    requires(RandomAccessIterator<Iterator1>&& RandomAccessIterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(x.base() >= y.base())) requires requires {
        { x.base() >= y.base() } -> std::convertible_to<bool>; } {
    return x.base() >= y.base();
}
template <class Iterator1, class Iterator2>
    requires(RandomAccessIterator<Iterator1>&& RandomAccessIterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(x.base() <= y.base())) requires requires {
        { x.base() <= y.base() } -> std::convertible_to<bool>; } {
    return x.base() <= y.base();
}
template <class Iterator1, class Iterator2>
    requires(RandomAccessIterator<Iterator1>&& RandomAccessIterator<Iterator2>)
MSTL_NODISCARD MSTL_CONSTEXPR decltype(auto) operator -(
    const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
    noexcept(noexcept(y.base() - x.base())) {
    return y.base() - x.base();
}
template <class Iterator>
    requires(RandomAccessIterator<Iterator>)
MSTL_CONSTEXPR reverse_iterator<Iterator> operator +(
    typename reverse_iterator<Iterator>::difference_type n, const reverse_iterator<Iterator>& x) 
    noexcept(noexcept(x + n)) {
    return x + n;
}
template <typename Iterator, typename T, typename Dist>
MSTL_NODISCARD MSTL_CONSTEXPR reverse_iterator<Iterator> 
make_reverse_iterator(Iterator it) noexcept(NothrowMoveConstructible<Iterator>) {
    return reverse_iterator<Iterator, T, Dist>(std::move(it));
}

MSTL_END_NAMESPACE__

#endif // MSTL_ITERATOR_HPP__
