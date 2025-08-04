#ifndef MSTL_ERRORLIB_H__
#define MSTL_ERRORLIB_H__
#include "basiclib.hpp"
#include <cassert>
#ifdef MSTL_SUPPORT_CUDA__
#include <cuda_runtime.h>
#endif
MSTL_BEGIN_NAMESPACE__

#define __MSTL_ERROR_CONSTRUCTOR(THIS, BASE, INFO) \
	constexpr explicit THIS(const char* const info = INFO, const char* const type = __type__) noexcept \
		: BASE(info, type) {}

#define __MSTL_ERROR_DERIVED_DESTRUCTOR(CLASS) \
	virtual ~CLASS() = default;

#define __MSTL_ERROR_FINAL_DESTRUCTOR(CLASS) \
	~CLASS() override = default;

#define __MSTL_ERROR_TYPE(CLASS) \
	static constexpr auto __type__ = TO_STRING(CLASS);

#define __MSTL_ERROR_WHAT() \
	const char* what() const { \
		return info_; \
	}

#define MSTL_ERROR_BUILD_DERIVED_CLASS(THIS, BASE, INFO) \
	struct THIS : BASE { \
		__MSTL_ERROR_CONSTRUCTOR(THIS, BASE, INFO) \
		__MSTL_ERROR_DERIVED_DESTRUCTOR(THIS) \
		__MSTL_ERROR_TYPE(THIS) \
	};

#define MSTL_ERROR_BUILD_FINAL_CLASS(THIS, BASE, INFO) \
	struct THIS final : BASE { \
		__MSTL_ERROR_CONSTRUCTOR(THIS, BASE, INFO) \
		__MSTL_ERROR_FINAL_DESTRUCTOR(THIS) \
		__MSTL_ERROR_TYPE(THIS) \
	};


struct Error {
	const char* const info_ = nullptr;
	const char* const type_ = nullptr;

	constexpr explicit Error(const char* const info = __type__, const char* const type = __type__) noexcept
		: info_(info), type_(type) {}

	__MSTL_ERROR_DERIVED_DESTRUCTOR(Error)
	__MSTL_ERROR_TYPE(Error)
    __MSTL_ERROR_WHAT()
};

MSTL_ERROR_BUILD_DERIVED_CLASS(MemoryError, Error, "Memory Operation Failed.")
MSTL_ERROR_BUILD_FINAL_CLASS(StopIterator, MemoryError, "Iterator or Pointer Access out of Range.")
MSTL_ERROR_BUILD_FINAL_CLASS(AssertionError, Error, "Assertion Failed.")
MSTL_ERROR_BUILD_DERIVED_CLASS(TypeCastError, MemoryError, "Type Cast Mismatch.")
MSTL_ERROR_BUILD_DERIVED_CLASS(ValueError, Error, "Function or Template Argument Invalid.")
MSTL_ERROR_BUILD_DERIVED_CLASS(LinkError, Error, "External Link Actions Failed.")
MSTL_ERROR_BUILD_DERIVED_CLASS(DeviceOperateError, Error, "Operate Device Failed.")
MSTL_ERROR_BUILD_FINAL_CLASS(FileOperateError, DeviceOperateError, "Device File Operation Failed.")
MSTL_ERROR_BUILD_FINAL_CLASS(MathError, ValueError, "Math Function Argument Invalid.")


#ifdef MSTL_SUPPORT_CUDA__
// specialization of MSTL_ERROR_BUILD_FINAL_CLASS for CUDA
struct CUDAMemoryError final : MemoryError {
	__MSTL_ERROR_CONSTRUCTOR(CUDAMemoryError, MemoryError, "CUDA GPU Memory Operation Failed.")

	constexpr explicit CUDAMemoryError(const cudaError_t err) noexcept
		: MemoryError(cudaGetErrorString(err), __type__) {}

	__MSTL_ERROR_FINAL_DESTRUCTOR(CUDAMemoryError)
	__MSTL_ERROR_WHAT()
	__MSTL_ERROR_TYPE(CUDAMemoryError)
};
#endif


// throw a new error and print stacktrace of boost is imported.
inline void Exception(const Error& err){
    std::cerr << "\nException : (" << err.type_ << ") " << err.info_ << "\n";
	throw err;
}

inline void Exception(const bool boolean, const Error& err = Error()) {
	if (boolean) return;
	Exception(err);
}


// just allowing void(void) function be called before process exit
MSTL_NORETURN inline void Exit(const bool abort = false, void(* func)() = nullptr) {
	if (abort) std::abort();
	if (func) std::atexit(func);
	std::exit(1);
}


#define MSTL_REPORT_ERROR(MESG) \
    { MSTL::Exception(MSTL::AssertionError(MESG)); }

#define MSTL_DEBUG_VERIFY(CON, MESG) \
	{ if (CON) {} else { MSTL_REPORT_ERROR(MESG); assert(false); } }

#define __MSTL_DEBUG_MESG_OPERATE_NULLPTR(ITER, ACT) "can`t " ACT ": " TO_STRING(ITER) " is pointing to nullptr."
#define __MSTL_DEBUG_MESG_OUT_OF_RANGE(CLASS, ACT) "can`t " ACT ": " TO_STRING(CLASS) " out of ranges."
#define __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(ITER) "not comparable :" TO_STRING(ITER) " container incompatible."

#define __MSTL_DEBUG_TAG_DEREFERENCE "dereference"
#define __MSTL_DEBUG_TAG_INCREMENT "increment"
#define __MSTL_DEBUG_TAG_DECREMENT "decrement"

MSTL_END_NAMESPACE__
#endif // MSTL_ERRORLIB_H__
