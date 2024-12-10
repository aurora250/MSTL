#ifndef MSTL_OBJECT_H__
#define MSTL_OBJECT_H__
#include "basiclib.h"

MSTL_BEGIN_NAMESPACE__

struct object {
	virtual void __show_data_only(std::ostream& _out) const = 0;
	virtual ~object() = 0;
};

MSTL_END_NAMESPACE__

#endif // MSTL_OBJECT_H__

