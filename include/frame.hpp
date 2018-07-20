#ifndef FRAME_HPP
#define FRAME_HPP

#include "view.hpp"
#include "attribute.hpp"

namespace View {

	struct Context {
		Major major = {3, &meet_attr};
		Minor minor = {3, &meet_attr};
		Mask mask = {SDL_GL_CONTEXT_PROFILE_CORE, &mask_attr};
		DoubleBuffer buffered = {true, &meet_attr};
		AlphaSize alpha = {8, &ignore_attr};
		RedSize red = {8, &meet_attr};
		GreenSize green = {8, &meet_attr};
		BlueSize blue = {8, &meet_attr};
		SDL_GLContext ctx;
		Context& set(void) {
			set_attrs(major, minor, mask, buffered, alpha, red, green, blue);
			return *this;
		}
		operator SDL_GLContext(void) const { return ctx; }
		virtual ~Context(void) { if(ctx) SDL_GL_DeleteContext(ctx); }
	};

	/** Represents a window and associated GL context. */
	struct Frame {
	public:
		std::string message;
	protected:
		int center = SDL_WINDOWPOS_CENTERED,
			flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
		SDL_Window *win = 0;
		Context ctx;
	public:
		/** Stream insertion operator; inserts the most recent message(s)/error(s). */
		template<class S>
		friend S& operator<<(S &s, Frame const& f) {
			if(f.message.length()) s << "Frame's message: " << f.message << '\n';
			return s;
		}
		/** Const access of the context. */
		SDL_GLContext getContext(void) const { return ctx; }
		/** Const access of the window. */
		SDL_Window *const getWindow(void) const { return win; }
		/** Constructor; currently initializes and sets a handful of hard-coded GL attributes */
		Frame(void);
		/** Destructor; destroys the stored window. */
		~Frame(void) {
			if(win)
				SDL_DestroyWindow(win);
			win = 0;
		}
	};
}

#endif
