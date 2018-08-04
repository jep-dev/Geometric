#ifndef DUAL2_HPP
#define DUAL2_HPP

#include "quaternion.tpp"

template<class S> struct DualQuaternion {
	Quaternion<S> u, v;
	S& operator[](unsigned i) {
		switch(i) {
			case 1: case 'i': return u.x;
			case 2: case 'j': return u.y;
			case 3: case 'k': return u.z;
			case 4: case 'E': return v.w;
			case 5: case 'I': return v.x;
			case 6: case 'J': return v.y;
			case 7: case 'K': return v.z;
			default: return u.w;
		}
	}
	S const& operator[](unsigned i) const {
		switch(i) {
			case 1: case 'i': return u.x;
			case 2: case 'j': return u.y;
			case 3: case 'k': return u.z;
			case 4: case 'E': return v.w;
			case 5: case 'I': return v.x;
			case 6: case 'J': return v.y;
			case 7: case 'K': return v.z;
			default: return u.w;
		}
	}
	DualQuaternion operator-(void) const { return {-u, -v}; }
	DualQuaternion operator*(void) const { return {*u, {-v.w, v.x, v.y, v.z}}; }
	template<class T>
	DualQuaternion<std::common_type_t<S,T>> operator+(DualQuaternion const& rhs) const
		{ return {u+rhs.u, v+rhs.v}; }
	template<class T>
	DualQuaternion<std::common_type_t<S,T>> operator-(DualQuaternion const& rhs) const
		{ return {u-rhs.u, v-rhs.v}; }
	template<class T>
	DualQuaternion<std::common_type_t<S,T>> operator*(DualQuaternion<T> const& rhs) const
		{ return {u*rhs.u, u*rhs.v + v*rhs.u}; }
	template<class T>
	DualQuaternion<std::common_type_t<S,T>> operator^(DualQuaternion<T> const& rhs) const
		{ return {u * rhs.u * *u, u * rhs.v * *u}; }
	template<class T>
	Quaternion<std::common_type_t<S,T>> operator^(Quaternion<T> const& rhs) const
		{ return {u * rhs * *u, v * rhs * *u - u * rhs * *v}; }
	DualQuaternion(S p, S q = 0, S r = 0, S s = 0, S w = 0, S x = 0, S y = 0, S z = 0):
		u{p, q, r, s}, v{w, x, y, z} {}
	DualQuaternion(Quaternion<S> u = {0}, Quaternion<S> v = {0}): u(u), v(v) {}

};

/*template<class S, class T>
T& operator<<(T&, DualQuaternion<S> const&);*/

#endif
