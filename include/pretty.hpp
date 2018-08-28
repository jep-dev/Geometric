#ifndef PRETTY_HPP
#define PRETTY_HPP

#include <string>

// Future support for reflection may add "typename(type)" to replace all of this.

#if defined(__unix__)
#define HAS_PRETTY true
#define PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_WIN32) || defined(_WIN64)
#define HAS_FUNCSIG true
#define PRETTY_FUNCTION __FUNCSIG__
#else
#define PRETTY_FUNCTION __func__
#endif

/** Very simple function for retrieving a signature containing the given template arguments. */
template<class... T> constexpr auto pretty(void) -> const char * { return PRETTY_FUNCTION; }
/** Very simple function for retrieving a signature containing the given arguments' types. */
template<class... T> constexpr auto pretty(T &&... t) -> const char * { return PRETTY_FUNCTION; }
/** Provides a filtered stream insertion operator to represent types as strings. */
template<class... T> struct Pretty {
	/** Same as top-level definitions of 'pretty'; returns compiler-defined function signature. */
    static constexpr const char *pretty(void) { return PRETTY_FUNCTION; }
	operator std::string(void) const;
	/** Stream insertion operator; strips all but the type sequence from 'pretty'. */
    template<class S>
    friend S& operator<<(S& s, Pretty const& p) {
		return s << std::string(p), s;
	}
	// Pedantic version - insertion into ostringstream& returns ostream&, etc.
	/*template<class S>
	friend auto operator<<(S &s, Pretty const& p) -> decltype(s << std::string()) {
		return s << std::string(p);
	}*/
};

/** Returns an instance of Pretty with the given types in the style of make_tuple. */
template<class... R>
Pretty<R...> make_pretty(R &&... r) { return {}; }

#endif
