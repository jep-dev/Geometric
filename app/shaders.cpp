#include "shaders.hpp"
#include <iostream>

int main(int argc, const char *argv[]) {
	endl(std::cout << "Warning: this will not work without a GL context; see app/main.cpp");

	using namespace View;
	const char *vertPath = "../default.vert",
		*fragPath = "../share/default.frag";
	int out = 0;
	GLuint vert, frag, prog;
		do {
			vert = glCreateShader(GL_VERTEX_SHADER);
			if(!glIsShader(vert)) return __LINE__;
			frag = glCreateShader(GL_FRAGMENT_SHADER);
			if(!glIsShader(frag)) return glDeleteShader(vert), __LINE__;
			prog = glCreateProgram();
			if(!glIsProgram(prog)) {
				glDeleteShader(frag);
				glDeleteShader(vert);
				return __LINE__;
			}
			if(!source(vert, vertPath)) {
				out = __LINE__;
				break;
			}
			if(!source(frag, fragPath)) {
				out = __LINE__;
				break;
			}
			if(!compile(vert)) {
				out = __LINE__;
				break;
			}
			if(!compile(frag)) {
				out = __LINE__;
				break;
			}
			if(!attach(prog, vert, frag)) {
				out = __LINE__;
				break;
			}
			if(!link(prog)) {
				out = __LINE__;
				break;
			}
		} while(0);
		glDeleteProgram(prog);
		glDeleteShader(frag);
		glDeleteShader(vert);
}
