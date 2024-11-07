#ifndef ERROR_H
#define ERROR_H
#include <iostream>
#include <initializer_list>
#include <functional>
#include <map>
#include "object.h"

namespace MSTL {
	typedef const char* cstring;
	typedef cstring const const_cstring;

	class Error : public object {
	public:
		cstring _info;
		cstring _type; 
		void _show_data_only(std::ostream& _out) const;

		explicit Error(cstring _info = __type__, cstring _type = __type__) noexcept;
		virtual ~Error();
		void __det__(std::ostream& _out = std::cout) const;
		static const_cstring __type__;
		// friend void Exception(Error* _err);   use non-mumber instead of friend function
	};

	class StopIterator : public Error {  // 迭代器越界或无值
	public:
		explicit StopIterator(cstring _info = "Iterator out of range or is NULL.") noexcept;
		static const_cstring __type__;
	};

	class AssertError : public Error {  // 断言错误 断言错误太宽泛，不爱用
	public:
		explicit AssertError(cstring _info = "Assert false!") noexcept;
		static const_cstring __type__;
	};

	class AttributeError : public Error {   // 对象无此属性
	public:
		explicit AttributeError(cstring _info = "No this attriubte in the object.") noexcept;
		static const_cstring __type__;
	};

	class MemoryError : public Error {   // 内存操作失败
	public:
		explicit MemoryError(cstring _info = "Memory operation falied!") noexcept;
		static const_cstring __type__;
	};

	class ValueError : public Error {   // 函数的参数非法
	public:
		explicit ValueError(cstring _info = "Function refused this value.") noexcept;
		static const_cstring __type__;
	};

	class RangeError : public Error {   // 数组越界或无值
	public:
		explicit RangeError(cstring _info = "Index out of range or value is NULL.") noexcept;
		static const_cstring __type__;
	};

	static std::map<const char* const, Error*> error_map {
		{StopIterator::__type__,	new StopIterator()}, 
		{AssertError::__type__,		new AssertError()},
		{AttributeError::__type__,	new AttributeError()},
		{MemoryError::__type__,		new MemoryError()},
		{ValueError::__type__,		new ValueError()},
		{RangeError::__type__,		new RangeError()}
	};

	extern void Exception(Error* _err);
	extern void Exception(bool _boolean, Error* _err = nullptr);
	extern void Assert(bool _boolean, const char* const _info = "Assert false!");
	extern void Exit(bool _abort = false, void(__cdecl* _func)(void) = nullptr);
	// just allow void(void) function to run before progess exit
}

#endif
