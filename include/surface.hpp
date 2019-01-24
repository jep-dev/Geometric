#ifndef SURFACE_HPP
#define SURFACE_HPP

///@cond
#include <vector>
///@endcond

#include "bezier.hpp"
#include "quaternion.hpp" // Quaternion
#include "dual.hpp" // Dual
#include "point.hpp" // Point
#include "math.tpp" // Point*Dual

template<class R>
R& emplace_vertices(R &vertices) { return vertices; }

template<class R, class S, class... T>
std::enable_if_t<std::is_arithmetic<S>::value, R&>
emplace_vertices(R & vertices, S s, T &&... t) {
	vertices.emplace_back(s);
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
R& emplace_indices(R & indices, S && s, T &&... t) {
	indices.emplace_back(std::forward<S>(s));
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
	return offset += M*N;
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
	return offset; // TODO
}

template<class R, class S, class T, class U,
	template<class...> class V, class... VT, template<class...> class W, class... WT>
unsigned cylinder(V<S,VT...> & vertices, W<T,WT...> & indices,
	Point<U> const& p0, Point<U> const& p1, R && radius = 1,
	unsigned M = 100, unsigned N = 100, unsigned offset = 0, bool uv = true) {
	typedef Point<U> Pt;
	Pt p10 = p1 - p0, n10 = p10.normalize();
	std::pair<Pt, Pt> perp = perpendicular(p10, true);
	auto const& u = perp.first, v = perp.second;
	for(unsigned i = 0, I = offset; i < M; i++, I += N) {
		U s = U(i)/(M-1);
		for(unsigned j = 0, J = 1; j < N; j++, J = /*j+1*/ (j+1) % N) {
			U t = U(j)/(N-1);
			Pt w = p0 + s * p10 + ((radius * t * u) ^ rotation<U>(M_PI*2*t, n10.x, n10.y, n10.z));
			emplace_vertices(vertices, w);
			if(uv) emplace_vertices(vertices, s, t);
			if(i && j < N)
					emplace_indices(indices, I+J, I+j, I-N+j, I-N+j, I-N+J, I+J);
		}
	}
	return offset + M*N;
}

template<class S, class T, class U,
	template<class...> class V, class... VT, template<class...> class W, class... WT>
unsigned surface(V<S,VT...> & vertices, W<T,WT...> & indices,
		DualQuaternion<U> const& nw, DualQuaternion<U> const& ne,
		DualQuaternion<U> const& sw, DualQuaternion<U> const& se,
		Point<U> const& p, Point<U> const& center,
		unsigned M = 100, unsigned N = 100, unsigned offset = 0, bool uv = true) {
	typedef DualQuaternion<U> Dq;
	typedef Point<U> Pt;
	for(unsigned i = 0, I = offset; i < M; i++, I += N) {
		U s = U(i)/(M-1);
		for(unsigned j = 0; j < N; j++) {
			U t = U(j)/(N-1);
			Dq xform = sclerp(nw, ne, sw, se, s, t);
			auto p2 = (p ^ xform) + center;
			emplace_vertices(vertices, p2);
			if(uv) emplace_vertices(vertices, s, t);
			if(i && j) {
				emplace_indices(indices, I+j, I-N+j-1, I+j-1, I-N+j-1, I+j, I-N+j);
			}
		}
	}
	return offset + M*N;
}

template<class V, class W, class U>
unsigned surface2(V & vertices, W & indices,
		const DualQuaternion<U> (&rowMajor)[3][3], Point<U> const& p, Point<U> const& center,
		unsigned M = 100, unsigned N = 100, unsigned offset = 0, bool uv = true) {
	typedef DualQuaternion<U> Dq;
	typedef Point<U> Pt;
	auto const& r0 = rowMajor[0], r1 = rowMajor[1], r2 = rowMajor[2];
	for(unsigned i = 0, I = offset; i < M; i++, I += N) {
		U s = U(i)/(M-1);
		for(unsigned j = 0; j < N; j++) {
			U t = U(j)/(N-1);
			//auto p2 = center + (r0[0]*(1-s) + r0[1]*s) * (1-t) + (r1[0]*(1-s) + r1[1]*s) * t;
			//auto p2 = center + linear(linear(r0[0], r0[1], s), linear(r1[0], r1[1], s), t);
			//auto p2 = center + linear(r0[0], r0[1], r1[0], r1[1], s, t);
			//auto p2 = center + quadratic(r0[0], r0[1], r0[2], r1[0], r1[1], r1[2],
					//r[2][0], r[2][1], r[2][2], s, t);
			//auto p2 = center + (p ^ linear(r0[0], r0[1], r0[2], r1[0], r1[1], r1[2], r2[0], r2[1], r2[2], s, t));
			auto p2 = center + (p ^ sclerp(
				sclerp(r0[0], r0[1], r1[0], r1[1], s, t),
				sclerp(r0[1], r0[2], r1[1], r1[2], s, t),
				sclerp(r1[0], r1[1], r2[0], r2[1], s, t),
				sclerp(r1[1], r1[2], r2[1], r2[2], s, t), s, t));

			emplace_vertices(vertices, p2);
			if(uv) emplace_vertices(vertices, s, t);
			if(i && j) {
				emplace_indices(indices, I-N+j-1, I+j, I+j-1, I-N+j-1, I-N+j, I+j);
			}
		}
	}
	return offset + M*N;
}
template<class V, class W, class U>
unsigned surface3(V & vertices, W & indices,
		Point<U> const (&rowMajor)[3][3], Point<U> const& center,
		unsigned M = 100, unsigned N = 100, unsigned offset = 0, bool uv = true) {
	typedef DualQuaternion<U> Dq;
	typedef Point<U> Pt;
	auto const& r0 = rowMajor[0], r1 = rowMajor[1], r2 = rowMajor[2];
	for(unsigned i = 0, I = offset; i < M; i++, I += N) {
		U s = U(i)/(M-1), s2 = s*s, S = 1-s, S2 = S*S, Ss = S*s;
		for(unsigned j = 0; j < N; j++) {
			U t = U(j)/(N-1), t2 = t*t, T = 1-t, T2 = T*T, Tt = T*t;

			auto pt = center +
				(((r0[0]*S + r0[1]*s)*S + r0[2]*s2)*T
				+((r1[0]*S + r1[1]*s)*S + r1[2]*s2)*2*t)*T
				+((r2[0]*S + r2[1]*s)*S + r2[2]*s2)*t*t;

			emplace_vertices(vertices, pt);
			if(uv) emplace_vertices(vertices, s, t);
			if(i && j) {
				emplace_indices(indices, I-N+j-1, I+j, I+j-1, I-N+j-1, I-N+j, I+j);
			}
		}
	}
	return offset + M*N;
}

template<class V, class W, class U, class FN>
unsigned field(V& vertices, W& indices, FN xform,
		Point<U> u0, Point<U> du, Point<U> dv, Point<U> center = {0, 0, 0},
		unsigned wmesh = 100, unsigned hmesh = 100, unsigned offset = 0, bool uv = true) {
	for(unsigned i = 0; i < hmesh; i++) {
		Point<U> v = u0 + U(i)*dv/hmesh;
		for(unsigned j = 0; j < wmesh; j++) {
			Point<U> u = v + U(j)*du/wmesh;
			emplace_vertices(vertices, center ^ xform(u.x, u.y, u.z));
			if(uv) emplace_vertices(vertices, u.x, u.y);
		}
	}
	for(unsigned i = 0; i < hmesh; i++) {
		for(unsigned j = 0; j < wmesh; j++) {
			auto index = offset + i*wmesh + j;
			emplace_indices(indices, index, index+1, index+1+wmesh,
					index+1+wmesh, index+wmesh, index);
		}
	}
	return offset + hmesh * wmesh;
}
template<class V, class W, class U, class FN>
unsigned fieldCube(V& vertices, W& indices, FN xform,
		Point<U> u0, Point<U> du, Point<U> dv, Point<U> dw, Point<U> center = {0, 0, 0},
		unsigned wmesh = 100, unsigned hmesh = 100, unsigned offset = 0, bool uv = true) {
	typedef Point<U> Pt;
	Pt north[] = {u0+du+dv, -du, dw}, east[] = {u0+du, dv, dw}, south[] = {u0, du, dw},
		west[] = {u0+dv, -dv, dw}, top[] = {u0+dw, du, dv}, bottom[] = {u0+dv, du, -dv};
	offset = field(vertices, indices, xform, north[0], north[1], north[2],
			center, wmesh, hmesh, offset, uv);
	offset = field(vertices, indices, xform, east[0], east[1], east[2],
			center, wmesh, hmesh, offset, uv);
	offset = field(vertices, indices, xform, south[0], south[1], south[2],
			center, wmesh, hmesh, offset, uv);
	offset = field(vertices, indices, xform, west[0], west[1], west[2],
			center, wmesh, hmesh, offset, uv);
	offset = field(vertices, indices, xform, top[0], top[1], top[2],
			center, wmesh, hmesh, offset, uv);
	offset = field(vertices, indices, xform, bottom[0], bottom[1], bottom[2],
			center, wmesh, hmesh, offset, uv);
	return offset;
}

#endif
