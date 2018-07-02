#include <iostream>
#include <iomanip>

#include <SDL_events.h>

#include "view.hpp"
#include "dual.hpp"

int main(int argc, const char *argv[]) {
	using namespace gl;
	using namespace glbinding;

	View::Frame f;
	Binding::initialize(false);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(f.getWindow());
	glClear(GL_COLOR_BUFFER_BIT);
	auto glErr = glGetError();
	if(glErr != GL_NO_ERROR)
		std::cout << "There was a GL ERROR! " << glErr << std::endl;
	SDL_Delay(1000);
	std::cout << f;
}
