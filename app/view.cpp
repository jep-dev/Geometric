#include <iostream>
#include <iomanip>

#include "initializer.hpp"
#include "presenter.hpp"
#include "shader.hpp"
#include "resource.hpp"
#include "streams.tpp"
/*
#define USE_DEVIL
#include "texture.hpp"
*/
#include "timing.hpp"

#include "quaternion.tpp"
#include "dual.tpp"
#include "point.hpp"
#include "math.tpp"
#include "joystick.hpp"

// Joystick, haptics, and gamecontroller cause udev error!
// Audio, timer, and video are safe
#define SUBSYSTEMS SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK

#define USE_MODEL 2

struct Hnd;
struct Hnd: Presenter<Hnd> {
	typedef enum {
		e_out=0, e_info, n_streams
	} e_stream;
	DualQuaternion<float> orientation = {1}, translation = {1},
			transform = orientation * translation;
	std::ostringstream streams[n_streams];
	bool enabled[n_streams] = { false };
	template<class... T>
	Hnd& enable(e_stream s, T... t) { return enabled[s] = true, enable(t...); }
	Hnd& enable(void) { return *this; }
	template<class... T>
	Hnd& disable(e_stream s, T... t) { return enabled[s] = true, enable(t...); }
	Hnd& disable(void) { return *this; }


	//std::map<unsigned, Joystick> joysticks;
	JoystickTable joysticks;

	std::size_t size(void) /* cannot be const */ {
		std::size_t total = 0;
		for(auto i = 0; i < n_streams; i++) {
			if(!enabled[i]) continue;
			auto size = streams[i].tellp();
			// Observe failure conditions resulting in size=-1
			if(size > 0) total += size;
		}
		return total;
	}
	Hnd& update(void) {
		if(!joysticks.size()) {
			if(streams[e_out].tellp() > 0) streams[e_out] << std::endl;
			streams[e_out] << "Skipped updating due to no joysticks";
			return *this;
		}
		auto & first = joysticks.begin() -> second;
		DualQuaternion<float> t = {1}, r = {1}; //orientation;
		//auto d = 1_e + transform.x*1_I + transform.y*1_J + transform.z*1_K;
		if(first.axes.find(0) != first.axes.cend()) {
			t = (1_e - (.1_I * first.axes[0])) * t;
		}
		if(first.axes.find(1) != first.axes.cend()) {
			t = (1_e - (.1_K * first.axes[1])) * t;
		}
		if(first.axes.find(3) != first.axes.cend()) {
			//r = rotation(M_PI/45 * first.axes[3], 0, 1, 0) * r;
			r = r * rotation(M_PI/4 * first.axes[3], 0, 1, 0);
		}
		if(first.axes.find(4) != first.axes.cend()) {
			//r = rotation(M_PI/45 * first.axes[4], -1, 0, 0) * r;
			r = rotation(M_PI/4 * first.axes[4], -1, 0, 0) * r;
		}
		orientation = r;// * orientation;
		translation = translation * (t ^ orientation);
		//transform = d;
		/*if(streams[e_out].tellp() > 0) streams[e_out] << std::endl;
		streams[e_out] << transform;*/
		set_model(transform = orientation * translation);
		/*project(left, right, bottom, top, near, far);
		project();*/
		return *this;
	}

	using Handler::operator();
	Events::Status operator()(SDL_JoyDeviceEvent const& jde) {
		switch(jde.type) {
			case SDL_JOYDEVICEADDED:
			if(streams[e_info].tellp() > 0) streams[e_info] << '\n';
				if(joysticks.open(jde.which))
					streams[e_info] << "Opened joystick " << jde.which;
				else streams[e_out] << "Could not open joystick " << jde.which;
				break;
			case SDL_JOYDEVICEREMOVED:
				joysticks.erase(jde.which);
				if(streams[e_info].tellp() > 0) streams[e_info] << '\n';
				streams[e_info] << "Removed joystick " << jde.which;
				break;
			default: break;
		}
		return { Events::StatusPass, jde.timestamp };
	}
	Events::Status operator()(SDL_JoyAxisEvent const& ja) {
		auto found = joysticks.find(ja.which);
		if(found == joysticks.end())
			return { Events::StatusWarn, ja.timestamp };
		auto & joy = found -> second;
		joy.axes[ja.axis] = ja.value/float(SDL_JOYSTICK_AXIS_MAX);
		if(streams[e_info].tellp() > 0) streams[e_info] << '\n';
		streams[e_info] << "Joy axis " << int(ja.axis) << " -> " << joy.axes[ja.axis];
		update();
		return { Events::StatusPass, ja.timestamp };
	}
	Events::Status operator()(SDL_JoyHatEvent const& jh) {
		auto found = joysticks.find(jh.which);
		if(found == joysticks.end())
			return { Events::StatusWarn, jh.timestamp };
		auto & joy = found -> second;
		joy.hat.first = joy.hat.second;
		joy.hat.second = jh.value;
		// jh.hat is another index somewhat like axes
		// TODO redesign 'hat' as 'hats'
		if(streams[e_info].tellp() > 0) streams[e_info] << '\n';
		streams[e_info] << "Joy hat " << int(jh.value);
		return { Events::StatusPass, jh.timestamp };
	}
	Events::Status operator()(SDL_JoyButtonEvent const& jb) {
		auto found = joysticks.find(jb.which);
		if(found == joysticks.end())
			return { Events::StatusWarn, jb.timestamp };
		auto & joy = found -> second;
		auto button = joy.buttons.find(jb.button);
		if(button == joy.buttons.end()) {
			joy.buttons.emplace(jb.button,
					Joystick::History<>{jb.state, jb.state});
			if(streams[e_info].tellp() > 0) streams[e_info] << '\n';
			streams[e_info] << "Button " << int(jb.button)
				<< ": " << int(jb.state);
		} else {
			if(streams[e_info].tellp() > 0) streams[e_info] << '\n';
			streams[e_info] << "Button " << int(jb.button) << ": "
				<< int(joy.buttons[jb.button].first) << " -> " << int(jb.state);
			button -> second.first = button -> second.second;
			button -> second.second = jb.state;
		}
		return { Events::StatusPass, jb.timestamp };
	}
	Events::Status operator()(SDL_KeyboardEvent const& k) {
		using namespace gl;
		using Streams::center;
		using DQ = decltype(transform);
		locate("model.u", "model.v");
		GLint mod[] = {locations["model.u"], locations["model.v"]};
		auto const& sym = k.keysym.sym;
		if(sym == SDLK_ESCAPE || sym == SDLK_q)
			return {Events::StatusQuit, k.timestamp};
		if(k.state == SDL_PRESSED) {
			switch(k.keysym.sym) {
				case SDLK_ESCAPE: case SDLK_q:
					return { Events::StatusQuit, k.timestamp };
				case SDLK_MINUS:
				case SDLK_KP_MINUS: project(-4, 4, -4, 4, 1, 10); break;
				case SDLK_KP_PLUS:
				case SDLK_EQUALS: project(-2.5, 2.5, -2.5, 2.5, 1, 10); break;
				default: break;
			}
		} else {
			bool print_location = false, print_projection = false, print_model = true;
			auto old_transform = transform;
			const char *pressed = "";
			switch(k.keysym.sym) {
				case SDLK_RETURN: pressed = "Return: "; transform = {1}; break;
				case SDLK_1: pressed = "1: "; transform = .1_x * transform; break;
				case SDLK_2: pressed = "2: "; transform = .1_y * transform; break;
				case SDLK_3: pressed = "3: "; transform = .1_z * transform; break;
				case SDLK_8: pressed = "8: ";
					transform = rotation(.1*M_PI, 0, 0, 1) * transform; break;
				case SDLK_9: pressed = "9: ";
					transform = rotation(.1*M_PI, 0, 1, 0) * transform; break;
				case SDLK_0: pressed = "0: ";
					transform = rotation(.1*M_PI, 1, 0, 0) * transform; break;
				default: print_model = false;
			}
			set_model(transform);

			switch(k.keysym.sym) {
				case SDLK_l: print_location = true; break;
				case SDLK_SPACE: print_projection = true;
				case SDLK_m: print_model = true; break;
				case SDLK_h: {
					auto lw = 6;
					auto pad = "  ";
					auto print_kv = [&] (const char *k, const char *v) {
						if(streams[e_out].tellp() > 0) streams[e_out] << '\n';
						center(streams[e_out], k, lw, false) << pad << v;
					};
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
			if(print_location) {
				if(streams[e_out].tellp() > 0) streams[e_out] << '\n';
				streams[e_out] << "Uniform locations:\n";
				for(auto const& l : locations)
					streams[e_out] << "\t[" << l.first << "] => " << l.second << "\n";
			}
			if(print_model && mod[0] >= 0 && mod[1] >= 0) {
				GLfloat u[4] = {0}, v[4] = {0};
				glGetUniformfv(program, mod[0], u);
				glGetUniformfv(program, mod[1], v);

				DualQuaternion<float> d = {u[0], u[1], u[2], u[3], v[0], v[1], v[2], v[3]};
				if(streams[e_out].tellp() > 0) streams[e_out] << '\n';
				streams[e_out] << pressed << "Model = " << std::string(d) << std::endl;
			}
		}
		return { Events::StatusPass, k.timestamp };
	}
	Events::Status operator()(SDL_ControllerButtonEvent const& b) {
		if(streams[e_out].tellp() > 0) streams[e_out] << '\n';
		streams[e_info] << "Caught controller " << b.button << ", state " << b.state;
		return { Events::StatusPass, b.timestamp };
	}
	Events::Status operator()(SDL_MouseButtonEvent const& b) {
		if(streams[e_info].tellp() > 0) streams[e_info] << '\n';
		streams[e_info] << "Caught mouse " << int(b.button)
			<< " " << ((b.type == SDL_MOUSEBUTTONDOWN) ? "press" : "release")
			<< " at (" << b.x << ", " << b.y << ")";
		return { Events::StatusPass, b.timestamp };
	}

	template<class S>
	Events::Status operator()(S const& s) {
		return { Events::StatusPass, s.timestamp };
	}
	Hnd& clear(void) {
		//return oss.str(""), *this;
		for(auto & i : streams)
			i.str("");
		return *this;
	}
	template<class S>
	friend S& operator<<(S& s, Hnd const& hnd) {
		//return s << hnd.oss.str(), s;
		std::ostringstream oss;
		bool printed = false;
		for(auto i = 0; i < n_streams; i++) {
			if(!hnd.enabled[i]) continue;
			oss << hnd.streams[i].str();
			if(oss.tellp() > 0) {
				if(printed) s << '\n';
				printed = true;
				s << oss.str();
			}
			oss.str("");
		}
		return s;
	}
};

template<class S, class T, class RAD = T>
void sphere(std::vector<S> &vertices, std::vector<gl::GLuint> &indices,
		Point<T> const& center, RAD radius = 1, unsigned M = 100, unsigned N = 100) {
	auto get_angle = [] (unsigned i, unsigned I, T t0, T t1)
			-> T { return t0 + (t1-t0)*i/(I-1); };
	auto get_theta = [=] (unsigned j) -> T { return get_angle(j, N, 0, T(M_PI*2)); };
	auto get_phi = [=] (unsigned i) -> T { return get_angle(i, M, -T(M_PI/2), T(M_PI/2)); };

	for(unsigned j = 0, N2 = N+1; j <= N; j++) {
		auto phi = get_phi(j), cp = cos(phi), sp = sin(phi);
		for(unsigned i = 0, M2 = M+1; i <= M; i++) {
			S theta = get_theta(i), ct = cos(theta), st = sin(theta);
			vertices.emplace_back(center.x + radius * ct * cp);
			vertices.emplace_back(center.y + radius * sp);
			vertices.emplace_back(center.z + radius * st * cp);
			auto row = j*M2;
			if(j) {
				int I = i ? i - 1 : M;
				indices.emplace_back(row+i);
				indices.emplace_back(row+I);
				indices.emplace_back(row+I-M2);
				indices.emplace_back(row+I-M2);
				indices.emplace_back(row+i-M2);
				indices.emplace_back(row+i);
			}
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
		for(auto const& px : {p.x, p.y, p.z})
			vertices.emplace_back(S(px));
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

template<class S, class T, class U>
void sheet(std::vector<S> & vertices, std::vector<T> & indices,
	Point<U> a, Point<U> b, Point<U> c, Point<U> d, unsigned M = 100, unsigned N = 100) {
	for(auto i = 0; i <= M; i++) {
		U s = U(i)/M;
		for(auto j = 0; j <= N; j++) {
			U t = U(j)/N;
			auto u = (1-t)*((1-s)*a+s*b) + t*(s*c+(1-s)*d);
			vertices.emplace_back(S(u.x));
			vertices.emplace_back(S(u.y));
			vertices.emplace_back(S(u.z));
			if(i && j) {
				auto index = (i-1)*(N+1)+j-1;
				indices.emplace_back(index);
				indices.emplace_back(index+1);
				indices.emplace_back(index+N+2);
				indices.emplace_back(index+N+2);
				indices.emplace_back(index+N+1);
				indices.emplace_back(index);
			}
		}
	}
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
	float width = 5, height = 5, depth = 10,
			near = 1, far = near + depth,
			mid = (near + far)/2,
			right = width/2, left = -right,
			top = height/2, bottom = -top;
	int wmesh = 126, hmesh = wmesh;
	auto scale = right * 0.25f;

	std::vector<GLfloat> points;
	std::vector<GLuint> indices;
	for(Point<float> p : {
			Point<float>{0, 0, -mid}}) {
#if USE_MODEL == 1
		cube(points, indices, p, right);
#elif USE_MODEL == 2
		sphere(points, indices, p, right, wmesh, hmesh);
#else
		{
			Point<GLfloat> a = {-1, -1, -mid}, b = {1, -1, -mid},
					c = {1, 1, -mid}, d = {-1, 1, -mid};
			sheet(points, indices, a, b, c, d, wmesh, hmesh);
		}
#endif
	}
	auto indicesSize = indices.size(), pointsSize = points.size();

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
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*std::map<unsigned, Joystick> joys;
	//std::map<Uint8, int> axes;
	float dead = .25;
	unsigned nJoysticks = SDL_NumJoysticks(), max_value = (1 << 15) - 1,
			l1 = 0, l2 = 1, r1 = 3, r2 = 4; // TODO query/ask for axis ID's?
	//axes[l1] = axes[l2] = axes[r1] = axes[r2] = 0;
	for(unsigned i = 0; i < nJoysticks; i++) {
		//SDL_Joystick *joy = SDL_JoystickOpen(i);
		//if(joy) joys.emplace(i, joy);
		Joystick joy = { i };
		if(joy) joys.emplace(i, std::move(joy));
	}*/

	auto used = hnd.init(share + "dual.glsl", share + "vert.glsl",
			gl::GL_FRAGMENT_SHADER, share + "frag.glsl");
	if(!used.good()) {
		cout << "Could not build shader program";
		if(used.length()) cout << ":\n  " << used;
		cout << endl;
		return 1;
	}
	/*string texPath = share + "link.png";
	Texture tex(texPath.c_str());
	if(!tex.created) {
		cout << "Not created" << endl;
	} else if(!tex.sourced) {
		cout << "Not sourced" << endl;
	} else {
		cout << "Texture... success?" << endl;
	}*/
	hnd.locate("l", "r", "b", "t", "n", "f", "model[0]", "model[1]")
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

	hnd.project(left, right, bottom, top, near, far);
	hnd.enable(Hnd::e_out);
	//hnd.enable(Hnd::e_info);


	auto stopwatch = Timing::make_stopwatch();
	float theta = 0;
	// Poll/render loop
	for(auto i = 0;; i++, theta += M_PI/8) {
		stopwatch.advance();
		auto polled = hnd.poll();
		if(!polled) {
			if(polled.errored()) cout << "Errored; ";
			else if(polled.warned()) cout << "Warned; ";
			else if(polled.failed()) cout << "Failed; ";
			cout << polled.message << std::endl;
			if(polled.quit()) hnd.joysticks.close_all();

			break;
		}
		// Task
		if(hnd.size()) {
			cout << /*"'" << */ hnd /* << "'" */ << endl;
			hnd.clear();
		}

		double ms_elapsed = stopwatch.update().count()*1000;
		//hnd.update(ms_elapsed);

		// Render
		hnd.frame.clear().draw(vao, GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, nullptr).flip();
		ms_elapsed = stopwatch.update().count()*1000;
		static const double ms_ideal = 1000/60.0;
		double ms_diff = ms_ideal - ms_elapsed;
		if(ms_diff > 0) {
			SDL_Delay(unsigned(ms_diff));
		}
		if((N >= 0) && (i >= N)) break;
	}

	if(hnd.size())
		cout /* << "'" */ << hnd /* << "'" */ << endl;
	//cout << "Frame's message:\n" << hnd.frame << endl;

	// Clean up what isn't done through RAII already
	glDeleteVertexArrays(1, &vao);
}
