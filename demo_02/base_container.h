#ifndef BASE_CONTAINER_H
#define BASE_CONTAINER_H
#include "object.h"
#include <iostream>

namespace MSTL {
	class container : public object {
	public:
		static const int epos;
	protected:
		size_t _size;
	public:
		virtual void __show_size_only(std::ostream& _out) const;
		virtual bool __in_boundary(int _pos) const;
		container(size_t _size = 0);
		virtual ~container() = 0;
		virtual size_t size() const;
		virtual bool empty() const;
	};
}

#endif // BASE_CONTAINER_H
