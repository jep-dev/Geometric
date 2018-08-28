#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

#include "pretty.tpp"
#include "utility.hpp"

struct False : std::false_type { };
struct True : std::true_type { };

template<class S, class = void>
struct HasLength : False { typedef False type; };
template<class S, class = void>
struct HasSize : False { typedef False type; };
template<class S, class = void>
struct HasTellp : False { typedef False type; };
template<class S, class = void>
struct HasSeekp : False { typedef False type; };
template<class S, class = void>
struct HasTellg : False { typedef False type; };
template<class S, class = void>
struct HasSeekg : False { typedef False type; };

template<class S, class = void, class = void>
struct HasTellpSeekp : False { typedef False type; };
template<class S, class = void, class = void>
struct HasTellgSeekg : False { typedef False type; };

template<class S>
struct HasLength<S, Detail::Void_t<decltype(&S::length)>>
	: True { typedef decltype(&S::length) type; };
template<class S>
struct HasSize<S, Detail::Void_t<decltype(&S::size)>>
	: True { typedef decltype(&S::size) type; };
template<class S>
struct HasTellg<S, Detail::Void_t<decltype(&S::tellg)>>
	: True { typedef decltype(&S::tellg) type; };
template<class S>
struct HasTellp<S, Detail::Void_t<decltype(&S::tellp)>>
	: True { typedef decltype(&S::tellp) type; };

template<class S>
struct HasTellpSeekp<S, Detail::Void_t<decltype(&S::tellp)>,
		Detail::Void_t<decltype(&S::seekp)>> : True {};
template<class S>
struct HasTellgSeekg<S, Detail::Void_t<decltype(&S::tellg)>,
		Detail::Void_t<decltype(&S::seekg)>> : True {};


template<class S, class T> using HasLength_t = typename HasLength<S, T>::type;
template<class S, class T> using HasSize_t = typename HasSize<S, T>::type;
template<class S, class T> using HasTellg_t  = typename HasTellg<S, T>::type;
template<class S, class T> using HasTellp_t = typename HasTellp<S, T>::type;

/** Proof of concept for HasLength */
template<class T> std::enable_if_t<HasLength<T>::value, const char*>
	size(T && t) { return t.length(), ".length()"; }
/** Proof of concept for HasSize; excludes types with length to remove ambiguity */
template<class T> std::enable_if_t<HasSize<T>::value && !HasLength<T>::value, const char*>
	size(T && t) { return t.size(), ".size()"; }

/* Removed due to ambiguity with HasTellpSeekp/HasTellgSeekg
	// Proof of concept for HasTellp
	template<class T> std::enable_if_t<HasTellp<T>::value, const char*>
		size(T && t) { return t.tellp(), ".tellp()"; }
	// Proof of concept for HasTellg
	template<class T> std::enable_if_t<HasTellg<T>::value, const char*>
		size(T && t) { return t.tellg(), ".tellg()"; }
*/
template<class T> std::enable_if_t<HasTellpSeekp<T>::value, const char*>
	size(T && t) { return t.tellp(), t.seekp(0, std::ios::end), ".tellp()/.seekp()"; }
template<class T> std::enable_if_t<HasTellgSeekg<T>::value, const char*>
	size(T && t) { return t.tellg(), t.seekg(0, std::ios::end), ".tellg()/.seekg()"; }

/** Fallback in case none are implemented; comment out to require one of the others */
template<class T> std::enable_if_t<!HasTellp<T>::value && !HasTellg<T>::value
		&& !HasLength<T>::value && !HasSize<T>::value, const char*>
	size(T& t) { return "n/a (1)"; }
template<class T>
const char* size(T const& t) { return "n/a (2)"; }

struct A { constexpr std::size_t length(void) const { return __LINE__; } };
struct B {
	constexpr std::size_t tellp(void) const { return __LINE__; }
	constexpr std::size_t seekp(int, const std::ios::seek_dir &) const { return __LINE__; }
};
struct C {
	constexpr std::size_t tellg(void) const { return __LINE__; }
	constexpr std::size_t seekg(int, const std::ios::seekdir&) const { return __LINE__; }
};
struct D { constexpr std::size_t size(void) const { return __LINE__; } };
struct AB : A, B {};
struct CD : C, D {};
struct Z {};

int main(int argc, const char *argv[]) {
	using std::cout;
	using std::endl;
	using std::ifstream;
	using std::ofstream;
	using std::ostringstream;
	using std::string;
	using std::pair;
	using std::vector;

	string buf = "Content in string";
	ostringstream oss;
	oss << "Content in ostringstream";

	cout << "In file " << __FILE__ << "...\n";

	ostringstream out;
	auto prn = [&out] (unsigned line, const char *result, std::string prettied) {
		out << prettied << " from line " << line << " -> " << result << "\n";
	};
	prn(__LINE__, size(A()), Pretty<A>());                         // calls length
	prn(__LINE__, size(B()), Pretty<B>());                         // calls tellp
	prn(__LINE__, size(C()), Pretty<C>());                         // calls tellg
	prn(__LINE__, size(D()), Pretty<D>());                         // calls size
	prn(__LINE__, size(Z()), Pretty<Z>());                         // calls fallback
	prn(__LINE__, size(vector<int>()), Pretty<vector<int>>());     // calls size
	prn(__LINE__, size(pair<int,int>()), Pretty<pair<int,int>>()); // calls fallback
	prn(__LINE__, size(ifstream()), Pretty<ifstream>());           // calls tellg
	prn(__LINE__, size(ofstream()), Pretty<ofstream>());           // calls tellp
	cout << out.str();

}
