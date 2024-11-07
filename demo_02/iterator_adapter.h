#ifndef ITERATOR_ADAPTER_H
#define ITERATOR_ADAPTER_H
#include <iterator>

namespace MSTL {
	// 为同STL算法协同工作， iterator_tag与iterator_traits仅仅写明，并不使用
	namespace iterator_tag {
		struct input_iterator_tag;
		struct output_iterator_tag;
		struct forward_iterator_tag;
		struct bidirectional_iterator_tag;
		struct random_access_iterator_tag;

		template <class T, class Distance> struct input_iterator {
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
		template <class T, class Distance> struct forward_iterator {
			typedef forward_iterator_tag iterator_category;
			typedef T                    value_type;
			typedef Distance             difference_type;
			typedef T* pointer;
			typedef T& reference;
		};
		template <class T, class Distance> struct bidirectional_iterator {
			typedef bidirectional_iterator_tag iterator_category;
			typedef T                          value_type;
			typedef Distance                   difference_type;
			typedef T* pointer;
			typedef T& reference;
		};
		template <class T, class Distance> struct random_access_iterator {
			typedef random_access_iterator_tag iterator_category;
			typedef T                          value_type;
			typedef Distance                   difference_type;
			typedef T* pointer;
			typedef T& reference;
		};
	}
	namespace iterator_traits {
		template <class Iterator>
		struct iterator_traits {
			typedef typename Iterator::iterator_category iterator_category;
			typedef typename Iterator::value_type        value_type;
			typedef typename Iterator::difference_type   difference_type;
			typedef typename Iterator::pointer           pointer;
			typedef typename Iterator::reference         reference;
		};
		template <class T>
		struct iterator_traits<T*> {
			typedef random_access_iterator_tag iterator_category;
			typedef T                          value_type;
			typedef ptrdiff_t                  difference_type;
			typedef T* pointer;
			typedef T& reference;
		};
		template <class T>
		struct iterator_traits<const T*> {
			typedef random_access_iterator_tag iterator_category;
			typedef T                          value_type;
			typedef ptrdiff_t                  difference_type;
			typedef const T* pointer;
			typedef const T& reference;
		};
	}
}

namespace MSTL {
	template <class Iterator>
	inline typename std::iterator_traits<Iterator>::iterator_category
		iterator_category(const Iterator&) {
		typedef typename std::iterator_traits<Iterator>::iterator_category category;
		return category();
	}
	template <class Iterator>
	inline typename std::iterator_traits<Iterator>::difference_type*
		distance_type(const Iterator&) {
		return static_cast<typename std::iterator_traits<Iterator>::difference_type*>(0);
	}
	template <class Iterator>
	inline typename std::iterator_traits<Iterator>::value_type*
		value_type(const Iterator&) {
		return static_cast<typename std::iterator_traits<Iterator>::value_type*>(0);
	}

	// __type_traits:
	struct __true_type;
	struct __false_type;

	template <class type>
	struct __type_traits {
		typedef __true_type this_dummy_member_must_be_first;
		typedef __false_type has_trivial_default_constructor;
		typedef __false_type has_trivial_copy_constructor;
		typedef __false_type has_trivial_assignment_operator;
		typedef __false_type has_trivial_destructor;
		typedef __false_type is_POD_type;
	};
	template<class T>
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

	template <class InputIterator, class Distance>
	inline void __advance(InputIterator& i, Distance n, std::input_iterator_tag) {
		while (n--) ++i;
	}
	template <class BidirectionalIterator, class Distance>
	inline void __advance(BidirectionalIterator& i, Distance n, std::bidirectional_iterator_tag) {
		if (n >= 0) 
			while (n--) ++i;
		else 
			while (n++) --i;
	}
	template <class RandomAccessIterator, class Distance>
	inline void __advance(RandomAccessIterator& i, Distance n, std::random_access_iterator_tag) { 
		i += n;
	}
	template <class InputIterator, class Distance>
	inline void advance(InputIterator& iter, Distance n) {
		__advance(iter, n, iterator_category(iter));
	}


}

#endif // ITERATOR_ADAPTER_H
