#include <iostream>
#include <iomanip>

#include "events.hpp"

#include "view.hpp"
#include "view/shaders.hpp"
#include "dual.hpp"

struct Hnd;
struct Hnd: Events::Handler<Hnd> {
	std::ostringstream oss;
	using Handler::operator();
	template<class... T>
	bool operator()(SDL_KeyboardEvent const& k, T &&... t) {
		switch(k.keysym.sym) {
			case SDLK_ESCAPE:
				oss << "Caught escape keypress\n";
				return false;
			default: break;
		}
		return true;
	}
	template<class... T>
	bool operator()(SDL_WindowEvent const& w, T &&... t) {
		switch(w.type) {
			case SDL_WINDOWEVENT_CLOSE:
				oss << "Caught window close event\n";
				return false;
			case SDL_WINDOWEVENT_MOVED: break;
				oss << "Window moved to " << w.data1 << ", " << w.data2 << "\n";
				return true;
			case SDL_WINDOWEVENT_ENTER:
				oss << "Window entered\n";
				return true;
			case SDL_WINDOWEVENT_LEAVE: break;
				oss << "Window left\n";
				return true;
			case SDL_WINDOWEVENT_RESIZED: break;
				oss << "Window resized\n";
				return true;
			default: return true;
		}
	}
	template<class... T>
	bool operator()(SDL_QuitEvent const& q, T &&... t) {
		oss << "Caught quit event\n";
		return false;
	}

	template<class S, class... T>
	bool operator()(S const& s, T &&... t) { return true; }

	template<class S>
	friend S& operator<<(S& s, Hnd const& hnd) {
		auto res = hnd.oss.str();
		if(res.length())
			s << "Hnd reports:\n" << res;
		return s;
	}
};

int main(int argc, const char *argv[]) {
	using namespace View;
	using namespace gl;
	using namespace glbinding;
	using std::cout;
	using std::endl;

	View::Frame f;
	Binding::initialize(false);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(f.getWindow());
	glClear(GL_COLOR_BUFFER_BIT);
	auto glErr = glGetError();
	if(glErr != GL_NO_ERROR)
		std::cout << "There was a GL ERROR! " << glErr << std::endl;

	const char *vertPath = "../share/default.vert", *fragPath = "../share/default.frag";
	static constexpr GLenum VERT = GL_VERTEX_SHADER, FRAG = GL_FRAGMENT_SHADER;

	/*{
		const char *reason = nullptr;
		GLuint vert, frag, prog;
		do {
			vert = glCreateShader(GL_VERTEX_SHADER);
			if(!glIsShader(vert)) { reason = "not a shader"; break; }
			frag = glCreateShader(GL_FRAGMENT_SHADER);
			if(!glIsShader(frag)) { reason = "not a shader"; break; }
			prog = glCreateProgram();
			if(!glIsProgram(prog)) { reason = "not a program"; break; }
			if(!source(vert, vertPath)) { reason = "couldn't source"; break; }
			if(!source(frag, fragPath)) { reason = "couldn't source"; break; }
			if(!compile(vert)) { reason = "couldn't compile"; break; }
			if(!compile(frag)) { reason = "couldn't compile"; break; }
			if(!attach(prog, vert, frag)) { reason = "couldn't attach"; break; }
			if(!link(prog)) { reason = "couldn't link"; break; }
		} while(0);
			if(glIsProgram(prog)) glDeleteProgram(prog);
		if(glIsShader(frag)) glDeleteShader(frag);
		if(glIsShader(vert)) glDeleteShader(vert);
		if(reason) cout << "Reason: " << reason;
	}*/

	Hnd hnd;
	for(auto i = 0; i < 50; i++) {
		if(!hnd.poll())
			break;
		// Task

		// Render

		SDL_Delay(100);
	}
	std::cout << hnd;
	std::cout << f;
}
