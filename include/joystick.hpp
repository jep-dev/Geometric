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

#endif
