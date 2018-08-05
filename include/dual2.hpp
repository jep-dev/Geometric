#ifndef DUAL2_HPP
#define DUAL2_HPP "dual2.hpp"
#define DUAL_IMPL DUAL2_HPP

#include "quaternion.tpp"

template<class S> struct DualQuaternion {
private:
	template<class... U> using Com = std::common_type_t<U...>;
public:
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
	auto operator+(DualQuaternion<T> const& rhs)
		-> DualQuaternion<std::common_type_t<S,T>> const { return {u+rhs.u, v+rhs.v}; }
	template<class T>
	auto operator-(DualQuaternion<T> const& rhs)
		-> DualQuaternion<std::common_type_t<S,T>> const { return {u-rhs.u, v-rhs.v}; }
	template<class T>
	auto operator*(DualQuaternion<T> const& rhs)
		-> DualQuaternion<std::common_type_t<S,T>> const { return {u*rhs.u, u*rhs.v + v*rhs.u}; }
	template<class C>
	auto operator*(C const& c)
		-> DualQuaternion<std::common_type_t<S,C>> const { return {c*u, c*v}; }
	template<class C>
	friend auto operator*(DualQuaternion<C> const& lhs, DualQuaternion<S> const& rhs)
		-> DualQuaternion<std::common_type_t<S,C>>
			{ return {lhs.u*rhs.u, lhs.u*rhs.v + lhs.v*rhs.u}; }
	template<class C>
	friend auto operator*(C const& c, DualQuaternion const& d)
		-> DualQuaternion<std::common_type_t<S,C>> { return {c*d.u, c*d.v}; }
	template<class T>
	auto operator^(DualQuaternion<T> const& rhs)
		-> DualQuaternion<std::common_type_t<S,T>> const
			{ return {u * rhs.u * *u, u * rhs.v * *u}; }
	template<class T>
	auto operator^(Quaternion<T> const& rhs) -> Quaternion<std::common_type_t<S,T>> const
			{ return {u * rhs * *u, v * rhs * *u - u * rhs * *v}; }
	bool operator==(DualQuaternion const& d) const { return u == d.u && v == d.v; }
	DualQuaternion(S p, S q = 0, S r = 0, S s = 0, S w = 0, S x = 0, S y = 0, S z = 0):
		u{p, q, r, s}, v{w, x, y, z} {}
	DualQuaternion(Quaternion<S> u = {0}, Quaternion<S> v = {0}): u(u), v(v) {}

};

template<class L, class R, class T>
auto sclerp(DualQuaternion<L> const& lhs, DualQuaternion<R> const& rhs, T && t)
		-> DualQuaternion<std::common_type_t<L,R,T>> {
	return {slerp(lhs.u, rhs.u, t), lerp(lhs.v, rhs.v, t)};
}

#endif
