#ifndef DUAL_HPP
#define DUAL_HPP

///@cond
#include <algorithm>
///@endcond

#include "math.hpp"
#include "quaternion.tpp"

/** A dual quaternion with a flat structure. */
template<class S> struct DualQuaternion {
	using type = S;
	S s, t, u, v, w, x, y, z;
	/** Const member access by index and unit. */
	const S& operator[](unsigned i) const {
		switch(i) {
			case 1: case 'i': return t;
			case 2: case 'j': return u;
			case 3: case 'k': return v;
			case 4: case 'E': return w;
			case 5: case 'I': return x;
			case 6: case 'J': return y;
			case 7: case 'K': return z;
			default: return s;
		}
	}
	/** Member access by index and unit. */
	S& operator[](unsigned i) {
		switch(i) {
			case 1: case 'i': return t;
			case 2: case 'j': return u;
			case 3: case 'k': return v;
			case 4: case 'E': return w;
			case 5: case 'I': return x;
			case 6: case 'J': return y;
			case 7: case 'K': return z;
			default: return s;
		}
	}
	/** Square, as in product with itself. */
	type square(void) const { return *this * *this; }
	/** Square of the norm, as in sum of squared elements. */
	type lengthSquared(void) const { return s*s + t*t + u*u + v*v + w*w + x*x + y*y + z*z; }
	/** Euclidean norm. */
	type length(void) const { return sqrt(lengthSquared()); }
	/** Conjugate; composed by the complex and dual conjugates. */
	DualQuaternion operator*(void) const { return {s, -t, -u, -v, -w, x, y, z}; }
	DualQuaternion operator-(void) const { return {-s, -t, -u, -v, -w, -x, -y, -z}; }
	DualQuaternion operator*(DualQuaternion const& d) const {
		return {
			s*d.s - t*d.t - u*d.u - v*d.v,  s*d.t + t*d.s + u*d.v - v*d.u,
			s*d.u + u*d.s - t*d.v + v*d.t,  s*d.v + v*d.s + t*d.u - u*d.t,
			s*d.w - t*d.x - u*d.y - v*d.z + w*d.s - x*d.t - y*d.u - z*d.v,
			s*d.x + t*d.w + u*d.z - v*d.y + w*d.t + x*d.s + y*d.v - z*d.u,
			s*d.y - t*d.z + u*d.w + v*d.x + w*d.u - x*d.v + y*d.s + z*d.t,
			s*d.z + t*d.y - u*d.x + v*d.w + w*d.v + x*d.u - y*d.t + z*d.s
		};
	}
	template<class C>
	auto operator+(DualQuaternion<C> const& r) -> DualQuaternion<std::common_type_t<S,C>> const {
		return {s + r.s, t + r.t, u + r.u, v + r.v, w + r.w, x + r.x, y + r.y, z + r.z};
	}
	template<class C>
	auto operator-(DualQuaternion<C> const& r) -> DualQuaternion<std::common_type_t<S,C>> const {
		return {s - r.s, t - r.t, u - r.u, v - r.v, w - r.w, x - r.x, y - r.y, z - r.z};
	}
	/** (Right) scalar product. */
	template<class C> DualQuaternion operator*(C const& c) const
		{ return {s*c, t*c, u*c, v*c, w*c, x*c, y*c, z*c}; }
	/** (Left) scalar product. */
	template<class C> friend DualQuaternion operator*(C const& c, DualQuaternion const& d)
		{ return {c*d.s, c*d.t, c*d.u, c*d.v, c*d.w, c*d.x, c*d.y, c*d.z}; }
	/*bool operator>(DualQuaternion const& d) const
		{ return s > d.s && t > d.t && u > d.u && v > d.v
				&& w > d.w && x > d.x && y > d.y && z > d.z; }*/
	/** Equality operator; true if and only if all members are identical. */
	bool operator==(DualQuaternion const& d) const
		{ return s == d.s && t == d.t && u == d.u && v == d.v
				&& w == d.w && x == d.x && y == d.y && z == d.z; }
};

/** Stream insertion operator; left generic to support ostringstream, etc. exactly. */
template<class L, class S>
L& operator<<(L&, DualQuaternion<S> const&);

template<class L, class R, class T>
auto sclerp(DualQuaternion<L> const& lhs, DualQuaternion<R> const& rhs, T && t)
		-> DualQuaternion<std::common_type_t<L,R,T>> {
	Quaternion<L> lu = {lhs.s, lhs.t, lhs.u, lhs.v}, lv = {lhs.w, lhs.x, lhs.y, lhs.z};
	Quaternion<R> ru = {rhs.s, rhs.t, rhs.u, rhs.v}, rv = {rhs.w, rhs.x, rhs.y, rhs.z};
	auto ou = slerp(lu, ru, t), ov = lerp(lv, rv, t);
	return {ou.w, ou.x, ou.y, ou.z, ov.w, ov.x, ov.y, ov.z};
}

#endif
