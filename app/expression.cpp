#include <map>
#include <iostream>
#include <sstream>

#include "expression.hpp"
#include "pretty.hpp"
#include "pretty.tpp"
#include "streams.hpp"
#include "streams.tpp"
#include "utility.hpp"



int main(int argc, const char *argv[]) {
	using namespace std;
	using namespace Detail;
	using T = float;
	using DQ = DualQuaternion<T>;
	using namespace Expressions;
	using Sym = Symbol<DQ>;
	using SymPtr = std::shared_ptr<Sym>;
	SymPtr one(new Scalar<DQ>(1_e)), u(new Variable<DQ>('x')),
		sum(new Sum<DQ>(std::move(one), std::move(u)));
	std::map<char, DQ> vars = {
		{'u', DQ{1}}
	};
	cout << *sum << endl;

	/*Cosine<T> c;
	Sine<T> s;
	cout << Pretty<decltype(c)>() << " -> " << Pretty<decltype(c.derive())>() << endl;
	cout << Pretty<decltype(s)>() << " -> " << Pretty<decltype(s.derive())>() << endl;*/
}
