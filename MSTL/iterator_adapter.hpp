#ifndef MSTL_ITERATOR_ADAPTER_H
#define MSTL_ITERATOR_ADAPTER_H
#include "iterator.hpp"
#include "basiclib.h"

MSTL_BEGIN_NAMESPACE__

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

#endif // MSTL_ITERATOR_ADAPTER_H
