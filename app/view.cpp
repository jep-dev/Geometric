#include <iostream>
#include <iomanip>

#include "dual.hpp"
#include "presenter.hpp"
#include "shader.hpp"
#include "texture.hpp"

struct Hnd;
struct Hnd: Presenter<Hnd> {
	std::ostringstream oss;
	using Handler::operator();
	Events::Status operator()(SDL_KeyboardEvent const& k) {
		switch(k.keysym.sym) {
			case SDLK_ESCAPE: case SDLK_q:
				return { Events::StatusQuit };
			default: break;
		}
		return {};
	}
	Events::Status operator()(SDL_WindowEvent const& w) {
		switch(w.event) {
			case SDL_WINDOWEVENT_CLOSE:
				*this << "Caught window close event";
				return { Events::StatusQuit };
			case SDL_WINDOWEVENT_MOVED: break;
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED: break;
			default: break;
		}
		return { };
	}
	Events::Status operator()(SDL_MouseButtonEvent const& q) {
		*this << "";
		oss << "Caught mouse " << int(q.button)
			<< " " << ((q.type == SDL_MOUSEBUTTONDOWN) ? "press" : "release")
			<< " at (" << q.x << ", " << q.y << ")";
		return { };
	}

	template<class S>
	Events::Status operator()(S const& s) {
		return { };
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

	int N = -1;
	if(argc > 1) {
		stringstream ss;
		ss << argv[1];
		ss >> N;
		if(ss.fail()) N = -1;
	}

	Hnd hnd;
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(hnd.frame.getWindow());
	glClear(GL_COLOR_BUFFER_BIT);
	auto glErr = glGetError();
	if(glErr != GL_NO_ERROR)
		cout << "A GL error occurred: " << glErr << endl;

	string self = argv[0], delim = "/", share = "share" + delim;
	auto pos = self.find_last_of(delim);
	if(pos == string::npos)
		delim = "\\", pos = self.find_last_of(delim);
	share = self.substr(0, pos+1) + ".." + delim + "share" + delim;
	string vertPath = share + "default.vert", fragPath = share + "default.frag";
	static constexpr GLenum VERT = GL_VERTEX_SHADER, FRAG = GL_FRAGMENT_SHADER;

	Program program;
	Shader vert(vertPath.c_str(), VERT), frag(fragPath.c_str(), FRAG);
	if(!vert.compile() || !program.attach(vert)) {
		cout << "Could not build " << vertPath;
		if(vert.status.length()) cout << ": " << vert.status;
		cout << endl;
		return 1;
	} else if(!frag.compile() || !program.attach(frag)) {
		cout << "Could not build " << fragPath;
		if(frag.status.length()) cout << ": " << frag.status;
		cout << endl;
		return 1;
	}

	float near = 1, far = 5, mid = (near + far)/2,
			right = 1, top = 1, width = 2*right, height = 2*top,
			mx = near/right, my = near/top,
			mzz = (far+near)/(near-far),
			mzw = 2*far*near/(near-far),
			mwz = -1;
	GLfloat points[][3] = {
		{-.5, 1.5, -.5}, {.5, 1.5, -.5}, {.5, 1.5, .5},
		{-.5, 1.5, -.5}, {.5, 1.5, .5}, {-.5, 1.5, .5}
	};

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindAttribLocation(program, 0, "pos");

	if(!program.link()) {
		cout << "Could not link program";
		if(program.status.length())
			cout << ": " << program.status;
		cout << endl;
		return 1;
	}
	glUseProgram(program);

	auto mvpString = "mvp";
	auto mvp = glGetUniformLocation(program, mvpString);
	if(mvp < 0) {
		cout << "Could not find uniform " << mvpString << '!' << endl;
		return 1;
	}
	GLfloat mvpData[16] = {
		mx, 0, 0, 0,
		0, 0, -mzz, -mzw,
		0, my, 0, 0,
		0, 0, mwz, 0,
	};
	glUniformMatrix4fv(mvp, 1, GL_FALSE, mvpData);

	for(auto i = 0;; i++) {
		if(!hnd.poll())
			break;
		// Task
		if(hnd.size()) {
			cout << hnd << endl;
			hnd.clear();
		}
		// Render
		hnd.frame.clear();
		glUseProgram(program);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		hnd.frame.draw();
		SDL_Delay(100);
		if((N >= 0) && (i >= N)) break;
	}
	if(hnd.size())
		cout << hnd << "\n";
	cout << "Frame's message:\n" << hnd.frame << endl;
	SDL_Quit();
}
