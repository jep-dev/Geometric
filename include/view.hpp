#ifndef VIEW_HPP
#define VIEW_HPP

#include <vector>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include <SDL.h>

namespace View {
using namespace gl;

/** Binds and buffers a vertex buffer object. */
template<class T, unsigned N>
void bufferData(GLenum target, GLuint vbo, const T (& data) [N], GLenum usage) {
	glBindBuffer(target, vbo);
	glBufferData(target, sizeof(data), data, usage);
}
template<class T>
void bufferData(GLenum target, GLuint vbo, std::vector<T> const& data, GLenum usage) {
	glBindBuffer(target, vbo);
	glBufferData(target, data.size()*sizeof(data[0]), &data[0], usage);
}

/** Directly draws an array of vertices */
void draw(GLuint vao, GLenum mode, GLint first, GLsizei count) {
	glBindVertexArray(vao);
	glDrawArrays(mode, first, count);
}
/** Directly draws an array of vertices multiple times, varying gl_InstanceID */
void draw(GLuint vao, GLenum mode, GLint first, GLsizei count, GLsizei prims) {
	glBindVertexArray(vao);
	glDrawArraysInstanced(mode, first, count, prims);
}
/** Draws an array of vertices by indices */
void draw(GLuint vao, GLenum mode, GLsizei count, GLenum type, const void *indices) {
	glBindVertexArray(vao);
	glDrawElements(mode, count, type, indices);
}
/** Draws an array of vertices by indices multiple times, varying gl_InstanceID */
void draw(GLuint vao, GLenum mode, GLsizei count,
		GLenum type, const void *indices, GLsizei prims) {
	glBindVertexArray(vao);
	glDrawElementsInstanced(mode, count, type, indices, prims);
}

}

#include "events.hpp"

#endif
