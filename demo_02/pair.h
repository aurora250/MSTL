#ifndef PAIR_H
#define PAIR_H
#include "object.h"
#include <iostream>

namespace MSTL {
	template <typename T1, typename T2>
	struct pair : public object {
	private:
		inline void _show_data_only(std::ostream& _out) const {
			_out << "first: " << this->first << std::endl;
			_out << "second: " << this->second;
		}
	public:
		static const char* const __type__;
		void __det__(std::ostream& _out = std::cout) const {
			split_line(_out);
			_out << "type: " << __type__ << std::endl;
			this->_show_data_only(_out);
			_out << std::endl;
			split_line(_out);
		}
		T1 first;
		T2 second;
		explicit pair(const T1& _f, const T2& _s) : first(_f), second(_s) {}
		explicit pair(const pair<T1, T2>& _p) : first(_p.first), second(_p.second) {}
		explicit pair(pair<T1, T2>&& _p) {
			this->swap(std::forward<pair<T1, T2>>(_p));
		}
		~pair() = default;
		void swap(pair<T1, T2>&& _p) noexcept {
			std::swap(this->first, _p.first);
			std::swap(this->second, _p.second);
		}
		bool empty() const { return false; }
		pair<T1, T2>& operator =(const pair<T1, T2>& _p) {
			if (*this == _p)return *this;
			this->first = _p.first;
			this->second = _p.second;
			return *this;
		}
		bool operator >(const pair<T1, T2>& _p) const {
			if (this->first > _p.first) return true;
			else if (this->first == _p.first) {
				if (this->second > _p.second) return true;
				else return false;
			}
			else return false;
		}
		bool operator <(const pair<T1, T2>& _p) const {
			if (this->first < _p.first) return true;
			else if (this->first == _p.first) {
				if (this->second < _p.second) return true;
				else return false;
			}
			else return false;
		}
		bool operator ==(const pair<T1, T2>& _p) const {
			return this->first == _p.first && this->second == _p.second;
		}
		bool operator !=(const pair<T1, T2>& _p) const {
			return not (*this == _p);
		}
		bool operator >=(const pair<T1, T2>& _p) const {
			return not (*this < _p);
		}
		bool operator <=(const pair<T1, T2>& _p) const {
			return not (*this > _p);
		}
		friend std::ostream& operator <<(std::ostream& _out, const MSTL::pair<T1, T2>& _p);
		friend std::istream& operator >>(std::istream& _in, MSTL::pair<T1, T2>& _p);
	};
	template <typename T1, typename T2>
	const char* const pair<T1, T2>::__type__ = "pair";

	template <typename T1, typename T2>
	std::ostream& operator <<(std::ostream& _out, const MSTL::pair<T1, T2>& _p) {
		_p._show_data_only(_out);
		return _out;
	}
	template <typename T1, typename T2>
	std::istream& operator >>(std::istream& _in, MSTL::pair<T1, T2>& _p) {
		_in << _p.first << _p.second;
		return _in;
	}

	template <typename T1, typename T2>
	pair<T1, T2>& make_pair(T1 _f, T2 _s) {
		return pair<T1, T2>(_f, _s);
	}
}

#endif
