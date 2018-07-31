#include <iostream>
#include <iomanip>

#include "presenter.hpp"
#include "shader.hpp"

struct Hnd;
struct Hnd: Presenter<Hnd> {
	std::ostringstream oss;
	using Handler::operator();
	Events::Status operator()(SDL_KeyboardEvent const& k) {
		switch(k.keysym.sym) {
			case SDLK_ESCAPE: case SDLK_q:
				return { Events::StatusQuit, k.timestamp };
			default: break;
		}
		return { Events::StatusPass, k.timestamp };
	}
	Events::Status operator()(SDL_WindowEvent const& w) {
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
	string vertPath = share + "default.vert", fragPath = share + "default.frag";

	Hnd hnd;
	auto used = hnd.init(vertPath.c_str(), fragPath.c_str());
	if(!used.good()) {
		cout << "Could not build shader program";
		if(used.length()) cout << ": " << used;
		cout << endl;
		return 1;
	}

	// Locate and initialize MVP matrix
	auto mvp = hnd.program.locate("mvp");
	if(mvp < 0)
		return cout << hnd.program.status, 1;
	hnd.project(mvp, top, right, near, far);

	// Create and initialize vertex array and buffer
	GLuint vao, vbo[2];
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindAttribLocation(hnd.program, 0, "pos");

	// Poll/render loop
	for(auto i = 0;; i++) {
		if(!hnd.poll())
			break;
		// Task
		if(hnd.size()) {
			cout << hnd << endl;
			hnd.clear();
		}
		// Render
		//hnd.frame.clear().draw(vao, GL_TRIANGLES, 0, indicesSize).flip();
		hnd.frame.clear().draw(vao, GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, nullptr).flip();
		SDL_Delay(100);
		if((N >= 0) && (i >= N)) break;
	}

	if(hnd.size())
		cout << hnd << "\n";
	cout << "Frame's message:\n" << hnd.frame << endl;

	// Clean up what isn't done through RAII already
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, vbo);
	SDL_Quit();
}
