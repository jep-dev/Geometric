#ifndef VIEW_HPP
#define VIEW_HPP

#include <functional>
#include <string>
//#include <iostream>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include <SDL.h>
//#include <SDL_video.h>

namespace View {
	/*
	template<SDL_GLattr A, class CMP = std::equal_to<int>>
	struct Attribute;
	*/
	template<SDL_GLattr A>
	struct Attribute;

	template<class T>
	struct CompareBit {
		bool operator()(T s, T t) {
			return (s & t) == t;
		}
	};
	template<class T>
	struct Ambivalent {
		bool operator()(T, T) const { return true; }
	};
	using Major = Attribute<SDL_GL_CONTEXT_MAJOR_VERSION>;
	using Minor = Attribute<SDL_GL_CONTEXT_MINOR_VERSION>;
	using Mask = Attribute<SDL_GL_CONTEXT_PROFILE_MASK>;
	using DoubleBuffer = Attribute<SDL_GL_DOUBLEBUFFER>;
	/*using Minor = Attribute<SDL_GL_CONTEXT_MINOR_VERSION, std::greater_equal<int>>;
	using Mask = Attribute<SDL_GL_CONTEXT_PROFILE_MASK, CompareBit<int>>;
	using DoubleBuffer = Attribute<SDL_GL_DOUBLEBUFFER, Ambivalent<int>>;*/
	using AlphaSize = Attribute<SDL_GL_ALPHA_SIZE>;
	using RedSize = Attribute<SDL_GL_RED_SIZE>;
	using GreenSize = Attribute<SDL_GL_GREEN_SIZE>;
	using BlueSize = Attribute<SDL_GL_BLUE_SIZE>;

	int get_attr(SDL_GLattr attr) {
		int out;
		SDL_GL_GetAttribute(attr, &out);
		return out;
	}
	void set_attr(SDL_GLattr key, int value) {
		SDL_GL_SetAttribute(key, value);
	}
	bool ignore_attr(int, int) { return true; }
	bool ignore_attr(SDL_GLattr, int) { return true; }
	bool match_attr(int lhs, int rhs) { return lhs == rhs; }
	bool meet_attr(int lhs, int meet) { return lhs >= meet; }
	bool match_attr(SDL_GLattr key, int in) { return match_attr(in, get_attr(key)); }
	bool mask_attr(int lhs, int mask) { return match_attr(lhs & mask, mask); }
	bool mask_attr(SDL_GLattr key, int in) { return mask_attr(get_attr(key), in); }

	//template<SDL_GLattr A, class CMP>
	template<SDL_GLattr A>
	struct Attribute {
		typedef SDL_GLattr key_type;
		typedef int value_type;
		//typedef CMP compare_type;
		typedef bool (compare_type) (int, int);

		static constexpr key_type key = A;
		int value;
		//compare_type compare;
		compare_type *compare = &match_attr;

		template<class S>
		int set(S const& s) { return value = s; }
		int set(void) const { return value; }
		int get(void) const { return get_attr(A); }
		operator bool(void) const { return compare(set(), get()); }
		bool operator&&(bool const& rhs) const { return bool(*this) && rhs; }
		bool operator||(bool const& rhs) const { return bool(*this) || rhs; }
		template<SDL_GLattr B>
		bool operator&&(Attribute<B> const& rhs) const { return *this && bool(rhs); }
		template<SDL_GLattr B>
		bool operator||(Attribute<B> const& rhs) const { return *this || bool(rhs); }
		template<class S>
		S& operator>>(S &s) { return s = value; }
	};
	/** Makes an attribute out of the given value-comparator pair, but can't form one
	 * out of a reference (requires refactoring to support captures) */
	template<SDL_GLattr A, class CMP>
	auto make_attr(int value, CMP && comp = {})
		-> Attribute<A> { return { value, std::forward<CMP>(comp) }; };


/* TODO list:
 *   take a page from <functional>; how to compose/curry?
 *   fix false negative for M>3, N<3 (e.g. 4.0) which evaluates as false.
 */

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
		set_attr(Major::key, major.value);
		set_attr(Minor::key, minor.value);
		set_attr(Mask::key, mask.value);
		set_attr(DoubleBuffer::key, buffered.value);
		set_attr(AlphaSize::key, alpha.value);
		set_attr(RedSize::key, red.value);
		set_attr(GreenSize::key, green.value);
		set_attr(BlueSize::key, blue.value);
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
		return f.message.length() ? (s << "Frame's message:\n" << f.message, s) : s;
		/*if(f.message.length())
			s << "Frame's message:\n" << f.message;
		return s;*/
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
