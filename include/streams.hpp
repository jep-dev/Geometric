#ifndef STREAMS_HPP
#define STREAMS_HPP

#include <algorithm>
#include <iosfwd>
#include <string>
#include <vector>

#include "utility.hpp"

#ifndef INDENT
#define INDENT "\t"
#endif

namespace Streams {
using std::string;
using std::vector;
using std::ostream;
using std::istream;
using std::ostringstream;
using std::istringstream;

template<class S>
std::ostream& center(std::ostream& out, S const& s, unsigned N, bool leftish = true);
std::string center(std::string const& out, unsigned N, char fill = ' ', bool leftish = true);

/** Abstract minimax (pair with first=min and second=max) */
template<class T>
std::pair<long, long> minimax(T && t) {
	long m0 = std::numeric_limits<long>::max(), m1 = 0;
	for(auto &it : t) {
		long len = it.length();
		m0 = std::min(len, m0);
		m1 = std::max(len, m1);
	}
	if(m0 > m1) m0 = m1;
	return {m0, m1};
}

template<class S, class = void>
struct HasInsert : std::false_type {};
template<class S, class T, class = void>
struct HasAdd : std::false_type {};
template<class S, class = void>
struct HasLength : std::false_type {};
template<class S, class = void, class = void>
struct HasTellpSeekp : std::false_type {};

template<class S>
struct HasInsert<S, Detail::Void_t<decltype(&S::operator<<)>> : std::true_type {};
template<class S, class T>
struct HasAdd<S, T, Detail::Void_t<decltype(std::declval<S>()+=std::declval<T>())>> : std::true_type {};
template<class S>
struct HasLength<S, Detail::Void_t<decltype(&S::length)>> : std::true_type {};
template<class S>
struct HasTellpSeekp<S, Detail::Void_t<decltype(&S::tellp)>,
		Detail::Void_t<decltype(&S::seekp)>> : std::true_type {};

template<class S>
constexpr bool has_insert(Detail::Tag<S> && s = {}) { return HasInsert<S>::value; }
template<class S, class T>
constexpr bool has_add(Detail::Tag<S> && s = {}, Detail::Tag<T> && t = {})
	{ return HasAdd<S, T>::value; }

template<class S>
constexpr bool has_length(Detail::Tag<S> && s = {}) { return HasLength<S>::value; }
template<class S>
constexpr bool has_tellp_seekp(Detail::Tag<S> && s = {}) { return HasTellpSeekp<S>::value; }

template<class S, class SV = typename S::value_type>
auto level(S& s, char fill = ' ')
		-> std::enable_if_t<HasInsert<SV>::value && HasTellpSeekp<SV>::value, S>& {
	auto mm = minimax(s);
	for(auto& it : s) {
		auto cur = it.tellp();
		it.seekp(0, std::ios_base::end);
		auto len = mm.second - it.tellp();
		it.seekp(cur);
		if(len > 0) it += std::string(len, fill);
	}
	return s;
}
template<class S, class SV = typename S::value_type>
auto level(S& s, char fill = ' ')
		-> std::enable_if_t<HasAdd<SV, std::string>::value && HasLength<SV>::value, S>& {
	auto mm = minimax(s);
	for(auto& it : s) {
		auto len = mm.second - it.length();
		if(len > 0) it += std::string(len, fill);
	}
	return s;
}

struct OStringStream : std::ostringstream {
	std::size_t size(void) const {
		auto s = str();
		return s.length();
	}
	std::size_t size(void) {
		auto pos = tellp();
		seekp(0, std::ios_base::end);
		auto out = tellp();
		seekp(pos);
		return std::max(0, int(out));
	}
	virtual ~OStringStream(void) {}
};

}

#endif
