#ifndef PRETTY_HPP
#define PRETTY_HPP

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
	/** Stream insertion operator; strips all but the type sequence from 'pretty'. */
    template<class S>
    friend S& operator<<(S& s, Pretty const& p) {
        using namespace std;
		string unknown = "(unknown)", orig = pretty(), lhs, rhs;
#ifdef HAS_FUNCSIG
		lhs = "<", rhs = ">";
#elif defined HAS_PRETTY
		lhs = "{", rhs = "}";
#else
        return s << unknown, s;
#endif
        // Match the earliest opening and latest closing brackets
        auto p0 = orig.find_first_of(lhs), p1 = orig.find_last_of(rhs);
        if(p0 == string::npos || p1 == string::npos) return s << unknown, s;
        p0++; // Offset by the width of the opening character
        auto dp = p1 - p0; // Resulting width
        if(p0 > p1) return s << unknown, s; // Inverted range
        auto result = orig.substr(p0, dp); // Range interior
        // Trim the space possibly used to separate adjacent brackets
        if(result[result.length()-1] == ' ') result.erase(result.length()-1);
        return s << result, s;
    }
};

/** Returns an instance of Pretty with the given types in the style of make_tuple. */
template<class... R>
Pretty<R...> make_pretty(R &&... r) { return {}; }

#endif
