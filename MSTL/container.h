#ifndef MSTL_CONTAINER_H__
#define MSTL_CONTAINER_H__
#include "check_type.h"
#include "concepts.hpp"
#include "errorlib.h"
#include "pair.hpp"
#include <iostream>

MSTL_BEGIN_NAMESPACE__
using namespace MSTL::concepts;

template <typename Container> requires(Detailable<Container>)
inline MSTL_CONSTEXPR void __show_data_only(const Container& c, std::ostream& _out) {
	using const_iterator = typename Container::const_iterator;
	size_t _band = c.size() - 1; size_t vl = 0;
	_out << '[' << std::flush;
	for (const_iterator iter = c.const_begin(); iter != c.const_end(); ++iter, ++vl) {
		_out << *iter << std::flush;
		if (vl != _band) _out << ", " << std::flush;
	}
	_out << ']' << std::flush;
}
template <typename T1, typename T2> requires(Printable<T1> && Printable<T2>)
inline MSTL_CONSTEXPR void __show_data_only(const std::pair<T1, T2>& p, std::ostream& _out) {
	_out << "{ " << p.first << ", " << p.second << " }";
}

template <typename Container> requires(Detailable<Container>)
MSTL_CONSTEXPR void detailof(const Container& c, std::ostream& _out = std::cout) {
	split_line(_out);
	_out << "type: " << check_type<Container>() << std::endl;
	_out << "size: " << c.size() << std::endl;
	_out << "data: " << std::flush;
	__show_data_only(c, _out);
	_out << std::endl;
	split_line(_out);
}

MSTL_END_NAMESPACE__

#endif // MSTL_CONTAINER_H__
