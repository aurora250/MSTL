#ifndef MSTL_CONTAINER_H__
#define MSTL_CONTAINER_H__
#include "check_type.h"
#include "concepts.hpp"
#include "errorlib.h"
#include "queue.hpp"
#include "utility.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Container>
	requires(Detailable<Container>)
inline void show_data_only(const Container& c, std::ostream& _out) {
	using const_iterator = typename Container::const_iterator;
	size_t _band = c.size() - 1; size_t vl = 0;
	_out << '[' << std::flush;
	for (const_iterator iter = c.cbegin(); iter != c.cend(); ++iter, ++vl) {
		_out << *iter << std::flush;
		if (vl != _band) _out << ", " << std::flush;
	}
	_out << ']' << std::flush;
}
template <typename T1, typename T2>
	requires(Printable<T1> && Printable<T2>)
inline void show_data_only(const MSTL::pair<T1, T2>& p, std::ostream& _out) {
	_out << "{ " << p.first << ", " << p.second << " }";
}
template <typename T1, typename T2>
inline std::ostream& operator <<(std::ostream& _out, const MSTL::pair<T1, T2>& _p) {
	show_data_only(_p, _out);
	return _out;
}

template <typename Container>
	requires(Detailable<Container>)
void detailof(const Container& c, std::ostream& _out = std::cout) {
	split_line(_out);
	_out << "type: " << MSTL::check_type<Container>() << std::endl;
	_out << "size: " << c.size() << std::endl;
	_out << "data: " << std::flush;
	show_data_only(c, _out);
	_out << std::endl;
	split_line(_out);
}

template <typename T, typename Sequence>
void detailof(const queue<T, Sequence>& pq, std::ostream& _out = std::cout) {
	split_line(_out);
	_out << "type: " << check_type<queue<T, Sequence>>() << std::endl;
	_out << "size: " << pq.size() << std::endl;
	_out << "data: " << std::flush;
	show_data_only(pq.seq_, _out);
	_out << std::endl;
	split_line(_out);
}

template <typename T, typename Sequence, typename Compare>
void detailof(const priority_queue<T, Sequence, Compare>& pq, std::ostream& _out = std::cout) {
	split_line(_out);
	_out << "type: " << check_type<priority_queue<T, Sequence, Compare>>() << std::endl;
	_out << "size: " << pq.size() << std::endl;
	_out << "data: " << std::flush;
	show_data_only(pq.seq_, _out);
	_out << std::endl;
	split_line(_out);
}

MSTL_END_NAMESPACE__

#endif // MSTL_CONTAINER_H__
