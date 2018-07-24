#ifndef FRAME_HPP
#define FRAME_HPP

#include "view.hpp"
#include "events.hpp"
#include "context.hpp"

namespace View {

/** Represents a window and associated GL context. */
struct Frame {
public:
	int flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	Events::Status status;
protected:
	static constexpr int center = SDL_WINDOWPOS_CENTERED;
	SDL_Window *win = 0;
	Context ctx;
public:
	/** Stream insertion operator; inserts the most recent message(s)/error(s). */
	template<class S>
	friend S& operator<<(S &s, Frame const& f) {
		if(f.status.message.length()) s << f.status.message;
		return s;
	}
	Frame& clear(void);
	Frame& draw(void);
	/** Const access of the context. */
	SDL_GLContext getContext(void) const { return ctx; }
	/** Const access of the window. */
	SDL_Window *const getWindow(void) const { return win; }
	/** Constructor; currently initializes and sets a handful of hard-coded GL attributes */
	Frame(void);
	/** Destructor; destroys the stored window. */
	virtual ~Frame(void) {
		if(win)
			SDL_DestroyWindow(win);
		win = 0;
	}
};

}

#endif
