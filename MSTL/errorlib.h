#ifndef MSTL_ERRORLIB_H__
#define MSTL_ERRORLIB_H__
#include "basiclib.h"
MSTL_BEGIN_NAMESPACE__

struct Error {
	typedef Error self;
	cstring_t _info;
	cstring_t _type;

	MSTL_CONSTEXPR explicit Error(cstring_t _info = __type__, cstring_t _type = __type__) noexcept 
		: _info(_info), _type(_type) {}
	virtual ~Error() = default;
	static MSTL_CONSTEXPR ccstring_t __type__ = TO_STRING(Error);
};

struct StopIterator : Error {  // 迭代器/索引越界
	typedef StopIterator self;
	MSTL_CONSTEXPR explicit StopIterator(cstring_t _info = "Iterator out of Range.") noexcept
		: Error(_info, __type__) {}
	static MSTL_CONSTEXPR ccstring_t __type__ = TO_STRING(StopIterator);
};

struct MemoryError : Error {   // 内存操作失败
	typedef MemoryError self;
	MSTL_CONSTEXPR explicit MemoryError(cstring_t _info = "Memory Operation Falied!") noexcept
		: Error(_info, __type__) {}
	static MSTL_CONSTEXPR ccstring_t __type__ = TO_STRING(MemoryError);
};

struct ValueError : Error {   // 函数的参数非法
	typedef ValueError self;
	MSTL_CONSTEXPR explicit ValueError(cstring_t _info = "Function Refused This Value.") noexcept
		: Error(_info, __type__) {}
	static MSTL_CONSTEXPR ccstring_t __type__ = TO_STRING(ValueError);
};

inline void show_data_only(const Error& e, std::ostream& _out) {
	_out << "Exception : (" << e._type << ") " << e._info << std::flush;
}
inline std::ostream& operator <<(std::ostream& _out, const Error& err) {
	show_data_only(err, _out);
	return _out;
}

inline void Exception(const Error& err){
	show_data_only(err, std::cerr);
	std::cerr << std::endl;
	throw err;
}

inline void Exception(const bool boolean, const Error& err = Error()) {
	if (boolean) return;
	Exception(err);
}


#ifdef MSTL_COMPILE_GCC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#elif defined(MSTL_COMPILE_CLANG__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
#elif defined(MSTL_COMPILE_MSVC__)
#pragma warning(push)
#pragma warning(disable : 4715)
#endif

// just allowing void(void) function be called before process exit
inline void Exit(const bool abort = false, void(* func)() = nullptr){
	if (func) {
		std::atexit(func);
	}
	else {
		if (abort) std::abort();
		std::exit(1);
	}
}

#ifdef MSTL_COMPILE_CLANG__
#pragma clang diagnostic pop
#elif defined(MSTL_COMPILE_GCC__)
#pragma GCC diagnostic pop
#elif defined(MSTL_COMPILE_MSVC__)
#pragma warning(pop)
#endif


#define MSTL_EXEC_MEMORY__ Exception(MemoryError());
#define MSTL_EXEC_UNWIND__ throw;

#define MSTL_TRY__ try
#define MSTL_CATCH_UNWIND__ catch(...)
#define MSTL_CATCH_UNWIND_THROW_U__(CON) MSTL_CATCH_UNWIND__ { CON; throw; }
#define MSTL_CATCH_UNWIND_THROW_M__(ACT) \
	MSTL_CATCH_UNWIND__ { \
		ACT; \
		MSTL_EXEC_MEMORY__ \
	};
#define MSTL_CATCH_ERROR__ catch(const Error& error)
#define MSTL_CATCH_ERROR_UNUSE__ catch(const Error&)


#define MSTL_REPORT_ERROR__(MESG) \
    { std::cerr << MESG << std::endl; } assert(false);

#ifdef MSTL_STATE_DEBUG__
#define MSTL_DEBUG_VERIFY__(CON, MESG) \
    { if (CON) {} else { MSTL_REPORT_ERROR__(MESG); } }
#else
#define MSTL_DEBUG_VERIFY__(CON, MESG) 
#endif

MSTL_END_NAMESPACE__
#endif // MSTL_ERRORLIB_H__
