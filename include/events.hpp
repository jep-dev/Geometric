#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <utility>

#include <SDL.h>

namespace Events {

	namespace Detail {
		template<class S> S&& (*Fwd)(S &&) = &std::forward<S>;
	}

/* TODO: use Void_t or equivalent to apply SFINAE once per handled event type;
 * with CRTP, the derived type can satisfy the contract of forwarding:
 *
 *   template<class DERIVED, class = void>
 *     Handler -> false_type
 *
 *     template<class DERIVED, class ARG, bool (DERIVED::* F) (ARG)>
 *       Handler<DERIVED, F> -> integral_constant<decltype(F), F>
 *
 *   template<class...>
 *     MultiHandler
 *
 *     template<class... S, class... T>
 *       MultiHandler<Handler<S...>, T...> -> Handler<S...>, MultiHandler<T...>
 *
 */
	template<class C>
	struct Handler {
		template<class... T>
		bool operator()(SDL_QuitEvent const&, T &&...) { return false; }

		template<class... T>
		bool operator()(SDL_Event const& ev, T &&... t) {
			using namespace Detail;
			auto &self = static_cast<C&>(*this);
			switch(ev.type) {
				case SDL_CONTROLLERAXISMOTION: return self(ev.caxis, Fwd<T>(t)...);
				case SDL_CONTROLLERBUTTONDOWN:
				case SDL_CONTROLLERBUTTONUP: return self(ev.cbutton, Fwd<T>(t)...);
				case SDL_CONTROLLERDEVICEADDED:
				case SDL_CONTROLLERDEVICEREMOVED:
				case SDL_CONTROLLERDEVICEREMAPPED: return self(ev.cdevice, Fwd<T>(t)...);
				case SDL_DOLLARGESTURE: return self(ev.dgesture, Fwd<T>(t)...);
				case SDL_JOYAXISMOTION: return self(ev.jaxis, Fwd<T>(t)...);
				case SDL_JOYBALLMOTION: return self(ev.jball, Fwd<T>(t)...);
				case SDL_JOYBUTTONDOWN: return self(ev.jbutton, Fwd<T>(t)...);
				case SDL_JOYDEVICEADDED:
				case SDL_JOYDEVICEREMOVED: return self(ev.jdevice, Fwd<T>(t)...);
				case SDL_JOYHATMOTION: return self(ev.jhat, Fwd<T>(t)...);
				case SDL_KEYDOWN:
				case SDL_KEYUP: return self(ev.key, Fwd<T>(t)...);
				case SDL_MOUSEMOTION: return self(ev.motion, Fwd<T>(t)...);
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP: return self(ev.button, Fwd<T>(t)...);
				case SDL_MOUSEWHEEL: return self(ev.wheel, Fwd<T>(t)...);
				case SDL_MULTIGESTURE: return self(ev.mgesture, Fwd<T>(t)...);
				case SDL_QUIT: return self(ev.quit, Fwd<T>(t)...);
				case SDL_SYSWMEVENT: return self(ev.syswm, Fwd<T>(t)...);
				case SDL_TEXTEDITING: return self(ev.edit, Fwd<T>(t)...);
				case SDL_TEXTINPUT: return self(ev.text, Fwd<T>(t)...);
				case SDL_WINDOWEVENT: return self(ev.window, Fwd<T>(t)...);
				default: return true;
			}
		}
		template<class... T>
		bool poll(T &&... t) {
			using namespace Detail;
			auto& self = static_cast<C&>(*this);
			SDL_Event ev;
			while(SDL_PollEvent(&ev)) {
				if(!self(ev, Fwd<T>(t)...))
					return false;
			}
			return true;
		}
	};
}

#endif
