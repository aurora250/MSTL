#ifndef OBJECT_H
#define OBJECT_H
#include <iostream>

namespace MSTL {
	class object {
	public:
		static const char* const __type__;
		virtual void __det__(std::ostream& _out = std::cout) const = 0;
	protected:
		virtual void _show_data_only(std::ostream& _out) const = 0;
	public:
		explicit object() noexcept;
		virtual ~object();
	};

	class NaN : public object {
	public:
		static const char* const __type__;
		virtual void __det__(std::ostream& _out = std::cout) const;
	protected:
		virtual void _show_data_only(std::ostream& _out) const;
	public:
		explicit NaN() noexcept;
		virtual ~NaN();
	};
}

#endif
