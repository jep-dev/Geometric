#ifndef DUAL_TPP
#define DUAL_TPP

#include "quaternion.hpp"
#include "quaternion.tpp"
#include <iosfwd>

template<class L, class S>
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
}

template<class L, class S>
L& print_fixed(L &lhs, DualQuaternion<S> const& rhs, unsigned prec) {
	bool nz = false;
	std::ostringstream oss;
	oss.precision(prec);
	oss << std::fixed;
	for(auto i : {'e', 'i', 'j', 'k', 'E', 'I', 'J', 'K'}) {
		auto const& q = rhs[i];
		if(std::abs(q) < std::pow(.1f, prec)) continue;
		auto q2 = q*q;
		if(q2 == q) oss << (nz ? "+" : "");
		else if(q2 == -q) oss << (nz ? "-" : "-");
		else if(q > 0) oss << (nz ? "+" : "") << q;
		else oss << (nz ? "-" : "-") << -q;
		oss << i;
		nz = true;
	}
	if(!nz) oss << '0';
	return lhs << oss.str(), lhs;
	/*print(oss, rhs);
	return lhs << oss.str(), lhs;*/
}

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
	std::ostringstream oss;
	print(oss, rhs);
	return lhs << oss.str(), lhs;
}
template<class S>
DualQuaternion<S>::operator std::string(void) const {
	std::ostringstream oss;
	print_fixed(oss, *this, 3);
	return oss.str();
}

template<class S>
std::string to_string(DualQuaternion<S> const& s) {
	std::ostringstream oss;
	print(oss, s);
	return oss.str();
}
template<class S>
std::string to_string(DualQuaternion<S> const& s, unsigned prec) {
	std::ostringstream oss;
	print_fixed(oss, s, prec);
	return oss.str();
}

#endif
