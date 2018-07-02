#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include <math.h>
#include <type_traits>

template<class S, class T = S> struct Quaternion {
	using Real_type = S;
	using Complex_type = T;
	using Common_type = std::common_type_t<Real_type, Complex_type>;
	S w; T x, y, z;
	/** Access the members by index or by unit. */
	const Common_type& operator[](unsigned i) const {
		switch(i) {
			case 1: case 'i': return x;
			case 2: case 'j': return y;
			case 3: case 'k': return z;
			default: return w;
		}
	}
	/** Access the members by index or by unit.
	 * This is an identical copy of the const variant, but appears necessary:
	 * a const method can only call other const methods, while the const methods
	 * are visible from any method, but can't return the required mutable reference. */
	Common_type& operator[](unsigned i) {
		switch(i) {
			case 1: case 'i': return x;
			case 2: case 'j': return y;
			case 3: case 'k': return z;
			default: return w;
		}
	}
	Common_type square(void) const {
		return w*w + x*x + y*y + z*z;
	}
	/** Get the norm of the quaternion. */
	Common_type length(void) const {
		return sqrt(square());
	}
	/*
		// Simple version - include zeros (useful for column comparisons)
		// auto i = 1;
		// const char *labels[] = {"e", "i", "j", "k"};
		// lhs << (rhs.w < rhs.w*0 ? "" : "+") << rhs.w << labels[0];
		// for(auto const& v : {rhs.x, rhs.y, rhs.z})
		// 	lhs << (v < v*0 ? "" : "+") << v << labels[i], i++;
		// return lhs;

		bool nz = false;
		for(auto i : {'e', 'i', 'j', 'k'}) {
			auto const& q = rhs[i];
			auto q0 = q*0;
			if(q == q0) continue;
			auto q2 = q*q;
			if(q2 == q) lhs << (nz ? "+" : "");
			else if(q2 == -q) lhs << (nz ? "-" : "-");
			else if(q > q0) lhs << (nz ? "+" : "") << q;
			else lhs << (nz ? "-" : "-") << -q;
			lhs << i;
			nz = true;
		}
		if(!nz) lhs << '0';
		return lhs;
	}*/
	Quaternion operator*(void) const { return {w, -x, -y, -z}; }
	//template<class U, class SU = std::common_type_t<S,U>, class TU = std::common_type<T,U>>
	template<class U>
	Quaternion operator*(U const& u) const { return {w*u, x*u, y*u, z*u}; }
	Quaternion operator-(void) const { return {-w, -x, -y, -z}; }

	template<class U> auto operator-(U && rhs) const -> decltype((*this) + -rhs) {
		return (*this) + -rhs;
		//return { w-r.w, x-r.x, y-r.y, z-r.z };
	}
	template<class... U> auto operator+(Quaternion<U...> const& r) const
	-> Quaternion<decltype(w+r.w), decltype(x+r.x)> {
		return { w+r.w, x+r.x, y+r.y, z+r.z };
	}
	template<class... U> auto operator*(Quaternion<U...> const& r) const
	-> Quaternion<decltype(w*r.w), decltype(x+r.x)> {
		return {
			w*r.w - x*r.x - y*r.y - z*r.z,  w*r.x + x*r.w + y*r.z - z*r.y,
			w*r.y + y*r.w - x*r.z + z*r.x,  w*r.z + z*r.w + x*r.y - y*r.x
		};
	}
};

#endif
