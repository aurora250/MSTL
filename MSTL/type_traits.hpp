#ifndef MSTL_TYPE_TRAITS_HPP__
#define MSTL_TYPE_TRAITS_HPP__
#include "basiclib.h"
#if MSTL_NEED_ITERATOR_TAG__
#include <iterator>
#endif
MSTL_BEGIN_NAMESPACE__

#if MSTL_NEED_ITERATOR_TAG__
#if defined(MSTL_COMPILE_GNUC__)
#else
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename T, typename Distance>
struct input_iterator {
	typedef input_iterator_tag iterator_category;
	typedef T                  value_type;
	typedef Distance           difference_type;
	typedef T* pointer;
	typedef T& reference;
};
struct output_iterator {
	typedef output_iterator_tag iterator_category;
	typedef void                value_type;
	typedef void                difference_type;
	typedef void                pointer;
	typedef void                reference;
};
template <typename T, typename Distance>
struct forward_iterator {
	typedef forward_iterator_tag iterator_category;
	typedef T                    value_type;
	typedef Distance             difference_type;
	typedef T* pointer;
	typedef T& reference;
};
template <typename T, typename Distance>
struct bidirectional_iterator {
	typedef bidirectional_iterator_tag iterator_category;
	typedef T                          value_type;
	typedef Distance                   difference_type;
	typedef T* pointer;
	typedef T& reference;
};
template <typename T, typename Distance>
struct random_access_iterator {
	typedef random_access_iterator_tag iterator_category;
	typedef T                          value_type;
	typedef Distance                   difference_type;
	typedef T* pointer;
	typedef T& reference;
};

template <typename Iterator>
struct iterator_traits {
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type        value_type;
	typedef typename Iterator::difference_type   difference_type;
	typedef typename Iterator::pointer           pointer;
	typedef typename Iterator::reference         reference;
};
template <typename T>
struct iterator_traits<T*> {
	typedef random_access_iterator_tag iterator_category;
	typedef T                          value_type;
	typedef ptrdiff_t                  difference_type;
	typedef T* pointer;
	typedef T& reference;
};
template <typename T>
struct iterator_traits<const T*> {
	typedef random_access_iterator_tag iterator_category;
	typedef T                          value_type;
	typedef ptrdiff_t                  difference_type;
	typedef const T* pointer;
	typedef const T& reference;
};
#endif 
#else
#if defined(MSTL_COMPILE_MSVC__)
using std::iterator_traits;
using std::output_iterator_tag;
using std::input_iterator_tag;
using std::forward_iterator_tag;
using std::bidirectional_iterator_tag;
using std::random_access_iterator_tag;
#endif
#endif

template <typename Iterator>
inline typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
	using category = typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<Iterator>::iterator_category;
	return category();
}
template <typename Iterator>
inline typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
	return static_cast<typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<Iterator>::difference_type*>(0);
}
template <typename Iterator>
inline typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
	return static_cast<typename MSTL_ITERATOR_TRATIS_FROM__ iterator_traits<Iterator>::value_type*>(0);
}

#if MSTL_NEED_SGI_TYPE_TRAITS__
// __type_traits:
struct __true_type {};
struct __false_type {};

template <typename type>
struct __type_traits {
	typedef __true_type	 this_dummy_member_must_be_first;
	typedef __false_type has_trivial_default_constructor;
	typedef __false_type has_trivial_copy_constructor;
	typedef __false_type has_trivial_assignment_operator;
	typedef __false_type has_trivial_destructor;
	typedef __false_type is_POD_type;
};
template<typename T>
struct __type_traits<T*> {
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

#define DEFAULT_TYPE_TRAITS__(OPT) \
	MSTL_TEMPLATE_NULL__  struct __type_traits<OPT> { \
		typedef __true_type has_trivial_default_constructor; \
		typedef __true_type has_trivial_copy_constructor; \
		typedef __true_type has_trivial_assignment_operator; \
		typedef __true_type has_trivial_destructor; \
		typedef __true_type is_POD_type; \
	};
DEFAULT_TYPE_TRAITS__(char)
DEFAULT_TYPE_TRAITS__(signed char)
DEFAULT_TYPE_TRAITS__(unsigned char)
DEFAULT_TYPE_TRAITS__(short)
DEFAULT_TYPE_TRAITS__(unsigned short)
DEFAULT_TYPE_TRAITS__(int)
DEFAULT_TYPE_TRAITS__(unsigned int)
DEFAULT_TYPE_TRAITS__(long)
DEFAULT_TYPE_TRAITS__(unsigned long)
DEFAULT_TYPE_TRAITS__(float)
DEFAULT_TYPE_TRAITS__(double)
DEFAULT_TYPE_TRAITS__(long double)
DEFAULT_TYPE_TRAITS__(long long)
DEFAULT_TYPE_TRAITS__(unsigned long long)
#endif // MSTL_NEED_SGI_TYPE_TRAITS__

MSTL_END_NAMESPACE__

#endif // MSTL_TYPE_TRAITS_HPP__
