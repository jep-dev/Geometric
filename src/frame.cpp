#include <sstream>

#include <glbinding/ContextInfo.h>
#include <glbinding/Meta.h>
#include <glbinding/Version.h>

#include "frame.hpp"

namespace View {

Frame::Frame(void) {
	using std::ostringstream;
	using std::ostream;
	ostringstream oss;

	int init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	do {
		if(init) {
			oss << "Unable to initialize.\n";
			break;
		}
		ctx.set();
		win = SDL_CreateWindow("Title", center, center, 720, 540, flags);
		if(!win) {
			oss << "Unable to create window.\n";
			break;
		} else {
			ctx.ctx = SDL_GL_CreateContext(win);
			if(!ctx.ctx) {
				oss << "Unable to create context.\n";
				break;
			}
			constexpr const char *matches[] = {" doesn't match ", " matches "},
				*plural[] = {"single", "double"},
				*onoff[] = {"off", "on"};
			auto print_profile = [](ostream &lhs, int rhs)
			-> ostream& {
				static constexpr auto compat = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
					core = SDL_GL_CONTEXT_PROFILE_CORE, es = SDL_GL_CONTEXT_PROFILE_ES;
				if(mask_attr(rhs, compat)) lhs << " compatibility";
				if(mask_attr(rhs, core)) lhs << " core";
				if(mask_attr(rhs, es)) lhs << " ES";
				return lhs;
			};
			auto print_version = [=](ostream &lhs, int major, int minor, int mask) -> ostream& {
				return print_profile(lhs << "version " << major << '.' << minor, mask);
			};
			auto print_format = [] (ostream &lhs, int a, int r, int g, int b) -> ostream& {
				return lhs << "ARGB " << a << r << g << b;
			};
			print_version(oss << "Found OpenGL ", ctx.major.get(),
				ctx.minor.get(), ctx.mask.get());
			if(!(ctx.major && ctx.minor && ctx.mask)) print_version(oss << "; expected ",
				ctx.major.set(), ctx.minor.set(), ctx.mask.set());
			oss << ";\n  Buffering: found " << plural[ctx.buffered.get()];
			if(!ctx.buffered) oss << "; expected " << onoff[ctx.buffered.set()];
			print_format(oss << ";\n  Format: found ", ctx.alpha.get(), ctx.red.get(),
				ctx.green.get(), ctx.blue.get());
			if(!(ctx.alpha && ctx.red && ctx.green && ctx.blue))
				print_format(oss << "; expected ", ctx.alpha.set(), ctx.red.set(),
					ctx.green.set(), ctx.blue.set());
			oss << ".";
			glbinding::Binding::initialize(false);
		}
	} while(0);
	status.code = Events::StatusWarn;
	status.message += oss.str();
	status.message += SDL_GetError();
}

Frame& Frame::clear(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	return *this;
}
Frame& Frame::flip(void) {
	SDL_GL_SwapWindow(win);
	return *this;
}

}
