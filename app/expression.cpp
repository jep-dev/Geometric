#include <map>
#include <iostream>
#include <sstream>

#include "expression.hpp"
#include "pretty.hpp"
#include "pretty.tpp"
#include "streams.hpp"
#include "streams.tpp"
#include "utility.hpp"

template<class S, class... T>
std::ostream& print_derivatives(std::ostream &os, std::shared_ptr<S> const& s, T &&... t) {
	using namespace std;
	for(auto it : {t...})
		os << "d/d" << it << " " << *s << " = " << *(s -> derive(it)) << endl;
	return os;
}

int main(int argc, const char *argv[]) {
	using namespace std;
	using namespace Detail;
	using namespace Expressions;

	using T = float;
	using DQ = DualQuaternion<T>;

	/*std::map<char, DQ> vars = {
		{'u', DQ{1}}
	};*/

	auto u = make_variable<DQ>('U');
	auto uu = make_product<DQ>(u, u);
	auto v = make_scalar<DQ>(DQ(1_e));
	auto uv = make_sum<DQ>(v, v);

	print_derivatives(cout, u, 'U');
	//print_derivatives(cout, uu, 'U');
	print_derivatives(cout, v, 'U');
	print_derivatives(cout, uv, 'U');
}
