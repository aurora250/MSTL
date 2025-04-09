#ifndef MSTL_ERRORLIB_H__
#define MSTL_ERRORLIB_H__
#include "basiclib.hpp"
#include <cassert>
MSTL_BEGIN_NAMESPACE__

#define __MSTL_ERROR_CONSTRUCTOR(THIS, BASE, INFO) \
	constexpr explicit THIS(const char* const info = INFO, const char* const type = __type__) noexcept \
		: BASE(info, type) {}

#define __MSTL_ERROR_DESTRUCTOR(CLASS) \
	virtual ~CLASS() = default;

#define __MSTL_ERROR_TYPE(CLASS) \
	static constexpr const char* const __type__ = TO_STRING(CLASS);

#define MSTL_ERROR_BUILD_CLASS(THIS, BASE, INFO) \
	struct THIS : BASE { \
		__MSTL_ERROR_CONSTRUCTOR(THIS, BASE, INFO) \
		__MSTL_ERROR_DESTRUCTOR(THIS) \
		__MSTL_ERROR_TYPE(THIS) \
	};


struct Error {
	const char* const info_ = nullptr;
	const char* const type_ = nullptr;

	constexpr explicit Error(const char* const info = __type__, const char* const type = __type__) noexcept
		: info_(info), type_(type) {}

	__MSTL_ERROR_DESTRUCTOR(Error)
	__MSTL_ERROR_TYPE(Error)
};

MSTL_ERROR_BUILD_CLASS(MemoryError, Error, "Memory Operation Failed.")
MSTL_ERROR_BUILD_CLASS(StopIterator, MemoryError, "Iterator or Pointer Visit out of Range.")
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


#define MSTL_EXEC_MEMORY__ MSTL::Exception(MSTL::MemoryError());
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
    { std::cerr << "\nAssert Failed! ( " << MESG << " )" << std::endl; } assert(false);

#ifdef MSTL_STATE_DEBUG__
#define MSTL_DEBUG_VERIFY__(CON, MESG) { if (CON) {} else { MSTL_REPORT_ERROR__(MESG); } }

#define __MSTL_DEBUG_MESG_OPERATE_NULLPTR(ITER, ACT) "can`t " ACT ": " TO_STRING(ITER) " is pointing to nullptr."
#define __MSTL_DEBUG_MESG_OUT_OF_RANGE(CLASS, ACT) "can`t " ACT ": " TO_STRING(CLASS) " out of ranges."
#define __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(ITER) "not comparable :" TO_STRING(ITER) " container incompatible."

#define __MSTL_DEBUG_TAG_DEREFERENCE "dereference"
#define __MSTL_DEBUG_TAG_INCREMENT "increment"
#define __MSTL_DEBUG_TAG_DECREMENT "decrement"

#else
#define MSTL_DEBUG_VERIFY__(CON, MESG)

#define __MSTL_DEBUG_MESG_OPERATE_NULLPTR(ITER, ACT)
#define __MSTL_DEBUG_MESG_OUT_OF_RANGE(CLASS, ACT)
#define __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(ITER)

#define __MSTL_DEBUG_TAG_DEREFERENCE
#define __MSTL_DEBUG_TAG_INCREMENT
#define __MSTL_DEBUG_TAG_DECREMENT

#endif

MSTL_END_NAMESPACE__
#endif // MSTL_ERRORLIB_H__
