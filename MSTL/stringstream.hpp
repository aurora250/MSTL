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
	basic_stringstream() = default;

	basic_stringstream(const self& str) : buffer_(str.buffer_) {}
	basic_stringstream(self&& str) noexcept : buffer_(MSTL::move(str.buffer_)) {}

	basic_stringstream(const value_type& str) : buffer_(str) {}
	basic_stringstream(value_type&& str) noexcept : buffer_(MSTL::move(str)) {}
	basic_stringstream(const char_type* str) : buffer_(str) {}

	~basic_stringstream() = default;

	explicit operator value_type() const { return buffer_; }

	void str(value_type&& str) noexcept { buffer_ = MSTL::move(str); }
	void str(const value_type& str) { buffer_ = str; }
	void str(const char_type* str) { buffer_ = str; }
	value_type str() const { return buffer_; }
	view_type view() const { return view_type(buffer_.c_str()); }

	void clear() { buffer_.clear(); }


	self& operator <<(bool x) {
		buffer_.append(MSTL::move((__stream_to_string<CharT>)(static_cast<int>(x))));
		return *this;
	}
	self& operator <<(int x) {
		buffer_.append(MSTL::move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	self& operator <<(unsigned int x) {
		buffer_.append(MSTL::move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	self& operator <<(long x) {
		buffer_.append(MSTL::move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	self& operator <<(unsigned long x) {
		buffer_.append(MSTL::move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	self& operator <<(MSTL_LLT x) {
		buffer_.append(MSTL::move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	self& operator <<(unsigned MSTL_LLT x) {
		buffer_.append(MSTL::move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	self& operator <<(float x) {
		buffer_.append(MSTL::move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	self& operator <<(double x) {
		buffer_.append(MSTL::move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	self& operator <<(long double x) {
		buffer_.append(MSTL::move((__stream_to_string<CharT>)(x)));
		return *this;
	}
	self& operator <<(char_type x) {
		buffer_.append(x);
		return *this;
	}
	self& operator <<(const char_type* x) {
		buffer_.append(x);
		return *this;
	}
	self& operator <<(const value_type& x) {
		buffer_.append(x);
		return *this;
	}
	self& operator <<(value_type&& x) {
		buffer_.append(MSTL::move(x));
		return *this;
	}
	template <typename T1, typename T2>
	self& operator <<(const pair<T1, T2>& pir) {
		(*this) << "{ " << pir.first << ", " << pir.second << " }";
		return *this;
	}
	template <typename T>
	self& operator <<(const shared_ptr<T>& ptr) {
		(*this) << ptr.get();
		return *this;
	}
	template <typename T, typename Deleter>
	self& operator <<(const unique_ptr<T, Deleter>& ptr) {
		(*this) << ptr.get();
		return *this;
	}
};

using stringstream = basic_stringstream<char>;
using wstringstream = basic_stringstream<wchar_t>;

MSTL_END_NAMESPACE__
#endif // MSTL_STRINGSTREAM_HPP__
