#ifndef MSTL_CONTAINER_H__
#define MSTL_CONTAINER_H__
#include "check_type.h"
#include "queue.hpp"
#include "stack.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Container>
	requires(is_detailable<Container>)
void show_data_only(const Container& c, std::ostream& _out) {
	_out << '[';
	for (auto iter = c.cbegin(); iter != c.cend();) {
		_out << *iter << std::flush;
		++iter;
		if (iter != c.cend()) _out << ", ";
	}
	_out << ']' << std::flush;
}

inline void show_data_only(const Error& e, std::ostream& _out) {
	_out << "Exception : (" << e._type << ") " << e._info << std::flush;
}
inline std::ostream& operator <<(std::ostream& _out, const Error& err) {
	show_data_only(err, _out);
	return _out;
}

template <typename T1, typename T2>
	requires(is_printable<T1> && is_printable<T2>)
void show_data_only(const MSTL::pair<T1, T2>& p, std::ostream& _out) {
	_out << "{ " << p.first << ", " << p.second << " }" << std::flush;
}
template <typename T1, typename T2>
std::ostream& operator <<(std::ostream& _out, const MSTL::pair<T1, T2>& _p) {
	show_data_only(_p, _out);
	return _out;
}

template <typename CharT, typename Traits, typename Alloc>
void show_data_only(const basic_string<CharT, Traits, Alloc>& _str, std::ostream& _out) {
	auto raw_str = _str.c_str();
	for (size_t idx = 0; idx < _str.size(); ++idx)
		_out << *(raw_str + idx);
	_out << std::flush;
}
template <typename CharT, typename Traits, typename Alloc>
std::ostream& operator <<(std::ostream& _out, const basic_string<CharT, Traits, Alloc>& _str) {
	show_data_only(_str, _out);
	return _out;
}

template <typename T, typename Deleter>
void show_data_only(const unique_ptr<T, Deleter>& ptr, std::ostream& out) {
	out << ptr.get() << " (unique_to: " << *ptr.get() << " )" << std::flush;
}
template <typename T, typename Deleter>
std::ostream& operator <<(std::ostream& out, const unique_ptr<T, Deleter>& ptr) {
	show_data_only(ptr, out);
	return out;
}

template <typename T>
void show_data_only(const shared_ptr<T>& ptr, std::ostream& out) {
	out << ptr.get() << " (shared_to: " << *ptr.get() << " )" << std::flush;
}
template <typename T>
std::ostream& operator <<(std::ostream& out, const shared_ptr<T>& ptr) {
	show_data_only(ptr, out);
	return out;
}

template <typename Container>
	requires(is_detailable<Container>)
void detailof(const Container& c, std::ostream& _out = std::cout) {
	split_line(_out);
	_out << "type: " << MSTL::check_type<Container>() << '\n';
	_out << "size: " << c.size() << '\n';
	_out << "data: ";
	show_data_only(c, _out);
	_out << std::endl;
	split_line(_out);
}

template <typename T, typename Sequence>
void detailof(const stack<T, Sequence>& stk, std::ostream& _out = std::cout) {
	split_line(_out);
	_out << "type: " << check_type<stack<T, Sequence>>() << '\n';
	_out << "size: " << stk.size() << '\n';
	_out << "data: ";
	show_data_only(stk.seq_, _out);
	_out << std::endl;
	split_line(_out);
}

template <typename T, typename Sequence>
void detailof(const queue<T, Sequence>& pq, std::ostream& _out = std::cout) {
	split_line(_out);
	_out << "type: " << check_type<queue<T, Sequence>>() << '\n';
	_out << "size: " << pq.size() << '\n';
	_out << "data: ";
	show_data_only(pq.seq_, _out);
	_out << std::endl;
	split_line(_out);
}

template <typename T, typename Sequence, typename Compare>
void detailof(const priority_queue<T, Sequence, Compare>& pq, std::ostream& _out = std::cout) {
	split_line(_out);
	_out << "type: " << check_type<priority_queue<T, Sequence, Compare>>() << '\n';
	_out << "size: " << pq.size() << '\n';
	_out << "data: ";
	show_data_only(pq.seq_, _out);
	_out << std::endl;
	split_line(_out);
}

MSTL_END_NAMESPACE__
#endif // MSTL_CONTAINER_H__
