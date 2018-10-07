#ifndef BEZIER_HPP
#define BEZIER_HPP

template<class T, class U>
T linear(T x, T y, U u) {
	return x*(1-u) + y*u;
}
template<class T, class U>
T linear(const T (&x) [2], const T (&y) [2], U u, U v) {
	return linear(linear(x[0], x[1], u), linear(y[0], y[1], u), v);
}
template<class T, class U>
T linear(const T (&x) [2][2], const T (&y) [2][2], U u, U v, U w) {
	return linaer(linear(linear(x[0][0], y[0][0], u), linear(x[0][1], y[0][1], u), v),
		linear(linear(x[1][0], y[1][0], u), linear(x[1][1], y[1][1], u), v), w);
}

template<class T, class U>
T quadratic(T x, T y, T z, U u) {
	return linear(linear(x, y, u), linear(y, z, u), u);
}
template<class T, class U>
T quadratic(const T (&x)[3], const T (&y)[3], const T (&z)[3], U const& u, U const& v) {
	return quadratic(quadratic(x[0], x[1], x[2], u),
			quadratic(y[0], y[1], y[2], u),
			quadratic(z[0], z[1], z[2], u), v);
}

template<class T, class U>
T cubic(T w, T x, T y, T z, U u) {
	return linear(quadratic(w, x, y, u), quadratic(x, y, z, u), u);
}
template<class T, class U>
T cubic(T (&w) [4], T (&x) [4], T (&y) [4], T (&z) [4], U u, U v) {
	return cubic(cubic(w[0], w[1], w[2], w[3], u),
			cubic(x[0], x[1], x[2], x[3], u),
			cubic(y[0], y[1], y[2], y[3], u),
			cubic(z[0], z[1], z[2], z[3], u), v);
}

#endif
