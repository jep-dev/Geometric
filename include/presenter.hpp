#ifndef PRESENTER_HPP
#define PRESENTER_HPP

#include <map>
#include <string>

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

	std::map<std::string, View::Shader> shaders;
	// std::map<std::string, bool> sourced, compiled, attached;
	// View::Shader vert = {gl::GL_VERTEX_SHADER}, frag = {gl::GL_FRAGMENT_SHADER};
	View::Program program;

	/** Initializer given both a type and a path; source/compile/attach and recurse on success */
	template<class... T>
	Events::Status init(const gl::GLenum type, std::string const& fpath, T &&... t) {
		auto emplaced = shaders.emplace(fpath, type);
		auto & cur = emplaced.first -> second;
		// Neither 'source' nor 'compile' uses force=true, so prior success is honored
		if(!cur.source(fpath.c_str()) || !cur.compile()) return cur.status;
		if(!program.attach(cur)) return program.status;
		return init(type, std::forward<T>(t)...);
	}

	// Initializer given a path without a type; provide a type and recurse
	/*template<class... T>
	Events::Status init(std::string const& fpath, T &&... t) {
		return init(gl::GL_VERTEX_SHADER, fpath, std::forward<T>(t)...);
	}*/

	/** Initializer in transition from one shader type to another; discard type1 and recurse */
	template<class... T>
	Events::Status init(const gl::GLenum type1, const gl::GLenum type2, T &&... t) {
		return init(type2, std::forward<T>(t)...);
	}
	/** Default initializer - links and uses the program as-is */
	Events::Status init(const gl::GLenum type = gl::GL_VERTEX_SHADER) {
		if(!program.link()) return program.status;
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
