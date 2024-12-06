#ifndef MSTL_ERRORLIB_H__
#define MSTL_ERRORLIB_H__
#include "basiclib.h"

MSTL_BEGIN_NAMESPACE__

typedef const char* cstring;
typedef const char* const const_cstring;

struct Error {
	typedef Error self;
	cstring _info;
	cstring _type;

	explicit Error(cstring _info = __type__, cstring _type = __type__) noexcept;
	virtual ~Error();
	void __show_data_only(std::ostream& _out) const;
	static const_cstring __type__;
	// friend void Exception(Error* _err);   use non-mumber instead of friend function
};

struct StopIterator : public Error {  // ������Խ�����ֵ
	typedef StopIterator self;
	explicit StopIterator(cstring _info = "Iterator out of range or is NULL.") noexcept;
	static const_cstring __type__;
};

struct AssertError : public Error {  // ���Դ��� ���Դ���̫����������
	typedef AssertError self;
	explicit AssertError(cstring _info = "Assert false!") noexcept;
	static const_cstring __type__;
};

struct AttributeError : public Error {   // �����޴�����
	typedef AttributeError self;
	explicit AttributeError(cstring _info = "No this attriubte in the object.") noexcept;
	static const_cstring __type__;
};

struct MemoryError : public Error {   // �ڴ����ʧ��
	typedef MemoryError self;
	explicit MemoryError(cstring _info = "Allocate or Deallocate falied!") noexcept;
	static const_cstring __type__;
};

struct ValueError : public Error {   // �����Ĳ����Ƿ�
	typedef ValueError self;
	explicit ValueError(cstring _info = "Function refused this value.") noexcept;
	static const_cstring __type__;
};

struct RangeError : public Error {   // ����Խ�����ֵ
	typedef RangeError self;
	explicit RangeError(cstring _info = "Index out of range or value is NULL.") noexcept;
	static const_cstring __type__;
};

struct SQLError : public Error {   // SQL����
	typedef SQLError self;
	explicit SQLError(cstring _info = "SQL operation error!") noexcept;
	static const_cstring __type__;
};

extern void Exception(Error* _err);
extern void Exception(bool _boolean, Error* _err = nullptr);
extern void Assert(bool _boolean, const char* const _info = "Assert false!");

extern void Exit(bool _abort = false, void(* _func)(void) = nullptr);
// just allow void(void) function to run before progess exit

#define MSTL_EXCE_EXP_THROW__(exp, error) Exception(exp, error);
#define MSTL_EXCE_PTR_THROW__(pointer) Exception(pointer);
#define MSTL_EXEC_MEMORY__ \
	Exception(new MemoryError());

#define MSTL_TRY__ try
#define MSTL_CATCH_UNWIND__ catch(...)
#define MSTL_CATCH_UNWIND_THROW_U__(action) MSTL_CATCH_UNWIND__ { action; throw; }
#define MSTL_CATCH_UNWIND_THROW_M__(action) \
	MSTL_CATCH_UNWIND__ { \
		action; \
		MSTL_EXEC_MEMORY__ \
	};
#define MSTL_CATCH_ERROR__ catch(Error& e)

MSTL_END_NAMESPACE__

#endif // MSTL_ERRORLIB_H__
