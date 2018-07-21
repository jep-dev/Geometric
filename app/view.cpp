#include <iostream>
#include <iomanip>

#include "dual.hpp"
#include "events.hpp"
#include "frame.hpp"
#include "shader.hpp"
#include "texture.hpp"

struct Hnd;
struct Hnd: Events::Handler<Hnd> {
	std::ostringstream oss;
	using Handler::operator();
	template<class... T>
	Events::Handled operator()(SDL_KeyboardEvent const& k, T &&... t) {
		switch(k.keysym.sym) {
			case SDLK_ESCAPE: case SDLK_q:
				return { Events::Handled::CODE_QUIT };
			default: break;
		}
		return { Events::Handled::CODE_PASS };
	}
	template<class... T>
	Events::Handled operator()(SDL_WindowEvent const& w, T &&... t) {
		switch(w.event) {
			case SDL_WINDOWEVENT_CLOSE:
				oss << "Caught window close event";
				return { Events::Handled::CODE_QUIT };
			case SDL_WINDOWEVENT_ENTER: /*oss << "Window entered\n"; */ break;
			case SDL_WINDOWEVENT_EXPOSED: /*oss << "Window exposed\n"; */ break;
			case SDL_WINDOWEVENT_LEAVE: /*oss << "Window left\n"; */ break;
			case SDL_WINDOWEVENT_MOVED: /*oss << "Window moved\n"; */ break;
			case SDL_WINDOWEVENT_NONE: /*oss << "Window... none?\n"; */ break;
			case SDL_WINDOWEVENT_MAXIMIZED: /*oss << "Window maximized\n"; */ break;
			case SDL_WINDOWEVENT_RESTORED: /*oss << "Window restored\n"; */ break;
			case SDL_WINDOWEVENT_HIDDEN:
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_SHOWN: /*oss << "Window visibility toggled\n"; */ break;
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED: /*oss << "Window resized\n"; */ break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			case SDL_WINDOWEVENT_FOCUS_LOST: /*oss << "Window focus changed\n"; */ break;
			default: break;
		}
		return { Events::Handled::CODE_PASS };
	}
	template<class... T>
	Events::Handled operator()(SDL_QuitEvent const& q, T &&... t) {
		oss << "Caught quit event";
		return { Events::Handled::CODE_QUIT };
	}
	template<class... T>
	Events::Handled operator()(SDL_MouseButtonEvent const& q, T &&... t) {
		oss << "Caught mouse " << int(q.button)
			<< " " << ((q.type == SDL_MOUSEBUTTONDOWN) ? "press" : "release")
			<< " at (" << q.x << ", " << q.y << ")\n";
		return { Events::Handled::CODE_PASS };
	}

	template<class S, class... T>
	Events::Handled operator()(S const& s, T &&... t) {
		return { Events::Handled::CODE_PASS };
	}

	template<class S>
	friend S& operator<<(S& s, Hnd const& hnd) {
		auto res = hnd.oss.str();
		if(res.length())
			s << res;
		return s;
	}
};

int main(int argc, const char *argv[]) {
	using namespace View;
	using namespace gl;
	using namespace glbinding;
	using std::cout;
	using std::endl;
	using std::flush;
	using std::string;

	View::Frame f;
	Binding::initialize(false);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(f.getWindow());
	glClear(GL_COLOR_BUFFER_BIT);
	auto glErr = glGetError();
	if(glErr != GL_NO_ERROR)
		std::cout << "A GL error occurred: " << glErr << std::endl;

	string self = argv[0], delim = "/", share = "share" + delim;
	auto pos = self.find_last_of(delim);
	if(pos == string::npos)
		delim = "\\", pos = self.find_last_of(delim);
	share = self.substr(0, pos+1) + ".." + delim + "share" + delim;
	//cout << "self = " << self << "; share = " << share << "." << endl;
	string vertPath = share + "default.vert", fragPath = share + "default.frag";
	static constexpr GLenum VERT = GL_VERTEX_SHADER, FRAG = GL_FRAGMENT_SHADER;

	Program program;
	Shader vert(vertPath.c_str(), VERT), frag(fragPath.c_str(), FRAG);
	if(!vert.compile() || !program.attach(vert)) {
		cout << "Could not build " << vertPath;
		if(vert.message.length()) cout << ": " << vert.message;
		cout << endl;
		return 1;
	} else if(!frag.compile() || !program.attach(frag)) {
		cout << "Could not build " << fragPath;
		if(frag.message.length()) cout << ": " << frag.message;
		cout << endl;
		return 1;
	}

	GLfloat points[][3] = {
		{-1, -1, 0}, {1, -1, 0}, {1, 1, 0},
		{-1, -1, 0}, {1, 1, 0}, {-1, 1, 0}
	};

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindAttribLocation(program, 0, "pos");

	if(!program.link()) {
		cout << "Could not link program";
		if(program.message.length())
			cout << ": " << program.message;
		cout << endl;
		return 1;
	}
	glUseProgram(program);

	Hnd hnd;
	for(auto i = 0; i < 50; i++) {
		if(!hnd.poll())
			break;
		// Task

		// Render

		SDL_Delay(100);
	}
	std::cout << "Hnd's message:\n" << hnd << "\n"
		"Frame's message:\n" << f << std::endl;
}
