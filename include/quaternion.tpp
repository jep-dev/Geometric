#ifndef QUATERNION_TPP
#define QUATERNION_TPP

#include "streams.hpp"
#include "quaternion.hpp"

/*template<class L, class S>
L& print(L &lhs, Quaternion<S> const& rhs) {
	bool nz = false;
	for(auto i : {'e', 'i', 'j', 'k'}) {
		auto const& q = rhs[i];
		auto q0 = q*0;
		if(q == q0) continue;
		auto q2 = q*q;
		if(q2 == q) lhs << (nz ? "+" : "");
		else if(q2 == -q) lhs << (nz ? "-" : "-");
		else if(q > 0) lhs << (nz ? "+" : "") << q;
		else lhs << (nz ? "-" : "-") << -q;
		lhs << i;
		nz = true;
	}
	if(!nz) lhs << '0';
	return lhs;
}*/
template<class S, class DELIM>
std::string to_string(Quaternion<S> const& qin, unsigned prec, DELIM delim) {
	auto margin = std::pow(.1f, prec<<1);
	bool nz = false;
	std::string out;
	for(auto i : {'e', 'i', 'j', 'k'}) {
		auto const& v = qin[i];
		auto stringified = to_string(v, prec);
		if(stringified == "0") continue;
		if(v > 0 && nz) out += '+';
		if(out == "1") {}
		else if(out == "-1") out += '-';
		else out += stringified;
		out += i;
		nz = true;
	}
	if(!nz) out += '0';
	return out;
}

template<class L, class S>
L& operator<<(L &lhs, Quaternion<S> const& rhs) {
	// Simple version - include zeros (useful for column comparisons)
	// auto i = 1;
	// const char *labels[] = {"e", "i", "j", "k"};
	// lhs << (rhs.w < rhs.w*0 ? "" : "+") << rhs.w << labels[0];
	// for(auto const& v : {rhs.x, rhs.y, rhs.z})
	// 	lhs << (v < v*0 ? "" : "+") << v << labels[i], i++;
	// return lhs;

	//bool nz = false;
	return lhs << std::string(rhs), lhs;
}
template<class S>
Quaternion<S>::operator std::string(void) const {
	return to_string(*this, 3);
}

#endif
