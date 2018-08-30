#include "shader.hpp"

namespace View {
namespace Detail {
	bool is_error(GLenum e) { return e == GL_NO_ERROR; }
	bool get_error(void) { return is_error(glGetError()); }

	bool is_sourced(GLuint shader) {
		if(!glIsShader(shader)) return false;
		GLint len;
		glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &len);
		return len;
	}

	bool set_source(GLuint shader, std::string const& content) {
		auto buf = content.c_str();
		auto gbuf = &content[0];
		glShaderSource(shader, 1, &gbuf, nullptr);
		return is_sourced(shader);
	}
	bool source(GLuint shader, const char *filename) {
		bool status;
		std::string lines;
		if(!Streams::readLines(filename, lines)) return false;
		set_source(shader, lines);
		return true;
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


Shader::operator GLuint(void) const { return value; }

bool Shader::source(const char *fname, bool force) {
	compiled = false;
	return sourced = Detail::source(value, fname);
}
bool Shader::set_source(std::string const& content) {
	compiled = false;
	return sourced = Detail::set_source(value, content);
}

bool Shader::compile(bool force) {
	if(!sourced) return false;
	if(compiled && !force) return true;
	compiled = Detail::compile(value);
	GLint len;
	glGetShaderiv(value, GL_INFO_LOG_LENGTH, &len);
	status.code = Events::StatusFail;
	status.message = std::string(unsigned(len), '\0');
	glGetShaderInfoLog(value, len, nullptr, &status.message[0]);
	return compiled;
}

Shader::Shader(GLuint value): value(value) {}

Shader::Shader(GLuint value, const char *fname): Shader(value) { source(fname); }

Shader::Shader(GLenum type): Shader(glCreateShader(type)) { }

Shader::Shader(const char *fname, GLenum type): Shader(type) { source(fname); }

Program::operator GLuint(void) const { return value; }

bool Program::link(bool force) {
	if(!attached && force) linked = false;
	if(linked && !force) return true;
	linked = Detail::link(value);
	int len;
	glGetProgramiv(*this, GL_INFO_LOG_LENGTH, &len);
	if(len <= 0) return linked;
	status.message = std::string(len, '\0');
	glGetProgramInfoLog(*this, len, nullptr, &status.message[0]);
	return linked;
}

GLuint Program::locate(const char *name) {
	auto location = glGetUniformLocation(value, name);
	if(location < 0) {
		status.code = Events::StatusWarn;
		status.message = "Could not find uniform ";
		status.message += name;
	}
	return location;
}

Program::Program(GLuint value): value(value) {}

Program::Program(void): Program(glCreateProgram()) {}

}
