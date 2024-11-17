#ifndef MSTL_OBJECT_H__
#define MSTL_OBJECT_H__
#include "basiclib.h"
#include <iostream>

MSTL_BEGIN_NAMESPACE__

class object {
protected:
	virtual void __show_data_only(std::ostream& _out) const = 0;
public:
	virtual void __det__(std::ostream& _out = std::cout) const = 0;

	explicit object() noexcept;
	virtual ~object() = 0;
};

MSTL_END_NAMESPACE__

#endif // MSTL_OBJECT_H__

