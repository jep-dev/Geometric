#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>

#include <SDL.h>

#include "joystick.hpp"
#include "parse.hpp"

#define INIT SDL_INIT_JOYSTICK

bool handle(SDL_KeyboardEvent const& ev) {
	switch(ev.keysym.sym) {
		case SDLK_ESCAPE:
		case SDLK_q:
			return false;
		default:
			return true;
	}
}
bool handle(SDL_Event const& ev) {
	switch(ev.type) {
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			return handle(ev.key);
		default:
			return true;
	}
}

int main(int argc, const char *argv[]) {
	using std::cout;
	using std::endl;
	using ISS = std::istringstream;

	int max_value = (1 << 15) - 1;
	float dead = .25;
	/*for(int i = 0; i < 10; i++) {
		int x = max_value * i / 10, y = x;
		auto p = deadzone(x, y, max_value, dead);
		cout << "For x = " << x << ", y = " << y << ", deadzone = " << dead << endl;
		cout << "\tadjusted = {" << p.first << ", " << p.second << "}" << endl;
	}
	return 0;*/


	int default_delay = 100, seconds = 5, default_iterations = seconds * 1000 / default_delay,
			iterations = default_iterations, delay = default_delay;
	if(argc > 1)
		iterations = Streams::parse_positive(argv[1], default_iterations);
	if(argc > 2)
		delay = Streams::parse_positive(argv[2], default_delay);

	int out = 0;
	bool cont = true;
	std::map<unsigned, SDL_Joystick*> joys;

	SDL_Init(INIT);
	do {
		SDL_ClearError();
		if((SDL_WasInit(0) & INIT) != INIT) {
			cout << "Could not initialize the joystick subsystem." << endl;
			out = 1;
			break;
		}

		unsigned nJoysticks = SDL_NumJoysticks();
		for(unsigned i = 0; i < nJoysticks; i++) {
			SDL_Joystick *joy = SDL_JoystickOpen(i);
			if(joy) joys.emplace(i, joy);
		}
		cout << "Out of " << nJoysticks << " joysticks, " << joys.size()
				<< " could be opened." << endl;
		if(!joys.size()) {
			out = 1;
			break;
		}
	} while(0);

	if(out) {
		SDL_Quit();
		return out;
	}

	std::map<Uint8, int> axes;
	unsigned l1 = 0, l2 = 1, r1 = 3, r2 = 4;
	axes[l1] = axes[l2] = axes[r1] = axes[r2] = 0;
	while(cont) {
		SDL_Event ev;
		while(SDL_PollEvent(&ev)) {
			if(!handle(ev)) {
				cont = false;
				break;
			}
			switch(ev.type) {
				case SDL_JOYAXISMOTION: {
					auto const& jaxis = ev.jaxis;
					if(jaxis.which == 0)
						axes[jaxis.axis] = jaxis.value;
				} break;
				default:
					break;
			}
		}
		auto lval = deadzone(axes[l1], axes[l2], max_value, dead),
				rval = deadzone(axes[r1], axes[r2], max_value, dead);
		cout << std::fixed << std::setprecision(3);
		cout << " Left stick: {" << lval.first << ", " << lval.second << "}" << endl;
		cout << "Right stick: {" << rval.first << ", " << rval.second << "}" << endl;
		iterations--;
		if(!iterations) break;
		SDL_Delay(delay);
	}
	for(auto const& joy : joys)
		SDL_JoystickClose(joy.second);
	return SDL_Quit(), out;

}
