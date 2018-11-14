#ifndef BEZIER_HPP
#define BEZIER_HPP

#include <utility>
#include <vector>

#include <functional> // TODO remove, used in poly-looping

#include "utility.hpp"
#include "sequence.hpp"

template<class T, class U>
T linear(T x, T y, U u) {
	return x*(1-u) + y*u;
}
template<class T, class U>
T linear(T x1, T x2, T y1, T y2, U s, U t) {
	return linear(linear(x1, x2, s), linear(y1, y2, s), t);
}
template<class T, class U>
T linear(const T (&x) [2], const T (&y) [2], U u, U v) {
	return linear(linear(x[0], x[1], u), linear(y[0], y[1], u), v);
}
template<class T, class U>
T linear(const T (&x) [2][2], const T (&y) [2][2], U u, U v, U w) {
	return linear(linear(linear(x[0][0], y[0][0], u), linear(x[0][1], y[0][1], u), v),
		linear(linear(x[1][0], y[1][0], u), linear(x[1][1], y[1][1], u), v), w);
}

template<class T, class U>
T quadratic(T x, T y, T z, U u) {
	return linear(linear(x, y, u), linear(y, z, u), u);
}
template<class T, class U>
T quadratic(T nw, T n, T ne, T w, T c, T e, T sw, T s, T se, U const& u, U const& v) {
	auto nnw = linear(nw, n, u), nne = linear(n, ne, u), north = linear(nnw, nne, u),
		wc = linear(w, c, u), ce = linear(c, e, u), center = linear(wc, ce, u),
		ssw = linear(sw, s, u), sse = linear(s, se, u), south = linear(ssw, sse, u),
		north_center = linear(north, center, v), center_south = linear(center, south, v);
	return linear(north_center, center_south, v);
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

/*
template<class S, class T, unsigned N>
S getNormalized(S const& s, T const (&bounds) [N]) {
	static_assert(N > 0, "Cannot normalize without at least an upper bound!");
	if(N > 1) return s >= bounds[0] && s < bounds[1];
	return s < bounds[0];
}
// In-place array normalization
template<class S, class T, unsigned N>
auto getNormalized(S (&params) [N], T const (&minima) [N], T const (&maxima) [N])
	-> decltype(params) { return getNormalized(params, minima, maxima, params); }

// Array normalization (not in-place)
template<class S, class T, class U, unsigned N, class R = S>
void getNormalized(R (&out) [N], S const (&in) [N], T const (&minima) [N], U const (&maxima) [N]) {
	typedef std::common_type_t<T, U> TU;
	typedef std::common_type_t<S, T> ST;
	typedef std::common_type_t<S, TU> STU;

	TU widths[N] = {0};
	std::transform(minima, minima+N, maxima, widths, std::minus<TU>());
	std::transform(params, params+N, minima, out, std::minus<ST>());
	std::transform(out, out+N, widths, out, std::divides<T>());
}

template<class S, class MAXIMA, class MINIMA>
struct PolyForEach;

template<class S, class T, T... MINIMA, T... MAXIMA>
struct PolyForEach<S, Detail::Seq<T, MINIMA...>, Detail::Seq<T, MAXIMA...>> {
	static constexpr unsigned NMAXIMA = sizeof...(MAXIMA), NMINIMA = sizeof...(MINIM),
			RANK = NMAXIMA;
	static_assert(NMAXIMA >= NMINIMA, "Only minima can be implicit (default to 0)");
	static constexpr unsigned maxima[RANK] = {MAXIMA...}, minima[RANK] = {MINIMA...};

	// Requires NMAXIMA == NMINIMA:
	//   static constexpr unsigned bounds[RANK][2] = {{MINIMA, MAXIMA}...};

	typedef unsigned indices_type [RANK];
	typedef S params_type [RANK];

	typedef void (*fnAct_type) (params_type & params);
	template<class R> struct Accumulate {
		typedef R (*fnAccumType) (R const&, R const&);
	};
};*/

/*template<class S, class U = unsigned, unsigned BOUNDS,
		class VS = std::vector<S>, class VU = std::vector<U>, class... TN>
VS polyLoop(S (*fn)(VS const&), VS (*norm) (VU const&, VU const&, TN &&...),
		VU const (&bounds) [BOUNDS], TN &&... tn) {
	VU params = {0};
	VS out;
	for(unsigned cursor = 0, N = bounds.size(); cursor < N;) {
		params[cursor]++;
		if(params[cursor] < bounds[cursor]) {
			cursor = 0;
		} else {
			for(unsigned i = 0; i < cursor; i++) params[i] = 0;
			for(; cursor < N; cursor++) {
				params[cursor]++;
				if(params[cursor] < bounds[cursor]) {
					cursor = 0;
					break;
				}
				params[cursor] = 0;
			}
			if(cursor) break;
			//out.emplace_back(fn(norm(params, bounds)));
			// auto normed = normalizeAll<S>(params, bounds);
			out.emplace_back(fn(params));
		}
	}
	return out;
}*/

// This was the beginning of an attempt to consolidate linear/quadratic/etc.
// The templates above can boil down to a single expression, but here, I can't
// seem to get around using dynamic sizes (e.g. binomial coefficients in a vector.)
/*template<unsigned DEG, class T, class... V>
T bezier(T const& t, V &&...) { return t; }
template<unsigned DEG, class T, unsigned W, class U, class...V,
		class C = std::remove_all_extents_t<T>>
C& bezier(T const (&t)[W], U const& u, V &&... v) {
	C out = {0};
	Binomial bin;
	auto coef = bin[DEG];
	for(unsigned i = 0; i < W; i++) {
		out += coef[i]
	}
}*/

#endif
