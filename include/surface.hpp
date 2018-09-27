#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <vector>
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include "quaternion.tpp"
#include "dual.tpp"
#include "point.hpp"
#include "math.tpp"
#include "streams.tpp"


template<class S, class T, class RAD = T>
void sphere(std::vector<S> &vertices, std::vector<gl::GLuint> &indices,
		Point<T> center = {}, RAD radius = 1, unsigned M = 100, unsigned N = 100) {
	auto get_angle = [] (unsigned i, unsigned I, T t0, T t1)
			-> T { return t0 + (t1-t0)*i/(I-1); };
	auto get_theta = [=] (unsigned j) -> T { return get_angle(j, N, 0, T(M_PI*2)); };
	auto get_phi = [=] (unsigned i) -> T { return get_angle(i, M, -T(M_PI/2), T(M_PI/2)); };

	for(unsigned j = 0, N2 = N+1; j <= N; j++) {
		auto phi = get_phi(j), cp = cos(phi), sp = sin(phi);
		for(unsigned i = 0, M2 = M+1; i <= M; i++) {
			S theta = get_theta(i), ct = cos(theta), st = sin(theta);
			vertices.emplace_back(center.x + radius * ct * cp);
			vertices.emplace_back(center.y + radius * sp);
			vertices.emplace_back(center.z + radius * st * cp);
			auto row = j*M2;
			if(j) {
				int I = i ? i - 1 : M;
				indices.emplace_back(row+i);
				indices.emplace_back(row+I);
				indices.emplace_back(row+I-M2);
				indices.emplace_back(row+I-M2);
				indices.emplace_back(row+i-M2);
				indices.emplace_back(row+i);
			}
		}
	}

}
template<class S, class T, class U = T, class W = std::common_type_t<S,T,U>>
void cube(std::vector<S> &vertices, std::vector<gl::GLuint> &indices,
		Point<T> center = {0, 0, 0}, U width = 1, bool print = true) {
	using std::cout;
	using std::endl;

	enum { tne=0, tse, tsw, tnw, bne, bse, bsw, bnw, nDirs };
	W scale = width / W(2);
	Point<W> e = {scale, 0, 0},  w = {-scale, 0, 0},
			n = {0, 0, -scale}, s = {0, 0, scale},
			t = {0, scale, 0},  b = {0, -scale, 0},
			c = {W(center.x), W(center.y), W(center.z)},
			points[] = {c, c, c, c, c, c, c, c};
	using Streams::center;
	auto face = [&] (unsigned tr, unsigned tl, unsigned bl, unsigned br, Point<W> const& p) {
		for(auto i : {tr, tl, bl, br}) {
			points[i] += p;
		}
		for(auto i : {tr, tl, bl, tr, bl, br})
			indices.emplace_back(i);
	};
	face(tne, tse, tsw, tnw, t); // Top
	face(bse, bsw, bnw, bne, b); // Bottom
	face(tnw, tne, bne, bnw, n); // North
	face(tse, tsw, bsw, bse, s); // South
	face(tne, tse, bse, bne, e); // East
	face(tsw, tnw, bnw, bsw, w); // West
	for(auto const& p : points) {
		for(auto const& px : {p.x, p.y, p.z})
			vertices.emplace_back(S(px));
	}
	{
		cout << std::right;
		auto verticesSize = vertices.size();
		for(unsigned i = 0; i < verticesSize; i+=3) {
			Point<gl::GLfloat> pt = {vertices[i], vertices[i+1], vertices[i+2]};
			cout << to_string(pt, 2) << endl;
		}
		cout << std::left;
	}
}
template<class S, class T>
void cube(std::vector<S> &vertices, std::vector<gl::GLuint> &indices,
		DualQuaternion<T> const& transform, T scale = 1) {
	enum { tne=0, tse, tsw, tnw, bne, bse, bsw, bnw };

	scale /= 2;
	DualQuaternion<T> north = 1_e + scale * 1_J, east = 1_e + scale * 1_I,
			south = 1_e - scale * 1_J, west = 1_e - scale * 1_I,
			top = 1_e + scale * 1_K, bottom = 1_e - scale * 1_K;

	for(auto p : {top * north * east, top * south * east,
			top * south * west, top * north * west,
			bottom * north * east, bottom * south * east,
			bottom * south * west, bottom * north * west}) {
		auto tp = transform * p * *transform;
		vertices.emplace_back(T(tp.x));
		vertices.emplace_back(T(tp.y));
		vertices.emplace_back(T(tp.z));
		//if(lim > 0) {
			std::cout << tp.x << "  " << tp.y << "  " << tp.z << std::endl;
			//lim--;
		//}
	}
	for(gl::GLuint i : {
			tse, tsw, bsw, tse, bsw, bse, // south/front
			tne, tse, bse, tse, bse, bne, // east/right
			tne, bne, bnw, tne, bnw, tnw, // north/back
			tnw, bnw, bsw, tnw, bsw, tsw, // west/left
			tne, tnw, tsw, tne, tsw, tse, // top
			bse, bsw, bnw, bse, bnw, bne, // bottom
		}) indices.emplace_back(i);
}

template<class S, class T, class U>
void sheet(std::vector<S> & vertices, std::vector<T> & indices,
	Point<U> a, Point<U> b, Point<U> c, Point<U> d, unsigned M = 100, unsigned N = 100) {
	for(auto i = 0; i <= M; i++) {
		U s = U(i)/M;
		for(auto j = 0; j <= N; j++) {
			U t = U(j)/N;
			auto u = (1-t)*((1-s)*a+s*b) + t*(s*c+(1-s)*d);
			vertices.emplace_back(S(u.x));
			vertices.emplace_back(S(u.y));
			vertices.emplace_back(S(u.z));
			if(i && j) {
				auto index = (i-1)*(N+1)+j-1;
				indices.emplace_back(index);
				indices.emplace_back(index+1);
				indices.emplace_back(index+N+2);
				indices.emplace_back(index+N+2);
				indices.emplace_back(index+N+1);
				indices.emplace_back(index);
			}
		}
	}
}


#endif
