#include <sstream>

#include <glbinding/ContextInfo.h>
#include <glbinding/Meta.h>
#include <glbinding/Version.h>

#include "frame.hpp"

namespace View {

Frame::Frame(int w, int h, int x, int y, int flags): flags(flags) {
	using std::ostringstream;
	using std::ostream;
	ostringstream oss;

	do {
		ctx.set();
		win = SDL_CreateWindow("Title", x, y, w, h, flags);
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
				*plural[] = {"Single", "Double"},
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
			print_version(oss << "  Found OpenGL ",
					ctx.major.get(), ctx.minor.get(), ctx.mask.get());
			if(ctx.major && ctx.minor && ctx.mask) oss << " (matches);\n";
			else print_version(oss << " (expected ",
					ctx.major.set(), ctx.minor.set(), ctx.mask.set()) << ");\n";
			oss << "  Found " << plural[ctx.buffered.get()] << " buffering";
			if(ctx.buffered) oss << " (matches);\n";
			else oss << " (expected " << plural[ctx.buffered.set()] << ");\n";

			print_format(oss << "  Format: found ",
					ctx.alpha.get(), ctx.red.get(), ctx.green.get(), ctx.blue.get());
			if(ctx.alpha && ctx.red && ctx.green && ctx.blue)
				oss << " (matches).\n";
			else print_format(oss << " (expected ", ctx.alpha.set(), ctx.red.set(),
					ctx.green.set(), ctx.blue.set()) << ").";
			// TODO initialize GLBinding from Presenter?
			glbinding::Binding::initialize(false);
		}
	} while(0);
	status.code = Events::StatusWarn;
	status.message += oss.str();
	status.message += SDL_GetError();
}

Frame& Frame::clear(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return *this;
}
Frame& Frame::flip(void) {
	SDL_GL_SwapWindow(win);
	return *this;
}

}
