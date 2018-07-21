#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "attribute.hpp"

namespace View {

struct Context {
	Major major = {3, &meet_attr};
	Minor minor = {3, &meet_attr};
	Mask mask = {SDL_GL_CONTEXT_PROFILE_CORE, &mask_attr};
	DoubleBuffer buffered = {true, &meet_attr};
	AlphaSize alpha = {8, &meet_attr};
	RedSize red = {8, &meet_attr};
	GreenSize green = {8, &meet_attr};
	BlueSize blue = {8, &meet_attr};
	SDL_GLContext ctx;
	Context& set(void);
	operator SDL_GLContext(void) const;
	virtual ~Context(void) { if(ctx) SDL_GL_DeleteContext(ctx); }
};

}

#endif
