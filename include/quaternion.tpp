#ifndef QUATERNION_TPP
#define QUATERNION_TPP

#include <iostream>
#include <sstream>

template<class L, class S, class T>
L& operator<<(L &lhs, Quaternion<S,T> const& rhs) {
	// Simple version - include zeros (useful for column comparisons)
	// auto i = 1;
	// const char *labels[] = {"e", "i", "j", "k"};
	// lhs << (rhs.w < rhs.w*0 ? "" : "+") << rhs.w << labels[0];
	// for(auto const& v : {rhs.x, rhs.y, rhs.z})
	// 	lhs << (v < v*0 ? "" : "+") << v << labels[i], i++;
	// return lhs;

	bool nz = false;
	std::ostringstream oss;
	for(auto i : {'e', 'i', 'j', 'k'}) {
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
