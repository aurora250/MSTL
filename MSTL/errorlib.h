#ifndef MSTL_ERRORLIB_H__
#define MSTL_ERRORLIB_H__
#include "basiclib.h"
#include "logging.h"
#include <assert.h>
MSTL_BEGIN_NAMESPACE__

static MSTL_CONSTEXPR ccstring_t MSTL_ASSERT_ERROR__ = "Assert Error!";

struct Error {
	typedef Error self;
	cstring_t _info;
	cstring_t _type;

	MSTL_CONSTEXPR explicit Error(cstring_t _info = __type__, cstring_t _type = __type__) noexcept 
		: _info(_info), _type(_type) {}
	virtual ~Error() = default;
	static MSTL_CONSTEXPR ccstring_t __type__ = TO_STRING(Error);
};

struct StopIterator : public Error {  // 迭代器/索引越界
	typedef StopIterator self;
	MSTL_CONSTEXPR explicit StopIterator(cstring_t _info = "Iterator out of Range.") noexcept
		: Error(_info, __type__) {}
	static MSTL_CONSTEXPR ccstring_t __type__ = TO_STRING(StopIterator);
};

struct AssertError : public Error {  // 断言错误 断言错误太宽泛，不爱用
	typedef AssertError self;
	MSTL_CONSTEXPR explicit AssertError(cstring_t _info = MSTL_ASSERT_ERROR__) noexcept
		: Error(_info, __type__) {}
	static MSTL_CONSTEXPR ccstring_t __type__ = TO_STRING(AssertError);
};

struct AttributeError : public Error {   // 对象无此属性
	typedef AttributeError self;
	MSTL_CONSTEXPR explicit AttributeError(cstring_t _info = "No This Attriubte in This Object.") noexcept
		: Error(_info, __type__) {}
	static MSTL_CONSTEXPR ccstring_t __type__ = TO_STRING(AttributeError);
};

struct MemoryError : public Error {   // 内存操作失败
	typedef MemoryError self;
	MSTL_CONSTEXPR explicit MemoryError(cstring_t _info = "Memory Operation Falied!") noexcept
		: Error(_info, __type__) {}
	static MSTL_CONSTEXPR ccstring_t __type__ = TO_STRING(MemoryError);
};

struct ValueError : public Error {   // 函数的参数非法
	typedef ValueError self;
	MSTL_CONSTEXPR explicit ValueError(cstring_t _info = "Function Refused This Value.") noexcept
		: Error(_info, __type__) {}
	static MSTL_CONSTEXPR ccstring_t __type__ = TO_STRING(ValueError);
};

inline void show_data_only(const Error& e, std::ostream& _out) {
	_out << "Exception : (" << e._type << ") " << e._info << std::flush;
}
inline std::ostream& operator <<(const Error& err, std::ostream& _out) {
	show_data_only(err, _out);
	return _out;
}

void Exception(const Error& _err);
inline void Exception(bool _boolean, const Error& _err = Error()) {
	if (_boolean) return;
	else Exception(_err);
}
inline void Assert(bool _boolean, const char* _info = MSTL_ASSERT_ERROR__) { // useless in release
	Exception(_boolean, AssertError(_info));
}

// just allow void(void) function to run before progess exit
MSTL_NORETURN void Exit(bool _abort = false, void(*_func)(void) = nullptr);

#define MSTL_EXEC_MEMORY__ Exception(MemoryError());

#define MSTL_TRY__ try
#define MSTL_CATCH_UNWIND__ catch(...)
#define MSTL_CATCH_UNWIND_THROW_U__(action) MSTL_CATCH_UNWIND__ { action; throw; }
#define MSTL_CATCH_UNWIND_THROW_M__(action) \
	MSTL_CATCH_UNWIND__ { \
		action; \
		MSTL_EXEC_MEMORY__ \
	};
#define MSTL_CATCH_ERROR__ catch(const Error& error)
#define MSTL_CATCH_ERROR_UNUSE__ catch(const Error&)


#define MSTL_REPORT_ERROR__(mesg) \
    { LOG_ERROR(mesg); } assert(false);

#ifdef MSTL_STATE_DEBUG__
#define MSTL_DEBUG_VERIFY__(CON, MESG) \
    { if (CON) {} else { MSTL_REPORT_ERROR__(MESG); } }
#else
#define MSTL_DEBUG_VERIFY__(con, mesg) 
#endif

MSTL_END_NAMESPACE__
#endif // MSTL_ERRORLIB_H__
