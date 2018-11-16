#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "math.hpp"

template<class S> struct Quaternion {
	using value_type = S;
	S w, x, y, z;

	/** Access the members by index or by unit. */
	const S& operator[](unsigned i) const {
		switch(i) {
			case 1: case 'i': case 'x': return x;
			case 2: case 'j': case 'y': return y;
			case 3: case 'k': case 'z': return z;
			default: return w;
		}
	}
	/** Access the members by index or by unit.
	 * This is an identical copy of the const variant, but appears necessary:
	 * a const method can only call other const methods, while the const methods
	 * are visible from any method, but can't return the required mutable reference. */
	S& operator[](unsigned i) {
		switch(i) {
			case 1: case 'i': case 'x': return x;
			case 2: case 'j': case 'y': return y;
			case 3: case 'k': case 'z': return z;
			default: return w;
		}
	}

	operator DualQuaternion<S>(void) const { return {w, x, y, z}; }
	template<class T> bool operator==(Quaternion<T> const& rhs) const
		{ return w == rhs.w && x == rhs.x && y == rhs.y && z == rhs.z; }

	template<class T>
	operator DualQuaternion<T>(void) const {
		return {w, x, y, z};
	}

	/** Square, as in product with itself. */
	S square(void) const { return w*w - x*x - y*y - z*z; }
	/** Square of the norm, as in sum of squared elements. */
	S lengthSquared(void) const { return w*w + x*x + y*y + z*z; }
	/** Euclidean norm. */
	S length(void) const { return sqrt(lengthSquared()); }


	/** Complex conjugate. */
	Quaternion operator*(void) const { return {w, -x, -y, -z}; }

	/** Negation operator. */
	Quaternion operator-(void) const { return {-w, -x, -y, -z}; }

	/** Difference; currently relies on left addition and negation. */
	template<class T> auto operator-(T && rhs) const -> decltype((*this) + -rhs)
		{ return (*this) + -rhs; }

	/** Component-wise sum. */
	template<class T, class ST = std::common_type_t<S,T>>
	Quaternion<ST> operator+(Quaternion<T> const& r) const
		{ return { w+r.w, x+r.x, y+r.y, z+r.z }; }

	Quaternion normalize(void) const {
		auto len = length();
		//return *this * (1/len);
		if(near_zero(len)) return {0}; // TODO
		if(near(len, 1)) return *this;
		return *this * (1/len);
	}
	template<class T, class ST = std::common_type_t<S,T>>
	std::enable_if<std::is_arithmetic<ST>::value, Quaternion<ST>>
	operator^(T const& t) const {
		auto len = lengthSquared();
		// Use proximity?
		if(len == 0) return 1;
		len = sqrt(len);
		auto r = pow(len, t);
		auto normed = *this / len;
		ST t0 = acos(normed.w), c0 = cos(t0), s0 = sin(t0),
			t1 = t0 * t, c1 = cos(t1), s1 = sin(t1), scale = len * s1 / s0;
		return { r * c1, normed.x * scale, normed.y * scale, normed.z * scale };
	}

	template<class U, class US = std::common_type_t<S,U>>
	Quaternion<US> operator/(U const& u) const
		{ return {w / u, x / u, y / u, z / u}; }

	template<class U, class US = std::common_type_t<S,U>>
	friend Quaternion<US> operator/(U const& u, Quaternion const& q)
		{ return *q * u / q.lengthSquared(); }

	template<class L, class LS = std::common_type_t<L, S>>
	friend Quaternion<LS> operator/(Quaternion<L> const& lhs, Quaternion<S> const& rhs)
		{ return lhs * *rhs / rhs.lengthSquared(); }

	operator std::string(void) const;
};

template<class S, unsigned M, class T = S>
Quaternion<T>& operator>>(const S (&s)[M], Quaternion<T> &q) {
	static_assert(M >= 4, "The source array must (at least) 4 elements long");
	return q.w = s[0], q.x = s[1], q.y = s[2], q.z = s[3], q;
}
template<class S, class T, class ST = std::common_type_t<S,T>>
Quaternion<ST> operator*(Quaternion<S> const& l, T const& r)
	{ return {l.w*r, l.x*r, l.y*r, l.z*r}; }
template<class S, class T, class ST = std::common_type_t<S,T>>
Quaternion<ST> operator*(Quaternion<S> const& l, Quaternion<T> const& r) {
	return {
		l.w*r.w - l.x*r.x - l.y*r.y - l.z*r.z,  l.w*r.x + l.x*r.w + l.y*r.z - l.z*r.y,
		l.w*r.y + l.y*r.w - l.x*r.z + l.z*r.x,  l.w*r.z + l.z*r.w + l.x*r.y - l.y*r.x
	};
}
template<class S, class T, class ST = std::common_type_t<S,T>>
Quaternion<ST> operator*(S && l, Quaternion<T> const& r)
	{ return {l * r.w, l * r.x, l * r.y, l * r.z}; }

template<class L, class R>
std::common_type_t<L,R> dot(Quaternion<L> const& l, Quaternion<R> const& r)
	{ return l.w*r.w + l.x*r.x + l.y*r.y + l.z*r.z; }

template<class W, class X, class Y, class Z, class S = std::common_type_t<W,X,Y,Z>>
Quaternion<S> rotation(W theta, X x, Y y, Z z) {
	auto c = cos(theta/2), s = sin(theta/2);
	return {S(c), S(s*x), S(s*y), S(s*z)};
}

template<class L, class R, class T, class LRT = std::common_type_t<L,R,T>>
Quaternion<LRT> nlerp(Quaternion<L> const& l, Quaternion<R> const& r, T const& t)
	{ return lerp(l.normalize(), r.normalize(), t); }

template<class L, class R, class T, class LRT = std::common_type_t<L,R,T>>
Quaternion<LRT> slerp(Quaternion<L> const& lhs,
		Quaternion<R> const& rhs, T && t, bool normalize = false) {
	auto l = lhs.normalize();
	auto r = rhs.normalize();
	auto d = dot(l, r);
	if(d < 0) d = -d, r = -r;
	if(d > .99) return normalize ? lerp(l, r, t) : lerp(lhs, rhs, t);
	auto t0 = acos(d), tt = t0 * t,
		st0 = sin(t0), stt = sin(tt),
		a = cos(tt) - d * stt / st0, b = stt / st0;
	return normalize ? a * l + b * r : a * lhs + b * rhs;
}
template<class L1, class R1 = L1, class L2 = L1, class R2 = L2,
		class S = L1, class T = S, class LRT = std::common_type_t<L1, R1, L2, R2, S, T>>
Quaternion<LRT> slerp(Quaternion<L1> const& l1, Quaternion<R1> const& r1,
Quaternion<L2> const& l2, Quaternion<R2> const& r2,
		S const& s, T const& t, bool normalize = false)
	{ return slerp(slerp(l1, r1, s), slerp(l2, r2, s), t); }

template<class S>
Quaternion<S> exp(Quaternion<S> const& q) {
	// e^(a+bi+cj+dk) = e^a e^bi ... = (e^a)(cb+sbi)(...)
	auto a = exp(q.w),
		cb = cos(q.x), sb = sin(q.x),
		cc = cos(q.y), sc = sin(q.y),
		cd = cos(q.z), sd = sin(q.z);
	return {a * cb * cc * cd,
		a * (sb * cc * cd + cb * sc * sd),
		a * (cb * sc * cd - sb * cc * sd),
		a * (cb * cc * sd + sb * sc * cd)};
}

// Generate user-defined literals for likely parameter types
#define USERDEF_TEMPLATE(CLASS,PARAM,UD,W,X,Y,Z) \
CLASS<PARAM> operator"" UD(long long unsigned d) \
	{ return {PARAM(W), PARAM(X), PARAM(Y), PARAM(Z)}; } \
CLASS<PARAM> operator"" UD(long double d) \
	{ return {PARAM(W), PARAM(X), PARAM(Y), PARAM(Z)}; }

// Generate user-defined literals for given suffixes / template arguments
#define USERDEF_TEMPLATES(CLASS, P1, UD1, P2, UD2, W, X, Y, Z) \
	USERDEF_TEMPLATE(CLASS, P1, UD1, W, X, Y, Z) \
	USERDEF_TEMPLATE(CLASS, P2, UD2, W, X, Y, Z)

// Generate user-defined literals in {""_e -> Q<float>, ..., ""_dk -> Q<double>}
USERDEF_TEMPLATES(Quaternion, float, _e, double, _de, d,0,0,0)
USERDEF_TEMPLATES(Quaternion, float, _i, double, _di, 0,d,0,0)
USERDEF_TEMPLATES(Quaternion, float, _j, double, _dj, 0,0,d,0)
USERDEF_TEMPLATES(Quaternion, float, _k, double, _dk, 0,0,0,d)

#undef USERDEF_TEMPLATE
#undef USERDEF_TEMPLATES

#endif
