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

	SharedSymbol<DQ> u = make_variable<DQ>('u'),
		v = make_variable<DQ>('v'),
		w = make_scalar<DQ>(DQ(1_e));
	vector<SharedSymbol<DQ>> symbols = {
		u, v, w, make_exp(u), make_exp(v), make_exp(w),
		make_product(u, u), make_product(v, v), make_product(w, w),
		make_product(u, v), make_product(u, w), make_product(v, w),
		make_sum(u, u), make_sum(u, v), make_sum(u, w),
		make_sum(u, u), make_sum(v, v), make_sum(w, w)
	};
	unsigned N = symbols.size();
	vector<string> lines(N+1, "");
	lines[0] = "Expression";
	ostringstream oss;
	for(unsigned i = 0; i < N; i++) {
		oss << *symbols[i];
		lines[i+1] += oss.str();
		oss.str("");
	}
	Streams::level_insert(lines, "  ");

	lines[0] += "Reduced";
	for(unsigned i = 0; i < N; i++) {
		oss << *symbols[i] -> simplify();
		lines[i+1] += oss.str();
		oss.str("");
	}
	Streams::level_insert(lines, "  ");
	for(char ch : {'u', 'v'}) {
		lines[0] += string("d/d") + ch;
		for(unsigned i = 0; i < N; i++) {
			oss << *(symbols[i] -> derive(ch));
			lines[i+1] += oss.str();
			oss.str("");
		}
		Streams::level_insert(lines, "  ");
	}
	for(auto const& line : lines)
		cout << line << endl;

	/*std::map<char, DQ> vars = {
		{'u', DQ{1}}
	};*/

	/*SharedSymbol<DQ> u = make_variable<DQ>('u'),
		v = make_variable<DQ>('v'),
		w = make_scalar<DQ>(DQ(1_e)),
		eu = make_exp(u), ev = make_exp(v),
		uu = make_product(u, u), vv = make_product(v, v), ww = make_product(w, w),
		uv = make_product(u, v), uw = make_product(u, w), vw = make_product(v, w),
		u_u = make_sum(u, u), u_v = make_sum(u, v), u_w = make_sum(u, w),
		u2 = make_sum(u, u), v2 = make_sum(v, v), w2 = make_sum(w, w);
	for(auto const& expr : {u, v, w, eu, ev, uu, uv, vv, ww, uv, uw, vw,
			u_u, u_v, u_w, u2, v2, w2})
		print_derivatives(cout, expr, 'u', 'v');*/
}
