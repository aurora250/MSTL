#ifndef MSTL_STRINGSTREAM_HPP__
#define MSTL_STRINGSTREAM_HPP__
#include "string.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename CharT>
class basic_stringstream {
public:
	using char_type			= CharT;
	using value_type		= basic_string<CharT>;
	using view_type			= basic_string_view<CharT>;
	using traits_type		= typename value_type::traits_type;
	using allocator_type	= typename value_type::allocator_type;
	using self				= basic_stringstream<CharT>;

private:
	basic_string<CharT> buffer_{};

public:
	MSTL_CONSTEXPR20 basic_stringstream() = default;

	MSTL_CONSTEXPR20 basic_stringstream(const self& str) : buffer_(str.buffer_) {}
	MSTL_CONSTEXPR20 basic_stringstream(self&& str) noexcept : buffer_(_MSTL move(str.buffer_)) {}

	MSTL_CONSTEXPR20 explicit basic_stringstream(const value_type& str) : buffer_(str) {}
	MSTL_CONSTEXPR20 explicit basic_stringstream(value_type&& str) noexcept : buffer_(_MSTL move(str)) {}
	MSTL_CONSTEXPR20 explicit basic_stringstream(const char_type* str) : buffer_(str) {}

	MSTL_CONSTEXPR20 ~basic_stringstream() = default;

	MSTL_CONSTEXPR20 explicit operator value_type() const { return buffer_; }

	MSTL_CONSTEXPR20 void str(value_type&& str) noexcept { buffer_ = _MSTL move(str); }
	MSTL_CONSTEXPR20 void str(const value_type& str) { buffer_ = str; }
	MSTL_CONSTEXPR20 void str(const char_type* str) { buffer_ = str; }
	MSTL_CONSTEXPR20 value_type str() const { return buffer_; }
	MSTL_CONSTEXPR20 view_type view() const { return view_type(buffer_.c_str()); }

	MSTL_CONSTEXPR20 void clear() { buffer_.clear(); }

	MSTL_CONSTEXPR20 self& operator <<(bool x) {
		buffer_.append(_MSTL move((__stream_to_string<CharT>)(static_cast<int>(x))));
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(int x) {
		buffer_.append(_MSTL move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(unsigned int x) {
		buffer_.append(_MSTL move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(long x) {
		buffer_.append(_MSTL move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(unsigned long x) {
		buffer_.append(_MSTL move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(MSTL_LLT x) {
		buffer_.append(_MSTL move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(unsigned MSTL_LLT x) {
		buffer_.append(_MSTL move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(float x) {
		buffer_.append(_MSTL move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(double x) {
		buffer_.append(_MSTL move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(long double x) {
		buffer_.append(_MSTL move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(char_type x) {
		buffer_.append(x);
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(const char_type* x) {
		buffer_.append(x);
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(const value_type& x) {
		buffer_.append(x);
		return *this;
	}
	MSTL_CONSTEXPR20 self& operator <<(value_type&& x) {
		buffer_.append(_MSTL move(x));
		return *this;
	}
	template <typename T1, typename T2>
	MSTL_CONSTEXPR20 self& operator <<(const pair<T1, T2>& pir) {
		*this << "{ " << pir.first << ", " << pir.second << " }";
		return *this;
	}
	template <typename T>
	MSTL_CONSTEXPR20 self& operator <<(const shared_ptr<T>& ptr) {
		*this << ptr.get();
		return *this;
	}
	template <typename T, typename Deleter>
	MSTL_CONSTEXPR20 self& operator <<(const unique_ptr<T, Deleter>& ptr) {
		*this << ptr.get();
		return *this;
	}
};

using stringstream = basic_stringstream<char>;
using wstringstream = basic_stringstream<wchar_t>;

MSTL_END_NAMESPACE__
#endif // MSTL_STRINGSTREAM_HPP__
