#ifndef MSTL_CONTAINER_H__
#define MSTL_CONTAINER_H__
#include "object.h"
#include <iostream>

MSTL_BEGIN_NAMESPACE__

struct container : public object {
	typedef size_t size_type;
	virtual void __det__(std::ostream& _out = std::cout) const = 0;
	virtual void __show_size_only(std::ostream& _out) const = 0;
	virtual ~container() = 0;
	virtual size_type size() const = 0;
	virtual bool empty() const = 0;
};

MSTL_END_NAMESPACE__

#endif // MSTL_CONTAINER_H__
