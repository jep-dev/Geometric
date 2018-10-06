#ifndef DUAL_TPP
#define DUAL_TPP

#include "dual.hpp"
#include "quaternion.hpp"
#include "quaternion.tpp"
#include "streams.hpp"

/*template<class L, class S>
L& print(L &lhs, DualQuaternion<S> const& rhs) {
	bool nz = false;
	for(auto i : {'e', 'i', 'j', 'k', 'E', 'I', 'J', 'K'}) {
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

template<class L, class S, class DELIM = const char*>
L& print(L &lhs, DualQuaternion<S> const& rhs, unsigned prec, DELIM delim = "+") {
	return lhs << to_string(rhs, prec, delim), lhs;
}

template<class L, class S>
L& operator<<(L &lhs, DualQuaternion<S> const& rhs) {
	return lhs << to_string(rhs, 0), lhs;
}

template<class S, class DELIM>
std::string to_string(DualQuaternion<S> const& s, unsigned prec, DELIM delim) {
	bool nz = false;
	std::string out;
	for(auto i : {'e', 'i', 'j', 'k', 'E', 'I', 'J', 'K'}) {
		auto const& v = s[i];
		auto stringified = to_unit_string(v, i, prec);
		if(!stringified.length()) continue;
		if(v > 0 && nz) out += '+';
		out += stringified;
		nz = true;
	}
	if(!nz) out += '0';
	return out;
}
template<class S>
DualQuaternion<S>::operator std::string(void) const
	{ return to_string(*this, 3, "+"); }

#endif
