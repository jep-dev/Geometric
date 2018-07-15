#include "utilities.hpp"
#include <utility>
#include "pretty.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

struct False : std::false_type { };
struct True : std::true_type { };

template<class S, class = void>
struct HasLength : False { typedef False type; };
template<class S, class = void>
struct HasSize : False { typedef False type; };
template<class S, class = void>
struct HasTellp : False { typedef False type; };
template<class S, class = void>
struct HasTellg : False { typedef False type; };


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
	: True { typedef True type; };

template<class S, class T> using HasLength_t = typename HasLength<S, T>::type;
template<class S, class T> using HasSize_t = typename HasSize<S, T>::type;
template<class S, class T> using HasTellg_t  = typename HasTellg<S, T>::type;
template<class S, class T> using HasTellp_t = typename HasTellp<S, T>::type;

/** Use the length method but return the line number for testing. */
template<class T> std::enable_if_t<HasLength<T>::value, std::size_t>
	size(T && t) { return t.length(), __LINE__; }
/** Use the size method but return the line number for testing. */
template<class T> std::enable_if_t<HasSize<T>::value && !HasLength<T>::value, std::size_t>
	size(T && t) { return t.size(), __LINE__; }
/** Use the tellp method but return the line number for testing. */
template<class T> std::enable_if_t<HasTellp<T>::value, std::size_t>
size(T && t) { return t.tellp(), __LINE__; }
	/*
	auto pos = t.tellp(), out = pos;
	t.seekp(0, std::ios::end);
	out = t.tellp();
	t.seekp(pos);
	return out;
	*/
/** Use the tellg method but return the line number for testing. */
template<class T> std::enable_if_t<HasTellg<T>::value, std::size_t>
size(T && t) { return t.tellg(), __LINE__; }
	/*
	auto pos = t.tellg(), out = pos;
	t.seekg(0, std::ios::end);
	out = t.tellg();
	t.seekg(pos);
	return out;
	*/

/** Fallback in case none are implemented; comment out to require one of the others */
template<class T> std::enable_if_t<!HasTellp<T>::value && !HasTellg<T>::value
		&& !HasLength<T>::value && !HasSize<T>::value, std::size_t>
size(T& t) { return __LINE__; }
template<class T>
std::size_t size(T const& t) { return __LINE__; }

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
	out << " ...Line " << (__LINE__+1) << " -> line "
		<< size(A()) << " (as " << Pretty<A>() << ")\n"
			<< " ...Line " << (__LINE__+1) << " -> line "
		<< size(B()) << " (as " << Pretty<B>() << ")\n"
			<< " ...Line " << (__LINE__+1) << " -> line "
		<< size(C()) << " (as " << Pretty<C>() << ")\n"
			<< " ...Line " << (__LINE__+1) << " -> line "
		<< size(D()) << " (as " << Pretty<D>() << ")\n"
			" ...Line " << (__LINE__+1) << " -> line "
		<< size(Z()) << " (as " << Pretty<Z>() << ")\n"
			" ...Line " << (__LINE__+1) << " -> line "
		<< size(vector<int>()) << " (as " << Pretty<vector<int>>() << ")\n"
			" ...Line " << (__LINE__+1) << " -> line "
		<< size(pair<int,int>()) << " (as " << Pretty<pair<int,int>>() << ")\n"
			" ...Line " << (__LINE__+1) << " -> line "
		<< size(ifstream()) << " (as " << Pretty<ifstream>() << ")\n"
			<< " ...Line " << (__LINE__+1) << " -> line "
		<< size(ofstream()) << " (as " << Pretty<ofstream>() << ")";
	cout << out.str() << endl;

}
