#ifndef SURFACE_HPP
#define SURFACE_HPP

///@cond
#include <vector>
///@endcond

#include "quaternion.hpp" // Quaternion
#include "dual.hpp" // Dual
#include "point.hpp" // Point
#include "math.tpp" // Point*Dual

template<class R>
R& emplace_vertices(R &vertices) { return vertices; }

template<class R, class S, class... T>
std::enable_if_t<std::is_arithmetic<S>::value, R&>
emplace_vertices(R & vertices, S const& s, T &&... t) {
	typedef typename R::value_type V;
	vertices.emplace_back(V(s));
	return emplace_vertices(vertices, std::forward<T>(t)...);
}
template<class R, template<class> class C, class S, class... T>
R& emplace_vertices(R & vertices, C<S> const& s, T &&... t) {
	typedef typename R::value_type V;
	vertices.emplace_back(V(s.x));
	vertices.emplace_back(V(s.y));
	vertices.emplace_back(V(s.z));
	return emplace_vertices(vertices, std::forward<T>(t)...);
}

template<class R>
R& emplace_indices(R & indices) { return indices; }
template<class R, class S, class... T>
R& emplace_indices(R & indices, S const& s, T &&... t) {
	indices.emplace_back(s);
	return emplace_indices(indices, std::forward<T>(t)...);
}

template<class S, class T, class U, class R = U,
		template<class...> class V, class... VT,
		template<class...> class W, class... WT>
unsigned sphere(V<S,VT...> &vertices, W<T,WT...> &indices, Point<U> center = {},
		R radius = 1, unsigned M = 100, unsigned N = 100, unsigned offset = 0, bool uv = true) {
	using namespace std;
	auto x = radius * 1_x;
	for(unsigned i = 0, I = offset; i < M; i++, I += N) {
		S s = S(i)/(M-1), phi = -M_PI/2 + M_PI*s;
		auto y = x ^ rotation(phi, 0, 0, 1);
		for(unsigned j = 0; j < N; j++) {
			S t = S(j)/(N-1), theta = 2*M_PI*t;
			auto z = center + (x ^ rotation(phi, 0, 0, 1) ^ rotation(theta, 0, 1, 0));
			emplace_vertices(vertices, z);
			if(uv) emplace_vertices(vertices, s, t);
			if(i && j) {
				auto index = I, J = (j+M-1) % N;
				emplace_indices(indices,
					index+J, index+J-N, index+j-N,
					index+j-N, index+j, index+J);
			}
		}
	}
	return indices.size();
}
template<class S, class T, class U = S, class W = U,
		template<class...> class VERT, class... VERTN,
		template<class...> class IND, class... INDN>
unsigned cube(VERT<S, VERTN...> &vertices, IND<T, INDN...> &indices,
		Point<U> center = {0, 0, 0}, W width = 1, unsigned offset = 0, bool uv = true) {
	using std::cout;
	using std::endl;

	enum { tne=0, tse, tsw, tnw, bne, bse, bsw, bnw, nDirs };
	S scale = S(width)/2;
	Point<S> e = {scale, 0, 0},  w = {-scale, 0, 0},
			n = {0, 0, -scale}, s = {0, 0, scale},
			t = {0, scale, 0},  b = {0, -scale, 0},
			c = {center.x, center.y, center.z},
			points[] = {c, c, c, c, c, c, c, c};
	auto face = [&] (unsigned tr, unsigned tl, unsigned bl, unsigned br, Point<S> const& p) {
		for(auto i : {tr, tl, bl, br})
			points[i] += p;
		for(auto i : {tr, tl, bl, tr, bl, br})
			indices.emplace_back(i+offset);
	};
	face(tne, tse, tsw, tnw, t); // Top
	face(bse, bsw, bnw, bne, b); // Bottom
	face(tnw, tne, bne, bnw, n); // North
	face(tse, tsw, bsw, bse, s); // South
	face(tne, tse, bse, bne, e); // East
	face(tsw, tnw, bnw, bsw, w); // West
	for(auto const& p : points) {
		emplace_vertices(vertices, p);
		if(uv) emplace_vertices(vertices, 0, 0);
	}
	return indices.size();
}

template<class S, class T, class U, class R>
unsigned cylinder(std::vector<S> & vertices, std::vector<T> & indices,
	Point<U> const& p0, Point<U> const& p1, R && radius = 1,
	unsigned M = 100, unsigned N = 100, unsigned offset = 0) {
	typedef Point<U> Pt;
	Pt p10 = p1 - p0, n10 = p10.normalize();
	std::pair<Pt, Pt> uv = perpendicular(p10, true);
	auto const& u = uv.first, v = uv.second;
	for(unsigned i = 0, I = offset; i < M; i++, I += N) {
		U s = U(i)/(M-1);
		for(unsigned j = 0, J = 1; j < N; j++, J = j+1 /*(j+1) % N*/) {
			U t = U(j)/(N-1);
			Pt w = p0 + s * p10 + ((radius * t * u) ^ rotation<U>(M_PI*2*t, n10.x, n10.y, n10.z));
			emplace_vertices(vertices, w, s, t);
			if(i && j < N)
					emplace_indices(indices, I+J, I+j, I-N+j, I-N+j, I-N+J, I+J);
		}
	}
	return indices.size();
}

template<class S, class T, class U,
	template<class...> class V, class... VT, template<class...> class W, class... WT>
unsigned surface(V<S,VT...> & vertices, W<T,WT...> & indices,
		DualQuaternion<U> const& nw, DualQuaternion<U> const& ne,
		DualQuaternion<U> const& sw, DualQuaternion<U> const& se,
		Point<U> const& p, Point<U> const& center,
		unsigned M = 100, unsigned N = 100, unsigned offset = 0) {
	typedef DualQuaternion<U> Dq;
	typedef Point<U> Pt;
	for(unsigned i = 0, I = offset; i < M; i++, I += N) {
		U s = U(i)/(M-1);
		for(unsigned j = 0; j < N; j++) {
			U t = U(j)/(N-1);
			Dq xform = sclerp(nw, ne, sw, se, s, t);
			auto p2 = (p ^ xform) + center;
			emplace_vertices(vertices, p2, s, t);
			if(i && j)
				emplace_indices(indices, I+j, I-N+j-1, I+j-1, I-N+j-1, I+j, I-N+j);
		}
	}
	return indices.size();
}

template<class S, class T, class U = S, class R = S,
	class DQ = DualQuaternion<U>, class PT = DualQuaternion<U>,
	template<class...> class V, class... VT,
	template<class...> class W, class... WT>
unsigned rope(V<S,VT...> & vertices, W<T,WT...> & indices,
		DQ const& u, DQ const& v, PT const& p, PT const& center, R && radius = 1,
		unsigned M = 100, unsigned N = 100, unsigned offset = 0) {
	typedef DualQuaternion<U> Dq;
	typedef Point<U> Pt;
	Pt prev_pt = center + (p ^ u), next_pt = center + (p ^ v);
	cylinder(vertices, indices, prev_pt, next_pt, radius, M, N, offset);

	/*Dq prev;
	for(unsigned i = 0, M2 = 10; i < M2; i++) {
		Dq cur = sclerp(u, v, U(i)/(M2-1));
		//Dq cur = u + (v-u) * U(i)/(M2-1);

		Pt x = center + (p ^ cur); //w = (prev_pt - x).normalize();
		cylinder(vertices, indices, prev_pt, x, radius, M/M2, N, indices.size());
	}*/
	return indices.size();
}

#endif
