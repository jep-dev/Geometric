#ifndef DUAL_HPP
// Note: __FILE__ expands to the caller's file, not this file; TODO
#define DUAL_HPP "dual.hpp"
#define DUAL_IMPL DUAL_HPP

///@cond
#include <algorithm>
///@endcond

#include "math.hpp"
#include "quaternion.tpp"

template<class S> struct DualQuaternion;

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
	template<class T> DualQuaternion& operator=(DualQuaternion<T> const& d) {
		s = d.s, t = d.t, u = d.u, v = d.v;
		w = d.w, x = d.w, y = d.y, z = d.z;
		return *this;
	}

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

	template<class T>
	operator DualQuaternion<T>(void) {
		return {T(s), T(t), T(u), T(v), T(w), T(x), T(y), T(z)};
	}
	template<class T>
	DualQuaternion& operator*=(DualQuaternion<T> const& t) {
		return *this = *this * DualQuaternion<S>{t.s, t.t, t.u, t.v, t.w, t.x, t.y, t.z};
	}
	template<class T>
	DualQuaternion& operator*=(T const& t) { return *this = *this * t; }

	template<class T>
	DualQuaternion& operator+=(DualQuaternion<T> const& t) {
		return *this = *this + DualQuaternion<S>{t.s, t.t, t.u, t.v, t.w, t.x, t.y, t.z};
	}
	template<class T>
	DualQuaternion& operator-=(DualQuaternion<T> const& t) {
		return *this = *this - DualQuaternion<S>{t.s, t.t, t.u, t.v, t.w, t.x, t.y, t.z};
	}
	template<class T>
	DualQuaternion& operator/=(Quaternion<T> const& q) {
		return *this = *this / Quaternion<S>{q.w, q.x, q.y, q.z};
	}

	/* First define operator+(T), operator-(T) if you want these, e.g.
	 *   template<class T> DualQuaternion operator+(Quaternion<T> const& q)
	 * If q is a rotation, does adding a rotation make sense?
	 * Adding to the dual components might, but quaternions can't represent translation on their
	 * own, so this interpretation is a problem, too.
	 * Even adding dual quaternions to one another is only intended for construction.
	 */
	/*
	template<class T>
	DualQuaternion& operator+=(T const& t) { return *this = *this + t; }
	template<class T>
	DualQuaternion& operator-=(T const& t) { return *this = *this - t; }
	*/


	/** Equality operator; true if and only if all members are identical. */
	bool operator==(DualQuaternion const& d) const
		{ return s == d.s && t == d.t && u == d.u && v == d.v
				&& w == d.w && x == d.x && y == d.y && z == d.z; }
	operator std::string(void) const;
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
template<class L1, class R1, class L2, class R2, class S, class T>
DualQuaternion<std::common_type_t<L1,R1,L2,R2,S,T>>
sclerp(DualQuaternion<L1> const& l1, DualQuaternion<R1> const& r1,
		DualQuaternion<L2> const& l2, DualQuaternion<R2> const& r2,
		S && s, T && t) {
	return sclerp(sclerp(l1, r1, s), sclerp(l2, r2, s), t);
}

template<class S>
std::string to_string(DualQuaternion<S> const& d);
template<class S>
std::string to_string(DualQuaternion<S> const& d, unsigned prec);

// Gosh, it sure would be nice to have templates right about now.
// Template user-defined literals require that the template argument is char...,
// which would force parsing to play a role in literals.
#define USERDEF_TEMPLATE(CLASS,PARAM,UD,S,T,U,V,W,X,Y,Z) \
CLASS<PARAM> operator"" UD(long long unsigned d) { \
	return {PARAM(S), PARAM(T), PARAM(U), PARAM(V), PARAM(W), PARAM(X), PARAM(Y), PARAM(Z)}; \
} \
CLASS<PARAM> operator"" UD(long double d) { \
	return {PARAM(S), PARAM(T), PARAM(U), PARAM(V), PARAM(W), PARAM(X), PARAM(Y), PARAM(Z)}; \
}
#define USERDEF_TEMPLATES(CLASS, P1, UD1, P2, UD2, S, T, U, V, W, X, Y, Z) \
	USERDEF_TEMPLATE(CLASS, P1, UD1, S, T, U, V, W, X, Y, Z) \
	USERDEF_TEMPLATE(CLASS, P2, UD2, S, T, U, V, W, X, Y, Z)

USERDEF_TEMPLATES(DualQuaternion, float, _e, double, _de, d,0,0,0, 0,0,0,0)
USERDEF_TEMPLATES(DualQuaternion, float, _i, double, _di, 0,d,0,0, 0,0,0,0)
USERDEF_TEMPLATES(DualQuaternion, float, _j, double, _dj, 0,0,d,0, 0,0,0,0)
USERDEF_TEMPLATES(DualQuaternion, float, _k, double, _dk, 0,0,0,d, 0,0,0,0)
USERDEF_TEMPLATES(DualQuaternion, float, _E, double, _dE, 0,0,0,0, d,0,0,0)
USERDEF_TEMPLATES(DualQuaternion, float, _I, double, _dI, 0,0,0,0, 0,d,0,0)
USERDEF_TEMPLATES(DualQuaternion, float, _J, double, _dJ, 0,0,0,0, 0,0,d,0)
USERDEF_TEMPLATES(DualQuaternion, float, _K, double, _dK, 0,0,0,0, 0,0,0,d)

#undef USERDEF_TEMPLATE
#undef USERDEF_TEMPLATES

#endif
