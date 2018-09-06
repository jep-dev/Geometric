#include <iostream>
#include <iomanip>

#include "initializer.hpp"
#include "presenter.hpp"
#include "shader.hpp"
#include "resource.hpp"
#include "streams.tpp"

#include "dual.tpp"
#include "point.hpp"
#include "math.tpp"

// Joystick, haptics, and gamecontroller cause udev error!
// Audio, timer, and video are safe
#define SUBSYSTEMS SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK

#define USE_CUBE 0

struct Hnd;
struct Hnd: Presenter<Hnd> {
	DualQuaternion<float> transform = {1};
	std::ostringstream oss;
	using Handler::operator();
	Events::Status operator()(SDL_KeyboardEvent const& k) {
		using namespace gl;
		using Streams::center;
		using DQ = decltype(transform);
		//locate("projection[0]", "projection[1]", "model[0]", "model[1]");
		//locate("projection", "model[0]", "model[1]");
		locate("model[0]", "model[1]");
		GLint //proj[] = {locations["projection[0]"], locations["projection[1]"]},
				//proj = locations["projection"],
				mod[] = {locations["model[0]"], locations["model[1]"]};
		auto const& sym = k.keysym.sym;
		if(sym == SDLK_ESCAPE || sym == SDLK_q)
			return {Events::StatusQuit, k.timestamp};
		if(k.state == SDL_PRESSED) {
			switch(k.keysym.sym) {
				case SDLK_ESCAPE: case SDLK_q:
					return { Events::StatusQuit, k.timestamp };
				default: break;
			}
		} else {
			switch(k.keysym.sym) {
				default: break;
			}
			bool print_location = false, print_projection = false, print_model = true;
			auto old_transform = transform;
			const char *pressed = "";
			switch(k.keysym.sym) {
				case SDLK_RETURN: pressed = "Return: "; transform = {1}; break;
				case SDLK_1: pressed = "1: "; transform = (1_e + .1_I) * transform; break;
				case SDLK_2: pressed = "2: "; transform = (1_e + .1_J) * transform; break;
				case SDLK_3: pressed = "3: "; transform = (1_e + .1_k) * transform; break;
				case SDLK_0: pressed = "0: ";
					transform = rotation(.1*M_PI, 1, 0, 0) * transform; break;
				case SDLK_9: pressed = "9: ";
					transform = rotation(.1*M_PI, 0, 1, 0) * transform; break;
				case SDLK_8: pressed = "8: ";
					transform = rotation(.1*M_PI, 0, 0, 1) * transform; break;
				default: print_model = false;
			}

			switch(k.keysym.sym) {
				case SDLK_l: print_location = true; break;
				case SDLK_SPACE: print_projection = true;
				case SDLK_m: print_model = true; break;
				/*case SDLK_5:
					oss << "Setting projection matrix (again)\n";
					project(-1, 1, -1, 1, 1, 3);
					print_projection = true;
					break;*/
				case SDLK_h: {
					auto lw = 6;
					auto pad = "  ";
					auto print_kv = [&] (const char *k, const char *v)
						-> decltype(oss)& { return center(oss, k, lw, false) << pad << v, oss; };
					print_kv("KEY", "BINDING\n");
					print_kv("h", "this help message\n");
					print_kv("l", "list uniform locations\n");
					print_kv("1|2|3", "choose model translation preset\n");
					print_kv("8|9|0", "choose model rotation preset\n");
					print_kv("return", "reset model transformation\n");
					print_kv("q|Esc", "quit\n");
				} break;
				default:
				break;
			}
			set_model(transform);
			if(print_location) {
				oss << "Uniform locations:\n";
				for(auto const& l : locations)
					oss << "\t[" << l.first << "] => " << l.second << "\n";
			}
			if(print_model && mod[0] >= 0 && mod[1] >= 0) {
				GLfloat u[4] = {0}, v[4] = {0};
				glGetUniformfv(program, mod[0], u);
				glGetUniformfv(program, mod[1], v);

				DualQuaternion<float> d = {u[0], u[1], u[2], u[3], v[0], v[1], v[2], v[3]};
				oss << pressed << "Model = " << std::string(d) << std::endl;
			}
			/*auto print_row = [&] (GLfloat w, GLfloat x, GLfloat y, GLfloat z, const char *p)
					-> std::ostringstream& {
				return oss << std::showpos << std::fixed << std::setprecision(1)
					<< "[" << std::setw(3) << w << " " << std::setw(3) << x << " "
					<< std::setw(3) << y << " " << std::setw(3) << z << "]" << p, oss;
			};
			if(print_projection && proj >= 0) {
				GLfloat p[16] = {0};
				glGetUniformfv(program, proj, p);
				endl(print_row(p[0], p[1], p[2], p[3], " X"));
				endl(print_row(p[4], p[5], p[6], p[7], " Y"));
				endl(print_row(p[8], p[9], p[10], p[11], " Z"));
				endl(print_row(p[11], p[12], p[13], p[14], " W"));
			}*/
			/*if(print_projection && proj[0] >= 0 && proj[1] >= 0) {
				GLfloat u[4] = {0}, v[4] = {0};
				glGetUniformfv(program, proj[0], u);
				glGetUniformfv(program, proj[1], v);
				oss << "Projection: ";
				print_row(u[0], u[1], u[2], u[3], "") << ", ";
				print_row(v[0], v[1], v[2], v[3], "") << std::endl;

				endl(print_row(u[0], 0, v[0], 0, " X"));
				endl(print_row(0, u[1], v[1], 0, " Y"));
				endl(print_row(0, 0, v[2], u[2], " Z"));
				endl(print_row(0, 0, v[3], 0, " W"));
			}*/
		}
		return { Events::StatusPass, k.timestamp };
	}
	Events::Status operator()(SDL_ControllerAxisEvent const& c) {
		typedef decltype(c.value) value_type;
		value_type const& value = c.value,
			value_max = std::numeric_limits<value_type>::max();
		auto fvalue = float(value) / value_max;
		oss << "Caught controller " << c.axis << " -> " << int(100*fvalue) << "%\n";
		return { Events::StatusPass, c.timestamp };
	}
	Events::Status operator()(SDL_ControllerButtonEvent const& b) {
		oss << "Caught controller " << b.button << ", state " << b.state << "\n";
		return { Events::StatusPass, b.timestamp };
	}
	Events::Status operator()(SDL_WindowEvent const& w) {
		switch(w.event) {
			case SDL_WINDOWEVENT_CLOSE:
				oss << "Caught window close event\n";
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
			<< " at (" << b.x << ", " << b.y << ")\n";
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
		//if(size()) oss << '\n';
		return oss << s, *this;
	}
	template<class S>
	friend S& operator<<(S& s, Hnd const& hnd) {
		auto res = hnd.oss.str();
		if(res.length()) s << res;
		return s;
	}
};

template<class S, class T, class RAD = T>
void sphere(std::vector<S> &vertices, std::vector<gl::GLuint> &indices,
		Point<T> const& center, RAD radius = 1, unsigned M = 10, unsigned N = 10) {
	auto get_angle = [] (unsigned i, unsigned I, T t0, T t1)
			-> T { return t0 + (t1-t0)*i/(I-1); };
	auto get_theta = [=] (unsigned j) -> T { return get_angle(j, M, 0, T(M_PI*2)); };
	auto get_phi = [=] (unsigned i) -> T { return get_angle(i, N, -T(M_PI/2), T(M_PI/2)); };

	Point<T> rel = {0, 0, T(radius)};
	for(unsigned j = 0; j < M; j++) {
		Quaternion<T> rj = rotation(get_theta(j), 0, 1, 0);
		for(unsigned i = 0; i < N; i++) {
			Quaternion<T> ri = rotation(get_phi(i), 1, 0, 0);
			auto pt = center + (rel ^ (rj * ri));
			vertices.emplace_back(S(pt.x));
			vertices.emplace_back(S(pt.y));
			vertices.emplace_back(S(pt.z));
			vertices.emplace_back(S(1));
		}
	}
	for(unsigned j0 = 0, j1 = 1; j0 < M; j0++, j1 = (j0+1) % M) {
		for(unsigned i0 = 0, i1 = 1; i0 < N; i0++, i1 = (i0+1) % N) {
			indices.emplace_back(j0*N+i0);
			indices.emplace_back(j0*N+i1);
			indices.emplace_back(j1*N+i1);
			indices.emplace_back(j0*N+i0);
			indices.emplace_back(j1*N+i1);
			indices.emplace_back(j1*N+i0);
		}
	}

}
template<class S, class T, class U = T, class W = std::common_type_t<S,T,U>>
void cube(std::vector<S> &vertices, std::vector<gl::GLuint> &indices,
		Point<T> const& center, U width = 1, bool print = true) {
	using std::cout;
	using std::endl;

	enum { tne=0, tse, tsw, tnw, bne, bse, bsw, bnw, nDirs };
	W scale = width / W(2);
	Point<W> e = {scale, 0, 0},  w = {-scale, 0, 0},
			n = {0, 0, -scale}, s = {0, 0, scale},
			t = {0, scale, 0},  b = {0, -scale, 0},
			c = {W(center.x), W(center.y), W(center.z)},
			points[] = {c, c, c, c, c, c, c, c};
	using Streams::center;
	auto face = [&] (unsigned tr, unsigned tl, unsigned bl, unsigned br, Point<W> const& p) {
		for(auto i : {tr, tl, bl, br}) {
			points[i] += p;
		}
		for(auto i : {tr, tl, bl, tr, bl, br})
			indices.emplace_back(i);
	};
	face(tne, tse, tsw, tnw, t); // Top
	face(bse, bsw, bnw, bne, b); // Bottom
	face(tnw, tne, bne, bnw, n); // North
	face(tse, tsw, bsw, bse, s); // South
	face(tne, tse, bse, bne, e); // East
	face(tsw, tnw, bnw, bsw, w); // West
	for(auto const& p : points) {
		for(auto const& px : {p.x, p.y, p.z}) {
			vertices.emplace_back(S(px));
		}
		vertices.emplace_back(S(1));
	}
	{
		cout << std::right;
		auto verticesSize = vertices.size();
		for(unsigned i = 0; i < verticesSize; i+=3) {
			Point<gl::GLfloat> pt = {vertices[i], vertices[i+1], vertices[i+2]};
			cout << to_string(pt, 2) << endl;
			/*if(i >= 32) break;
			auto j = i & 15, k = i & 3;
			if(k == 3) {
				cout << ";  ";
			} else {
				if(k) cout << ", ";
				cout << std::setw(5) << vertices[i];
			}
			if(j == 15) endl(cout);*/
		}
		cout << std::left;
	}
}
template<class S, class T>
void cube(std::vector<S> &vertices, std::vector<gl::GLuint> &indices,
		DualQuaternion<T> const& transform, T scale = 1) {
	enum { tne=0, tse, tsw, tnw, bne, bse, bsw, bnw };

	scale /= 2;
	DualQuaternion<T> north = 1_e + scale * 1_J, east = 1_e + scale * 1_I,
			south = 1_e - scale * 1_J, west = 1_e - scale * 1_I,
			top = 1_e + scale * 1_K, bottom = 1_e - scale * 1_K;

	for(auto p : {top * north * east, top * south * east,
			top * south * west, top * north * west,
			bottom * north * east, bottom * south * east,
			bottom * south * west, bottom * north * west}) {
		auto tp = transform * p * *transform;
		vertices.emplace_back(T(tp.x));
		vertices.emplace_back(T(tp.y));
		vertices.emplace_back(T(tp.z));
		vertices.emplace_back(1);
		//if(lim > 0) {
			std::cout << tp.x << "  " << tp.y << "  " << tp.z << std::endl;
			//lim--;
		//}
	}
	for(gl::GLuint i : {
			tse, tsw, bsw, tse, bsw, bse, // south/front
			tne, tse, bse, tse, bse, bne, // east/right
			tne, bne, bnw, tne, bnw, tnw, // north/back
			tnw, bnw, bsw, tnw, bsw, tsw, // west/left
			tne, tnw, tsw, tne, tsw, tse, // top
			bse, bsw, bnw, bse, bnw, bne, // bottom
		}) indices.emplace_back(i);
}

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
	float width = 2, height = 2, depth = 6,
			near = 2, far = near + depth,
			mid = (near + far)/2,
			right = width/2, left = -right,
			top = height/2, bottom = -top;
	auto scale = right * 0.25f;

#if USE_CUBE > 0
	std::vector<GLfloat> points;
	std::vector<GLuint> indices;
	/*for(DualQuaternion<float> transformation : {
			1_e + 3 * 1_I, 1_e - 3 * 1_I,
			1_e + 3 * 1_J, 1_e - 3 * 1_J,
			1_e + 3 * 1_K, 1_e - 3 * 1_K
		}) cube(points, indices, transformation, scale);*/
	for(Point<float> p : {
			Point<float>{0, 0, -mid}}) {
#if USE_CUBE == 1
		cube(points, indices, p, .5);
#else
		sphere(points, indices, p, .5);
#endif
	}
	auto indicesSize = indices.size(), pointsSize = points.size();
# else
	/*GLfloat points[][3] = {
			{-right*0.9f, -top*0.9f, mid}, {right*0.9f, -top*0.9f, mid},
			{right*0.9f, top*0.9f, mid}, {-right*0.9f, top*0.9f, mid}
			//{-right, mid, -top}, {right, mid, -top}, {right, mid, top}, {-right, mid, top}
		};*/
	GLfloat points[][4] = {
			{0.1*-right, -0.1*top, -mid}, {0.1*right, -0.1*top, -mid},
			{0.1*right, 0.1*top, -mid}, {-0.1*right, 0.1*top, -mid}
			//{-right, -top, -mid}, {right, -top, -mid}, {right, top, -mid}, {-right, top, -mid}
		};
	for(auto const& p : points)
		cout << p[0] << "  " << p[1] << "  " << p[2] << endl;
	//GLuint indices[] = {0, 1, 2, 0, 2, 3};
	GLuint indices[] = {0, 2, 1, 0, 1, 3};
	auto indicesSize = sizeof(indices)/sizeof(indices[0]);
#endif

	// Locate shaders from execution path
	string self = argv[0], delim = "/", share = "share" + delim;
	auto pos = self.find_last_of(delim);
	if(pos == string::npos)
		delim = "\\", pos = self.find_last_of(delim);
	share = self.substr(0, pos+1) + ".." + delim + "share" + delim;

	Initializer initializer([](void) { SDL_Init(SUBSYSTEMS); },
		[](void) -> bool {
			return (SDL_WasInit(0) & SUBSYSTEMS) == SUBSYSTEMS;
		}, SDL_Quit);
	SDL_ClearError(); // TODO remove when udev errors are resolved

	if(!initializer) {
		cout << "Could not initialize SDL";
		string err = SDL_GetError();
		if(err.length()) cout << ":\n  " << err;
		cout << endl;
		return 1;
	}

	Hnd hnd;
	cout << hnd.frame << endl;
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto used = hnd.init(share + "dual.glsl", share + "vert.glsl",
			gl::GL_FRAGMENT_SHADER, share + "frag.glsl");
	if(!used.good()) {
		cout << "Could not build shader program";
		if(used.length()) cout << ":\n  " << used;
		cout << endl;
		return 1;
	}
	//hnd.locate("projection[0]", "projection[1]", "model[0]", "model[1]")
	hnd.locate("projection", "model[0]", "model[1]")
		.project(left, right, bottom, top, near, far);

	// Create and initialize vertex array and buffer
	GLuint vao;
	Buffer<2> vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	bufferData(GL_ARRAY_BUFFER, vbo[0], points, GL_STATIC_DRAW);
	bufferData(GL_ELEMENT_ARRAY_BUFFER, vbo[1], indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindAttribLocation(hnd.program, 0, "pos_in");
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);

	/*{
		GLint attrib_size, attrib_stride;
		GLenum attrib_type;

		auto pos = glGetAttribLocation(hnd.program, "pos_in");
		glGetVertexAttribiv(pos, GL_VERTEX_ATTRIB_ARRAY_SIZE, &attrib_size);
		glGetVertexAttribiv(pos, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &attrib_stride);
		glGetVertexAttribiv(pos, GL_VERTEX_ATTRIB_ARRAY_TYPE, &attrib_type);

		auto err = glGetError();
		if(err != GL_NO_ERROR) {
			cout << "GL error: " << err << endl;
		} else {
			cout << "Attrib " << pos << " has size " << attrib_size
				<< ", stride " << attrib_stride << (attrib_stride ? "" : " (contiguous)")
				<< ", and type " << attrib_type << endl;
		}
	}*/


	cout << "\e[s";
	float theta = 0;
	// Poll/render loop
	for(auto i = 0;; i++, theta += M_PI/8) {
		if(!hnd.poll())
			break;
		// Task
		if(hnd.size()) {
			cout << hnd << flush;
			hnd.clear();
		}
		/*float ctheta = cos(theta), stheta = sin(theta);
		glUniform4f(mu, ctheta, 0, stheta, 0);
		glUniform4f(mv, 0, 0, 0.2, 0);*/
		// Render
		hnd.frame.clear().draw(vao, GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, nullptr).flip();
		SDL_Delay(100);
		if((N >= 0) && (i >= N)) break;
	}

	if(hnd.size())
		cout << hnd << "\n";
	//cout << "Frame's message:\n" << hnd.frame << endl;

	// Clean up what isn't done through RAII already
	glDeleteVertexArrays(1, &vao);
}
