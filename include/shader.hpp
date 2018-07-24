#ifndef SHADER_HPP
#define SHADER_HPP

///@cond
#include <deque>
#include <map>
#include <utility>
#include <vector>
///@endcond

#include "reader.hpp"
#include "view.hpp"

namespace View {

struct Shader;
struct Object;
struct Program;

using namespace gl;

namespace Detail {
	bool is_error(GLenum e) { return e == GL_NO_ERROR; }
	bool get_error(void) { return is_error(glGetError()); }

	bool is_sourced(GLuint shader) {
		if(!glIsShader(shader)) return false;
		GLint len;
		glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &len);
		return len;
	}

	bool source(GLuint shader, const char *filename) {
		bool status;
		std::string lines;
		if(!Streams::readLines(filename, lines)) return false;
		auto buf = lines.c_str();
		auto gbuf = &lines[0];
		glShaderSource(shader, 1, &gbuf, nullptr);
		return is_sourced(shader);
	}

	bool is_compiled(GLuint shader) {
		if(!glIsShader(shader)) return false;
		GLint cmp;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &cmp);
		return GL_TRUE == cmp;
	}
	bool compile(GLuint shader) {
		if(is_compiled(shader)) return true;
		glCompileShader(shader);
		return is_compiled(shader);
	}
	template<class... S>
	bool attach(GLuint program, S &&... s) {
		for(auto sh : {GLuint(s)...}) {
			if(!is_compiled(sh))
				return false;
			glAttachShader(program, sh);
		}
		return true;
	}
	bool is_linked(GLuint program) {
		if(!glIsProgram(program)) return false;
		GLint cmp;
		glGetProgramiv(program, GL_LINK_STATUS, &cmp);
		return GL_TRUE == cmp;
	}
	bool link(GLuint program) {
		if(!is_linked(program)) {
			glLinkProgram(program);
			if(!is_linked(program))
				return false;
		}
		return true;
	}
}

struct Shader {
	GLuint value;
	bool sourced = false, compiled = false;
	std::string message;
	operator GLuint(void) const { return value; }
	bool source(const char *fname, bool force = true) {
		compiled = false;
		return sourced = Detail::source(value, fname);
	}
	bool compile(bool force = true) {
		if(!sourced) return false;
		if(compiled && !force) return true;
		compiled = Detail::compile(value);
		GLint len;
		glGetShaderiv(value, GL_INFO_LOG_LENGTH, &len);
		message = std::string(unsigned(len), '\0');
		glGetShaderInfoLog(value, len, nullptr, &message[0]);
		return compiled;
	}
	Shader(GLuint value): value(value) {}
	Shader(GLuint value, const char *fname): Shader(value) { source(fname); }
	Shader(GLenum type = GL_VERTEX_SHADER): Shader(glCreateShader(type)) { }
	Shader(const char *fname, GLenum type = GL_VERTEX_SHADER): Shader(type) { source(fname); }
	virtual ~Shader(void) { glDeleteShader(value); value = 0; }
};
struct Program {
	GLuint value;
	bool attached = false, linked = false;
	std::string message;
	operator GLuint(void) const { return value; }
	template<class... S>
	bool attach(S &&... s) {
		for(auto b : {(glIsShader(s) && Detail::is_sourced(s) && Detail::is_compiled(s))...})
			if(!b) return attached = false;
		return attached = Detail::attach(value, std::forward<S>(s)...);
	}
	bool link(bool force = true) {
		if(!attached && force) linked = false;
		if(linked && !force) return true;
		linked = Detail::link(value);
		int len;
		glGetProgramiv(*this, GL_INFO_LOG_LENGTH, &len);
		if(len <= 0) return linked;
		message = std::string(len, '\0');
		glGetProgramInfoLog(*this, len, nullptr, &message[0]);
		return linked;
	}
	Program(GLuint value): value(value) {}
	Program(void): Program(glCreateProgram()) {}
	virtual ~Program(void) { glDeleteProgram(value); value = 0; }
};

}

#endif
