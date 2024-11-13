#ifndef MSTL_TYPE_TRAITS_HPP__
#define MSTL_TYPE_TRAITS_HPP__
#include <iterator>
#include "basiclib.h"

MSTL_BEGIN_NAMESPACE__

namespace iterator_tag {
#if MSTL_NEED_ITERATOR_TAG__
	// 为同STL算法协同工作, iterator_tag与iterator_traits仅仅写明，并不使用
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
}

template <typename Iterator>
inline typename std::iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
	using category = std::iterator_traits<Iterator>::iterator_category;
	return category();
}
template <typename Iterator>
inline typename std::iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
	return static_cast<typename std::iterator_traits<Iterator>::difference_type*>(0);
}
template <typename Iterator>
inline typename std::iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
	return static_cast<typename std::iterator_traits<Iterator>::value_type*>(0);
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
	template<> \
	struct __type_traits<OPT> \
	{ \
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
#if _WIN64
DEFAULT_TYPE_TRAITS__(__int64)
DEFAULT_TYPE_TRAITS__(unsigned __int64)
#endif // _WIN64

#endif // MSTL_NEED_SGI_TYPE_TRAITS__

template <typename InputIterator, typename Distance>
inline void __advance(InputIterator& i, Distance n, std::input_iterator_tag) {
	while (n--) ++i;
}
template <typename BidirectionalIterator, typename Distance>
inline void __advance(BidirectionalIterator& i, Distance n, std::bidirectional_iterator_tag) {
	if (n >= 0)
		while (n--) ++i;
	else
		while (n++) --i;
}
template <typename RandomAccessIterator, typename Distance>
inline void __advance(RandomAccessIterator& i, Distance n, std::random_access_iterator_tag) {
	i += n;
}
template <typename InputIterator, typename Distance>
inline void advance(InputIterator& iter, Distance n) {
	__advance(iter, n, iterator_category(iter));
}

template <typename InputIterator, typename Distance>
inline void __distance(InputIterator first, InputIterator last, Distance& n, std::input_iterator_tag) {
	while (first != last) { ++first; ++n; }
}
template <typename RandomAccessIterator, typename Distance>
inline void __distance(RandomAccessIterator first, RandomAccessIterator last,
	Distance& n, std::random_access_iterator_tag) {
	n += last - first;
}
template <typename InputIterator, typename Distance>
inline void distance(InputIterator first, InputIterator last, Distance& n) {
	__distance(first, last, n, iterator_category(first));
}
template <typename InputIterator>
inline typename std::iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, std::input_iterator_tag) {
	typename std::iterator_traits<InputIterator>::difference_type n = 0;
	while (first != last) { ++first; ++n; }
	return n;
}
template <typename RandomAccessIterator>
inline typename std::iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, std::random_access_iterator_tag) {
	return last - first;
}
template <typename InputIterator>
inline typename std::iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
	using category = std::iterator_traits<InputIterator>::iterator_category;
	return __distance(first, last, category());
}

MSTL_END_NAMESPACE__

#endif // MSTL_TYPE_TRAITS_HPP__
