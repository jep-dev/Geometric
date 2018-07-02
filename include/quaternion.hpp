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
	/** Square, as in product with itself. */
	Common_type square(void) const {
		return w*w - x*x - y*y - z*z;
	}
	/** Square of the norm, as in sum of squared elements. */
	Common_type lengthSquared(void) const {
		return w*w + x*x + y*y + z*z;
	}
	/** Euclidean norm. */
	Common_type length(void) const {
		return sqrt(square());
	}
	/** Complex conjugate. */
	Quaternion operator*(void) const { return {w, -x, -y, -z}; }
	/** (Right) scalar multiplication. */
	template<class U>
	Quaternion operator*(U const& u) const { return {w*u, x*u, y*u, z*u}; }
	/** Negation operator. */
	Quaternion operator-(void) const { return {-w, -x, -y, -z}; }
	/** Difference; currently relies on left addition and negation. */
	template<class U> auto operator-(U && rhs) const -> decltype((*this) + -rhs) {
		return (*this) + -rhs;
	}
	/** Component-wise sum. */
	template<class... U> auto operator+(Quaternion<U...> const& r) const
	-> Quaternion<decltype(w+r.w), decltype(x+r.x)> {
		return { w+r.w, x+r.x, y+r.y, z+r.z };
	}
	/** Product. */
	template<class... U> auto operator*(Quaternion<U...> const& r) const
	-> Quaternion<decltype(w*r.w), decltype(x+r.x)> {
		return {
			w*r.w - x*r.x - y*r.y - z*r.z,  w*r.x + x*r.w + y*r.z - z*r.y,
			w*r.y + y*r.w - x*r.z + z*r.x,  w*r.z + z*r.w + x*r.y - y*r.x
		};
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

#endif
