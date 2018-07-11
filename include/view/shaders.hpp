#ifndef SHADERS_HPP
#define SHADERS_HPP

#include <deque>
#include <map>
#include <utility>
#include <vector>

#include "reader.hpp"
#include "view.hpp"

#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>

namespace View {
	using namespace gl;
	struct Shader;
	struct Object;
	struct Program;

	using namespace gl;

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
		std::string lines = Streams::readLines(filename, &status);
		if(!status) return false;
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

#endif
