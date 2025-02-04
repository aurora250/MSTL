#ifndef MSTL_FUNCTION_HPP__
#define MSTL_FUNCTION_HPP__
#include "functor.hpp"
MSTL_BEGIN_NAMESPACE__

// not:
template <typename Predicate>
class unary_negate : public unary_function<typename Predicate::argument_type, bool> {
protected:
	Predicate pred;
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit unary_negate(const Predicate& x) : pred(x) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR bool operator ()(const typename Predicate::argument_type& x) const {
		return !pred(x);
	}
};
template <typename Predicate>
MSTL_FADEPRECATED MSTL_CONSTEXPR unary_negate<Predicate> not1(const Predicate& pred) {
	return unary_negate<Predicate>(pred);
}

template <typename Predicate>
class binary_negate : 
	public binary_function<typename Predicate::first_argument_type, 
	typename Predicate::second_argument_type, bool> {
protected:
	Predicate pred;
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit binary_negate(const Predicate& x) : pred(x) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR bool operator ()(const typename Predicate::first_argument_type& x,
		const typename Predicate::second_argument_type& y) const {
		return !pred(x, y);
	}
};
template <typename Predicate>
MSTL_FADEPRECATED MSTL_CONSTEXPR binary_negate<Predicate> not2(const Predicate& pred) {
	return binary_negate<Predicate>(pred);
}

// bind:
template <typename Operation>
class binder1st :
	public unary_function<typename Operation::second_argument_type, typename Operation::result_type> {
protected:
	Operation op;
	typename Operation::first_argument_type value;
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit binder1st(
		const Operation x, const typename Operation::first_argument_type& y) : op(x), value(y) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR typename Operation::result_type operator ()(
		const typename Operation::second_argument_type& x) {
		return op(value, x);
	}
};
template <class Operation, class T>
MSTL_FADEPRECATED MSTL_CONSTEXPR binder1st<Operation> bind1st(const Operation& op, const T& x) {
  return binder1st<Operation>(op, typename Operation::first_argument_type(x));
}

template <class Operation> 
class binder2nd :
	public unary_function<typename Operation::first_argument_type, typename Operation::result_type> {
protected:
	Operation op;
	typename Operation::second_argument_type value;
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit binder2nd(
		const Operation& x, const typename Operation::second_argument_type& y) : op(x), value(y) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR typename Operation::result_type operator()(
		const typename Operation::first_argument_type& x) const {
		return op(x, value); 
	}
};
template <class Operation, class T>
MSTL_FADEPRECATED MSTL_CONSTEXPR binder2nd<Operation> bind2nd(const Operation& op, const T& x) {
  return binder2nd<Operation>(op, typename Operation::second_argument_type(x));
}

// compose:
template <class Operation1, class Operation2>
class unary_compose : 
	public unary_function<typename Operation2::argument_type, typename Operation1::result_type> {
protected:
	Operation1 op1;
	Operation2 op2;
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit unary_compose(
		const Operation1& x, const Operation2& y) : op1(x), op2(y) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR typename Operation1::result_type operator()(
		const typename Operation2::argument_type& x) const {
		return op1(op2(x));
	}
};
template <class Operation1, class Operation2>
MSTL_FADEPRECATED MSTL_CONSTEXPR unary_compose<Operation1, Operation2> compose1(
	const Operation1& op1, const Operation2& op2) {
	return unary_compose<Operation1, Operation2>(op1, op2);
}

template <class Operation1, class Operation2, class Operation3>
class binary_compose
	: public unary_function<typename Operation2::argument_type, typename Operation1::result_type> {
protected:
	Operation1 op1;
	Operation2 op2;
	Operation3 op3;
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit binary_compose(
		const Operation1& x, const Operation2& y, const Operation3& z) :
		op1(x), op2(y), op3(z) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR typename Operation1::result_type operator()(
		const typename Operation2::argument_type& x) const {
		return op1(op2(x), op3(x));
	}
};

template <class Operation1, class Operation2, class Operation3>
MSTL_FADEPRECATED MSTL_CONSTEXPR binary_compose<Operation1, Operation2, Operation3>
compose2(const Operation1& op1, const Operation2& op2, const Operation3& op3) {
	return binary_compose<Operation1, Operation2, Operation3>(op1, op2, op3);
}

// pointer_function:
template <class Arg, class Result>
class pointer_to_unary_function : public unary_function<Arg, Result> {
protected:
	Result(* ptr)(Arg);
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR pointer_to_unary_function() : ptr(nullptr) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit pointer_to_unary_function(Result(* x)(Arg)) : ptr(x) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR Result operator()(Arg x) const {
		if (ptr == nullptr) return Result();
		else return ptr(x);
	}
};
template <class Arg, class Result>
MSTL_FADEPRECATED MSTL_CONSTEXPR pointer_to_unary_function<Arg, Result> ptr_fun(Result(* x)(Arg)) {
	return pointer_to_unary_function<Arg, Result>(x);
}

template <class Arg1, class Arg2, class Result>
class pointer_to_binary_function : public binary_function<Arg1, Arg2, Result> {
protected:
	Result(* ptr)(Arg1, Arg2);
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR pointer_to_binary_function() : ptr(nullptr) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit pointer_to_binary_function(Result(* x)(Arg1, Arg2)) : ptr(x) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR Result operator()(Arg1 x, Arg2 y) const {
		if (ptr == nullptr) return Result();
		else return ptr(x, y);
	}
};
template <class Arg1, class Arg2, class Result>
MSTL_FADEPRECATED MSTL_CONSTEXPR pointer_to_binary_function<Arg1, Arg2, Result> 
ptr_fun(Result(* x)(Arg1, Arg2)) {
	return pointer_to_binary_function<Arg1, Arg2, Result>(x);
}

// member_function:
template <class S, class T>
class mem_fun_t : public unary_function<T*, S> {
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit mem_fun_t(S(T::* pf)()) : f(pf) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR S operator ()(T* p) const { return (p->*f)(); }
private:
	S(T::* f)();
};
template <class S, class T>
MSTL_FADEPRECATED MSTL_CONSTEXPR mem_fun_t<S, T> mem_fun(S(T::* f)()) {
	return mem_fun_t<S, T>(f);
}

template <class S, class T>
class const_mem_fun_t : public unary_function<const T*, S> {
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit const_mem_fun_t(S(T::* pf)() const) : f(pf) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR S operator ()(const T* p) const { return (p->*f)(); }
private:
	S(T::* f)() const;
};
template <class S, class T>
MSTL_FADEPRECATED MSTL_CONSTEXPR const_mem_fun_t<S, T> mem_fun(S(T::* f)() const) {
	return const_mem_fun_t<S, T>(f);
}

template <class S, class T>
class mem_fun_ref_t : public unary_function<T, S> {
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit mem_fun_ref_t(S(T::* pf)()) : f(pf) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR S operator ()(T& r) const { return (r.*f)(); }
private:
	S(T::* f)();
};
template <class S, class T>
MSTL_FADEPRECATED MSTL_CONSTEXPR mem_fun_ref_t<S, T> mem_fun_ref(S(T::* f)()) {
	return mem_fun_ref_t<S, T>(f);
}

template <class S, class T>
class const_mem_fun_ref_t : public unary_function<T, S> {
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit const_mem_fun_ref_t(S(T::* pf)() const) : f(pf) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR S operator ()(const T& r) const { return (r.*f)(); }
private:
	S(T::* f)() const;
};
template <class S, class T>
MSTL_FADEPRECATED MSTL_CONSTEXPR const_mem_fun_ref_t<S, T> mem_fun_ref(S(T::* f)() const) {
	return const_mem_fun_ref_t<S, T>(f);
}

template <class S, class T, class A>
class mem_fun1_t : public binary_function<T*, A, S> {
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit mem_fun1_t(S(T::* pf)(A)) : f(pf) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR S operator()(T* p, A x) const { return (p->*f)(x); }
private:
	S(T::* f)(A);
};
template <class S, class T, class A>
MSTL_FADEPRECATED MSTL_CONSTEXPR mem_fun1_t<S, T, A> mem_fun1(S(T::* f)(A)) {
	return mem_fun1_t<S, T, A>(f);
}

template <class S, class T, class A>
class const_mem_fun1_t : public binary_function<const T*, A, S> {
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit const_mem_fun1_t(S(T::* pf)(A) const) : f(pf) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR S operator()(const T* p, A x) const { return (p->*f)(x); }
private:
	S(T::* f)(A) const;
};
template <class S, class T, class A>
MSTL_FADEPRECATED MSTL_CONSTEXPR const_mem_fun1_t<S, T, A> mem_fun1(S(T::* f)(A) const) {
	return const_mem_fun1_t<S, T, A>(f);
}

template <class S, class T, class A>
class mem_fun1_ref_t : public binary_function<T, A, S> {
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit mem_fun1_ref_t(S(T::* pf)(A)) : f(pf) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR S operator()(T& r, A x) const { return (r.*f)(x); }
private:
	S(T::* f)(A);
};
template <class S, class T, class A>
MSTL_FADEPRECATED MSTL_CONSTEXPR mem_fun1_ref_t<S, T, A> mem_fun1_ref(S(T::* f)(A)) {
	return mem_fun1_ref_t<S, T, A>(f);
}

template <class S, class T, class A>
class const_mem_fun1_ref_t : public binary_function<T, A, S> {
public:
	MSTL_FADEPRECATED MSTL_CONSTEXPR explicit const_mem_fun1_ref_t(S(T::* pf)(A) const) : f(pf) {}
	MSTL_FADEPRECATED MSTL_CONSTEXPR S operator()(const T& r, A x) const { return (r.*f)(x); }
private:
	S(T::* f)(A) const;
};
template <class S, class T, class A>
MSTL_FADEPRECATED MSTL_CONSTEXPR const_mem_fun1_ref_t<S, T, A> mem_fun1_ref(S(T::* f)(A) const) {
	return const_mem_fun1_ref_t<S, T, A>(f);
}

MSTL_END_NAMESPACE__
#endif // MSTL_FUNCTION_HPP__
