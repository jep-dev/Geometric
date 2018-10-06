#ifndef PRESENTER_HPP
#define PRESENTER_HPP

#include <map>

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
	float right = 5, left = -right, top = 5, bottom = -top,
			near = 1, far = 10;

	std::map<std::string, gl::GLint> locations;

	S& locate(void) { return static_cast<S&>(*this); }
	template<class U, class... V>
	S& locate(U const& u, V const&... v) {
		locations[u] = program.locate(u);
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
	Events::Status init(gl::GLenum type = gl::GL_VERTEX_SHADER) {
		if(!program.link()) return program.status;
		use();
		return {};
	}

	S& use(void) {
		gl::glUseProgram(program);
		return static_cast<S&>(*this);
	}
	template<class T>
	S& set_view(DualQuaternion<T> const& v) {
		locate("view.u", "view.v");
		gl::glUniform4f(locations["view.u"], v.s, v.t, v.u, v.v);
		gl::glUniform4f(locations["view.v"], v.w, v.x, v.y, v.z);
		return static_cast<S&>(*this);
	}
	template<class T>
	S& set_model(DualQuaternion<T> const& m) {
		locate("model.u", "model.v");
		gl::glUniform4f(locations["model.u"], m.s, m.t, m.u, m.v);
		gl::glUniform4f(locations["model.v"], m.w, m.x, m.y, m.z);
		return static_cast<S&>(*this);
	}
	S& project(void) {
		using namespace View;
		float a = 1;
		int w = 0, h = 0, x = 0, y = 0;
		SDL_GetWindowSize(frame, &w, &h);
		SDL_GetWindowPosition(frame, &x, &y);
		if(w > 0 && h > 0) {
			a = float(h)/w;
			glViewport(0, 0, w, h);
		}
		locate("a", "l", "r", "b", "t", "n", "f");
		glUniform1f(locations["a"], a);
		glUniform1f(locations["l"], left);
		glUniform1f(locations["r"], right);
		glUniform1f(locations["b"], bottom);
		glUniform1f(locations["t"], top);
		glUniform1f(locations["n"], near);
		glUniform1f(locations["f"], far);

		return static_cast<S&>(*this);
	}
	S& project(float l, float r, float b, float t, float n, float f) {
		left = l; right = r;
		bottom = b; top = t;
		near = n; far = f;
		return project();
	}
	Events::Status operator()(SDL_WindowEvent const& ev) {
		switch(ev.event) {
			case SDL_WINDOWEVENT_CLOSE:
				return { Events::StatusQuit, ev.timestamp };
			case SDL_WINDOWEVENT_MOVED:
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				project();
				break;
			default: break;
		}
		return { Events::StatusPass, ev.timestamp };
	}

	template<class... T>
	Presenter(T &&... t): frame(std::forward<T>(t)...) {}
};

#endif
