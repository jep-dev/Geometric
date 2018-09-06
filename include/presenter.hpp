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
	Model<float> model;

	View::ShaderTable shaders;
	View::Program program;

	std::map<std::string, gl::GLint> locations;
	S& locate(void) { return static_cast<S&>(*this); }
	template<class U, class... V>
	S& locate(U const& u, V const&... v) {
		if((locations[u] = program.locate(u)) < 0)
			locations.erase(u);
		return locate(v...);
	}

	// Initializer given a path without a type; provide a type and recurse
	template<class... T>
	Events::Status init(std::string const& fpath, T &&... t)
		{ return init(gl::GL_VERTEX_SHADER, fpath, std::forward<T>(t)...); }
	/** Initializer in transition from one shader type to another; discard type1 and recurse */
	template<class... T>
	Events::Status init(const gl::GLenum type1, const gl::GLenum type2, T &&... t)
		{ return init(type2, std::forward<T>(t)...); }

	/** Initializer given both a type and a path; source/compile/attach and recurse on success */
	template<class... T>
	Events::Status init(const gl::GLenum type, std::string const& fpath, T &&... t) {
		auto emplaced = shaders.objects.emplace(fpath, type);
		auto & cur = emplaced.first -> second;
		// Neither 'source' nor 'compile' uses force=true, so prior success is honored
		if(!cur.source(fpath.c_str()) || !cur.compile()) return cur.status;
		if(!program.attach(cur)) return program.status;
		return init(type, std::forward<T>(t)...);
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
	template<class T>
	S& set_model(DualQuaternion<T> const& m) {
		locate("model[0]", "model[1]");
		gl::glUniform4f(locations["model[0]"], m.s, m.t, m.u, m.v);
		gl::glUniform4f(locations["model[1]"], m.w, m.x, m.y, m.z);
		return static_cast<S&>(*this);
	}
	//gl::GLfloat projection[16] = {0};
	S& project(float l, float r, float b, float t, float n, float f) {
		using namespace View;

		// TODO Why the FUCK are uniforms/constants/locals behaving differently
		// Uniforms init and change correctly, but used in expressions, produce
		// a black screen or even unshaded (white) geometry
		// Disabling for now

		// Optimized method - decompose projection matrix into one multiply-add
		/*locate("projection[0]", "projection[1]");
		// Multiply by {x, y, w, 0}
		glUniform4f(locations["projection[0]"], 2*n/r, 2*n/t, 2*n*f/(n-f), 0);
		// Multiply by z
		glUniform4f(locations["projection[1]"], (r+l)/(r-l), (t+b)/(t-b), (n+f)/(n-f), -1);*/

		// Traditional method - projection matrix is opaque to shader
		/*locate("projection");
		GLfloat values[] = {
				2*n/r, 0, (r+l)/(r-l), 0,
				0, 2*n/t, (t+b)/(t-b), 0,
				0,     0, (n+f)/(n-f), 0, //2*n*f/(n-f),
				0,     0,          -1, 0};
		glUniformMatrix4fv(locations["projection"], 1, GL_FALSE, values);*/

		return static_cast<S&>(*this);
	}

	template<class... T>
	Presenter(T &&... t): frame(std::forward<T>(t)...) {}
};

#endif
