#ifndef MSTL_CONTAINER_H__
#define MSTL_CONTAINER_H__
#include "object.h"
#include <iostream>

MSTL_BEGIN_NAMESPACE__

class container : public object {
public:
	typedef size_t size_type;

	static const int epos;
protected:
	size_type _size;
public:
	virtual void __show_size_only(std::ostream& _out) const;
	virtual bool __in_boundary(int _pos) const;
	container(size_type _size = 0);
	virtual ~container() = 0;
	virtual size_type size() const;
	virtual bool empty() const;
};

MSTL_END_NAMESPACE__

#endif // MSTL_CONTAINER_H__
