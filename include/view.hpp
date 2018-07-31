#ifndef VIEW_HPP
#define VIEW_HPP

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include <SDL.h>

namespace View {
using namespace gl;

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
