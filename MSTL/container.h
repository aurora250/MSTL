#ifndef MSTL_CONTAINER_H__
#define MSTL_CONTAINER_H__
#include "object.h"
#include "check_type.h"
#include <iostream>

MSTL_BEGIN_NAMESPACE__

struct container : public object {
	typedef size_t size_type;
	virtual ~container() = 0;
	virtual size_type size() const = 0;
	virtual bool empty() const = 0;
};

template <typename Container>
inline void __show_size_only(const Container& c, std::ostream& _out) {
	_out << "size: " << c.size() << std::endl;
}
template <typename Container>
inline void __show_data_only(const Container& c, std::ostream& _out) {
	using const_iterator = typename Container::const_iterator;
	const_iterator _band = c.const_end();
	--_band;
	_out << '[' << std::flush;
	for (const_iterator iter = c.const_begin(); iter != c.const_end(); ++iter) {
		_out << *iter << std::flush;
		if (iter != _band) _out << ", " << std::flush;
	}
	_out << ']' << std::flush;
}

template <typename Container>
void __det__(const Container& c, std::ostream& _out = std::cout) {
	split_line(_out);
	_out << "type: " << Container::__type__ << std::endl;
	_out << "check type: " << check_type<Container::self>() << std::endl;
	__show_size_only(c, _out);
	_out << "data: " << std::flush;
	__show_data_only(c, _out);
	_out << std::endl;
	split_line(_out);
}

MSTL_END_NAMESPACE__

#endif // MSTL_CONTAINER_H__
