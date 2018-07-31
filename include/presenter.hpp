#ifndef PRESENTER_HPP
#define PRESENTER_HPP

#include "frame.hpp"
#include "model.hpp"
#include "shader.hpp"

template<class S>
struct Presenter;

template<class S>
struct Presenter: Events::Handler<S> {
	using Events::Handler<S>::operator();
	View::Frame frame;
	Model model;

	View::Shader vert = {gl::GL_VERTEX_SHADER}, frag = {gl::GL_FRAGMENT_SHADER};
	View::Program program;

	Events::Status init(const char *vpath, const char *fpath) {
		if(!vert.source(vpath) || !vert.compile() || !program.attach(vert))
			return vert.status;
		if(!frag.source(fpath) || !frag.compile() || !program.attach(frag))
			return frag.status;
		if(!program.link())
			return program.status;
		use(program);
		return {};
	}
	S& use(gl::GLuint program) {
		gl::glUseProgram(program);
		return static_cast<S&>(*this);
	}
	gl::GLfloat projection[16] = {0};
	S& project(gl::GLuint dest, float right, float top, float near, float far) {
		using namespace View;
		float mx = near / right, mz = near / top, mwy = -1,
			dy = near - far,
			myy = (far + near) / dy,
			myw = 2 * near * far / dy;
		projection[0] = mx;
		projection[6] = -myy;
		projection[7] = -mwy;
		projection[9] = mz;
		projection[14] = myw;
		glUniformMatrix4fv(0, 1, GL_FALSE, projection);
		return static_cast<S&>(*this);
	}

	template<class... T>
	Presenter(T &&... t): frame(std::forward<T>(t)...) {}
};

#endif
