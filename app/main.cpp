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
	Events::Handled operator()(SDL_KeyboardEvent const& k, T &&... t) {
		switch(k.keysym.sym) {
			case SDLK_ESCAPE:
				oss << "Caught escape keypress\n";
				return { Events::Handled::CODE_QUIT };
			default: break;
		}
		return { Events::Handled::CODE_PASS };
	}
	template<class... T>
	Events::Handled operator()(SDL_WindowEvent const& w, T &&... t) {
		switch(w.type) {
			case SDL_WINDOWEVENT_CLOSE:
				oss << "Caught window close event\n";
				return { Events::Handled::CODE_QUIT };
			case SDL_WINDOWEVENT_MOVED: break;
				oss << "Window moved to " << w.data1 << ", " << w.data2 << "\n";
				return { Events::Handled::CODE_PASS };
			case SDL_WINDOWEVENT_ENTER:
				oss << "Window entered\n";
				return { Events::Handled::CODE_PASS };
			case SDL_WINDOWEVENT_LEAVE: break;
				oss << "Window left\n";
				return { Events::Handled::CODE_PASS };
			case SDL_WINDOWEVENT_RESIZED: break;
				oss << "Window resized\n";
				return { Events::Handled::CODE_PASS };
			default: return { Events::Handled::CODE_PASS };
		}
	}
	template<class... T>
	Events::Handled operator()(SDL_QuitEvent const& q, T &&... t) {
		oss << "Caught quit event\n";
		return { Events::Handled::CODE_QUIT };
	}

	template<class S, class... T>
	Events::Handled operator()(S const& s, T &&... t) {
		return { Events::Handled::CODE_PASS };
	}

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
	using std::string;

	View::Frame f;
	Binding::initialize(false);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(f.getWindow());
	glClear(GL_COLOR_BUFFER_BIT);
	auto glErr = glGetError();
	if(glErr != GL_NO_ERROR)
		std::cout << "There was a GL ERROR! " << glErr << std::endl;

	string self = argv[0], delim = "/", share = "share" + delim;
	auto pos = self.find_last_of(delim);
	if(pos == string::npos)
		delim = "\\", pos = self.find_last_of(delim);
	share = self.substr(0, pos+1) + ".." + delim + "share" + delim;
	cout << "self = " << self << "; share = " << share << "." << endl;
	string vertPath = share + "default.vert", fragPath = share + "default.frag";
	static constexpr GLenum VERT = GL_VERTEX_SHADER, FRAG = GL_FRAGMENT_SHADER;

	Program program;
	Shader vert{GL_VERTEX_SHADER}, frag{GL_FRAGMENT_SHADER};
	do {
		if(!vert.source(vertPath.c_str())) {
			cout << "Could not source " << vertPath << endl;
		} else if(!vert.compile()) {
			cout << "Could not compile " << vertPath << endl;
		} else if(!program.attach(vert)) {
			cout << "Could not link " << vertPath << endl;
		} else if(!frag.source(fragPath.c_str())) {
			cout << "Could not source " << fragPath << endl;
		} else if(!frag.compile()) {
			cout << "Could not compile " << fragPath << endl;
		} else if(!program.attach(frag)) {
			cout << "Could not attach " << fragPath << endl;
		} else if(!program.link()) {
			cout << "Could not link program" << endl;
		} else {
			continue;
		}
		return 1;
	} while(0);

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
