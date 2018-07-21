#include "context.hpp"

namespace View {

Context& Context::set(void) {
	set_attrs(major, minor, mask, buffered, alpha, red, green, blue);
	return *this;
}
Context::operator SDL_GLContext(void) const {
	return ctx;
}

}
