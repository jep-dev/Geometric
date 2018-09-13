#ifndef QUATERNION_TPP
#define QUATERNION_TPP

#include <sstream>
#include <iomanip>

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
/*template<class S>
std::string to_string(Quaternion<S> const& q) {
	std::ostringstream oss;
	print(oss, q);
	return oss.str();
}*/
template<class L, class S, class DELIM = const char*>
L& print(L &lhs, Quaternion<S> const& rhs, unsigned prec = 0, DELIM delim = "+") {
	std::ostringstream oss;
	if(prec) {
		oss.precision(prec);
		oss << std::fixed;
	}
	bool nz = false;
	for(auto i : {'e', 'i', 'j', 'k'}) {
		auto const& q = rhs[i];
		if(prec) {
			if(std::abs(q) < std::pow(.1f, prec)) continue;
		} else {
			auto q0 = q*0;
			if(q == q0) continue;
		}
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
template<class S, class DELIM>
std::string to_string(Quaternion<S> const& q, unsigned prec, DELIM delim) {
	std::ostringstream oss;
	return print(oss, q, prec, delim), oss.str();
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
	std::ostringstream oss;
	print(oss, rhs);
	return lhs << oss.str(), lhs;
}
template<class S>
Quaternion<S>::operator std::string(void) const {
	return to_string(*this, 3);
}

#endif
