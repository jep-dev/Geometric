#ifndef STREAMS_HPP
#define STREAMS_HPP

#include <algorithm>
#include <string>
#include <vector>

#include <iosfwd>
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

using std::declval;
using std::true_type;
using std::false_type;


struct OStringStream;

template<class S>
ostream& center(ostream& out, S const& s, unsigned N, bool leftish = true);
string center(string const& out, unsigned N, char fill = ' ', bool leftish = true);

string trim(string const& s, bool leading = true, bool trailing = true) {
	long size = s.length(), front = 0, back = size-1;
	if(size == 0) return "";

	auto match = [] (char c) -> bool {
		switch(c) {
			case ' ': case '\t':
			case '\r': case '\n': return false;
			default: return true;
		}
	};

	if(leading) for(front = 0; front <= back; front++)
		if(match(s[front])) break;
	if(trailing) for(back = size-1; back >= front; back--)
		if(match(s[back])) break;

	return s.substr(front, back-front+1);
}

/** @brief Abstract minimax (pair with first=min and second=max.)
 * Note that it could still be abstract with respect to size/length/tellp and to begin/end
 * with more work.
 */
template<class T> using rm_ref_t = std::remove_reference_t<T>;
template<class T> using rm_const_t = std::remove_const_t<T>;
template<class T> using rm_cv_t = std::remove_cv_t<T>;

template<class T> using First_t = rm_ref_t<decltype(*std::begin(declval<T&>()))>;
template<class T, class V = First_t<T>, class F = void>
struct MiniMax;

template<class T, class V>
struct MiniMax<T, V, Detail::Void_t<decltype(&V::length)>> {
	std::pair<long, long> operator()(T& t) const {
		std::pair<long, long> out(std::numeric_limits<long>::max(), 0);
		for(auto const& i : t) {
			auto len = i.length();
			if(len < out.first) out.first = len;
			if(len > out.second) out.second = len;
		}
		return out;
	}
};
template<class T, class V>
struct MiniMax<T, V, Detail::Void_t<decltype(&V::tellp)>> {
	std::pair<long, long> operator()(T& t) const; /*{
		std::pair<long, long> out(std::numeric_limits<long>::max(), 0);
		for(auto& i : t) {
			auto pos = i.tellp();
			if(pos < 0) continue;
			i.seekp(0, std::ios_base::end);
			auto len = i.tellp();
			i.seekp(pos);
			if(len < 0) continue;
			if(len < out.first) out.first = len;
			if(len > out.second) out.second = len;
		}
		return out;
	}*/
};
template<class T>
std::pair<long, long> minimax(T& t) {
	return MiniMax<T, First_t<T>>()(t);
}
/*template<class T, class V = First_t<T>, class F = void>
std::pair<long, long> minimax(T const& t);

template<class T>
std::pair<long, long> minimax<T, First_t<T>, decltype(&First_t<T>::length)>(T const& t) {
	long m0 = std::numeric_limits<long>::max(), m1 = 0;
	for(auto &it : t) {
		long len = it.length();
		m0 = std::min(len, m0);
		m1 = std::max(len, m1);
	}
	if(m0 > m1) m0 = m1;
	return {m0, m1};
}*/
template<class S, class = void>
struct HasInsert : false_type {};
template<class S, class T, class = void>
struct HasAdd : false_type {};
template<class S, class = void>
struct HasLength : false_type {};
template<class S, class = void, class = void>
struct HasTellpSeekp : false_type {};

template<class S>
struct HasInsert<S, Detail::Void_t<decltype(&S::operator<<)>> : true_type {};
template<class S, class T>
struct HasAdd<S, T, Detail::Void_t<decltype(declval<S>()+=declval<T>())>> : true_type {};
template<class S>
struct HasLength<S, Detail::Void_t<decltype(&S::length)>> : true_type {};
template<class S>
struct HasTellpSeekp<S, Detail::Void_t<decltype(&S::tellp)>,
		Detail::Void_t<decltype(&S::seekp)>> : true_type {};

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
		-> std::enable_if_t<HasInsert<SV>::value && HasTellpSeekp<SV>::value, S>&; /*{
	auto mm = minimax(s);
	for(auto& it : s) {
		auto cur = it.tellp();
		it.seekp(0, std::ios_base::end);
		auto len = mm.second - it.tellp();
		it.seekp(cur);
		if(len > 0) it += std::string(len, fill);
	}
	return s;
}*/
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

}

#endif
