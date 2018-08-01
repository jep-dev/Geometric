#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "view.hpp"

namespace View {
//template<unsigned N, void (*GEN)(GLsizei, const GLuint*), void (*DEL)(GLsizei, const GLuint*)>
template<unsigned N, decltype(glGenBuffers) *GEN, decltype(glDeleteBuffers) *DEL>
struct Resource {
	GLuint data[N];
	GLuint const& operator[](unsigned i) const { return data[i]; }
	GLuint& operator[](unsigned i) { return data[i]; }
	Resource(void) { GEN(N, data); }
	virtual ~Resource(void) { DEL(N, data); }
};

template<unsigned N>
struct VertexArray: Resource<N, &glGenVertexArrays, &glDeleteVertexArrays> {};

template<unsigned N>
struct Buffer: Resource<N, &glGenBuffers, &glDeleteBuffers> {};

}

#endif
