#include <map>
#include <iostream>
#include <sstream>

#include "expression.hpp"
#include "pretty.hpp"
#include "pretty.tpp"
#include "math.tpp"
#include "streams.hpp"
#include "streams.tpp"
#include "utility.hpp"

template<class S, class... T>
std::ostream& print_derivatives(std::ostream &os, std::shared_ptr<S> const& s, T &&... t) {
	using namespace std;
	for(auto it : {t...})
		os << "d/d" << it << " " << *s << " = " << *(s -> derive(it) -> simplify()) << endl;
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

	SharedSymbol<DQ> u = make_variable<DQ>('u'),
		v = make_variable<DQ>('v'),
		w = make_scalar<DQ>(DQ(1_e)),
		uu = make_product(u, u), vv = make_product(v, v), ww = make_product(w, w),
		uv = make_product(u, v), uw = make_product(u, w), vw = make_product(v, w),
		u_u = make_sum(u, u), u_v = make_sum(u, v), u_w = make_sum(u, w),
		u2 = make_sum(u, u), v2 = make_sum(v, v), w2 = make_sum(w, w);
	for(auto const& expr : {u, v, w, uu, uv, vv, ww, uv, uw, vw,
			u_u, u_v, u_w, u2, v2, w2})
		print_derivatives(cout, expr, 'u', 'v');
}
