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
template<class> struct Sum;
template<class> struct Difference;
template<class> struct Product;

typedef enum {
	e_undefined = 0, e_scalar, e_variable, e_sum, e_difference, e_product
} EType;

template<class T>
struct Symbol: std::enable_shared_from_this<Symbol<T>> {
	virtual EType type(void) const = 0;
	bool negative = false, conj_complex = false, conj_dual = false;
	std::shared_ptr<Symbol> derive(char c);
	template<class S> bool operator==(S const& s) const { return false; }
	friend std::ostream& operator<<(std::ostream &lhs, Symbol<T> const& rhs) {
		switch(rhs.type()) {
			case e_scalar: return lhs << (Scalar<T>&) rhs, lhs;
			case e_variable: return lhs << (Variable<T>&) rhs, lhs;
			case e_sum: return lhs << (Sum<T>&) rhs, lhs;
			case e_difference: return lhs << (Difference<T>&) rhs, lhs;
			//case e_product: return lhs << (Product<T>&) rhs, lhs;
			default: return lhs;
		}
	}
	virtual ~Symbol(void) {}
};

template<class T>
struct Scalar : Symbol<T>, std::enable_shared_from_this<Scalar<T>> {
	T value = {};
	virtual EType type(void) const { return e_scalar; }
	std::shared_ptr<Symbol<T>> derive(char c) {
		return new Scalar<T>{0};
	}
	using Symbol<T>::operator==;
	bool operator==(Scalar<T> const& t) const {
		return value == t.value && this -> negative == t.negative
				&& this -> conj_complex == t.conj_complex
				&& this -> conj_dual = t.conj_dual;
	}
	friend std::ostream& operator<<(std::ostream &lhs, Scalar<T> const& rhs) {
		return lhs << rhs.value, lhs;
	}
	Scalar(Scalar const& s): value(s.value) {}
	Scalar(T const& t): value(t) {}
	virtual ~Scalar(void) {}
};

template<class T>
struct Variable : Symbol<T>, std::enable_shared_from_this<Variable<T>> {
	char value;
	virtual EType type(void) const { return e_variable; }
	std::shared_ptr<Symbol<T>> derive(char c) {
		return new Scalar<T>{value == c ? this -> negative ? -1 : 1 : 0};
	}
	using Symbol<T>::operator==;
	bool operator==(Variable<T> const& t) const {
		return value == t.value && this -> negative == t.negative
				&& this -> conj_complex == t.conj_complex && this -> conj_dual = t.conj_dual;
	}
	friend std::ostream& operator<<(std::ostream &lhs, Variable<T> const& rhs) {
		return lhs << rhs.value, lhs;
	}
	Variable(Variable<T> const& v): value(v.value) {}
	Variable(char k): value(k) {}
	virtual ~Variable(void) {}
};

template<class T>
struct Sum : Symbol<T>, std::enable_shared_from_this<Sum<T>> {
	std::shared_ptr<Symbol<T>> first, second;
	virtual EType type(void) const { return e_sum; }
	std::shared_ptr<Symbol<T>> derive(char c) {
		return new Sum<T>(first -> derive(c), second -> derive(c));
	}
	using Symbol<T>::operator==;
	bool operator==(Sum<T> const& t) const {
		if(first) {
			if(!t.first) return false;
			if(*first != *t.first) return false;
		} else if(t.first) return false;
		if(second) {
			if(!t.second) return false;
			if(*second != *t.second) return false;
		} else if(t.second) return false;
		return this -> negative == t.negative && this -> conj_complex == t.conj_complex
				&& this -> conj_dual == t.conj_dual;
		/*return *first == *t.first && *second == *t.second && this -> negative == t.negative
				&& this -> conj_complex == t.conj_complex && this -> conj_dual = t.conj_dual;*/
	}
	friend std::ostream& operator<<(std::ostream &lhs, Sum<T> const& rhs) {
		return lhs << *rhs.first << '+' << *rhs.second, lhs;
	}
	Sum(std::shared_ptr<Symbol<T>> first, std::shared_ptr<Symbol<T>> second):
		first(first), second(second) {}
	virtual ~Sum(void) {}
};

template<class T>
struct Difference : Symbol<T>, std::enable_shared_from_this<Difference<T>> {
	std::shared_ptr<Symbol<T>> first, second;
	virtual EType type(void) const { return e_difference; }
	std::shared_ptr<Symbol<T>> derive(char c) {
		return new Difference<T>(first -> derive(), second -> derive(c));
	}
	using Symbol<T>::operator==;
	bool operator==(Difference<T> const& t) const {
		if(first) {
			if(!t.first) return false;
			if(*first != *t.first) return false;
		} else if(t.first) return false;
		if(second) {
			if(!t.second) return false;
			if(*second != *t.second) return false;
		} else if(t.second) return false;
		return this -> negative == t.negative && this -> conj_complex == t.conj_complex
				&& this -> conj_dual == t.conj_dual;
		/*return *first == *t.first && *second == *t.second && this -> negative == t.negative
				&& this -> conj_complex == t.conj_complex && this -> conj_dual == t.conj_dual;*/
	}
	Difference(std::shared_ptr<Symbol<T>> first, std::shared_ptr<Symbol<T>> second):
			first(first), second(second) {}
};

template<class T>
struct Product : Symbol<T>, std::enable_shared_from_this<Product<T>> {
	std::shared_ptr<Symbol<T>> first, second;
	virtual EType type(void) const { return e_product; }
	std::shared_ptr<Symbol<T>> derive(char c) {
		return new Sum<Product<T>>(new Product<T>(first -> derive()),
			new Product<T>(second -> derive(c)));
	}
	using Symbol<T>::operator==;
	bool operator==(Product<T> const& t) const {
		if(first) {
			if(!t.first) return false;
			if(*first != *t.first) return false;
		} else if(t.first) return false;
		if(second) {
			if(!t.second) return false;
			if(*second != *t.second) return false;
		} else if(t.second) return false;
		return this -> negative == t.negative && this -> conj_complex == t.conj_complex
				&& this -> conj_dual == t.conj_dual;
		/*return *first == *t.first && *second == *t.second && this -> negative == t.negative
				&& this -> conj_complex == t.conj_complex && this -> conj_dual == t.conj_dual;*/
	}
	Product(std::shared_ptr<Symbol<T>> first, std::shared_ptr<Symbol<T>> second):
			first(first), second(second) {}
};

}

#endif
