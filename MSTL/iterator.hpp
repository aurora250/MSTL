#ifndef MSTL_ITERATOR_HPP__
#define MSTL_ITERATOR_HPP__
#include "basiclib.h"
#include "type_traits.hpp"

MSTL_BEGIN_NAMESPACE__

template <typename InputIterator, typename Distance>
inline void __advance(InputIterator& i, Distance n, MSTL_ITERATOR_TRATIS_FROM__ input_iterator_tag) {
    while (n--) ++i;
}
template <typename BidirectionalIterator, typename Distance>
inline void __advance(BidirectionalIterator& i, Distance n, MSTL_ITERATOR_TRATIS_FROM__ bidirectional_iterator_tag) {
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}
template <typename RandomAccessIterator, typename Distance>
inline void __advance(RandomAccessIterator& i, Distance n, MSTL_ITERATOR_TRATIS_FROM__ random_access_iterator_tag) {
    i += n;
}
template <typename InputIterator, typename Distance>
inline void advance(InputIterator& iter, Distance n) {
    __advance(iter, n, (iterator_category)(iter));
}

template <typename InputIterator, typename Distance>
inline void __distance(InputIterator first, InputIterator last,
    Distance& n, MSTL_ITERATOR_TRATIS_FROM__ input_iterator_tag) {
    while (first != last) { ++first; ++n; }
}
template <typename RandomAccessIterator, typename Distance>
inline void __distance(RandomAccessIterator first, RandomAccessIterator last,
    Distance& n, MSTL_ITERATOR_TRATIS_FROM__ random_access_iterator_tag) {
    n += last - first;
}
template <typename InputIterator, typename Distance>
inline void distance(InputIterator first, InputIterator last, Distance& n) {
    MSTL::__distance(first, last, n, (iterator_category)(first));
}

template <typename InputIterator>
inline typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, MSTL_ITERATOR_TRATIS_FROM__ input_iterator_tag) {
    typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) { ++first; ++n; }
    return n;
}
template <typename RandomAccessIterator>
inline typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, MSTL_ITERATOR_TRATIS_FROM__ random_access_iterator_tag) {
    return last - first;
}
template <typename InputIterator>
inline typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    using category = typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<InputIterator>::iterator_category;
    return __distance(first, last, category());
}

template <typename Container>
class back_insert_iterator {
private:
    Container* container;
public:
    typedef std::output_iterator_tag        iterator_category;
    typedef void                            value_type;
    typedef void                            difference_type;
    typedef void                            pointer;
    typedef void                            reference;
    typedef back_insert_iterator<Container> self;

    explicit back_insert_iterator(Container& x) : container(&x) {}
    self& operator =(const typename Container::value_type& value) {
        container->push_back(value);
        return *this;
    }
    self& operator *() { return *this; }
    self& operator ++() { return *this; }
    self& operator ++(int) { return *this; }
};
template <typename Container>
inline back_insert_iterator<Container> back_inserter(Container& x) {
    return back_insert_iterator<Container>(x);
}

template <typename Container>
class front_insert_iterator {
private:
    Container* container;
public:
    typedef std::output_iterator_tag    iterator_category;
    typedef void                        value_type;
    typedef void                        difference_type;
    typedef void                        pointer;
    typedef void                        reference;
    typedef front_insert_iterator<Container> self;

    explicit front_insert_iterator(Container& x) : container(&x) {}
    self& operator =(const typename Container::value_type& value) {
        container->push_front(value);
        return *this;
    }
    self& operator *() { return *this; }
    self& operator ++() { return *this; }
    self& operator ++(int) { return *this; }
};
template <typename Container>
inline front_insert_iterator<Container> front_inserter(Container& x) {
    return front_insert_iterator<Container>(x);
}

template <typename Container>
class insert_iterator {
private:
    Container* container;
    typename Container::iterator iter;
public:
    typedef std::output_iterator_tag    iterator_category;
    typedef void                        value_type;
    typedef void                        difference_type;
    typedef void                        pointer;
    typedef void                        reference;
    typedef insert_iterator<Container>  self;

    insert_iterator(Container& x, typename Container::iterator i)
        : container(&x), iter(i) {}
    self& operator =(const typename Container::value_type& value) {
        iter = container->insert(iter, value);
        ++iter;
        return *this;
    }
    self& operator *() { return *this; }
    self& operator ++() { return *this; }
    self& operator ++(int) { return *this; }
};
template <typename Container, typename Iterator>
inline insert_iterator<Container> inserter(Container& x, Iterator i) {
    return insert_iterator<Container>(x, typename Container::iterator(i));
}

MSTL_END_NAMESPACE__

#endif // MSTL_ITERATOR_HPP__
