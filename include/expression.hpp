#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <memory>
#include <iosfwd>

#include "math.hpp"
#include "quaternion.tpp"
#include "dual.tpp"

#include "geometric.hpp"

namespace Expressions {

template<class> struct Symbol;
template<class> struct Scalar;
template<class> struct Variable;
template<class> struct Negative;
template<class> struct Conjugate;
template<class> struct Sum;
template<class> struct Difference;
template<class> struct Product;

template<class T> using SharedSymbol = std::shared_ptr<Symbol<T>>;
template<class T> using SharedScalar = std::shared_ptr<Scalar<T>>;
template<class T> using SharedVariable = std::shared_ptr<Variable<T>>;
template<class T> using SharedNegative = std::shared_ptr<Negative<T>>;
template<class T> using SharedConjugate = std::shared_ptr<Conjugate<T>>;
template<class T> using SharedSum = std::shared_ptr<Sum<T>>;
template<class T> using SharedDifference = std::shared_ptr<Difference<T>>;
template<class T> using SharedProduct = std::shared_ptr<Product<T>>;

template<class S, class... T>
SharedScalar<S> make_scalar(T &&... t)
	{ return std::make_shared<Scalar<S>>(t...); }
template<class S>
SharedScalar<S> make_scalar(char c)
	{ return std::make_shared<Scalar<S>>(c); }
template<class S, class... T>
SharedVariable<S> make_variable(T &&... t)
	{ return std::make_shared<Variable<S>>(std::forward<T>(t)...); }
template<class S, class... T>
SharedNegative<S> make_negative(T &&... t)
	{ return std::make_shared<Negative<S>>(std::forward<T>(t)...); }
template<class S, class... T>
SharedConjugate<S> make_conjugate(T &&... t)
	{ return std::make_shared<Conjugate<S>>(std::forward<T>(t)...); }
template<class S, class... T>
SharedSum<S> make_sum(T &&... t)
	{ return std::make_shared<Sum<S>>(std::forward<T>(t)...); }
template<class S, class... T>
SharedDifference<S> make_difference(T &&... t)
	{ return std::make_shared<Difference<S>>(std::forward<T>(t)...); }
template<class S, class... T>
SharedProduct<S> make_product(T &&... t)
	{ return std::make_shared<Product<S>>(std::forward<T>(t)...); }

typedef enum {
	e_undefined = 0, e_scalar, e_variable,
		e_negative, e_conjugate,
		e_sum, e_difference,
		e_product
} EType;

template<class T>
struct Symbol: std::enable_shared_from_this<Symbol<T>> {
	virtual EType type(void) const = 0;
	std::shared_ptr<Symbol> derive(char c) const {
		switch(this -> type()) {
			case e_scalar: return ((Scalar<T>*) this) -> derive(c);
			case e_variable: return ((Variable<T>*) this) -> derive(c);
			case e_negative: return ((Negative<T>*) this) -> derive(c);
			case e_conjugate: return ((Conjugate<T>*) this) -> derive(c);
			case e_sum: return ((Sum<T>*) this) -> derive(c);
			case e_difference: return ((Difference<T>*) this) -> derive(c);
			case e_product: return ((Product<T>*) this) -> derive(c);
			default: return nullptr;
		}
	}
	template<class S> bool operator==(S const& s) const { return false; }
	friend std::ostream& operator<<(std::ostream &lhs, Symbol<T> const& rhs) {
		switch(rhs.type()) {
			case e_scalar: return lhs << (Scalar<T>&) rhs, lhs;
			case e_variable: return lhs << (Variable<T>&) rhs, lhs;
			case e_negative: return lhs << (Negative<T>&) rhs, lhs;
			case e_conjugate: return lhs << (Conjugate<T>&) rhs, lhs;
			case e_sum: return lhs << (Sum<T>&) rhs, lhs;
			case e_difference: return lhs << (Difference<T>&) rhs, lhs;
			case e_product: return lhs << (Product<T>&) rhs, lhs;
			default: return lhs;
		}
	}
	virtual ~Symbol(void) {}
};

template<class T>
struct Scalar : Symbol<T>, std::enable_shared_from_this<Scalar<T>> {
	T value = {};
	virtual EType type(void) const { return e_scalar; }
	SharedSymbol<T> derive(char c) const {
		return make_scalar<T>(T{0});
	}
	using Symbol<T>::operator==;
	bool operator==(Scalar<T> const& t) const {
		return value == t.value;
	}
	friend std::ostream& operator<<(std::ostream &lhs, Scalar<T> const& rhs) {
		return lhs << rhs.value, lhs;
	}
	Scalar(Scalar const& s): value(s.value) {}
	Scalar(T const& t): value(t) {}
	template<class Q, class R, class... S>
	Scalar(Q && q, R && r, S &&... s):
		value(std::forward<Q>(q), std::forward<R>(r), std::forward<S>(s)...) {}
	template<class S>
	Scalar(S && s): value(T{s}) {}
	template<class... S>
	Scalar(S &&... s): value(std::forward<S>(s)...) {}
	virtual ~Scalar(void) {}
};

template<class T>
struct Variable : Symbol<T>, std::enable_shared_from_this<Variable<T>> {
	char value;
	virtual EType type(void) const { return e_variable; }
	SharedSymbol<T> derive(char c) const {
		return make_scalar<T>(Detail::ValueType_t<T>(value == c));
	}
	using Symbol<T>::operator==;
	bool operator==(Variable<T> const& t) const {
		return value == t.value;
	}
	friend std::ostream& operator<<(std::ostream &lhs, Variable<T> const& rhs) {
		return lhs << rhs.value, lhs;
	}
	Variable(Variable<T> const& v): value(v.value) {}
	Variable(char k): value(k) {}
	virtual ~Variable(void) {}
};

template<class T>
struct Negative : Symbol<T>, std::enable_shared_from_this<Negative<T>> {
	SharedSymbol<T> value;
	virtual EType type(void) const { return e_negative; }
	SharedSymbol<T> derive(char c) const {
		SharedSymbol<T> dvalue = value -> derive(c);
		return make_negative<T>(dvalue);
	}
	using Symbol<T>::operator==;
	bool operator==(Negative<T> const& n) const {
		return !value && !n.value || value && n.value && *value == *n.value;
	}
	/*template<class S>
	Negative(SharedSymbol<T> const& s): value(s) {}*/
	//template<class S, class E = std::enable_if_t<!std::is_same<S, Symbol<T>>::value, S>>
	template<class S>
	Negative(std::shared_ptr<S> const& s): value((SharedSymbol<T>)s) {}
		//Negative((SharedSymbol<T>)(s)) {}
	Negative(char c): Negative(make_variable<T>(c)) {}
	virtual ~Negative(void) {}
};

template<class T>
struct Conjugate : Symbol<T>, std::enable_shared_from_this<Conjugate<T>> {
	SharedSymbol<T> value;
	virtual EType type(void) const { return e_conjugate; }
	SharedSymbol<T> derive(char c) const {
		return make_conjugate<T>(
			(SharedSymbol<T>) (value -> derive(c)));
	}
	using Symbol<T>::operator==;
	bool operator==(Conjugate<T> const& n) const {
		return !value && !n.value || value && n.value && *value == *n.value;
	}
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
		if(dfirst -> type() == e_scalar) {
			auto const& fvalue = ((Scalar<T>&)*dfirst).value;
			if(dsecond -> type() == e_scalar) {
				auto const& svalue = ((Scalar<T>&)*dsecond).value;
				return make_scalar<T>(fvalue + svalue);
			}
			if(fvalue == fvalue*0) return dsecond;
		}
		if(dsecond -> type() == e_scalar) {
			auto const& svalue = ((Scalar<T>&)*dsecond).value;
			if(svalue == svalue*0) return dfirst;
		}
		return (SharedSymbol<T>) make_sum<T>(SharedSymbol<T>(dfirst), SharedSymbol<T>(dsecond));
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
		SharedSymbol<T> dfirst = first -> derive(c), dsecond = second -> derive(c);
		return make_difference<T>(dfirst, dsecond);
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
		/*SharedSymbol<T> dfirst = first -> derive(c),
				dsecond = second -> derive(c),
				lhs = make_product<T>(dfirst, second),
				rhs = make_product<T>(first, dsecond);
		return make_sum<T>(lhs, rhs);*/
		return first;
		/*return make_sum<T>(
		make_product<T>(first -> derive(c), second)
				make_product<T>(first, second -> derive(c)));*/
	}
	using Symbol<T>::operator==;
	bool operator==(Product<T> const& t) const {
		if((first ^ t.first) || (second ^ t.second)) return false;
		if(first && *first == *t.first || second && *second == *t.second) return false;
		return true;
	}
	Product(SharedSymbol<T> first, SharedSymbol<T> second):
			first(first), second(second) {}
	template<class R, class S>
	Product(std::shared_ptr<R> first, std::shared_ptr<S> second):
			Product((SharedSymbol<T>) first, (SharedSymbol<T>) second) {}
};

}

#endif
