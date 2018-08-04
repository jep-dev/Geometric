#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "math.hpp"

template<class S> struct Quaternion {
	using type = S;
	S w, x, y, z;
	/** Access the members by index or by unit. */
	const S& operator[](unsigned i) const {
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
	S& operator[](unsigned i) {
		switch(i) {
			case 1: case 'i': return x;
			case 2: case 'j': return y;
			case 3: case 'k': return z;
			default: return w;
		}
	}
	/** Square, as in product with itself. */
	S square(void) const {
		return w*w - x*x - y*y - z*z;
	}
	/** Square of the norm, as in sum of squared elements. */
	S lengthSquared(void) const {
		return w*w + x*x + y*y + z*z;
	}
	/** Euclidean norm. */
	S length(void) const {
		return sqrt(lengthSquared());
	}
	/** Complex conjugate. */
	Quaternion operator*(void) const { return {w, -x, -y, -z}; }
	/** (Right) scalar multiplication. */
	template<class T>
	Quaternion<std::common_type_t<S,T>> operator*(T const& t) const { return {w*t, x*t, y*t, z*t}; }
	/** Negation operator. */
	Quaternion operator-(void) const { return {-w, -x, -y, -z}; }
	/** Difference; currently relies on left addition and negation. */
	template<class T> auto operator-(T && rhs) const -> decltype((*this) + -rhs) {
		return (*this) + -rhs;
	}
	/** Component-wise sum. */
	template<class T> auto operator+(Quaternion<T> const& r) const
	-> Quaternion<std::common_type_t<S,T>> {
		return { w+r.w, x+r.x, y+r.y, z+r.z };
	}
	/** Product. */
	template<class T> auto operator*(Quaternion<T> const& r) const
	-> Quaternion<std::common_type_t<S,T>> {
		return {
			w*r.w - x*r.x - y*r.y - z*r.z,  w*r.x + x*r.w + y*r.z - z*r.y,
			w*r.y + y*r.w - x*r.z + z*r.x,  w*r.z + z*r.w + x*r.y - y*r.x
		};
	}
	Quaternion normalize(void) const {
		auto len = length();
		return *this * (1/len);
		/*if(near_zero(len)) return {0}; // TODO
		if(near(len, 1)) return *this;
		return *this * (1/len);*/
	}
	template<class U>
	friend auto operator*(U && u, Quaternion const& q)
	-> Quaternion<std::common_type_t<U,S>> {
		return {u * q.w, u * q.x, u * q.y, u * q.z};
	}
	/** (Left) scalar multiplication. */
	/* // TODO reinstate but with lower precedence than product with quaternion
	template<class U>
	friend auto operator*(U const& u, Quaternion const& q)
	-> Quaternion<decltype(q.w*u), decltype(q.x*u)> {
		static_assert(!std::is_same<U,Quaternion>::value, "Quaternion product should be used");
		return {u*q.w, u*q.x, u*q.y, u*q.z};
	}*/

};

template<class W, class X, class Y, class Z>
Quaternion<std::common_type_t<W,X,Y,Z>> rotation(W theta, X x, Y y, Z z) {
	auto c = cos(theta/2), s = sin(theta/2);
	return {c, s*x, s*y, s*z};
}

template<class L, class R, class T>
Quaternion<std::common_type_t<L,R,T>> lerp(Quaternion<L> const& lhs,
		Quaternion<R> const& rhs, T t) {
	return lhs * (1-t) + rhs * t;
}
/*template<class L, class R, class T>
Quaternion<std::common_type_t<L,R,T>> nlerp(Quaternion<L> const& lhs,
		Quaternion<R> const& rhs, T && t) {*/
template<class L, class R, class T>
auto nlerp(L && l, R && r, T && t) -> decltype(lerp(l, r, t)) {
	return nlerp(std::forward<L>(l), std::forward<R>(r), std::forward<T>(t)).normalize();
}

#endif
