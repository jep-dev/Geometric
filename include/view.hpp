#ifndef VIEW_HPP
#define VIEW_HPP

#include <functional>
#include <string>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include <SDL.h>
//#include <SDL_video.h>

namespace View {

	/** Represents a GL attribute key, required value, and the comparator between values. */
	template<SDL_GLattr A, template<class, class...> class CMP = std::equal_to, class... T>
	struct Attribute {
		int required = 0;
		/** Applies the comparator to the required and current values. */
		operator bool(void) const {
			int current = 0;
			SDL_GL_GetAttribute(A, &current);
			return CMP<int, T...>()(current, required);
		}
	};
	/* TODO: Consider typical usage for version requirements:
	 *   Attribute<Major, greater_equal>{M} + Attribute<Minor, greater_equal>{N}
	 *   -- True negative for: Major==M && Minor<N || Major<M
	 *   -- True positive for: Major>=M && Minor>=N
	 *   -- False negative for Major>=M && Minor<N
	 *   -- (No false positive)
	 * Addition for booleans makes little sense; try boolean operators:
	 *   Attr<Major, ge>{M} || (Attr<Major, eq>{M} && Attr<Minor, ge>{N})
	 * Composition should distribute the operators to evaluations:
	 *   operator||(Attr<A...> lhs, Attr<B...> rhs) ->
	 *     Or<Attr<A...>,Attr<B...>> {
	 *       operator bool(void) const { return bool(lhs) || bool(rhs); }
	 *     }
	 */

	/** Represents a window and associated GL context. */
	struct Frame {
	protected:
		int center = SDL_WINDOWPOS_CENTERED,
			flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
		SDL_Window *win = 0;
		SDL_GLContext ctx;
		std::string message;
	public:
		/** Stream insertion operator; inserts the most recent message(s)/error(s). */
		template<class S>
		friend S& operator<<(S &s, Frame const& f) {
			return s << f.message, s;
		}
		/** Const access of the context. */
		SDL_GLContext const& getContext(void) const { return ctx; }
		/** Const access of the window. */
		SDL_Window *const getWindow(void) const { return win; }
		/** Constructor; currently initializes and sets a handful of hard-coded GL attributes */
		Frame(void) {
			int init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

			if(init) {
				message = "Unable to initialize.\n";
				return;
			}
			win = SDL_CreateWindow("Title", center, center, 720, 540, flags);
			if(!win) {
				message += "Unable to create window.\n";
			} else {
				ctx = SDL_GL_CreateContext(win);
				if(!ctx) {
					message += "Unable to create context.\n";
				} else {
					int major, minor;
					std::cout << (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major), major)
						<< '.' << (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor), minor)
						<< "\n";
				}
			}
			message += SDL_GetError();
		}
		/** Destructor; destroys the stored window. */
		~Frame(void) {
			if(win)
				SDL_DestroyWindow(win);
			win = 0;
		}
	};
}

#endif
