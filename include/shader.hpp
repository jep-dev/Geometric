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
	bool is_error(GLenum e);
	bool get_error(void);
	bool is_sourced(GLuint shader);
	bool source(GLuint shader, const char *filename);
	bool is_compiled(GLuint shader);
	bool compile(GLuint shader);
	bool is_linked(GLuint program);
	bool link(GLuint program);

	template<class... S>
	bool attach(GLuint program, S &&... s) {
		for(auto sh : {GLuint(s)...}) {
			if(!is_compiled(sh))
				return false;
			glAttachShader(program, sh);
		}
		return true;
	}
}

struct Shader {
	GLuint value;
	bool sourced = false, compiled = false;
	Events::Status status;
	operator GLuint(void) const;
	bool source(const char *fname, bool force = true);
	bool compile(bool force = true);
	Shader(GLuint value);
	Shader(GLuint value, const char *fname);
	Shader(GLenum type = GL_VERTEX_SHADER);
	Shader(const char *fname, GLenum type = GL_VERTEX_SHADER);
	virtual ~Shader(void) { glDeleteShader(value); value = 0; }
};
struct Program {
	GLuint value;
	bool attached = false, linked = false;
	Events::Status status;
	operator GLuint(void) const;
	template<class... S>
	bool attach(S &&... s) {
		using namespace Detail;
		for(auto b : {(glIsShader(s) && is_sourced(s) && is_compiled(s))...})
			if(!b) return attached = false;
		return attached = Detail::attach(value, std::forward<S>(s)...);
	}
	bool link(bool force = true);
	GLuint locate(const char *name);
	Program(GLuint value);
	Program(void);
	virtual ~Program(void) { glDeleteProgram(value); value = 0; }
};

}

#endif
