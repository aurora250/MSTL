#ifndef MSTL_ERROR_H__
#define MSTL_ERROR_H__
#include <iostream>
#include <initializer_list>
#include <map>
#include "object.h"

MSTL_BEGIN_NAMESPACE__

typedef const char* cstring;
typedef cstring const const_cstring;

struct Error : public object {
	typedef Error self;
	cstring _info;
	cstring _type;
	void __show_data_only(std::ostream& _out) const;

	explicit Error(cstring _info = __type__, cstring _type = __type__) noexcept;
	virtual ~Error();
	void __det__(std::ostream& _out = std::cout) const;
	static const_cstring __type__;
	// friend void Exception(Error* _err);   use non-mumber instead of friend function
};

struct StopIterator : public Error {  // 迭代器越界或无值
	typedef StopIterator self;
	explicit StopIterator(cstring _info = "Iterator out of range or is NULL.") noexcept;
	static const_cstring __type__;
};

struct AssertError : public Error {  // 断言错误 断言错误太宽泛，不爱用
	typedef AssertError self;
	explicit AssertError(cstring _info = "Assert false!") noexcept;
	static const_cstring __type__;
};

struct AttributeError : public Error {   // 对象无此属性
	typedef AttributeError self;
	explicit AttributeError(cstring _info = "No this attriubte in the object.") noexcept;
	static const_cstring __type__;
};

struct MemoryError : public Error {   // 内存操作失败
	typedef MemoryError self;
	explicit MemoryError(cstring _info = "Allocate or Deallocate falied!") noexcept;
	static const_cstring __type__;
};

struct ValueError : public Error {   // 函数的参数非法
	typedef ValueError self;
	explicit ValueError(cstring _info = "Function refused this value.") noexcept;
	static const_cstring __type__;
};

struct RangeError : public Error {   // 数组越界或无值
	typedef RangeError self;
	explicit RangeError(cstring _info = "Index out of range or value is NULL.") noexcept;
	static const_cstring __type__;
};

enum __error {
	StopIter, AssertErr, AttriErr, MemErr, ValErr, RangeErr
};

static std::map<__error, Error*> error_map{
	{__error::StopIter,		new StopIterator()},
	{__error::AssertErr,	new AssertError()},
	{__error::AttriErr,		new AttributeError()},
	{__error::MemErr,		new MemoryError()},
	{__error::ValErr,		new ValueError()},
	{__error::RangeErr,		new RangeError()}
};

extern void Exception(Error* _err);
extern void Exception(bool _boolean, Error* _err = nullptr);
extern void Assert(bool _boolean, const char* const _info = "Assert false!");

extern void Exit(bool _abort = false, void(__cdecl* _func)(void) = nullptr);
// just allow void(void) function to run before progess exit

#define MSTL_EXCE_EXP_THROW__(exp, error) Exception(exp, error);
#define MSTL_EXCE_PTR_THROW__(pointer) Exception(pointer);
#define MSTL_EXEC_MAP_THROW__(index) Exception(error_map[__error::index]);

#define MSTL_EXEC_MEMORY__ MSTL_EXCE_PTR_THROW__(error_map[__error::MemErr])

#define MSTL_TRY__ try
#define MSTL_CATCH_UNWIND__ catch(...)
#define MSTL_CATCH_UNWIND_THROW_U__(action) MSTL_CATCH_UNWIND__ { action; throw; }
#define MSTL_CATCH_UNWIND_THROW_M__(action) \
	MSTL_CATCH_UNWIND__ { \
		action; \
		MSTL_EXEC_MEMORY__ \
	};
#define MSTL_CATCH_ERROR__ catch(Error e)

MSTL_END_NAMESPACE__

#endif // MSTL_ERROR_H__
