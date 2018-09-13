#ifndef POINT_HPP
#define POINT_HPP

#include "math.hpp"

template<class S>
struct Point {
	S x = 0, y = 0, z = 0;
	template<class T, class ST = std::common_type_t<S,T>>
	Point<ST> operator+(Point<T> const& p) const { return {x+p.x, y+p.y, z+p.z}; }
	template<class T>
	Point<S>& operator+=(Point<T> const& p)
		{ return x += S(p.x), y += S(p.y), z += S(p.z), *this; }
	template<class T, class ST = std::common_type_t<S,T>>
	friend Point<ST> operator*(T const& t, Point<S> const& p)
		{ return {t*p.x, t*p.y, t*p.z}; }
	template<class T, class ST = std::common_type_t<S,T>>
	Point<ST> operator*(T const& t)
		{ return {x*t, y*t, z*t}; }

	template<class T, class ST = std::common_type_t<S,T>>
	Point<ST> operator-(Point<T> const& p) const { return {x-p.x, y-p.y, z-p.z}; }
	template<class T>
	Point<S>& operator-=(Point<T> const& p)
		{ return x -= S(p.x), y -= S(p.y), z -= S(p.z), *this; }

	template<class T, class ST = std::common_type_t<S,T>>
	Point<ST> operator^(Quaternion<T> const& t) const {
		ST p = -t.x*x - t.y*y - t.z*z, q = t.w*x + t.y*z - t.z*y,
			r = t.w*y - t.x*z + t.z*x, s = t.w*z + t.x*y - t.y*x;
		return {
			-p*t.x + q*t.w - r*t.z + s*t.y,
			-p*t.y + q*t.z + r*t.w - s*t.x,
			-p*t.z - q*t.y + r*t.x + s*t.w
		};
	}
	template<class T, class ST = std::common_type_t<S,T>>
	Point<ST> operator^(DualQuaternion<T> const& t) const {
		ST p = -t.t*x - t.u*y - t.v*z, q = t.s*x + t.u*z - t.v*y,
			r = t.s*y - t.t*z + t.v*x, s = t.s*z + t.t*y - t.u*x;
		return {
			-p*t.t + q*t.s - r*t.v + s*t.u
				+ 2*(-t.w*x + t.x*t.s),
			-p*t.u + q*t.v + r*t.s - s*t.t
				+ 2*(-t.w*y + t.y*t.s),
			-p*t.v - q*t.u + r*t.t + s*t.s
				+ 2*(-t.w*z + t.z*t.s)
		};
	}

	friend struct DualQuaternion<S>;
	operator DualQuaternion<S>(void) const { return {1, 0, 0, 0, 0, x, y, z}; }
	template<class T>
	friend T& operator<<(T &t, Point const& p) { return t << to_string(p), t; }
		//{ return t << (DualQuaternion<S>) p, t; }
	operator std::string(void) const { return std::string((DualQuaternion<S>) *this); }
};
template<class S, class T, class ST = std::common_type_t<S,T>>
ST dot(Point<S> const& s, Point<T> const& t)
	{ return ST(s.x*t.x + s.y*t.y + s.z*t.z); }
template<class S, class T, class ST = std::common_type_t<S,T>>
Point<ST> cross(Point<S> const& s, Point<T> const& t)
	{ return {s.y*t.z-s.z*t.y, s.z*t.x-s.x*t.z, s.x*t.y-s.y*t.x}; }

/*template<class S>
std::string to_string(Point<S> const& p)
	{ return "<" + to_string(p.x) + ", " + to_string(p.y) + ", " + to_string(p.z) + ">"; }*/
template<class S, class DELIM>
std::string to_string(Point<S> const& p, unsigned prec, DELIM delim) {
	return "(" + to_string(p.x, prec, delim) + ", " + to_string(p.y, prec, delim)
			+ ", " + to_string(p.z, prec, delim) + ")";
}

#endif
