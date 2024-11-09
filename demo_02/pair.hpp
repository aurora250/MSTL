#ifndef PAIR_H
#define PAIR_H
#include "object.h"
#include <iostream>

MSTL_BEGIN_NAMESPACE__

template <typename T1, typename T2>
struct pair : public object {
public:
	typedef T1				first_type;
	typedef T2				second_type;
	typedef pair<T1, T2>	self;

	static const char* const __type__;
	inline void __show_data_only(std::ostream& _out) const {
		_out << "first: " << this->first << std::endl;
		_out << "second: " << this->second;
	}
	void __det__(std::ostream& _out = std::cout) const {
		split_line(_out);
		_out << "type: " << __type__ << std::endl;
		_out << "check type: " << check_type<self>() << std::endl;
		this->__show_data_only(_out);
		_out << std::endl;
		split_line(_out);
	}
	first_type first;
	second_type second;
	explicit pair(const first_type& _f = first_type(), const second_type& _s = second_type()) :
		first(_f), second(_s) {}
	explicit pair(const self& _p) : first(_p.first), second(_p.second) {}
	explicit pair(self&& _p) {
		this->swap(std::forward<self>(_p));
	}
	~pair() = default;
	void swap(self&& _p) noexcept {
		std::swap(this->first, _p.first);
		std::swap(this->second, _p.second);
	}
	bool empty() const { return false; }
	self& operator =(const self& _p) {
		if (*this == _p) return *this;
		this->first = _p.first;
		this->second = _p.second;
		return *this;
	}
	bool operator >(const self& _p) const {
		if (this->first > _p.first) return true;
		else if (this->first == _p.first) {
			if (this->second > _p.second) return true;
			else return false;
		}
		else return false;
	}
	bool operator <(const self& _p) const {
		if (this->first < _p.first) return true;
		else if (this->first == _p.first) {
			if (this->second < _p.second) return true;
			else return false;
		}
		else return false;
	}
	bool operator ==(const self& _p) const {
		return this->first == _p.first && this->second == _p.second;
	}
	bool operator !=(const self& _p) const {
		return not (*this == _p);
	}
	bool operator >=(const self& _p) const {
		return not (*this < _p);
	}
	bool operator <=(const self& _p) const {
		return not (*this > _p);
	}
};

template <typename T1, typename T2>
const char* const pair<T1, T2>::__type__ = "pair";

template <typename T1, typename T2>
std::ostream& operator <<(std::ostream& _out, const pair<T1, T2>& _p) {
	_p.__show_data_only(_out);
	return _out;
}
template <typename T1, typename T2>
std::istream& operator >>(std::istream& _in, pair<T1, T2>& _p) {
	_in << _p.first << _p.second;
	return _in;
}

template <typename T1, typename T2>
pair<T1, T2> make_pair(T1 _f, T2 _s) {
	return pair<T1, T2>(_f, _s);
}

MSTL_END_NAMESPACE__

#endif
