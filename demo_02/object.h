#ifndef MSTL_OBJECT_H
#define MSTL_OBJECT_H
#include "check_type.h"
#include "basiclib.h"
#include <iostream>

MSTL_BEGIN_NAMESPACE__

class object {
public:
	virtual void __det__(std::ostream& _out = std::cout) const = 0;
protected:
	virtual void __show_data_only(std::ostream& _out) const = 0;
public:
	explicit object() noexcept;
	virtual ~object() = 0;
};

MSTL_END_NAMESPACE__

#endif

