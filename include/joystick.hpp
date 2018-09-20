#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include <utility>

std::pair<float, float> deadzone(float x, float y, float dead) {
	float R = x*x + y*y, r, dr;
	if(R == 0) return {0, 0};
	R = sqrt(R);
	r = (R - dead) / (1 - dead);
	if(r < 0) return {0, 0};
	dr = r / R;
	return {x * dr, y * dr};
}

std::pair<float, float> deadzone(int x, int y, int max_value, float dead) {
	float fx = float(x) / max_value, fy = float(y) / max_value;
	return deadzone(fx, fy, dead);
}

struct Joystick {
	template<class S = Uint8, class T = S>
	using History = std::pair<S,T>;
	SDL_JoystickID id;
	SDL_Joystick *joystick = 0;
	std::map<unsigned, float> axes;
	std::map<unsigned, History<>> buttons;
	History<> hat;

	operator SDL_Joystick*(void) const { return joystick; }
	operator bool(void) const //{ return joystick; }
		{ return SDL_JoystickGetAttached(joystick); }
	void open(void) {
		open(id);
	}
	bool open(unsigned i) {
		close();
		id = i;
		joystick = SDL_JoystickOpen(id);
		return joystick;
		//return *this;
	}
	void close(void) {
		//if(SDL_JoystickGetAttached(joystick))
			SDL_JoystickClose(joystick);
		joystick = 0;
	}
	Joystick(unsigned i) : id(id) {
		open();
	}
	virtual ~Joystick(void) {
		close();
	}
};

#endif
