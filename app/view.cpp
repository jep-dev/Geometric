#include <iostream>
#include <iomanip>

#include "initializer.hpp"
#include "presenter.hpp"
#include "shader.hpp"
#include "resource.hpp"

struct Hnd;
struct Hnd: Presenter<Hnd> {
	std::ostringstream oss;
	using Handler::operator();
	Events::Status operator()(SDL_KeyboardEvent const& k) {
		auto mu = program.locate("model.u"), mv = program.locate("model.v"),
				u = program.locate("u"), v = program.locate("v");
		switch(k.keysym.sym) {
			case SDLK_ESCAPE: case SDLK_q:
				return { Events::StatusQuit, k.timestamp };
			case SDLK_1: {
				gl::glUniform4f(mu, 1, 0, 0, 0);
				gl::glUniform4f(mv, 0, 0, 0, 0);
				gl::glUniform4f(u, 1, 0, 0, 0);
				gl::glUniform4f(v, 0, 0, 0, 0);
			} break;
			case SDLK_2: {
				gl::glUniform4f(mu, 1, 0, 0, 0);
				gl::glUniform4f(mv, 0, 1, 0, 0);
				gl::glUniform4f(u, 1, 0, 0, 0);
				gl::glUniform4f(v, 0, 1, 0, 0);
			} break;
			case SDLK_3: {
				gl::glUniform4f(mu, 1, 0, 0, 0);
				gl::glUniform4f(mv, 0, 0, 1, 0);
				gl::glUniform4f(u, 1, 0, 0, 0);
				gl::glUniform4f(v, 0, 0, 1, 0);
			} break;
			case SDLK_4: {
				gl::glUniform4f(mu, 0, 1, 0, 0);
				gl::glUniform4f(mv, 0, 0, 0, 0);
				gl::glUniform4f(u, 0, 1, 0, 0);
				gl::glUniform4f(v, 0, 0, 0, 0);
			} break;
			default: break;
		}
		return { Events::StatusPass, k.timestamp };
	}
	Events::Status operator()(SDL_ControllerAxisEvent const& c) {
		typedef decltype(c.value) value_type;
		value_type const& value = c.value,
			value_max = std::numeric_limits<value_type>::max();
		auto fvalue = float(value) / value_max;
		*this << "";
		oss << "Caught controller " << c.axis << " -> " << int(100*fvalue) << "%";
		return { Events::StatusPass, c.timestamp };
	}
	Events::Status operator()(SDL_ControllerButtonEvent const& b) {
		*this << "";
		oss << "Caught controller " << b.button << ", state " << b.state;
		return { Events::StatusPass, b.timestamp };
	}
	Events::Status operator()(SDL_WindowEvent const& w) {
		*this << "";
		switch(w.event) {
			case SDL_WINDOWEVENT_CLOSE:
				*this << "Caught window close event";
				return { Events::StatusQuit, w.timestamp };
			case SDL_WINDOWEVENT_MOVED: break;
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED: break;
			default: break;
		}
		return { Events::StatusPass, w.timestamp };
	}
	Events::Status operator()(SDL_MouseButtonEvent const& b) {
		*this << "";
		oss << "Caught mouse " << int(b.button)
			<< " " << ((b.type == SDL_MOUSEBUTTONDOWN) ? "press" : "release")
			<< " at (" << b.x << ", " << b.y << ")";
		return { Events::StatusPass, b.timestamp };
	}

	template<class S>
	Events::Status operator()(S const& s) {
		return { Events::StatusPass, s.timestamp };
	}
	std::size_t size(void) {
		return oss.tellp();
	}
	Hnd& clear(void) {
		return oss.str(""), *this;
	}
	template<class S>
	Hnd& operator<<(S const& s) {
		if(size()) oss << '\n';
		return oss << s, *this;
	}
	template<class S>
	friend S& operator<<(S& s, Hnd const& hnd) {
		auto res = hnd.oss.str();
		if(res.length()) s << res;
		return s;
	}
};

int main(int argc, const char *argv[]) {
	using namespace std;
	using namespace View;
	using namespace glbinding;

	// Get/set number of iterations, using -1 as unlimited
	int N = -1;
	if(argc > 1) {
		stringstream ss;
		ss << argv[1];
		ss >> N;
		if(ss.fail()) N = -1;
	}
	// Initialize projection matrix values and vertices
	float near = 1, far = 2, mid = (near + far)/2, right = 1, top = 1;
	GLfloat points[][3] = {
		/*{-right, mid, -top}, {right, mid, -top}, {right, mid, top},
		{-right, mid, -top}, {right, mid, top}, {-right, mid, top}*/
		{-right, mid, -top}, {right, mid, -top}, {right, mid, top}, {-right, mid, top}
	};
	GLuint indices[] = {0, 1, 2, 2, 3, 0};
	auto indicesSize = sizeof(indices)/sizeof(indices[0]);

	// Locate shaders from execution path
	string self = argv[0], delim = "/", share = "share" + delim;
	auto pos = self.find_last_of(delim);
	if(pos == string::npos)
		delim = "\\", pos = self.find_last_of(delim);
	share = self.substr(0, pos+1) + ".." + delim + "share" + delim;

	Initializer initializer([](void) { SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER); },
		[](void) -> bool {
			auto inits = SDL_INIT_VIDEO | SDL_INIT_TIMER;
			return (SDL_WasInit(0) & inits) == inits;
		}, SDL_Quit);
	if(!initializer) {
		cout << "Could not initialize SDL";
		string err = SDL_GetError();
		if(err.length()) cout << ":\n  " << err;
		cout << endl;
		return 1;
	}

	Hnd hnd;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto used = hnd.init(gl::GL_VERTEX_SHADER, share + "dual.glsl", share + "vert.glsl",
			gl::GL_FRAGMENT_SHADER, share + "frag.glsl");
	// auto used = hnd.init(vertPath.c_str(), fragPath.c_str());
	if(!used.good()) {
		cout << "Could not build shader program";
		if(used.length()) cout << ":\n  " << used;
		cout << endl;
		return 1;
	}

	// Locate and initialize MVP matrix
	auto projection = hnd.program.locate("projection");
	if(projection < 0)
		return cout << hnd.program.status, 1;
	hnd.project(projection, top, right, near, far);

	/*auto u = hnd.program.locate("u"), v = hnd.program.locate("v");
	glUniform4f(u, 1, 0, 0, 0);
	glUniform4f(v, 1, 0, 0, 0);*/

	// Create and initialize vertex array and buffer
	GLuint vao;
	Buffer<2> vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	bufferData(GL_ARRAY_BUFFER, vbo[0], points, GL_STATIC_DRAW);
	bufferData(GL_ELEMENT_ARRAY_BUFFER, vbo[1], indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindAttribLocation(hnd.program, 0, "pos");

	cout << "\e[s";
	// Poll/render loop
	for(auto i = 0;; i++) {
		if(!hnd.poll())
			break;
		// Task
		if(hnd.size()) {
			cout << "\e[u\e[K" << hnd << flush;
			hnd.clear();
		}
		// Render
		hnd.frame.clear().draw(vao, GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, nullptr).flip();
		SDL_Delay(100);
		if((N >= 0) && (i >= N)) break;
	}

	if(hnd.size())
		cout << hnd << "\n";
	cout << "Frame's message:\n" << hnd.frame << endl;

	// Clean up what isn't done through RAII already
	glDeleteVertexArrays(1, &vao);
}
