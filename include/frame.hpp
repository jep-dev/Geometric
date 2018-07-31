#ifndef FRAME_HPP
#define FRAME_HPP

#include "view.hpp"
#include "context.hpp"

namespace View {

/** Represents a window and associated GL context. */
struct Frame {
public:
	int flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	Events::Status status;
protected:
	static constexpr int center = SDL_WINDOWPOS_CENTERED;
	SDL_Window *win = 0;
	Context ctx;
public:
	/** Stream insertion operator; inserts the most recent message(s)/error(s). */
	template<class S>
	friend S& operator<<(S &s, Frame const& f) {
		return s << f.status, s;
	}
	Frame& clear(void);
	/** Directly draws an array of vertices */
	Frame& draw(GLuint vao, GLenum mode, GLint first, GLsizei count) {
		glBindVertexArray(vao);
		glDrawArrays(mode, first, count);
		return *this;
	}
	/** Directly draws an array of vertices multiple times, varying gl_InstanceID */
	Frame& draw(GLuint vao, GLenum mode, GLint first, GLsizei count, GLsizei prims) {
		glBindVertexArray(vao);
		glDrawArraysInstanced(mode, first, count, prims);
		return *this;
	}
	/** Draws an array of vertices by indices */
	Frame& draw(GLuint vao, GLenum mode, GLsizei count, GLenum type, const void *indices) {
		glBindVertexArray(vao);
		glDrawElements(mode, count, type, indices);
		return *this;
	}
	/** Draws an array of vertices by indices multiple times, varying gl_InstanceID */
	Frame& draw(GLuint vao, GLenum mode, GLsizei count,
			GLenum type, const void *indices, GLsizei prims) {
		glBindVertexArray(vao);
		glDrawElementsInstanced(mode, count, type, indices, prims);
		return *this;
	}
	Frame& flip(void);
	operator SDL_GLContext(void) const { return ctx; }
	operator SDL_Window *const (void) const { return win; }
	/** Constructor; currently initializes and sets a handful of hard-coded GL attributes */
	Frame(void);
	/** Destructor; destroys the stored window. */
	virtual ~Frame(void) {
		if(win)
			SDL_DestroyWindow(win);
		win = 0;
	}
};

}

#endif
