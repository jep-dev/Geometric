#ifndef EVENTS_HPP
#define EVENTS_HPP

///@cond
#include <utility>
///@endcond

#include <SDL.h>

namespace Events {

namespace Detail {
	template<class S> S&& (*Fwd)(S &&) = &std::forward<S>;
}
enum EStatus {
	StatusPass = 1, StatusQuit = StatusPass << 1, StatusError = StatusQuit << 1,
		StatusWarn = StatusPass | StatusError, StatusFail = StatusQuit | StatusError
};
struct Status {
	uint32_t code = StatusPass;
	std::string message = "";
	uint64_t timestamp = SDL_GetPerformanceCounter();
	std::size_t length(void) const { return message.length(); }

	static bool passed(uint32_t h) { return h & StatusPass; }
	static bool quit(uint32_t h) { return h & StatusQuit; }
	static bool errored(uint32_t h) { return h & StatusError; }
	static bool warned(uint32_t h) { return (h & StatusWarn) == StatusWarn; }
	static bool failed(uint32_t h) { return (h & StatusFail) == StatusFail; }
	static bool bad(uint32_t h) { return quit(h) || errored(h); }
	static bool good(uint32_t h) { return passed(h) && !bad(h); }
	template<class S>
	friend S& operator<<(S &lhs, Status const& rhs) {
		return lhs << rhs.message, lhs;
	}

	operator bool(void) const { return good(code); }
};

template<class C>
struct Handler {
	template<class... T>
	Status operator()(SDL_QuitEvent const&, T &&...) { return {StatusQuit}; }

	template<class... T>
	Status operator()(SDL_Event const& ev, T &&... t) {
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
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP: return self(ev.jbutton, Fwd<T>(t)...);
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
			default: return {};
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
