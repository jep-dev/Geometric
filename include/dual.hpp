#ifndef DUAL_HPP
// Note: __FILE__ expands to the caller's file, not this file; TODO
#define DUAL_HPP "dual.hpp"
#define DUAL_IMPL DUAL_HPP

///@cond
#include <algorithm>
///@endcond

#include "math.hpp"
#include "quaternion.hpp"

/** A dual quaternion with a flat structure. */
template<class S> struct DualQuaternion {
	using value_type = S;
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
	operator Point<S>(void) const { return {x, y, z}; }

	/** Square, as in product with itself. */
	value_type square(void) const { return *this * *this; }
	/** Square of the norm, as in sum of squared elements. */
	value_type lengthSquared(void) const { return s*s + t*t + u*u + v*v + w*w + x*x + y*y + z*z; }
	/** Euclidean norm. */
	value_type length(void) const { return sqrt(lengthSquared()); }
	/** Conjugate; composed by the complex and dual conjugates. */
	template<class T> DualQuaternion& operator=(DualQuaternion<T> const& d) {
		s = d.s, t = d.t, u = d.u, v = d.v;
		w = d.w, x = d.w, y = d.y, z = d.z;
		return *this;
	}

	DualQuaternion operator*(void) const { return {s, -t, -u, -v, -w, x, y, z}; }
	DualQuaternion operator~(void) const { return {s, t, u, v, w, -x, -y, -z}; }
	DualQuaternion operator-(void) const { return {-s, -t, -u, -v, -w, -x, -y, -z}; }

	/* If q is a rotation, does adding a rotation make sense?
	 * Adding to the dual components might, if only non-duals could represent
	 * translation on their own.
	 * Even adding dual quaternions to one another is only intended for construction.
	 */
	template<class T>
	DualQuaternion& operator+=(T const& t) { return *this = *this + t; }
	template<class T>
	DualQuaternion& operator-=(T const& t) { return *this = *this - t; }

	template<class C, class SC = std::common_type_t<S,C>>
	DualQuaternion<SC> operator+(DualQuaternion<C> const& r) const
		{ return {s + r.s, t + r.t, u + r.u, v + r.v, w + r.w, x + r.x, y + r.y, z + r.z}; }

	template<class C, class SC = std::common_type_t<S,C>>
	DualQuaternion<SC> operator-(DualQuaternion<C> const& r) const
		{ return {s - r.s, t - r.t, u - r.u, v - r.v, w - r.w, x - r.x, y - r.y, z - r.z}; }

	/** (Right) scalar product. */
	template<class C, class SC = std::common_type_t<S,C>>
	DualQuaternion<SC> operator*(C const& c) const
		{ return {s*c, t*c, u*c, v*c, w*c, x*c, y*c, z*c}; }
	/** (Left) scalar product. */
	template<class C, class SC = std::common_type_t<S,C>>
	friend DualQuaternion<SC> operator*(C const& c, DualQuaternion const& d)
		{ return {c*d.s, c*d.t, c*d.u, c*d.v, c*d.w, c*d.x, c*d.y, c*d.z}; }
	template<class C, class SC = std::common_type_t<S,C>>
	DualQuaternion<SC> operator/(Quaternion<C> const& c)
		{ return *this * *c / c.lengthSquared(); }
	template<class C, class SC = std::common_type_t<S,C>>
	DualQuaternion<SC> operator/(C const& c)
		{ return {s/c, t/c, u/c, v/c, w/c, x/c, y/c, z/c}; }

	template<class T>
	operator DualQuaternion<T>(void) {
		return {T(s), T(t), T(u), T(v), T(w), T(x), T(y), T(z)};
	}
	template<class C, class SC = std::common_type_t<S,C>>
	std::enable_if_t<!std::is_same<DualQuaternion<S>, C>::value, DualQuaternion<SC>>
	operator*=(C const& c) { return *this = *this * c; }
	template<class C>
	DualQuaternion& operator*=(Quaternion<C> const& c) {
		return *this = *this * c;
	}
	template<class C>
	DualQuaternion& operator*=(DualQuaternion<C> const& c) {
		return *this = *this * c;
	}
	/*template<class T>
	DualQuaternion& operator*=(DualQuaternion<T> const& t) {
		return *this = *this * DualQuaternion<S>{t.s, t.t, t.u, t.v, t.w, t.x, t.y, t.z};
	}*/

	template<class T>
	DualQuaternion& operator+=(DualQuaternion<T> const& t)
		{ return *this = *this + DualQuaternion<S>{t.s, t.t, t.u, t.v, t.w, t.x, t.y, t.z}; }
	template<class T>
	DualQuaternion& operator-=(DualQuaternion<T> const& t)
		{ return *this = *this - DualQuaternion<S>{t.s, t.t, t.u, t.v, t.w, t.x, t.y, t.z}; }
	template<class T>
	DualQuaternion& operator/=(Quaternion<T> const& q)
		{ return *this = *this / Quaternion<S>{q.w, q.x, q.y, q.z}; }

	/*template<class T, class ST = std::common_type_t<S,T>>
	std::enable_if_t<std::is_arithmetic<T>::value, DualQuaternion<ST>>
	operator^(T const& r) const {
		return {}; // TODO
		//return (DualQuaternion<ST>)*this;
	}*/
	template<class T, class V = typename T::value_type, class SV = std::common_type_t<S,V>>
	DualQuaternion<SV> operator^(T const& r) const { return r * *this * *r; }

	/** Equality operator; true if and only if all members are identical. */
	bool operator==(DualQuaternion const& d) const
		{ return s == d.s && t == d.t && u == d.u && v == d.v
				&& w == d.w && x == d.x && y == d.y && z == d.z; }
	operator std::string(void) const;
};

template<class S, class T, class ST = std::common_type_t<S,T>>
DualQuaternion<ST> to_dual(Quaternion<S> const& s, Quaternion<T> const& t)
	{ return {s.w, s.x, s.y, s.z, t.w, t.x, t.y, t.z}; }

template<class S, unsigned SN, class T = S, unsigned TN = SN,
		class ST = std::common_type_t<S,T>>
DualQuaternion<ST> to_dual(const S (&s)[SN], const T (&t)[TN]) {
	static_assert(SN >= 4 && TN >= 4, "Components must contain (at least) 4 elements");
	return {s[0], s[1], s[2], s[3], t[0], t[1], t[2], t[3]};
}
template<class X, class Y = X, class Z = X, class XYZ = std::common_type_t<X,Y,Z>>
DualQuaternion<XYZ> point(X && x, Y && y, Z && z)
	{ return {(XYZ)1, (XYZ)0, (XYZ)0, (XYZ)0, (XYZ)0, (XYZ)x, (XYZ)y, (XYZ)z}; }
template<class X, class Y = X, class Z = X, class XYZ = std::common_type_t<X,Y,Z>>
DualQuaternion<XYZ> translation(X && x, Y && y, Z && z)
	{ return point<X,Y,Z,XYZ>(x/2, y/2, z/2); }

// TODO interpretation of dot product? Mind the dual*dual components?
template<class S, class T>
std::common_type_t<S,T> dot(DualQuaternion<S> const& l, DualQuaternion<T> const& r) {
	return l.s*r.s + l.t*r.t + l.u*r.u + l.v*r.v
			+ l.w*r.w + l.x*r.x + l.y*r.y + l.z*r.z;
}

template<class L, class R, class T, class LRT = std::common_type_t<L,R,T>>
DualQuaternion<LRT> lerp(DualQuaternion<L> const& lhs, DualQuaternion<R> const& rhs, T const& t)
	{ return lhs*(1-t) + (rhs-lhs)*t; }
/*template<class L1, class R1, class L2, class R2, class S, class T, class LRST>
DualQuaternion<LRST> lerp(DualQuaternion<L1> const& l1, DualQuaternion<R1> const& r1,
		DualQuaternion<L2> const& l2, DualQuaternion<R2> const& r2, S && s, T && t)
	{ return lerp(lerp(l1, r1, s), lerp(l2, r2, s), t); }*/
template<class L, class R, class T, class LRT = std::common_type_t<L,R,T>>
DualQuaternion<LRT> sclerp(DualQuaternion<L> const& lhs, DualQuaternion<R> const& rhs,
		T && t, bool normalize = false) {
	auto ou = slerp(Quaternion<L>{lhs.s, lhs.t, lhs.u, lhs.v},
			Quaternion<R>{rhs.s, rhs.t, rhs.u, rhs.v}, t, normalize),
		ov = lerp(Quaternion<L>{lhs.w, lhs.x, lhs.y, lhs.z},
			Quaternion<R>{rhs.w, rhs.x, rhs.y, rhs.z}, t);
	return {ou.w, ou.x, ou.y, ou.z, ov.w, ov.x, ov.y, ov.z};
}
template<class L1, class R1 = L1, class L2 = L1, class R2 = R1, class S = L1, class T = S,
		class LRST = std::common_type_t<L1, R1, L2, R2, S, T>>
DualQuaternion<LRST> sclerp(DualQuaternion<L1> const& l1, DualQuaternion<R1> const& r1,
		DualQuaternion<L2> const& l2, DualQuaternion<R2> const& r2,
			S && s, T && t, bool normalize = false)
	{ return sclerp(sclerp(l1, r1, s, normalize), sclerp(l2, r2, s, normalize), t, normalize); }


template<class S, class C, class SC = std::common_type_t<S,C>>
DualQuaternion<SC> operator*(DualQuaternion<S> const& l, Quaternion<C> const& r) {
	auto u = Quaternion<SC>{l.s, l.t, l.u, l.v} * r,
		v = Quaternion<SC>{l.w, l.x, l.y, l.z} * r;
	return {u.w, u.x, u.y, u.z, v.w, v.x, v.y, v.z};
}

template<class S, class C, class SC = std::common_type_t<S,C>>
DualQuaternion<SC> operator*(Quaternion<S> const& l, DualQuaternion<C> const& r) {
	return {
		l.w*r.s - l.x*r.t - l.y*r.u - l.z*r.v,
		l.w*r.t + l.x*r.s + l.y*r.v - l.z*r.u,
		l.w*r.u + l.y*r.s - l.x*r.v + l.z*r.t,
		l.w*r.v + l.z*r.s + l.x*r.u - l.y*r.t,
		l.w*r.w - l.x*r.x - l.y*r.y - l.z*r.z,
		l.w*r.x + l.x*r.w + l.y*r.z - l.z*r.y,
		l.w*r.y - l.x*r.z + l.y*r.w + l.z*r.x,
		l.w*r.z + l.x*r.y - l.y*r.x + l.z*r.w
	};
}
template<class S, class C, class SC = std::common_type_t<S,C>>
DualQuaternion<SC> operator*(DualQuaternion<S> const& l, DualQuaternion<C> const& r) {
		return {
			l.s*r.s - l.t*r.t - l.u*r.u - l.v*r.v,  l.s*r.t + l.t*r.s + l.u*r.v - l.v*r.u,
			l.s*r.u + l.u*r.s - l.t*r.v + l.v*r.t,  l.s*r.v + l.v*r.s + l.t*r.u - l.u*r.t,
			l.s*r.w - l.t*r.x - l.u*r.y - l.v*r.z + l.w*r.s - l.x*r.t - l.y*r.u - l.z*r.v,
			l.s*r.x + l.t*r.w + l.u*r.z - l.v*r.y + l.w*r.t + l.x*r.s + l.y*r.v - l.z*r.u,
			l.s*r.y - l.t*r.z + l.u*r.w + l.v*r.x + l.w*r.u - l.x*r.v + l.y*r.s + l.z*r.t,
			l.s*r.z + l.t*r.y - l.u*r.x + l.v*r.w + l.w*r.v + l.x*r.u - l.y*r.t + l.z*r.s
		};
}
template<class S, class T, class ST = std::common_type_t<S,T>>
DualQuaternion<ST> operator+(Quaternion<S> const& q, DualQuaternion<T> const& r)
	{ return {q.w+r.s, q.x+r.t, q.y+r.u, q.z+r.v, r.w, r.x, r.y, r.z}; }
template<class S, class T, class ST = std::common_type_t<S,T>>
DualQuaternion<ST> operator+(DualQuaternion<S> const& q, Quaternion<T> const& r)
	{ return r + q; }

template<class S, class C, class SC = std::common_type_t<S,C>>
std::enable_if_t<!std::is_same<S,C>::value, DualQuaternion<SC>>
operator+(DualQuaternion<S> const& lhs, DualQuaternion<C> const& rhs) {
	DualQuaternion<SC> l = lhs, r = rhs;
	return l + r;
}

// Gosh, it sure would be nice to have templates right about now.
// Template user-defined literals require that the template argument is char...,
// which would force parsing to play a role in literals.
#define USERDEF_TEMPLATE(CLASS,PARAM,UD,W,X,Y,Z) \
CLASS<PARAM> operator"" UD(long long unsigned d) \
	{ return {0, 0, 0, 0, PARAM(W), PARAM(X), PARAM(Y), PARAM(Z)}; } \
CLASS<PARAM> operator"" UD(long double d) \
	{ return {0, 0, 0, 0, PARAM(W), PARAM(X), PARAM(Y), PARAM(Z)}; }
#define USERDEF_TEMPLATES(CLASS, P1, UD1, P2, UD2, W, X, Y, Z) \
	USERDEF_TEMPLATE(CLASS, P1, UD1, W, X, Y, Z) \
	USERDEF_TEMPLATE(CLASS, P2, UD2, W, X, Y, Z)

USERDEF_TEMPLATES(DualQuaternion, float, _E, double, _dE, d,0,0,0)
USERDEF_TEMPLATES(DualQuaternion, float, _I, double, _dI, 0,d,0,0)
USERDEF_TEMPLATES(DualQuaternion, float, _J, double, _dJ, 0,0,d,0)
USERDEF_TEMPLATES(DualQuaternion, float, _K, double, _dK, 0,0,0,d)

#undef USERDEF_TEMPLATE
#undef USERDEF_TEMPLATES

#endif
