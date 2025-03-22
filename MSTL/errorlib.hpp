#ifndef MSTL_ERRORLIB_H__
#define MSTL_ERRORLIB_H__
#include "basiclib.h"
#include <cassert>
MSTL_BEGIN_NAMESPACE__

#define __MSTL_ERROR_CONSTRUCTOR(THIS, BASE, INFO) \
	constexpr explicit THIS(ccstring_t info = INFO, ccstring_t type = __type__) noexcept \
		: BASE(info, type) {}

#define __MSTL_ERROR_DESTRUCTOR(CLASS) \
	virtual ~CLASS() = default;

#define __MSTL_ERROR_TYPE(CLASS) \
	static constexpr ccstring_t __type__ = TO_STRING(CLASS);

#define MSTL_ERROR_BUILD_CLASS(THIS, BASE, INFO) \
	struct THIS : BASE { \
		__MSTL_ERROR_CONSTRUCTOR(THIS, BASE, INFO) \
		__MSTL_ERROR_DESTRUCTOR(THIS) \
		__MSTL_ERROR_TYPE(THIS) \
	};


struct Error {
	ccstring_t info_ = nullptr;
	ccstring_t type_ = nullptr;

	constexpr explicit Error(ccstring_t info = __type__, ccstring_t type = __type__) noexcept
		: info_(info), type_(type) {}

	__MSTL_ERROR_DESTRUCTOR(Error)
	__MSTL_ERROR_TYPE(Error)
};

MSTL_ERROR_BUILD_CLASS(StopIterator, Error, "Iterator or Pointer Visit out of Range.")
MSTL_ERROR_BUILD_CLASS(MemoryError, Error, "Memory Operation Failed.")
MSTL_ERROR_BUILD_CLASS(ValueError, Error, "Function Argument Invalid.")


inline void show_data_only(const Error& err, std::ostream& out) {
	out << "Exception : (" << err.type_ << ") " << err.info_ << std::flush;
}
inline std::ostream& operator <<(std::ostream& out, const Error& err) {
	show_data_only(err, out);
	return out;
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
	if (func)
		std::atexit(func);
	else {
		if (abort)
			std::abort();
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
#define MSTL_CATCH_ERROR__ catch(const MSTL::Error& error)
#define MSTL_CATCH_ERROR_UNUSE__ catch(const MSTL::Error&)


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
