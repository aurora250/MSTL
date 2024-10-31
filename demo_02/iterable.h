#ifndef ITERABLE_H
#define ITERABLE_H
#include "object.h"
#include <iostream>

namespace MSTL {
	class sciterable : public object {
	public:
		static const char* const __type__;
		static const int epos;
	protected:
		size_t _size;
		size_t _capacity;

		void _clear_sc_only();
		void _set_sc_only(size_t _size, size_t _cap);
		virtual void _show_sc_only(std::ostream& _out) const;
		virtual bool _in_boundary(int _pos) const;
	public:
		explicit sciterable(size_t _size = 1, size_t _capacity = 1) noexcept;
		virtual ~sciterable() = 0;
		virtual size_t size() const;
		virtual size_t capacity() const;
		virtual bool empty() const;
	};

	class siterable : public object {
	public:
		static const char* const __type__;
		static const int epos;
	protected:
		size_t _size;

		void _clear_size_only();
		void _set_size_only(size_t _size);
		virtual void _show_size_only(std::ostream& _out) const;
		virtual bool _in_boundary(int _pos) const;
	public:
		explicit siterable(size_t _size = 1) noexcept;
		virtual ~siterable();
		virtual size_t size() const;
		virtual bool empty() const;
	};
}

#endif
