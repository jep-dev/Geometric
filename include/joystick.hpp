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
	template<class S> using History = std::pair<S,S>;
	using uHistory = History<Uint8>;
	SDL_JoystickID id;
	SDL_Joystick *joystick = 0;
	std::map<unsigned, float> axes;
	std::map<unsigned, uHistory> buttons;
	uHistory hat;

	operator SDL_Joystick*(void) const { return joystick; }
	operator bool(void) const //{ return joystick; }
		{ return SDL_JoystickGetAttached(joystick); }

	Joystick& setAxis(unsigned i, float v) {
		return axes.find(i) -> second = v, *this;
	}

	void open(void) {
		open(id);
	}
	bool open(unsigned i) {
		//close();
		id = i;
		joystick = SDL_JoystickOpen(id);
		return *this;
	}
	void close(void) {
		if(joystick && SDL_JoystickGetAttached(joystick))
			SDL_JoystickClose(joystick);
		joystick = 0;
	}
	Joystick(unsigned i) {
		open(i);
	}
	Joystick(void) {}
	virtual ~Joystick(void) {}
};

struct JoystickTable: std::map<Uint8, Joystick> {
	bool contains(unsigned i) const {
		return find(i) != cend();
	}
	bool open(unsigned i) {
		if(contains(i))
			return (*this)[i].open(i);
		Joystick j = {i};
		if(!j) return false;
		emplace(i, std::move(j));
		return true;
	}
	void close(unsigned i) {
		if(contains(i))
			find(i) -> second.close();
	}
	void close_all(void) {
		for(auto it = begin(), e = end(); it != e; it++) {
			it -> second.close();
		}
	}
};

#endif
