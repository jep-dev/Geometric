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
	bool set_source(GLuint shader, std::string const& content);
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
	bool set_source(std::string const& value);
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

struct ShaderTable {
	std::map<std::string, std::string> sources;
	std::map<std::string, View::Shader> objects;

	template<class U, class V>
	bool add_source(U const& key, V const& value, bool needs_source = true) {
		if(needs_source) {
			std::string lines;
			if(!Streams::readLines(value, lines))
				return false;
			sources.emplace(key, lines);
		} else sources.emplace(key, value);
		return true;
	}
	template<class U>
	Events::Status add_shader(const gl::GLenum type, U const& key) {
		auto emplaced = objects.emplace(key, type);
		return emplaced.first -> status;
	}
	template<class U>
	Events::Status compile_shader(U const& key) {
		auto it = objects.find(key);
		if(it == objects.end()) return false;
		return it -> compile();
	}
	template<class U>
	Events::Status attach(Program &program, U const& key) {
		auto it = objects.find(key);
		if(it == objects.end())
			return { Events::StatusError };
		if(!program.attach(*it))
			// TODO use program.status after setting it from program.attach
			return { Events::StatusError };
		return {};
	}
	// TODO determine if attach order matters
	Events::Status attach_all(Program &program) {
		for(auto & it : objects) {
			if(!program.attach(it.second))
				// TODO see note in attach()
				return { Events::StatusError };
		}
		return {};
	}

};


}

#endif
