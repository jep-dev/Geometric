#ifndef DUAL_TPP
#define DUAL_TPP

#include "quaternion.hpp"
#include "quaternion.tpp"
#include <sstream>

template<class L, class S>
L& operator<<(L &lhs, DualQuaternion<S> const& rhs) {
	/*std::ostringstream oss;
	Quaternion<S,S> p = {rhs.s, rhs.t, rhs.u, rhs.v},
		q = {rhs.w, rhs.x, rhs.y, rhs.z};
	oss << p << "+(" << q << ")E";
	return lhs << oss.str(), lhs;*/
	/*for(auto i : {'e', 'i', 'j', 'k', 'E', 'I', 'J', 'K'}) {
		oss << rhs[i] << i << (i == 'e' ? "" : "+");
	}
	lhs << oss.str();
	return lhs;*/

	bool nz = false;
	std::ostringstream oss;
	for(auto i : {'e', 'i', 'j', 'k', 'E', 'I', 'J', 'K'}) {
		auto const& q = rhs[i];
		auto q0 = q*0;
		if(q == q0) continue;
		auto q2 = q*q;
		if(q2 == q) oss << (nz ? "+" : "");
		else if(q2 == -q) oss << (nz ? "-" : "-");
		else if(q > q0) oss << (nz ? "+" : "") << q;
		else oss << (nz ? "-" : "-") << -q;
		oss << i;
		nz = true;
	}
	if(!nz) oss << '0';
	return lhs << oss.str(), lhs;
}

#endif
