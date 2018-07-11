#include <sstream>

#include <glbinding/ContextInfo.h>
#include <glbinding/Meta.h>
#include <glbinding/Version.h>

#include "view.hpp"

namespace View {

Frame::Frame(void) {
	using std::ostringstream;
	using std::ostream;
	ostringstream oss;

	int init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	/*SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);*/

	static constexpr auto
		key_major = SDL_GL_CONTEXT_MAJOR_VERSION,
			key_minor = SDL_GL_CONTEXT_MINOR_VERSION,
		key_doubled = SDL_GL_DOUBLEBUFFER,
		key_alpha = SDL_GL_ALPHA_SIZE, key_red = SDL_GL_RED_SIZE,
			key_green = SDL_GL_GREEN_SIZE, key_blue = SDL_GL_BLUE_SIZE;
	static constexpr int
		value_major = 3, value_minor = 3, value_mask = SDL_GL_CONTEXT_PROFILE_CORE,
		value_buffered = 1, value_alpha = 8, value_red = 8, value_green = 8, value_blue = 8;


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
			oss << ".\n";
		}
	} while(0);
	message += oss.str();
	message += SDL_GetError();
}

}
