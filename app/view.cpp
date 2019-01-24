#include <iostream>
#include <iomanip>

#include "initializer.hpp"
#include "presenter.hpp"

#include "resource.hpp"
#include "streams.tpp"
#include "timing.hpp"

#include "quaternion.tpp"
#include "dual.tpp"
#include "point.hpp"
#include "surface.hpp"

#define USE_MODEL 0

#ifndef SUBSYSTEMS
#define SUBSYSTEMS SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK
// Joystick, haptics, and gamecontroller cause udev error!
// Audio, timer, and video are safe. Burn the error to recover.
#endif

/*#define USE_DEVIL
#include "texture.hpp"
Events::Status test_texture(void) {
	string texPath = share + "link.png";
	Texture tex(texPath.c_str());
	// Currently breaks here, as in, GL will not create a texture.
	// Maybe it doesn't identify it as a texture if it does?
	if(!tex.created)
		return { Events::StatusError };
	if(!tex.sourced)
		return { Events::StatusWarn };
	return {};
}*/

struct Hnd;
struct Hnd: Presenter<Hnd> {
	bool fullscreen = true;
	typedef enum {
		e_out=0, e_info, n_streams
	} e_stream;
	DualQuaternion<float> orientation = {1}, translation = {1},
			transform = orientation * translation;
	Streams::OStringStream streams[n_streams];
	bool enabled[n_streams] = { false };
	template<class... T>
	Hnd& enable(e_stream s, T... t) { return enabled[s] = true, enable(t...); }
	Hnd& enable(void) { return *this; }
	template<class... T>
	Hnd& disable(e_stream s, T... t) { return enabled[s] = true, enable(t...); }
	Hnd& disable(void) { return *this; }


	std::size_t size(void) const {
		std::size_t total = 0;
		for(auto i = 0; i < n_streams; i++) {
			if(!enabled[i]) continue;
			auto size = streams[i].size();
			// Observe failure conditions resulting in size=-1
			if(size > 0) total += size;
		}
		return total;
	}
	Hnd& update(void) {
		if(!joysticks.size()) return *this;
		auto & first = joysticks.begin() -> second;
		auto & buttons = first.buttons;
		auto button = buttons.find(7);
		if(button != buttons.end()) {
			if(button -> second.first != button -> second.second) {
				if(button -> second.second) {
					if(fullscreen) frame.restore();
					else frame.maximize();
					fullscreen = !fullscreen;
				}
			}
		}
		DualQuaternion<float> t = 1_e, r = 1_e;
		float theta = 0, phi = 0, psi = 0, x = 0, z = 0;
		x = first.axes[0];
		z = first.axes[1];
		theta = first.axes[3];
		phi = first.axes[4];
		psi -= (1+first.axes[2])/2;
		psi += (1+first.axes[5])/2;

		auto dead1 = .25, dead2 = .1, dead3 = .1;
		auto xz = deadzone(x, z, dead1), pt = deadzone(theta, phi, dead2),
				pp = deadzone(psi, psi, dead3);
		orientation = rotation<float>(pt.first * M_PI/32, 0, 1, 0)
				* rotation<float>(pt.second * M_PI/32, -1, 0, 0)
				* rotation<float>(pp.first * M_PI/32, 0, 0, 1)
				* orientation;
		translation = ((1_e - .1_I * xz.first - .1_K * xz.second) ^ *orientation) * translation;
		if(streams[e_out].tellp() > 0) streams[e_out] << std::endl;
		streams[e_out] << transform;
		set_view(transform = orientation * translation);
		return *this;
	}

	using Handler::operator();

	Events::Status operator()(SDL_KeyboardEvent const& k) {
		using namespace gl;
		using Streams::center;
		using DQ = decltype(transform);
		locate("model.u", "model.v", "view.u", "view.v");
		GLint uv_model[] = {locations["model.u"], locations["model.v"]},
			uv_view[] = {locations["view.u"], locations["view.v"]};
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
				case SDLK_EQUALS:
					set_view(transform = {1});
					project(-2.5, 2.5, -2.5, 2.5, 1, 10);
					break;
				default: break;
			}
		} else {
			bool print_location = false, print_projection = false,
				print_model = true, print_view = false;
			auto old_transform = transform;
			const char *pressed = "";
			switch(k.keysym.sym) {
				case SDLK_RETURN:
					print_view = print_model = true;
					break;
				default: break;
			}
			set_view(transform);

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
			if(print_model && uv_model[0] >= 0 && uv_model[1] >= 0) {
				GLfloat u[4] = {0}, v[4] = {0};
				glGetUniformfv(program, uv_model[0], u);
				glGetUniformfv(program, uv_model[1], v);

				DualQuaternion<float> d = {u[0], u[1], u[2], u[3], v[0], v[1], v[2], v[3]};
				if(streams[e_out].tellp() > 0) streams[e_out] << '\n';
				streams[e_out] << "{" << uv_model[0] << ", " << uv_model[1] << "}: "
					"Model = " << std::string(d) << std::endl;
			}

			if(print_view && uv_view[0] >= 0 && uv_view[1] >= 0) {
				GLfloat u[4] = {0}, v[4] = {0};
				glGetUniformfv(program, uv_view[0], u);
				glGetUniformfv(program, uv_view[1], v);
				DualQuaternion<float> d = {u[0], u[1], u[2], u[3], v[0], v[1], v[2], v[3]};
				if(streams[e_out].tellp() > 0) streams[e_out] << '\n';
				streams[e_out] << "{" << uv_view[0] << ", " << uv_view[1] << "}: "
					"View = " << std::string(d) << std::endl;
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
		for(auto & i : streams)
			i.str("");
		return *this;
	}
	template<class S>
	friend S& operator<<(S& s, Hnd const& hnd) {
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
	using Presenter<Hnd>::Presenter;
};

int main(int argc, const char *argv[]) {
	using namespace std;
	using namespace View;
	using namespace glbinding;

	map<string, bool> models = {
		{"sphere", false},
		{"cylinder", false},
		{"surface", false},
		{"surface2", false},
		{"surface3", false},
		{"field", false},
		{"fieldcube", false}
	};
	string nos[] = {"--no-", "-n"};
	bool met_yes = false;
	for(auto i = 1; i < argc; i++) {
		string arg = argv[i];
		bool met_no = false;
		for(auto const& no : nos) {
			auto pos = arg.find(no);
			if(pos != 0) continue;
			arg = arg.substr(no.length());
			models[arg] = false;
			met_no = true;
			break;
		}
		if(!met_no) models[arg] = met_yes = true;
	}
	if(!met_yes) models["sphere"] = true;

	for(auto const& it : models) {
		cout << "models[" << it.first << "] = " << boolalpha << it.second << endl;
	}

	// Initialize projection matrix values and vertices
	float width = 5, height = 5, depth = 30,
			near = 1, far = near + depth,
			mid = (near + far)/2,
			right = width/2, left = -right,
			top = height/2, bottom = -top;
	int wmesh = 64, hmesh = wmesh;
	auto scale = right;

	std::vector<GLfloat> points;
	std::vector<GLuint> indices;
	unsigned indicesSize = 0, offset = 0;
	Point<float> p = {0, 0, -mid};
	if(models["cylinder"]) {
		offset = cylinder(points, indices, p - scale*1_y, p,
				scale, wmesh, hmesh, offset);
		offset = cylinder(points, indices, p, p + scale*1_y,
				scale, wmesh, hmesh, offset);
	}
	if(models["sphere"]) {
		offset = sphere(points, indices, p - scale*1_y, scale, wmesh, hmesh, offset);
		offset = sphere(points, indices, p + scale*1_y, scale, wmesh, hmesh, offset);
	}
	if(models["surface"]) {
		float theta = M_PI/5;
		float phi = theta;
		auto north = rotation<float>(phi, scale, 0, 0),
			south = rotation<float>(-phi, scale, 0, 0),
			west = rotation<float>(theta, 0, scale, 0),
			east = rotation<float>(-theta, 0, scale, 0),
			nw = north * west, ne = north * east,
			sw = south * west, se = south * east;
		offset = surface(points, indices, ne + 1_I + 1_J, nw - 1_I + 1_J,
			se + 1_I - 1_J, sw - 1_I - 1_J, -scale/2*1_z, p,
			wmesh, hmesh, offset);
		offset = surface(points, indices, ne + 1_I + 1_J - 1_K, nw - 1_I + 1_J - 1_K,
			se + 1_I - 1_J - 1_K, sw - 1_I - 1_J - 1_K,
			-scale/2*1_z, p, wmesh, hmesh, offset);
	}
	if(models["surface2"]) {
		float theta = M_PI/5;
		float phi = theta;
		DualQuaternion<float> north = rotation(phi, scale, 0, 0),
			south = rotation(-phi, scale, 0, 0),
			west = rotation(theta, 0, scale, 0),
			east = rotation(-theta, 0, scale, 0),
			nw = north * west, ne = north * east,
			sw = south * west, se = south * east;
		/*offset = surface2(points, indices, {{nw - 1_I + 1_J, ne + 1_I + 1_J},
			{sw - 1_I - 1_J, se + 1_I - 1_J}}, -scale/2*1_z, p,
			wmesh, hmesh, offset);*/
		offset = surface2(points, indices, {{nw - scale*(1_I+1_J), sclerp(nw, ne, .5f), ne},
			{sclerp(nw, sw, .5f), sclerp(sclerp(nw, ne, .5f), sclerp(sw, se, .5f), .5f),
				sclerp(ne, se, .5f)},
			{sw, sclerp(sw, se, .5f), se}}, -scale/2*1_z, p,
			wmesh, hmesh, offset);
	}
	if(models["surface3"]) {
		offset = surface3(points, indices, {{scale*(-1_x+1_y), scale*1_y, scale*(1_x+1_y)},
			{scale*-1_x, 0_x, scale*1_x},
			{scale*(-1_x-1_y), scale*-1_y, scale*(1_x-1_y)}}, p,
			wmesh, hmesh, offset);
	}
	if(models["field"]) {
		/*auto fn = [=] (float u, float v) -> DualQuaternion<float> {
			return translation<float>(u*scale, v*scale,
					//scale/4*sin(float(8*M_PI*sqrt(u*u+v*v))));
					scale/4*sin(8*M_PI*sqrt(u*u+v*v)));
		};*/
		auto fn = [=] (float u, float v, float w) -> DualQuaternion<float> {
			return translation<float>(u*4*scale, v*4*scale, //w*scale);
					//scale/4*sin(float(8*M_PI*sqrt(u*u+v*v))));
					scale/4*sin(8*M_PI*sqrt(u*u+v*v)));
		};
		offset = field(points, indices, fn,
				Point<float>{-1, -1, 0}, Point<float>{2, 0, 0}, Point<float>{0, 2, 0},
				p, wmesh, hmesh, offset);
	}
	if(models["fieldCube"]) {
		auto fn = [=] (float u, float v, float w) -> DualQuaternion<float> {
			Point<float> a = {scale*u, scale*v, scale*w};
			a = a ^ rotation<float>(M_PI/4, 0, 1, 0);
			a = a ^ rotation<float>(M_PI/8, 1, 0, 0);
			a = a ^ rotation<float>(v*M_PI/8, 0, 1, 0);
			return translation<float>(a.x, a.y, a.z);
			//auto a = (cos(M_PI*2*(u+v+w))+3)/4;
			//return trans_rotation<float>(u*scale, v*scale, w*scale, M_PI/16*(u+v+w), 0, 0, 1);
			//return translation<float>(u*scale, v*scale, w*scale);
		};
		offset = fieldCube(points, indices, fn, Point<float>{-1, -1, -1},
				Point<float>{2, 0, 0}, Point<float>{0, 2, 0}, Point<float>{0, 0, 2},
				p, wmesh, hmesh, offset);
	}
	indicesSize = indices.size();

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

	SDL_DisplayMode mode;
	SDL_GetDesktopDisplayMode(0, &mode);

	Hnd hnd(mode.w, mode.h);
	cout << hnd.frame << endl;

	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// TODO results in a GL_INVALID_ENUM
	// glEnable(GL_TEXTURE_2D);

	auto used = hnd.init(share + "dual.glsl", share + "uv_vert.glsl",
			gl::GL_FRAGMENT_SHADER, share + "uv_frag.glsl");
	if(!used.good()) {
		cout << "Could not build shader program";
		if(used.length()) cout << ":\n  " << used;
		cout << endl;
		return 1;
	}
	hnd.locate("l", "r", "b", "t", "n", "f")
		.locate("model.u", "model.v", "view.u", "view.v")
		.project(left, right, bottom, top, near, far);

	// Create and initialize vertex array and buffer
	GLuint vao;
	Buffer<2> vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	bufferData(GL_ARRAY_BUFFER, vbo[0], points, GL_STATIC_DRAW);
	vector<GlslAttribute> attributes = {
		{"pos_in", hnd.program, 0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0},
		{"uv_in", hnd.program, 1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*) 9}};
	for(auto & attr : attributes) {
		cout << "Attrib '" << attr.name << "' (" << attr << ") "
				"has size " << attr.size() << ", "
				"stride " << attr.stride() << (attr.stride() ? "" : " (contiguous)") << ", "
				"and type " << attr.type() << endl;
	}

	bufferData(GL_ELEMENT_ARRAY_BUFFER, vbo[1], indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);

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

		stopwatch.update();
		double ms_elapsed; // = stopwatch.count()*1000;
		hnd.update();

		// Render
		hnd.frame.clear().draw(vao, GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, nullptr).flip();

		ms_elapsed = stopwatch.update().count()*1000;
		static const double ms_ideal = 1000/60.0;
		double ms_diff = ms_ideal - ms_elapsed;

		if(ms_diff > 0) SDL_Delay(unsigned(ms_diff));
	}

	if(hnd.size())
		cout /* << "'" */ << hnd /* << "'" */ << endl;
	//cout << "Frame's message:\n" << hnd.frame << endl;

	// Clean up what isn't done through RAII already
	glDeleteVertexArrays(1, &vao);
}
