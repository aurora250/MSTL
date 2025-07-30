#ifndef MSTL_DETAILOF_HPP__
#define MSTL_DETAILOF_HPP__
#include "check_type.hpp"
#include "queue.hpp"
#include "stack.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_INLINE17 constexpr uint32_t MSTL_SPLIT_LENGTH = 15U;

inline void split_line(std::ostream& out = std::cout, uint32_t size = MSTL_SPLIT_LENGTH, const char split = '-') {
	while (size--) out << split;
	out << '\n';
}

template <typename Container>
void show_data_only(const Container& c, std::ostream& _out) {
	_out << '[';
    auto last = _MSTL cend(c);
    auto first = _MSTL cbegin(c);
	for (auto iter = first; iter != last; ++iter) {
	    if(iter != first) _out << ", ";
		_out << *iter;
	}
	_out << ']' << std::flush;
}

template <typename T1, typename T2>
void show_data_only(const _MSTL pair<T1, T2>& p, std::ostream& _out) {
	_out << "{ " << p.first << ", " << p.second << " }" << std::flush;
}
template <typename T1, typename T2>
std::ostream& operator <<(std::ostream& _out, const _MSTL pair<T1, T2>& _p) {
	show_data_only(_p, _out);
	return _out;
}

template <typename CharT, typename Traits, typename Alloc>
void show_data_only(const basic_string<CharT, Traits, Alloc>& str, std::ostream& _out) {
	auto raw_str = str.c_str();
	for (size_t idx = 0; idx < str.size(); ++idx)
		_out << *(raw_str + idx);
	_out << std::flush;
}
template <typename CharT, typename Traits, typename Alloc>
std::ostream& operator <<(std::ostream& _out, const basic_string<CharT, Traits, Alloc>& str) {
	show_data_only(str, _out);
	return _out;
}

template <typename CharT, typename Traits>
void show_data_only(const basic_string_view<CharT, Traits>& str, std::ostream& _out) {
	auto raw_str = str.data();
	for (size_t idx = 0; idx < str.size(); ++idx)
		_out << *(raw_str + idx);
	_out << std::flush;
}
template <typename CharT, typename Traits>
std::ostream& operator <<(std::ostream& _out, const basic_string_view<CharT, Traits>& view) {
	show_data_only(view, _out);
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

inline void show_data_only(nullptr_t, std::ostream& out) {
    out << "nullptr" << std::flush;
}
inline std::ostream& operator <<(std::ostream& out, const nullptr_t null) {
    show_data_only(null, out);
    return out;
}

template <typename Container>
void detailof(const Container& c, std::ostream& out = std::cout) {
	split_line(out);
	out << "type: " << _MSTL check_type<Container>() << '\n';
	out << "size: " << _MSTL size(c) << '\n';
	out << "data: ";
	_MSTL show_data_only(c, out);
	out << std::endl;
	_MSTL split_line(out);
}

template <typename T, typename Sequence>
void detailof(const stack<T, Sequence>& stk, std::ostream& out = std::cout) {
	split_line(out);
	out << "type: " << check_type<stack<T, Sequence>>() << '\n';
	out << "size: " << stk.size() << '\n';
	out << "data: ";
	show_data_only(stk.seq_, out);
	out << std::endl;
	split_line(out);
}

template <typename T, typename Sequence>
void detailof(const queue<T, Sequence>& pq, std::ostream& out = std::cout) {
	split_line(out);
	out << "type: " << check_type<queue<T, Sequence>>() << '\n';
	out << "size: " << pq.size() << '\n';
	out << "data: ";
	show_data_only(pq.seq_, out);
	out << std::endl;
	split_line(out);
}

template <typename T, typename Sequence, typename Compare>
void detailof(const priority_queue<T, Sequence, Compare>& pq, std::ostream& out = std::cout) {
	split_line(out);
	out << "type: " << check_type<priority_queue<T, Sequence, Compare>>() << '\n';
	out << "size: " << pq.size() << '\n';
	out << "data: ";
	show_data_only(pq.pair_.value, out);
	out << std::endl;
	split_line(out);
}

MSTL_END_NAMESPACE__
#endif // MSTL_DETAILOF_HPP__
