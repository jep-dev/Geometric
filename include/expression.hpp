#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <memory>
#include <iosfwd>

#include "math.hpp"
#include "quaternion.tpp"
#include "dual.tpp"

#include "geometric.hpp"

namespace Expressions {

template<class S, class T>
constexpr bool commutes(void) {
	return std::is_arithmetic<S>::value || std::is_arithmetic<T>::value;
}

template<class> struct Symbol;
template<class> struct Scalar;
template<class> struct Variable;
template<class> struct Negative;
template<class> struct Conjugate;
template<class> struct Sum;
template<class> struct Difference;
template<class> struct Product;
/*template<class> struct Exp;
template<class> struct Cosine;
template<class> struct Sine;*/
/*template<class> struct Power;
template<class> struct Log;
template<class> struct Tangent;*/

template<class T> using SharedSymbol = std::shared_ptr<Symbol<T>>;
template<class T> using SharedScalar = std::shared_ptr<Scalar<T>>;
template<class T> using SharedVariable = std::shared_ptr<Variable<T>>;
template<class T> using SharedNegative = std::shared_ptr<Negative<T>>;
template<class T> using SharedConjugate = std::shared_ptr<Conjugate<T>>;
template<class T> using SharedSum = std::shared_ptr<Sum<T>>;
template<class T> using SharedDifference = std::shared_ptr<Difference<T>>;
template<class T> using SharedProduct = std::shared_ptr<Product<T>>;
/*template<class T> using SharedExp = std::shared_ptr<Exp<T>>;
template<class T> using SharedCosine = std::shared_ptr<Cosine<T>>;
template<class T> using SharedSine = std::shared_ptr<Sine<T>>;*/
/*template<class T> using SharedPower = std::shared_ptr<Product<T>>;
template<class T> using SharedLog = std::shared_ptr<Log<T>>;
template<class T> using SharedTangent = std::shared_ptr<Tangent<T>>;*/

template<class S, class... T>
SharedSymbol<S> make_scalar(T &&... t)
	{ return (SharedSymbol<S>) std::make_shared<Scalar<S>>(t...); }
template<class S, class... T>
SharedSymbol<S> make_variable(T &&... t)
	{ return (SharedSymbol<S>) std::make_shared<Variable<S>>(std::forward<T>(t)...); }
template<template<class> class TS, class S>
SharedSymbol<S> make_negative(std::shared_ptr<TS<S>> const& t)
	{ return (SharedSymbol<S>) std::make_shared<Negative<S>>(t); }
template<template<class> class TS, class S>
SharedSymbol<S> make_conjugate(std::shared_ptr<TS<S>> const& t)
	{ return (SharedSymbol<S>) std::make_shared<Conjugate<S>>(t); }
template<template<class> class TS1, template<class> class TS2, class S>
SharedSymbol<S> make_sum(std::shared_ptr<TS1<S>> const& t1, std::shared_ptr<TS2<S>> const& t2)
	{ return (SharedSymbol<S>) std::make_shared<Sum<S>>(t1, t2); }
template<template<class> class TS1, template<class> class TS2, class S>
SharedSymbol<S> make_difference(std::shared_ptr<TS1<S>> const& t1, std::shared_ptr<TS2<S>> const& t2)
	{ return (SharedSymbol<S>) std::make_shared<Difference<S>>(t1, t2); }
template<template<class> class TS1, template<class> class TS2, class S>
SharedSymbol<S> make_product(std::shared_ptr<TS1<S>> const& t1, std::shared_ptr<TS2<S>> const& t2)
	{ return (SharedSymbol<S>) std::make_shared<Product<S>>(t1, t2); }
/*template<template<class> class TS, class S>
SharedSymbol<S> make_exp(std::shared_ptr<TS<S>> const& t)
	{ return (SharedSymbol<S>) std::make_shared<Exp<S>>(t); }
template<template<class> class TS, class S>
SharedSymbol<S> make_cosine(std::shared_ptr<TS<S>> const& t)
	{ return (SharedSymbol<S>) std::make_shared<Cosine<S>>(t); }
template<template<class> class TS, class S>
SharedSymbol<S> make_sine(std::shared_ptr<TS<S>> const& t)
	{ return (SharedSymbol<S>) std::make_shared<Sine<S>>(t); }*/
/*
template<template<class> class TS1, template<class> class TS2, class S>
SharedSymbol<S> make_power(std::shared_ptr<TS1<S>> const& t1, std::shared_ptr<TS2<S>> const& t2)
	{ return (SharedSymbol<S>) std::make_shared<Power<S>>(t1, t2); }
template<template<class> class TS, class S>
SharedSymbol<S> make_log(std::shared_ptr<TS<S>> const& t)
	{ return (SharedSymbol<S>) std::make_shared<Log<S>>(t); }
template<template<class> class TS, class S>
SharedSymbol<S> make_tangent(std::shared_ptr<TS<S>> const& t)
	{ return (SharedSymbol<S>) std::make_shared<Tangent<S>>(t); }*/

typedef enum {
	e_undefined = 0, e_scalar, e_variable,
		e_negative, e_conjugate,
		e_sum, e_difference,
		e_product,
		// e_exp, e_cosine, e_sine
		// e_power, e_log, e_tangent
} EType;

template<class T>
struct Symbol: std::enable_shared_from_this<Symbol<T>> {
	virtual EType type(void) const = 0;
	virtual SharedSymbol<T> simplify(void) const {
		switch(this -> type()) {
			case e_scalar: return ((Scalar<T> *const) this) -> simplify();
			case e_variable: return ((Variable<T> *const) this) -> simplify();
			case e_negative: return ((Negative<T> *const) this) -> simplify();
			case e_conjugate: return ((Conjugate<T> *const) this) -> simplify();
			case e_sum: return ((Sum<T> *const) this) -> simplify();
			case e_difference: return ((Difference<T> *const) this) -> simplify();
			case e_product: return ((Product<T> *const) this) -> simplify();
			/*case e_exp: return ((Exp<T> *const) this) -> simplify();
			case e_cosine: return ((Cosine<T> *const) this) -> simplify();
			case e_sine: return ((Sine<T> *const) this) -> simplify();*/
			/*case e_power: return ((Power<T> *const) this) -> simplify();
			case e_log: return ((Log<T> *const) this) -> simplify();
			case e_tangent: return ((Tangent<T> *const) this) -> simplify();*/
			default: return nullptr;
		}
	}
	std::shared_ptr<Symbol> derive(char c) const {
		switch(this -> type()) {
			case e_scalar: return ((Scalar<T> *const) this) -> derive(c);
			case e_variable: return ((Variable<T> *const) this) -> derive(c);
			case e_negative: return ((Negative<T> *const) this) -> derive(c);
			case e_conjugate: return ((Conjugate<T> *const) this) -> derive(c);
			case e_sum: return ((Sum<T> *const) this) -> derive(c);
			case e_difference: return ((Difference<T> *const) this) -> derive(c);
			case e_product: return ((Product<T> *const) this) -> derive(c);
			/*case e_exp: return ((Exp<T> *const) this) -> derive(c);
			case e_cosine: return ((Cosine<T> *const) this) -> derive(c);
			case e_sine: return ((Sine<T> *const) this) -> derive(c);*/
			/*case e_power: return ((Power<T> *const) this) -> derive(c);
			case e_log: return ((Log<T> *const) this) -> derive(c);
			case e_tangent: return ((Tangent<T> *const) this) -> derive(c);*/
			default: return nullptr;
		}
	}
	template<class S> bool operator==(S const& s) const { return false; }
	friend std::ostream& operator<<(std::ostream &lhs, Symbol<T> const& rhs) {
		switch(rhs.type()) {
			case e_scalar: return lhs << (Scalar<T> const&) rhs, lhs;
			case e_variable: return lhs << (Variable<T> const&) rhs, lhs;
			case e_negative: return lhs << (Negative<T> const&) rhs, lhs;
			case e_conjugate: return lhs << (Conjugate<T> const&) rhs, lhs;
			case e_sum: return lhs << (Sum<T> const&) rhs, lhs;
			case e_difference: return lhs << (Difference<T> const&) rhs, lhs;
			case e_product: return lhs << (Product<T> const&) rhs, lhs;
			/*case e_exp: return lhs << (Exp<T> const&) rhs, lhs;
			case e_cosine: return lhs << (Cosine<T> const&) rhs, lhs;
			case e_sine: return lhs << (Sine<T> const&) rhs, lhs;*/
			/*case e_power: return lhs << (Power<T> const&) rhs, lhs;
			case e_log: return lhs << (Log<T> const&) rhs, lhs;
			case e_tangent: return lhs << (Tangent<T> const&) rhs, lhs;*/
			default: return lhs;
		}
	}
	virtual ~Symbol(void) {}
};

template<class T>
struct Scalar : Symbol<T>, std::enable_shared_from_this<Scalar<T>> {
	T value = {};
	virtual EType type(void) const { return e_scalar; }
	SharedSymbol<T> simplify(void) const { return make_scalar<T>(value); }
	SharedSymbol<T> derive(char c) const
		{ return make_scalar<T>(T{0}); }
	using Symbol<T>::operator==;
	bool operator==(Scalar<T> const& t) const
		{ return value == t.value; }
	friend std::ostream& operator<<(std::ostream &lhs, Scalar<T> const& rhs)
		{ return lhs << rhs.value, lhs; }
	Scalar(Scalar const& s): value(s.value) {}
	Scalar(T const& t): value(t) {}
	template<class... S>
	Scalar(Detail::ValueType_t<true, T> const& t, S const&... s):
		value({t, (Detail::ValueType_t<true, T>) s...}) {}
	/*template<class... S>
	Scalar(T && t, S &&... s):
		value({std::forward<T>(t), std::forward<S>(s)...}) {}*/
	virtual ~Scalar(void) {}
};

template<class T>
struct Variable : Symbol<T>, std::enable_shared_from_this<Variable<T>> {
	char name;
	T value = {1};
	virtual EType type(void) const { return e_variable; }
	SharedSymbol<T> derive(char c) const
		{ return make_scalar<T>(Detail::ValueType_t<true, T>(name == c)); }
	SharedSymbol<T> simplify(void) const {
		if(value == value*0)
			return make_scalar<T>(0);
		return make_variable<T>(name, value);
	}
	using Symbol<T>::operator==;
	bool operator==(Variable<T> const& t) const
		{ return t.name == name && t.value == value; }
	friend std::ostream& operator<<(std::ostream &lhs, Variable<T> const& rhs)
		{ return lhs << to_string(rhs.value) << rhs.name, lhs; }
	Variable(Variable<T> const& v): name(v.name), value(v.value) {}
	Variable(char k, T value = {1}): name(k), value(value) {}
	virtual ~Variable(void) {}
};

template<class T>
struct Negative : Symbol<T>, std::enable_shared_from_this<Negative<T>> {
	SharedSymbol<T> value;
	virtual EType type(void) const { return e_negative; }
	SharedSymbol<T> derive(char c) const
		{ return make_negative(value -> derive(c)); }
	SharedSymbol<T> simplify(void) const {
		auto type = value -> type();
		if(type == e_negative)
			return ((Negative<T>&)*value).value -> simplify();
		return make_negative(value -> simplify());
	}
	using Symbol<T>::operator==;
	bool operator==(Negative<T> const& n) const {
		if(value ^ n.value) return false;
		if(value) return *value == *n.value;
		return true;
	}
	template<class S>
	Negative(std::shared_ptr<S> const& s): value((SharedSymbol<T>)s) {}
	template<class U, class... V>
	Negative(std::shared_ptr<U> const& u, char c, V &&... v):
		Negative(c, u, std::forward<V>(v)...) {}
	template<class U, class... V>
	Negative(char c, std::shared_ptr<U> const& u, V &&... v):
		Negative(make_product(make_variable<T>(c), u), std::forward<V>(v)...) {}
	Negative(char c): Negative(make_variable<T>(c)) {}

	virtual ~Negative(void) {}
};

template<class T>
struct Conjugate : Symbol<T>, std::enable_shared_from_this<Conjugate<T>> {
	SharedSymbol<T> value;
	virtual EType type(void) const { return e_conjugate; }
	SharedSymbol<T> derive(char c) const
		{ return make_conjugate((SharedSymbol<T>) (value -> derive(c))); }
	SharedSymbol<T> simplify(void) const {
		auto type = value -> type();
		if(type == e_conjugate)
			return ((Conjugate<T>&)*value).value -> simplify();
		return make_conjugate(value -> simplify());
	}
	using Symbol<T>::operator==;
	bool operator==(Conjugate<T> const& n) const
		{ return !value && !n.value || value && n.value && *value == *n.value; }
	Conjugate(SharedSymbol<T> const& s): value(s) {}
	template<class S>
	Conjugate(std::shared_ptr<S> const& s): Conjugate((SharedSymbol<T>)(s)) {}
	Conjugate(char c): Conjugate(make_variable<T>(c)) {}
	virtual ~Conjugate(void) {}
};

template<class T>
struct Sum : Symbol<T>, std::enable_shared_from_this<Sum<T>> {
	SharedSymbol<T> first, second;
	virtual EType type(void) const { return e_sum; }
	SharedSymbol<T> derive(char c) const {
		auto dfirst = first -> derive(c), dsecond = second -> derive(c);
		return make_sum(dfirst, dsecond) -> simplify();
	}
	SharedSymbol<T> simplify(void) const {
		auto sfirst = first -> simplify(), ssecond = second -> simplify();
		auto tfirst = sfirst -> type(), tsecond = ssecond -> type();
		if(tfirst == e_scalar && tsecond == e_scalar) {
			return make_scalar<T>(((Scalar<T>&)*sfirst).value + ((Scalar<T>&)*ssecond).value);
		}
		if(tfirst == e_variable && tsecond == e_variable) {
			auto & vfirst = (Variable<T>&)*sfirst, vsecond = (Variable<T>&)*ssecond;
			if(vfirst.name == vsecond.name)
				return make_variable<T>(vfirst.name, vfirst.value + vsecond.value);
		}
		return make_sum(sfirst, ssecond);
	}
	using Symbol<T>::operator==;
	bool operator==(Sum<T> const& t) const {
		if((first ^ t.first) || (second ^ t.second)) return false;
		if(first && *first == *t.first || second && *second == *t.second) return false;
		return true;
	}
	friend std::ostream& operator<<(std::ostream &lhs, Sum<T> const& rhs) {
		if(rhs.first) lhs << *rhs.first;
		else lhs << "!";
		lhs << '+';
		if(rhs.second) lhs << *rhs.second;
		else lhs << "!";
		return lhs;
	}
	Sum(SharedSymbol<T> first, SharedSymbol<T> second):
		first(first), second(second) {}
	template<class R, class S>
	Sum(std::shared_ptr<R> first, std::shared_ptr<S> second):
		Sum((SharedSymbol<T>)first, (SharedSymbol<T>)second) {}
	virtual ~Sum(void) {}
};

template<class T>
struct Difference : Symbol<T>, std::enable_shared_from_this<Difference<T>> {
	SharedSymbol<T> first, second;
	virtual EType type(void) const { return e_difference; }
	SharedSymbol<T> derive(char c) const {
		SharedSymbol<T> dfirst = first -> derive(c),
			dsecond = second -> derive(c);
		return make_difference(dfirst, dsecond) -> simplify();
	}
	SharedSymbol<T> simplify(void) const {
		auto tfirst = first -> type(), tsecond = second -> type();
		if(tfirst == e_scalar && tsecond == e_scalar)
			return make_scalar<T>(((Scalar<T>&)*first).value - ((Scalar<T>&)*second).value);
		if(tfirst == e_variable && tsecond == e_variable) {
			auto & sfirst = (Variable<T>&)*first, ssecond = (Variable<T>&)*second;
			if(sfirst.name == ssecond.name)
				return make_variable<T>(sfirst.name, sfirst.value - ssecond.value);
		}
		return make_difference(first, second);
	}
	using Symbol<T>::operator==;
	bool operator==(Difference<T> const& t) const {
		if((first ^ t.first) || (second ^ t.second)) return false;
		if(first && *first == *t.first || second && *second == *t.second) return false;
		return true;
	}
	Difference(SharedSymbol<T> first, SharedSymbol<T> second):
			first(first), second(second) {}
	template<class R, class S>
	Difference(std::shared_ptr<R> first, std::shared_ptr<S> second):
			Difference((SharedSymbol<T>) first, (SharedSymbol<T>) second) {}
};
template<class T>
struct Product : Symbol<T>, std::enable_shared_from_this<Product<T>> {
	SharedSymbol<T> first, second;
	virtual EType type(void) const { return e_product; }
	SharedSymbol<T> derive(char c) const {
		auto dfirst = first -> derive(c), dsecond = second -> derive(c);
		return make_sum(make_product(first, dsecond),
			make_product(dfirst, second)) -> simplify();
	}
	SharedSymbol<T> simplify(void) const {
		auto sfirst = first -> simplify(), ssecond = second -> simplify();
		auto first_type = sfirst -> type(), second_type = ssecond -> type();
		if(first_type == e_scalar && second_type == e_variable) {
			auto & kfirst = (Scalar<T>&)*sfirst;
			auto & vsecond = (Variable<T>&)*ssecond;
			return make_variable<T>(vsecond.name, kfirst.value * vsecond.value);
		}
		if(first_type == e_variable && second_type == e_scalar) {
			auto & vfirst = (Variable<T>&)*sfirst;
			auto & ksecond = (Scalar<T>&)*ssecond;
			return make_variable<T>(vfirst.name, vfirst.value * ksecond.value);
		}
		if(first_type == e_negative && second_type == e_negative) {
			auto & nfirst = (Negative<T>&)*sfirst, nsecond = (Negative<T>&)*ssecond;
			return make_product(nfirst.value, nsecond.value);
		}
		if(first_type == e_conjugate && second_type == e_conjugate) {
			auto & cfirst = (Conjugate<T>&)*sfirst, csecond = (Conjugate<T>&)*ssecond;
			return make_conjugate(make_product(csecond.simplify(), cfirst.simplify()));
		}
		return make_product(sfirst, ssecond);
	}
	using Symbol<T>::operator==;
	bool operator==(Product<T> const& t) const {
		if((first ^ t.first) || (second ^ t.second)) return false;
		if(first && *first == *t.first || second && *second == *t.second) return false;
		return true;
	}
	friend std::ostream& operator<<(std::ostream &lhs, Product<T> const& rhs) {
		if(rhs.first) lhs << *rhs.first;
		else lhs << "!";
		lhs << '*';
		if(rhs.second) lhs << *rhs.second;
		else lhs << "!";
		return lhs;
	}
	Product(SharedSymbol<T> first, SharedSymbol<T> second):
		first(first), second(second) {}
	template<class R, class S>
	Product(std::shared_ptr<R> first, std::shared_ptr<S> second):
		Product((SharedSymbol<T>)first, (SharedSymbol<T>)second) {}
	virtual ~Product(void) {}
};

/*template<class T>
struct Exp: Symbol<T>, std::enable_shared_from_this<Exp<T>> {
	SharedSymbol<T> value;
	virtual EType type(void) const { return e_exp; }
	virtual SharedSymbol<T> simplify(void) const {
		if(value -> type() != e_scalar)
			return make_exp(value -> simplify());
		return make_scalar<T>(exp(((Scalar<T> const&) *value).value));
	}
	std::shared_ptr<Symbol<T>> derive(char c) const {
		return make_product(make_exp(value), value -> derive(c)) -> simplify();
	}
	using Symbol<T>::operator==;
	bool operator==(Exp<T> const& s) const {
		return *value == s.value;
	}
	template<class S>
	friend S& operator<<(S &s, Exp<T> const& e) {
		return s << "exp(" << e.value << ")", s;
	}
	virtual ~Exp(void) {}
};

template<class T>
struct Cosine: Symbol<T>, std::enable_shared_from_this<Cosine<T>> {
	SharedSymbol<T> value;
	virtual EType type(void) const { return e_cosine; }
	virtual SharedSymbol<T> simplify(void) const {
		auto operand = value -> simplify();
		if(operand -> type() != e_scalar)
			return make_cosine(operand);
		auto val = ((SharedScalar<T> const&) *operand).value;
		return make_scalar<T>(cos(value));
	}
	std::shared_ptr<Symbol<T>> derive(char c) const {
		return make_product(make_negative(make_sine(value -> simplify())),
			value -> derive(c) -> simplify()) -> simplify();
	}
	using Symbol<T>::operator==;
	bool operator==(Cosine<T> const& c) const {
		return *value == c.value;
	}
	friend std::ostream& operator<<(std::ostream &lhs, Cosine<T> const& rhs) {
		return lhs << "cos(" << *rhs.value << ")", lhs;
	}
	virtual ~Cosine(void) {}
};

template<class T>
struct Sine: Symbol<T>, std::enable_shared_from_this<Sine<T>> {
	SharedSymbol<T> value;
	virtual EType type(void) const { return e_sine; }
	virtual SharedSymbol<T> simplify(void) const {
		auto operand = value -> simplify();
		if(operand -> type() != e_scalar)
			return make_sine(operand);
		auto val = ((SharedScalar<T> const&) *operand).value;
		return make_scalar<T>(sin(value));
	}
	std::shared_ptr<Symbol<T>> derive(char c) const {
		return make_product(make_sine(value), value -> derive(c));
	}
	using Symbol<T>::operator==;
	bool operator==(Sine<T> const& s) const {
		return *value == s.value;
	}
	template<class S> bool operator==(S const& s) const { return false; }
	friend std::ostream& operator<<(std::ostream &lhs, Sine<T> const& rhs) {
		return lhs << "sin(" << *rhs.value << ")", lhs;
	}
	virtual ~Sine(void) {}
};*/

}

#endif
